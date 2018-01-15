#include <chrono>
#include <cstring>
#include <fstream>
#include <iostream>
#include <numeric>
#include <sys/stat.h>
#include <sys/unistd.h>

#include "elfio/elf_types.hpp"
#include "elfio/elfio.hpp"
#include "elfio/elfio_dump.hpp"

#include "optparse.h"

#include "Classyfier.h"
#include "Object.h"
#include "Symbol32.h"
#include "VTable.h"

static std::vector<VTable> vtables;
static Symbol32 pureVirtualSymbol(std::numeric_limits<uint32_t>::max(), "pure_virtual", "__cxa_pure_virtual",
                                  0, 0, 4, 0x00, 0x00, 1, 0x00);

bool getVTablesContent(ELFIO::elfio& reader, std::vector<VTable>& vtables, std::vector<Symbol32>& symbols) {
    using namespace ELFIO;
    const auto& section = reader.sections[8];
    if (section == nullptr) {
        return false;
    }

    relocation_section_accessor accessor(reader, section);
    for (Elf_Xword entry = 0; entry < accessor.get_entries_num(); ++entry) {
        Elf64_Addr offset = 0;
        Elf_Word symbolOffset = 0;
        Elf_Word type;
        Elf_Sxword addend;
        if (accessor.get_entry(entry, offset, symbolOffset, type, addend)) {
            for (auto& table : vtables) {
                if (offset >= table.baseOffset && offset <= table.maxOffset) {
                    bool found = false;
                    for (auto& symbol : symbols) {
                        if (symbolOffset == symbol.index) {
                            table.content[offset] = (&symbol);
                            found = true;
                            break;
                        }
                    }

                    // if the symbol wasn't found in the list, probably is `__cxa_pure_virtual`
                    if (!found) {
                        table.content[offset] = &pureVirtualSymbol;
                    }
                }
            }
        }
    }

    return true;
}

std::vector<Symbol32> getSymbols(const ELFIO::endianess_convertor& convertor, const ELFIO::section& section,
                                 const ELFIO::section& stringSection) {
    auto entriesCount = section.get_size() / section.get_entry_size();
    std::vector<Symbol32> entries;
    for (uint32_t i = 0; i < entriesCount; ++i) {
        auto pSym = reinterpret_cast<const ELFIO::Elf32_Sym*>(section.get_data() + i * section.get_entry_size());

        // Swap Endianness if needed
        uint32_t nameOffset = convertor(pSym->st_name);
        uint32_t valueOffset = convertor(pSym->st_value);
        uint32_t size = convertor(pSym->st_size);
        unsigned char bind = ELF_ST_BIND(pSym->st_info);
        unsigned char type = static_cast<unsigned char>(ELF_ST_TYPE(pSym->st_info));
        uint16_t sectionIndex = convertor(pSym->st_shndx);
        unsigned char other = pSym->st_other;

        // If symbol is from dependency, discard...
        if (valueOffset == 0 || size == 0) {
            continue;
        }

        // Link Symbol String
        const char* name = stringSection.get_data() + nameOffset;

        // Discard, non standard symbols
        if (name[0] != '_' || name[1] != 'Z') {
            continue;
        }

        // Discard, standard method symbols && typeinfo symbol
        if ((name[2] == 'N' && name[3] == 'S' && name[4] == 't') || (name[2] == 'T' && (name[3] == 'I' ||
                                                                                        (name[3] == 'V' &&
                                                                                         name[4] == 'S' &&
                                                                                         name[5] == 't') ||
                                                                                        name[3] == 'S'))) {
            continue;
        }

        // Demangled
        const char* demangled = demangle(name);

        // Push Symbol
        entries.emplace_back(i, demangled, name, nameOffset, valueOffset, size, bind, type, sectionIndex, other);

        // Check if it's a virtual table symbol
        if (sectionIndex == 17) {
            if (std::strncmp(name, "_ZTV", 4) == 0) {
                vtables.emplace_back(
                        new Symbol32(i, demangled, name, nameOffset, valueOffset, size, bind, type, sectionIndex,
                                     other));
            }
        }
    }
    return std::move(entries);
}

