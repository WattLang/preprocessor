#pragma once

#include <map>

#include "IMacro.hpp"

#define INCLUDE_MACRO       "include"
#define FORCE_INCLUDE_MACRO "force_include"

class IncludeModule : public IMacro {

    struct Include {
        Include(const std::string& FileIn, size_t Index, bool Force) : File(FileIn), OnIndex(Index), ForceInclude(Force) {}
        std::string File;
        size_t OnIndex;
        bool ForceInclude;
    };

    std::vector<Include>     Includes;
    std::vector<std::string> IncludedFiles;

public:

    IncludeModule() : IMacro({INCLUDE_MACRO, FORCE_INCLUDE_MACRO}, "IncludeModule") {}

    virtual bool PushCommandList(const std::vector<MacroInformation>& Macros, std::ostream& ErrorOutputStream) override;
    virtual bool Proccess(std::string& Data, std::ostream& ErrorOutputStream) override;
    virtual bool ClearCommandList(std::ostream& ErrorOutputStream) override;

};
