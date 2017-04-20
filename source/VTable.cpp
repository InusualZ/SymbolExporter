#include "VTable.h"

#include <regex>

VTable::VTable(const Symbol& signature, const std::map<uint64, Symbol>& content) 
    : symbol(std::move(signature)), symbols(std::move(content)) {}

const std::string& VTable::getSignature() const {
    return symbol.getSymbol();
}

uint64 VTable::getOffset() const {
    return symbol.getOffset();
}

uint64 VTable::getSize() const {
    return symbol.getSize() / 4 - 2;
}

const Symbol& VTable::getSymbol(uint64 offset) {
    return symbols[offset];
}

const std::map<uint64, Symbol>& VTable::getSymbols() const {
    return symbols;
}

std::ostream& operator<<(std::ostream& stream, const VTable& vtable) {
    stream << vtable.getSignature() << " VTable(" << vtable.getOffset() << ", " << vtable.getSize() << ")"<< std::endl;
    for (const auto& symbol : vtable.getSymbols()) {
        stream << symbol.first << ' ' << symbol.second << std::endl;
    }
    return stream;
}

VTable VTable::fromSymbol(const ELFIO::elfio& reader, const Symbol& symbol) {
    const auto& section = reader.sections[SHT_HASH];
	ELFIO::relocation_section_accessor accessor(reader, section);
	std::map<uint64, Symbol> symbols;
    size_t count = 0;
    size_t vtableSize = symbol.getSize() / 4 - 2;
	for (ELFIO::Elf_Xword entry = 0; entry < accessor.get_entries_num() && count < vtableSize; ++entry) {
		ELFIO::Elf64_Addr offset;
		ELFIO::Elf64_Addr symbolValue;
		std::string symbolName;
		ELFIO::Elf_Word type;
		ELFIO::Elf_Sxword addend;
		ELFIO::Elf_Sxword calcValue;
		if (accessor.get_entry(entry, offset, symbolValue, symbolName, type, addend, calcValue)) {
			for (std::size_t i = 0; i < vtableSize && count < vtableSize; ++i) {
				if (offset == symbol.getOffset() + 8 + i * 4) {
                    if (symbolName != "__cxa_pure_virtual") {
                        symbolName = Symbol::demangleSymbol(symbolName);

                    }

                    if (!symbolName.empty()) {
						std::smatch match;
						if (std::regex_match(symbolName, match, std::regex("^.*?::(.*?)$"))) {
							symbols[offset] = Symbol(symbolValue, 4, match[1]);

							if (match[1].str().find('~') != std::string::npos) {
								symbols[offset].isDestructor = true;
							}
						} else {
							symbols[offset] = Symbol(symbolValue, 4, symbolName);
						}
						++count;
                    }
				}
			}
		}
	}

    return VTable(symbol, symbols);
}