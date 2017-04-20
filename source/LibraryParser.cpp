#include "LibraryParser.h"

#include <iostream>
#include <fstream>
#include <regex>

#include "elfio/elfio.hpp"

#include "Common.h"

LibraryParser::LibraryParser(const std::string& binaryPath) {
    parse(binaryPath);
}

void LibraryParser::parse(const std::string& binaryPath) {
    ELFIO::elfio reader;
    if (!reader.load(binaryPath)) {
        std::cerr << "FAILED TO LOAD BINARY: " << binaryPath << std::endl;
        return;
    }

    std::cout << "Reading symbols...\n";
    repository = std::move(Symbol::readSymbols(reader));
    std::cout << repository.size() << " symbols found!\n";

    std::cout << "Reading VTables...\n";
    uint32 skipped = parseVTables(reader);
    if (skipped > 0) {
        std::cout << skipped << " vtables have been skipped\n";
    }
    std::cout << vtableRepository.size() << " vtables found!\n";
}

uint32 LibraryParser::parseVTables(const ELFIO::elfio& reader) {
    uint32 skipped = 0;
    for (const auto& symbol : repository) {
        std::smatch match;
        if (std::regex_match(symbol.getSymbol(), match, std::regex("vtable\\sfor\\s(.*)"))) {
            if (std::regex_match(match[1].str(), std::regex("^[a-zA-Z_]{1}[a-zA-Z_0-9]+$"))) {
                Symbol vSymbol(symbol.getOffset(), symbol.getSize(), match[1].str());
                const auto& vtable = VTable::fromSymbol(reader, vSymbol);
                vtableRepository.push_back(vtable);
            } else {
                ++skipped;
                //std::cerr << "Skipping: " << match[1] << std::endl;
            }
        }
    }

    return skipped;
}

const std::vector<Symbol>& LibraryParser::getSymbols() const {
    return repository;
}

std::vector<VTable>& LibraryParser::getVTables() {
    return vtableRepository;
}