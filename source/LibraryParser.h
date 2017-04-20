#pragma once

#include <vector>

#include "Symbol.h"
#include "VTable.h"

static const int SYMBOLS_ONLY = 0x01;
static const int VTABLE_ONLY = 0x02;
static const int SYMBOL_AND_VTABLES = SYMBOLS_ONLY | VTABLE_ONLY;


class LibraryParser {
private:
    std::vector<Symbol> repository;
    std::vector<VTable> vtableRepository;

public:
    LibraryParser(const std::string& binaryPath);

private:
    void parse(const std::string& binaryPath);

    uint32 parseVTables(const ELFIO::elfio& reader);

public:
    const std::vector<Symbol>& getSymbols() const;

    std::vector<VTable>& getVTables();
};