#include "DefineModule.hpp"

bool DefineModule::PushCommandList(const std::vector<MacroInformation>& Macros, std::ostream& ErrorOutputStream) {

    for(auto& Macro : Macros) {

        if(Macro.Type == UNDEFINE_KEYWORD) {
            Undefines[Macro.Data] = Macro.DefinedOnIndex;
        }
        else if(Macro.Type == DEFINE_KEYWORD) {
            size_t DefinitionSeperator = Macro.Data.find(DEFINITION_VALUE_SEPERATOR);

            Defines[Macro.Data.substr(0, DefinitionSeperator)].Value
                = Macro.Data.substr(DefinitionSeperator + 1, Macro.Data.size() - DefinitionSeperator);
            Defines[Macro.Data.substr(0, DefinitionSeperator)].DefinedOnIndex = Macro.DefinedOnIndex;
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
                    //i += DKV.second.Value.size() - DKV.first.size();
                }
                else {
                    break;
                }
            } else {
                Data.replace(i, DKV.first.size(), DKV.second.Value);
                //i += DKV.second.Value.size() - DKV.first.size();
            }

        }
    }

    return true;

}

bool DefineModule::ClearCommandList(std::ostream& ErrorOutputStream) {
    Defines.clear();
    Undefines.clear();
    return true;
}