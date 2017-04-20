#include "Object.h"

#include <regex>

Object::Object() : type(Type::NONE), vtable(nullptr) {}

Object::Object(Type objectType, const std::string& objectName) 
    : type(objectType), name(std::move(objectName)), vtable(nullptr) {}

Object::Object(Type objectType, const std::string& objectName, const Symbol& originalSymbol) 
    : type(objectType), name(std::move(objectName)), symbol(std::move(originalSymbol)), vtable(nullptr) {}

void Object::setType(Type newType) {
    type = newType;
}

Object::Type Object::getType() const {
    return type;
}

void Object::setVTable(VTable* newVTable) {
	vtable = newVTable;

	for (const auto& vtableIT : vtable->getSymbols()) {
		for (std::vector<Object*>::const_iterator it = childs.begin(); it != childs.end();) {
			const auto& child = *it;
			if (child->getName() == vtableIT.second.getSymbol()) {
				it = childs.erase(it);
			} else {
				++it;
			}
		}
	}
}


VTable* Object::getVTable() const {
	return vtable;
}

const std::string& Object::getName() const {
    return name;
}

Symbol& Object::getSymbol() {
    return symbol;
}

void Object::addChild(Object* newChild) {
    childs.push_back(newChild);
}

Object* Object::getChild(const std::string& objectName) {
    if (name == objectName) {
        return this;
    }

    for (auto& object : childs) {
        const auto& child = object->getChild(objectName);
        if (child) {
            return child;
        } 
    }

    return nullptr;
}

Object* Object::getChild(const Symbol& objectSymbol){
    if (symbol.getSymbol() == objectSymbol.getSymbol()) {
        return this;
    }

    for (auto& object : childs) {
        const auto& child = object->getChild(objectSymbol);
        if (child) {
            return child;
        } 
    }

    return nullptr;
}

bool Object::hasChilds() const {
    return childs.size() != 0;
}

const std::vector<Object*>& Object::getChildObjects() const {
    return childs;
}

Object::operator bool() const {
	return type != Type::NONE;
}

std::ostream& operator<<(std::ostream& stream, const Object& object) {
	switch(object.type) {
	case Object::Type::NAMESPACE:
		stream << "namespace " << object.name << " {\n\n";

		for (const auto& child : object.childs) {
			stream << *child;
		}

		stream << "\n};\n";
		break;

	case Object::Type::CLASS:
	{
		stream << "class " << object.name << " {\n\npublic:\n";
		if (object.vtable) {
			stream << "\n\t// Virtual\n";
			bool haveDestructor = false;
			for (const auto& symbol : object.vtable->getSymbols()) {
				if (!symbol.second.isDestructor || !haveDestructor) {
					stream << "\tvirtual void " << symbol.second.getSymbol() << ";\n";
					if (symbol.second.isDestructor) {
						haveDestructor = true;
					}
				}
			}
		}

		std::vector<Object*> copyList = object.childs;
		stream << "\n\t// Non Virtual\n";
		for (std::vector<Object*>::const_iterator it = copyList.begin(); it != copyList.end();) {
			const auto& child = *it;
			if (child->getType() == Object::Type::METHOD && !child->getSymbol().isGlobal) {
				stream << '\t' << *child;
				it = copyList.erase(it);
			} else {
				++it;
			}
		}

		stream << "\n\t// Static\n";
		for (std::vector<Object*>::const_iterator it = copyList.begin(); it != copyList.end();) {
			const auto& child = *it;
			if (child->getType() == Object::Type::METHOD && child->getSymbol().isGlobal) {
				stream << "\tstatic " << *child;
				it = copyList.erase(it);
			} else {
				++it;
			}
		}

		stream << "\n\t// Field\n";
		for (std::vector<Object*>::const_iterator it = copyList.begin(); it != copyList.end();) {
			const auto& child = *it;
			if (child->getType() == Object::Type::FIELD) {
				stream << "\tstatic " << *child;
				it = copyList.erase(it);
			} else {
				++it;
			}
		}

		for (const auto& child : copyList) {
			stream << *child;
		}

		stream << "\n};\n";
		break;
	}

	case Object::Type::METHOD:
		if (!object.symbol.isConstructor && !object.symbol.isDestructor) {
			stream << "void ";
		}

		stream << object.name << ";\n";
		break;

	case Object::Type::FIELD:
		stream << "void* " << object.name << ";\n";
		break;
	}

	return stream.flush();
}