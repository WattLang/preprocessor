#pragma once

#include "IMacro.hpp"

class DefineModule : public IMacro {

public:

    virtual bool PushCommandList(const std::vector<MacroInformation>& Macros, std::ostream& ErrorOutputStream) override;
    virtual bool Proccess(std::string& Data, std::ostream& ErrorOutputStream) override;

};
