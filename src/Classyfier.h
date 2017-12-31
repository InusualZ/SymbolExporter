#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "Object.h"
#include "Symbol32.h"
#include "VTable.h"

class Classyfier {
private:
    std::vector<Symbol32> symbols;
    std::vector<VTable> vtables;

    std::vector<std::string> blacklistClasses;

    std::vector<Object*> objects;

public:
    explicit Classyfier(const std::vector<Symbol32>& symbolRepo);

    explicit Classyfier(const std::vector<Symbol32>& symbolRepo, const std::vector<std::string>& blacklist);

    void setSymbols(const std::vector<Symbol32>& symbolRepo);

    const std::vector<Symbol32>& getSymbols() const;

    void setVTables(const std::vector<VTable>& vtableRepo);

    const std::vector<VTable>& getVTables() const;

    void setBlacklistClasses(const std::vector<std::string>& blacklist);

    const std::vector<std::string>& getBlacklist() const;

    const std::vector<Object*>& getObjects() const;

    uint32_t parse();

    bool parse(Symbol32& symbol);

private:
    std::vector<std::string> extractValues(const std::string& value) const;

};