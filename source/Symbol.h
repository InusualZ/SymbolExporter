#pragma once

#include <ostream>
#include <string>
#include <vector>

#include "Common.h"

#include "elfio/elfio.hpp"

class Symbol {
private:
	uint64 offset;
	uint64 size;
	std::string symbol;

public:
	bool isGlobal;
	bool isDestructor;
	bool isConstructor;

public:
	Symbol();

	Symbol(uint64 symOffset, uint64 symSize, const std::string& sym);

	const std::string& getSymbol() const;

	uint64 getOffset() const;

	uint64 getSize() const;

	operator bool() const;

	friend std::ostream& operator<<(std::ostream& stream, const Symbol& symbol);

	static Symbol fromString(const std::string& symbolLine);

	static std::vector<Symbol> readSymbols(const ELFIO::elfio& reader);

	static std::string demangleSymbol(const std::string& symbol);

	static std::vector<Symbol> demangleSymbols(const std::vector<Symbol>&& symbols);
};