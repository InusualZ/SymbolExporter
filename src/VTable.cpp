#include "VTable.h"

#include <cstring>

#include "Symbol32.h"

VTable::VTable(Symbol32* symbol_)  : symbol(symbol_) {
    size = symbol_->size / 4 - 2;
    baseOffset = symbol_->valueOffset + 8;
    maxOffset = baseOffset + size * 4;

    auto len = strlen(symbol->demangled);
    for (size_t i = 11; i < len; ++i) {
        name += symbol->demangled[i];
    }
}