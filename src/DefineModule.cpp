#include "DefineModule.hpp"

bool DefineModule::PushCommandList(const std::vector<MacroInformation>& Macros, std::ostream& ErrorOutputStream) {

    if(Macros[0].Type == "define") {
        for(auto& Macro : Macros) {

            size_t DefinitionSeperator = Macro.Data.find(':');

            Defines[Macro.Data.substr(0, DefinitionSeperator)].Value
                = Macro.Data.substr(DefinitionSeperator + 1, Macro.Data.size() - DefinitionSeperator);
            Defines[Macro.Data.substr(0, DefinitionSeperator)].DefinedOnIndex = Macro.DefinedOnIndex;

        }
    }
    else if(Macros[0].Type == "undefine") {
        for(auto& Macro : Macros) {

            Undefines[Macro.Data] = Macro.DefinedOnIndex;

        }
    }

    return true;

}

bool DefineModule::Proccess(std::string& Data, std::ostream& ErrorOutputStream) {

    for(auto& DKV : Defines) {
        for(size_t i = 0; i < Data.size();) {

            i = Data.find(DKV.first, i);
            if(i == std::string::npos){
                break;
            }

            if(Undefines.count(DKV.first) > 0) {
                if(i < Undefines[DKV.first]) {
                    Data.replace(i, DKV.first.size(), DKV.second.Value);
                }
                else {
                    break;
                }
            } else {
                Data.replace(i, DKV.first.size(), DKV.second.Value);
            }

        }
    }

    return true;

}