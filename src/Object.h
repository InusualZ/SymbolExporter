#pragma once

#include <string>
#include <regex>
#include <vector>
#include <iostream>

#include "Symbol32.h"
#include "VTable.h"

struct Object {
    enum Type {
        NONE,
        NAMESPACE,
        CLASS,
        METHOD,
        FIELD
    };

    Type type;
    std::string name;
    Symbol32* symbol;
    std::vector<Object*> childs;
	VTable* vtable;

    Object(Type objectType, std::string objectName, Symbol32* originalSymbol);

    Object* getChild(const std::string& objectName);

    void setVTable(VTable* newVTable);

    friend std::ostream& operator<<(std::ostream& stream, const Object& object);
};