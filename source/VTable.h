#pragma once

#include <map>

#include "elfio/elfio.hpp"

#include "Common.h"
#include "Symbol.h"

class VTable {
private:
    Symbol symbol;
    std::map<uint64, Symbol> symbols;

public:
    VTable(const Symbol& signature, const std::map<uint64, Symbol>& content);

    const std::string& getSignature() const;

    uint64 getOffset() const;

    uint64 getSize() const;

    const Symbol& getSymbol(uint64 offset);

    const std::map<uint64, Symbol>& getSymbols() const;

    friend std::ostream& operator<<(std::ostream& stream, const VTable& vtable);

    static VTable fromSymbol(const ELFIO::elfio& reader, const Symbol& symbol);
};