#pragma once

#include <cstdint>
#include <map>

#include "Symbol32.h"

struct VTable {
    std::string name;
    Symbol32* symbol;
    std::uint32_t size;
    std::uint32_t baseOffset;
    std::uint32_t maxOffset;
    std::map<uint32_t, Symbol32*> content;

    explicit VTable(Symbol32* symbol_) : symbol(symbol_) {
        size = symbol_->size / 4 - 2;
        baseOffset = symbol_->valueOffset + 8;
        maxOffset = baseOffset + size * 4;

        auto len = strlen(symbol->demangled);
        for (size_t i = 11; i < len; ++i) {
            name += symbol->demangled[i];
        }
    }
};