#pragma once

#include <string>
#include <vector>

#include "../Symbol.h"
#include "../VTable.h"

#include "Object.h"

class Classyfier {
private:
    std::vector<Symbol> symbols;
    std::vector<VTable> vtables;

    std::vector<std::string> blacklistClasses;

    std::vector<Object*> objects;

public:
    Classyfier(const std::vector<Symbol>& symbolRepo);
    Classyfier(const std::vector<Symbol>& symbolRepo, const std::vector<std::string>& blacklist);

    void setSymbols(const std::vector<Symbol>& symbolRepo);
    const std::vector<Symbol>& getSymbols() const;

    void setVTables(const std::vector<VTable>& vtableRepo);
    const std::vector<VTable>& getVTables() const;

    void setBlacklistClasses(const std::vector<std::string>& blacklist);
    const std::vector<std::string>& getBlacklist() const;

    const std::vector<Object*>& getObjects() const;

	uint32 parse();

    bool parse(const Symbol& symbol);

private:
    std::vector<std::string> extractValues(const std::string& value) const;

};