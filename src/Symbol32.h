#pragma once

#include <cstdint>
#include <cstring>

#if defined(__GNUC__) || defined(__MINGW32__)
#include <cxxabi.h>
#else
#error "The program can only work on GCC Compilers"
#endif

static const char* demangle(const char* mangled) {
    int status;
    char* demangled = abi::__cxa_demangle(mangled, nullptr, nullptr, &status);
    if (status != 0) {
        return "";
    }

    return demangled;
}

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

    Symbol32(uint32_t index_, const char* demangled_, const char* mangled_, uint32_t nameOffset_, uint32_t valueOffset_, uint32_t size_, unsigned char bind_, unsigned char type_, uint16_t sectionIndex_, unsigned char other_)
            : index(index_), demangled(demangled_), mangled(mangled_), nameOffset(nameOffset_), valueOffset(valueOffset_), size(size_), bind(bind_), type(type_),
              sectionIndex(sectionIndex_), other(other_) {
        isStatic = sectionIndex != 11 && bind == 1;
        isDestructor = std::strstr(demangled, "::~") != nullptr;
    }
};