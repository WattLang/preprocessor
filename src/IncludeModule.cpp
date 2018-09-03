#include "IncludeModule.hpp"

#include <fstream>
#include <sstream>
#include <algorithm>

bool IncludeModule::PushCommandList(const std::vector<MacroInformation>& Macros, const std::string& FileName) {
    for(auto& Macro : Macros) {
        if(Macro.Type == INCLUDE_MACRO) {
            mIncludes[FileName].emplace_back(Macro.Data, Macro.DefinedOnIndex + 1, false);
        }
        else if(Macro.Type == FORCE_INCLUDE_MACRO) {
            mIncludes[FileName].emplace_back(Macro.Data, Macro.DefinedOnIndex + 1, true);
        }
    }
    return true;
}

bool IncludeModule::Proccess(std::string& Data, const std::string& FileName) {
    std::ifstream File;
    std::stringstream StringStream;
    for(const auto& Include : mIncludes[FileName]) {
        if(std::find(mIncludedFiles[FileName].begin(), mIncludedFiles[FileName].end(), Include.File) == mIncludedFiles[FileName].end() || Include.ForceInclude) {
            std::string AbsoluteFileName;
            if(Include.File[0] == '/') {
                AbsoluteFileName = Include.File;
                AbsoluteFileName.erase(0, 1);    // Removes the '/'
            }
            else {
                size_t Folder = FileName.rfind('/');
                AbsoluteFileName = FileName.substr(0, Folder + 1).append(Include.File);
            }
            File.open(AbsoluteFileName);
            if(!File.is_open()) {
                ws::errorln("Failed to open file: \"", AbsoluteFileName, "\"!");
                return false;
            }
            StringStream << File.rdbuf();
            Data.insert(Include.OnIndex, StringStream.str());

            StringStream.str("");
            StringStream.clear();

            File.close();

            mIncludedFiles[FileName].emplace_back(Include.File);
        }
    }
    return true;
}

bool IncludeModule::ClearCommandList(const std::string&) {
    mIncludes.clear();
    mIncludedFiles.clear();
    return true;
}
