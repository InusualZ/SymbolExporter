cmake_minimum_required(VERSION 3.7)
project(SymbolExporter)

if (MSVC)
    message(FATAL_ERROR "Sorry, Visual Studio is not supported. Only MinGW")
endif (MSVC)

set(NAME "SymbolExporter")
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast -Wall -static-libgcc -static-libstdc++")

set(SRC
        src/Classyfier.cpp
        src/Main.cpp
        src/Object.cpp)

set(INCLUDES
        src/elfio/elf_types.hpp
        src/elfio/elfio.hpp
        src/elfio/elfio_dump.hpp
        src/elfio/elfio_dynamic.hpp
        src/elfio/elfio_header.hpp
        src/elfio/elfio_note.hpp
        src/elfio/elfio_relocation.hpp
        src/elfio/elfio_section.hpp
        src/elfio/elfio_segment.hpp
        src/elfio/elfio_strings.hpp
        src/elfio/elfio_symbols.hpp
        src/elfio/elfio_utils.hpp

        src/optparse.h

        src/Classyfier.h
        src/Object.h
        src/Symbol32.h
        src/VTable.h
        )

add_executable(${NAME} ${SRC} ${INCLUDES})
