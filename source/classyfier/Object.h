#pragma once

#include <string>
#include <vector>

#include "../Symbol.h"
#include "../VTable.h"

class Object {
public:
    enum Type {
        NONE,
        NAMESPACE,
        CLASS,
        METHOD,
        FIELD
    };

private:
    Type type;
    std::string name;
    Symbol symbol;
    std::vector<Object*> childs;
	VTable* vtable;

private:
    Object();

public:
    Object(Type objectType, const std::string& objectName);
    Object(Type objectType, const std::string& objectName, const Symbol& originalSymbol);

    void setType(Type newType);

    Type getType() const;

	void setVTable(VTable* newVTable);

	VTable* getVTable() const;

	const std::string& getName() const;

    Symbol& getSymbol();

    void addChild(Object* newChild);

    Object* getChild(const std::string& objectName);

    Object* getChild(const Symbol& objectSymbol);

    bool hasChilds() const;

    const std::vector<Object*>& getChildObjects() const;

    operator bool() const;

	friend std::ostream& operator<<(std::ostream& stream, const Object& object);
};