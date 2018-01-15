#include "Object.h"

Object::Object(Type objectType, std::string objectName, Symbol32* originalSymbol)
        : type(objectType), name(std::move(objectName)), symbol(originalSymbol), vtable(nullptr) {}


Object* Object::getChild(const std::string& objectName) {
    if (name == objectName) {
        return this;
    }

    for (auto& object : childs) {
        const auto& child = object->getChild(objectName);
        if (child != nullptr) {
            return child;
        }
    }

    return nullptr;
}

void Object::setVTable(VTable* newVTable) {
    vtable = newVTable;

    for (const auto& vtableIT : vtable->content) {
        for (std::vector<Object*>::const_iterator it = childs.begin(); it != childs.end();) {
            const auto& child = *it;
            if (std::strstr(vtableIT.second->demangled, child->name.c_str()) != nullptr) {
                it = childs.erase(it);
            } else {
                ++it;
            }
        }
    }
}

std::ostream& operator<<(std::ostream& stream, const Object& object) {
    switch (object.type) {
        case Object::Type::NAMESPACE:
            stream << "namespace " << object.name << " {\n\n";

            for (const auto& child : object.childs) {
                stream << *child;
            }

            stream << "\n};\n";
            break;

        case Object::Type::CLASS: {
            stream << "class " << object.name << " {\n\npublic:\n";
            if (object.vtable) {
                bool haveDestructor = false;
                for (const auto& symbol : object.vtable->content) {
                    if (std::strstr(symbol.second->demangled, object.name.c_str()) == nullptr) {
                        continue;
                    }

                    if (!symbol.second->isDestructor) {
                        if (Object::commentMangledSymbol) {
                            stream << "\t// @symbol " << symbol.second->mangled << '\n';
                        }

                        stream << "\tvirtual void "
                               << std::regex_replace(symbol.second->demangled, std::regex(".+?[:]{2}(.+)$"), "$1")
                               << ";\n";
                    } else if (!haveDestructor) {
                        if (Object::commentMangledSymbol) {
                            stream << "\t// @symbol " << symbol.second->mangled << '\n';
                        }

                        stream << "\tvirtual " << symbol.second->demangled + object.name.length() + 2 << ";\n";
                        haveDestructor = true;
                    }
                }
            }

            std::vector<Object*> copyList = object.childs;
            stream << '\n';
            for (std::vector<Object*>::const_iterator it = copyList.begin(); it != copyList.end();) {
                const auto& child = *it;
                if (child->type == Object::Type::METHOD && !child->symbol->isStatic) {
                    if (Object::commentMangledSymbol) {
                        stream << "\t// @symbol " << child->symbol->mangled << '\n';
                    }

                    stream << '\t' << *child;
                    it = copyList.erase(it);
                } else {
                    ++it;
                }
            }

            stream << '\n';
            for (std::vector<Object*>::const_iterator it = copyList.begin(); it != copyList.end();) {
                const auto& child = *it;
                if (child->type == Object::Type::METHOD && child->symbol->isStatic) {
                    if (Object::commentMangledSymbol) {
                        stream << "\t// @symbol " << child->symbol->mangled << '\n';
                    }

                    stream << "\tstatic " << *child;
                    it = copyList.erase(it);
                } else {
                    ++it;
                }
            }

            stream << '\n';
            for (std::vector<Object*>::const_iterator it = copyList.begin(); it != copyList.end();) {
                const auto& child = *it;
                if (child->type == Object::Type::FIELD) {
                    if (Object::commentMangledSymbol) {
                        stream << "\t// @symbol " << child->symbol->mangled << '\n';
                    }

                    stream << "\tstatic " << *child;
                    it = copyList.erase(it);
                } else {
                    ++it;
                }
            }

            for (const auto& child : copyList) {
                if (Object::commentMangledSymbol) {
                    stream << "\t// @symbol " << child->symbol->mangled << '\n';
                }

                stream << *child;
            }

            stream << "\n};\n";
            break;
        }

        case Object::Type::METHOD:
            if (!object.symbol->isConstructor && !object.symbol->isDestructor) {
                stream << "void ";
            }

            stream << object.name << ";\n";
            break;

        case Object::Type::FIELD:
            stream << "void* " << object.name << ";\n";
            break;
    }

    return stream;
}