#include "define.h"

bool CDefines::set(const std::string &Name, const std::string &Value) {
    bool Overwritten = (DefineTable.find(Name) != DefineTable.end());
    DefineTable[Name] = Value;
    return Overwritten;
}

bool CDefines::unset(const std::string &Name) {
    bool Found = false;
    auto It = DefineTable.find(Name);
    if (It != DefineTable.end()) {
        DefineTable.erase(It);
        Found = true;
    }
    return Found;
}

optional<std::string> CDefines::get(const std::string &Name) {
    auto It = DefineTable.find(Name);
    if (It != DefineTable.end())
        return It->second;
    else
        return std::nullopt;
}

bool CDefines::setArray(const std::string &Name, const std::vector<std::string> &Arr) {
    bool Found = DefArrayTable.find(Name) != DefArrayTable.end();
    DefArrayTable[Name] = Arr;
    return Found;
}

optional<const std::vector<std::string>> CDefines::getArray(const std::string &Name) {
    auto It = DefArrayTable.find(Name);
    if (It != DefArrayTable.end() && !DefArrayTable[Name].empty())
        return It->second;
    else
        return std::nullopt;
}

bool CDefines::unsetArray(const std::string &Name) {
    bool Found = false;
    auto It = DefArrayTable.find(Name);
    if (It != DefArrayTable.end()) {
        DefArrayTable.erase(It);
        Found = true;
    }
    return Found;
}

void CDefines::clear() {
    DefineTable.clear();
    DefArrayTable.clear();
}

bool CDefines::defined(const std::string &Name) {
    if (DefineTable.count(Name) > 0)
        return true;
    else if (DefArrayTable.count(Name) > 0)
        return true;
    else
        return false;
}
