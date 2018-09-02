#pragma once

#include <map>

#include "IMacro.hpp"

class IncludeModule : public IMacro {

    std::vector<std::pair<std::string, size_t>> Includes; //File name and then location in string

public:

    IncludeModule() : IMacro({"include"}, "IncludeModule") {}

    virtual bool PushCommandList(const std::vector<MacroInformation>& Macros, std::ostream& ErrorOutputStream) override;
    virtual bool Proccess(std::string& Data, std::ostream& ErrorOutputStream) override;
    virtual bool ClearCommandList(std::ostream& ErrorOutputStream) override;

};
