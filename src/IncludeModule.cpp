#include "IncludeModule.hpp"

#include <fstream>
#include <sstream>

bool IncludeModule::PushCommandList(const std::vector<MacroInformation>& Macros, std::ostream& ErrorOutputStream) {

    for(auto& Macro : Macros) {
        Includes.emplace_back(Macro.Data, Macro.DefinedOnIndex + 1);
    }


    return true;

}
bool IncludeModule::Proccess(std::string& Data, std::ostream& ErrorOutputStream) {


    std::ifstream File;
    std::stringstream StringStream;
    for(auto& Include : Includes) {

        File.open(Include.first);
        if(!File.is_open()) {
            ErrorOutputStream << "Failed to open file: \"" << Include.first << "\"!\n";
            return false;
        }
        StringStream << File.rdbuf();
        Data.insert(Include.second, StringStream.str());

        StringStream.str("");
        StringStream.clear();
        
        File.close();

    }

    Includes.clear();


    return true;

}

bool IncludeModule::ClearCommandList(std::ostream& ErrorOutputStream) {
    Includes.clear();
    return true;
}