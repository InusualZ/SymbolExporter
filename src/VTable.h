#pragma once

#include <cstdint>
#include <map>

class Symbol32;

struct VTable {
    std::string name;
    Symbol32* symbol;
    std::uint32_t size;
    std::uint32_t baseOffset;
    std::uint32_t maxOffset;
    std::map<uint32_t, Symbol32*> content;

    explicit VTable(Symbol32* symbol_);
};