#include "Symbol.h"

#include <regex>

#if defined(__GNUC__) || defined(__MINGW32__)
	#include <cxxabi.h>
#else
	#error "The program can only work on GCC Compilers"
#endif

Symbol::Symbol() 
	: offset(0), size(0), isGlobal(false), isDestructor(false), isConstructor(false) {}

Symbol::Symbol(uint64 symOffset, uint64 symSize, const std::string& sym) 
	: offset(symOffset), size(symSize), symbol(std::move(sym)), isGlobal(false), isDestructor(false), isConstructor(false) {}

uint64 Symbol::getOffset() const {
	return offset;
}

uint64 Symbol::getSize() const {
	return size;
}

const std::string& Symbol::getSymbol() const {
	return symbol;
}

Symbol::operator bool() const {
	return !symbol.empty() && offset != 0 && size != 0;
}

std::ostream& operator<<(std::ostream& stream, const Symbol& symbol) {
	return stream << symbol.getOffset() << ' ' << symbol.getSize() << ' ' << symbol.getSymbol();
}

Symbol Symbol::fromString(const std::string& symbolLine) {
	uint64 offset = 0, size = 0;
	std::string symbol;

	std::smatch match;
	if (std::regex_match(symbolLine, match, std::regex("([0-9a-z]*)\\s([0-9a-z]*)\\s(.+)"))) {
		offset = (uint64)atoll(match[1].str().c_str());
		size = (uint64)atoll(match[2].str().c_str());
		symbol = match[3];
	} else {
		std::cerr << "FAILED TO PARSE SYMBOL LINE: " << symbolLine << std::endl;
	}

	return std::move(Symbol(offset, size, symbol));
}

std::vector<Symbol> Symbol::readSymbols(const ELFIO::elfio& reader) {
	std::vector<Symbol> symbols;
	for (int x = 0; x < reader.sections.size(); ++x) {
		if (x != SHT_SYMTAB && x != SHT_DYNSYM) {
			continue;
		}

		ELFIO::symbol_section_accessor accessor(reader, reader.sections[x]);
		for (ELFIO::Elf_Half i = 0; i < accessor.get_symbols_num(); ++i) {
			std::string name;
			ELFIO::Elf64_Addr offset;
			ELFIO::Elf_Xword size;
			unsigned char bind;
			unsigned char type;
			ELFIO::Elf_Half section_index;
			unsigned char other;
			if (accessor.get_symbol(i, name, offset, size, bind, type, section_index, other)) {
				name = Symbol::demangleSymbol(name);
				if (!name.empty() && offset != 0 && size != 0) {
					Symbol s(offset, size, name);
					s.isGlobal = type == STB_GLOBAL;
					symbols.push_back(std::move(s));
				}
			}
		}
	}

	return std::move(symbols);
}

std::string Symbol::demangleSymbol(const std::string& symbol) {
	int status;
	char* demangled = abi::__cxa_demangle(symbol.c_str(), nullptr, nullptr, &status);
	if (status != 0) {
		return "";
	}

	return demangled;
}

std::vector<Symbol> Symbol::demangleSymbols(const std::vector<Symbol>&& symbols) {
	std::vector<Symbol> demangledSymbols;
	for (const auto& symbol : symbols) {
		std::string symbolString = Symbol::demangleSymbol(symbol.getSymbol());
		if (!symbolString.empty()) {
			demangledSymbols.push_back(Symbol(symbol.getOffset(), symbol.getSize(), symbolString));
		}
	}

	return std::move(demangledSymbols);
}