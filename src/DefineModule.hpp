#pragma once

#include <map>

#include "IModule.hpp"

constexpr auto DEFINE_KEYWORD             = "define"; 
constexpr auto UNDEFINE_KEYWORD           = "undefine";
constexpr auto DEFINITION_VALUE_SEPERATOR = ':';


class DefineModule : public IModule {

    struct Definition {
        std::string Value;
        size_t DefinedOnIndex;
    };

    std::map<std::string, Definition> mDefines;   //Definition and what it's value along with when it was defined
    std::map<std::string, size_t>     mUndefines; //Definition and then index it was undefined

public:

    DefineModule() : IModule({DEFINE_KEYWORD, UNDEFINE_KEYWORD}, "DefineModule"){}

    virtual bool PushCommandList(const std::vector<MacroInformation>& Macros, const std::string& FileName) override;
    virtual bool Proccess(std::string& Data, const std::string& FileName) override;
    virtual bool ClearCommandList(const std::string& FileName) override;

};
