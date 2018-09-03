#include "IncludeModule.hpp"

#include <fstream>
#include <sstream>
#include <algorithm>

bool IncludeModule::PushCommandList(const std::vector<MacroInformation>& Macros, const std::string& FileName, std::ostream&) {

    for(auto& Macro : Macros) {
        if(Macro.Type == INCLUDE_MACRO) {
            Includes[FileName].emplace_back(Macro.Data, Macro.DefinedOnIndex + 1, false);
        }
        else if(Macro.Type == FORCE_INCLUDE_MACRO) {
            Includes[FileName].emplace_back(Macro.Data, Macro.DefinedOnIndex + 1, true);
        }
    }


    return true;

}
bool IncludeModule::Proccess(std::string& Data, const std::string& FileName, std::ostream& ErrorOutputStream) {


    std::ifstream File;
    std::stringstream StringStream;
    for(const auto& Include : Includes[FileName]) {

        if(std::find(IncludedFiles[FileName].begin(), IncludedFiles[FileName].end(), Include.File) == IncludedFiles[FileName].end() || Include.ForceInclude) {
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
                ErrorOutputStream << "Failed to open file: \"" << AbsoluteFileName << "\"!\n";
                return false;
            }
            StringStream << File.rdbuf();
            Data.insert(Include.OnIndex, StringStream.str());

            StringStream.str("");
            StringStream.clear();

            File.close();

            IncludedFiles[FileName].emplace_back(Include.File);
        }

    }

    Includes.clear();


    return true;

}

bool IncludeModule::ClearCommandList(const std::string&, std::ostream&) {
    Includes.clear();
    return true;
}
