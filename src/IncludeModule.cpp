#include "IncludeModule.hpp"

#include <fstream>
#include <sstream>
#include <algorithm>

bool IncludeModule::PushCommandList(const std::vector<MacroInformation>& Macros, std::ostream& ErrorOutputStream) {

    for(auto& Macro : Macros) {
        if(Macro.Type == INCLUDE_MACRO) {
            Includes.emplace_back(Macro.Data, Macro.DefinedOnIndex + 1, false);
        }
        else if(Macro.Type == FORCE_INCLUDE_MACRO) {
            Includes.emplace_back(Macro.Data, Macro.DefinedOnIndex + 1, true);
        }
    }


    return true;

}
bool IncludeModule::Proccess(std::string& Data, std::ostream& ErrorOutputStream) {


    std::ifstream File;
    std::stringstream StringStream;
    for(auto& Include : Includes) {

        if(std::find(IncludedFiles.begin(), IncludedFiles.end(), Include.File) == IncludedFiles.end() || Include.ForceInclude) {
            File.open(Include.File);
            if(!File.is_open()) {
                ErrorOutputStream << "Failed to open file: \"" << Include.File << "\"!\n";
                return false;
            }
            StringStream << File.rdbuf();
            Data.insert(Include.OnIndex, StringStream.str());

            StringStream.str("");
            StringStream.clear();
            
            File.close();

            IncludedFiles.emplace_back(Include.File);
        }

    }

    Includes.clear();


    return true;

}

bool IncludeModule::ClearCommandList(std::ostream& ErrorOutputStream) {
    Includes.clear();
    return true;
}