#pragma once

#include <ostream>
#include <string>
#include <vector>

struct MacroInformation {
    MacroInformation(const std::string& Type, const std::string& Data, size_t DefinedOn) : Type(Type), Data(Data), DefinedOnIndex(DefinedOn) {}
    std::string Type;
    std::string Data;
    size_t DefinedOnIndex;
};

class IMacro {
public:
    IMacro() {};
    virtual ~IMacro() {}

    virtual bool PushCommandList(const std::vector<MacroInformation>& Macros, std::ostream& ErrorOutputStream) = 0;
    virtual bool Proccess(std::string& Data, std::ostream& ErrorOutputStream) = 0;
};