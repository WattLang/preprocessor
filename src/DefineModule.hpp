#pragma once

#include <map>

#include "IMacro.hpp"

#define DEFINE_KEYWORD "define"
#define UNDEFINE_KEYWORD "undefine"
#define DEFINITION_VALUE_SEPERATOR ':'

class DefineModule : public IMacro {

    struct Definition {
        std::string Value;
        size_t DefinedOnIndex;
    };

    std::map<std::string, Definition> Defines;   //Definition and what it's value along with when it was defined
    std::map<std::string, size_t>     Undefines; //Definition and then index it was undefined

public:

    DefineModule() : IMacro({DEFINE_KEYWORD, UNDEFINE_KEYWORD}, "DefineModule"){}

    virtual bool PushCommandList(const std::vector<MacroInformation>& Macros, std::ostream& ErrorOutputStream) override;
    virtual bool Proccess(std::string& Data, std::ostream& ErrorOutputStream) override;
    virtual bool ClearCommandList(std::ostream& ErrorOutputStream) override;

};
