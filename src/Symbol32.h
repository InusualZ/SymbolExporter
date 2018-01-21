#pragma once

#include <cstdint>

struct Symbol32 {
    uint32_t index;

    const char* demangled;
    const char* mangled;

    uint32_t nameOffset;
    uint32_t valueOffset;
    uint32_t size;
    unsigned char bind;
    unsigned char type;
    uint16_t sectionIndex;
    unsigned char other;

    bool isDestructor = false;
    bool isConstructor = false;
    bool isStatic = false;

    Symbol32(uint32_t index_, const char* demangled_, const char* mangled_, uint32_t nameOffset_, uint32_t valueOffset_,
             uint32_t size_, unsigned char bind_, unsigned char type_, uint16_t sectionIndex_, unsigned char other_);

    static const char* demangle(const char* mangled);
};