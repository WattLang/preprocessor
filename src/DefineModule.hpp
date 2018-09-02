#pragma once

#include <map>

#include "IMacro.hpp"

class DefineModule : public IMacro {

    struct Definition {
        std::string Value;
        size_t DefinedOnIndex;
    };

    std::map<std::string, Definition> Defines;   //Definition and what it's value along with when it was defined
    std::map<std::string, size_t>     Undefines; //Definition and then index it was undefined

public:

    virtual bool PushCommandList(const std::vector<MacroInformation>& Macros, std::ostream& ErrorOutputStream) override;
    virtual bool Proccess(std::string& Data, std::ostream& ErrorOutputStream) override;

};