int main(int argc, const char** argv) {
    optparse::OptionParser parser = optparse::OptionParser().description("SymbolExporter v1.0");

    parser.add_option("-i", "--input").dest("input").help("ELF File").
            metavar("FILE");
    parser.add_option("-o", "--output").dest("output").set_default("output/").
            help("Folder where the output would be stored");
    parser.add_option("-m", "--mangled").dest("writeMangled").
            help("Would write mangled symbol on comment form before the demangled symbol");

    const optparse::Values options = parser.parse_args(argc, argv);

    std::string inputFile = options["input"];
    if (inputFile.empty()) {
        std::cerr << "Failed! You have to specify a valid input file!\n";
        return 1;
    }

    std::string outputFolder = options["output"];

    // Throw error if contain slashes
    if (outputFolder.empty() || outputFolder.find_first_of('\\') != std::string::npos) {
        std::cerr << "Failed! You have to specify a valid output folder!\n";
        return 1;
    }

    // If missing, add backslash
    if (outputFolder.back() != '/') {
        outputFolder += '/';
    }

    Object::commentMangledSymbol = options.is_set("writeMangled");

    ELFIO::elfio reader;
    if (!reader.load(inputFile)) {
        std::cerr << "FAILED TO LOAD BINARY: " << inputFile << std::endl;
        return 1;
    }

    if (reader.get_class() != ELFCLASS32) {
        std::cerr << "Only Elf 32bit is supported! Contact Developer!\n";
        return 1;
    }

    std::cout << "Let's read the Symbols" << '\n';

    const auto& symSection = reader.sections[".dynsym"];
    if (symSection == nullptr) {
        std::cerr << "Unable to retrieve symbol section!\n";
        return 1;
    }

    const auto& stringSection = reader.sections[symSection->get_link()];
    if (stringSection == nullptr) {
        std::cerr << "Unable to retrieve string section!\n";
        return 1;
    }

    auto symbols = getSymbols(reader.get_convertor(), *symSection, *stringSection);
    std::cout << "The # of symbol read was: " << symbols.size() << '\n';

    std::cout << "VTables Identified: " << vtables.size() << '\n';
    getVTablesContent(reader, vtables, symbols);
    std::cout << "Done! Gathering their information\n";

    std::cout << "Writing Symbols to disk\n";
    std::ofstream outputFile(outputFolder + "symbols.txt", std::ios_base::out | std::ios_base::trunc);
    for (const auto& symbol : symbols) {
        outputFile << "[0x" << std::setfill('0') << std::setw(8) << std::hex << symbol.valueOffset - 1 << "] " << symbol.demangled << '\n';
    }
    outputFile.close();

    std::cout << "Writing Virtual Tables to disk\n";
    outputFile.open(outputFolder + "vtables.txt", std::ios_base::out | std::ios_base::trunc);
    for (const auto& table : vtables) {
        outputFile << '\n' << table.symbol->demangled << '\n';
        for (const auto& it : table.content) {
            outputFile << it.second->demangled << '\n';
        }
    }
    outputFile.close();

    std::cout << "Converting Symbols to Class\n";

    Classyfier converter(symbols);
    std::cout << converter.parse() << " types have been skipped\n";

    std::cout << "Linking VTables\n";
    for (auto& table : vtables) {
        for (const auto& object : converter.getObjects()) {
            const auto& child = object->getChild(table.name);
            if (child != nullptr) {
                child->setVTable(&table);
            }
        }
    }

    // Separate headers
    outputFolder += "Headers/";

    std::cout << "Writting to disk " << converter.getObjects().size() << " classes\n";

    // Create folder
    mkdir(outputFolder.c_str(), 0777);

    // Serialize objects to class
    std::ofstream clazz;
    for (const auto& object : converter.getObjects()) {
        clazz.open(outputFolder + object->name + ".h", std::ios_base::out | std::ios_base::trunc);
        clazz << "#pragma once\n\n";
        clazz << *object;
        clazz.close();
    }

    std::cout << "DONE!\n";

    return 0;
}

