#pragma once

#include <map>

#include "IModule.hpp"

constexpr auto INCLUDE_MACRO       = "include";
constexpr auto FORCE_INCLUDE_MACRO = "force_include";

class IncludeModule : public IModule {

    struct Include {
        Include(const std::string& FileIn, size_t Index, bool Force) : File(FileIn), OnIndex(Index), ForceInclude(Force) {}
        std::string File;
        size_t OnIndex;
        bool ForceInclude;
    };


    std::map<std::string, std::vector<Include>>     mIncludes;       // File name, includes
    std::map<std::string, std::vector<std::string>> mIncludedFiles;  // Fine name, include name

public:

    IncludeModule() : IModule({INCLUDE_MACRO, FORCE_INCLUDE_MACRO}, "IncludeModule") {}

    virtual bool PushCommandList(const std::vector<MacroInformation>& Macros, const std::string& FileName) override;
    virtual bool Proccess(std::string& Data, const std::string& FileName) override;
    virtual bool ClearCommandList(const std::string& FileName) override;

};
