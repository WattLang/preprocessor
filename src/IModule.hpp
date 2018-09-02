#pragma once

#include <ostream>
#include <string>
#include <vector>

struct MacroInformation {
    MacroInformation(const std::string& Type, const std::string& Data, size_t DefinedOn)
     : Type(Type), Data(Data), DefinedOnIndex(DefinedOn) {}
    std::string Type;
    std::string Data;
    size_t DefinedOnIndex;
};

class IModule {
public:
    IModule(const std::vector<std::string>& MacroCommandsIn, const std::string& NameIn)
     : MacroCommands(MacroCommandsIn), Name(NameIn)
    {};
    virtual ~IModule() {}

    const std::vector<std::string> MacroCommands;
    const std::string              Name;

    virtual bool PushCommandList(const std::vector<MacroInformation>& Macros, const std::string& FileName, std::ostream& ErrorOutputStream) = 0;
    virtual bool Proccess(std::string& Data, const std::string& FileName, std::ostream& ErrorOutputStream) = 0;
    virtual bool ClearCommandList(const std::string& FileName, std::ostream& ErrorOutputStream) = 0;
};