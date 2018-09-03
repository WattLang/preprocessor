#include "DefineModule.hpp"

bool DefineModule::PushCommandList(const std::vector<MacroInformation>& Macros, const std::string&) {

    for(auto& Macro : Macros) {

        if(Macro.Type == UNDEFINE_KEYWORD) {
            mUndefines[Macro.Data] = Macro.DefinedOnIndex;
        }
        else if(Macro.Type == DEFINE_KEYWORD) {
            size_t DefinitionSeperator = Macro.Data.find(DEFINITION_VALUE_SEPERATOR);

            mDefines[Macro.Data.substr(0, DefinitionSeperator)].Value
                = Macro.Data.substr(DefinitionSeperator + 1, Macro.Data.size() - DefinitionSeperator);
            mDefines[Macro.Data.substr(0, DefinitionSeperator)].DefinedOnIndex = Macro.DefinedOnIndex;
        }

    }

    return true;

}

bool DefineModule::Proccess(std::string& Data, const std::string&) {

    for(auto& DKV : mDefines) {
        for(size_t i = 0; i < Data.size();) {

            i = Data.find(DKV.first, i);
            if(i == std::string::npos){
                break;
            }

            if(mUndefines.count(DKV.first) > 0) {
                if(i < mUndefines[DKV.first]) {
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

bool DefineModule::ClearCommandList(const std::string&) {
    mDefines.clear();
    mUndefines.clear();
    return true;
}
