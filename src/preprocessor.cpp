#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <sstream>
#include <algorithm>
#include <cstring>

#include "module.h"

#include "IModule.hpp"
#include "DefineModule.hpp"
#include "IncludeModule.hpp"

constexpr auto MACRO_IDENTIFIER = "@";
constexpr auto MACRO_START      = "[";
constexpr auto MACRO_END        = "]";

using StringPair = std::pair<std::string, std::string>;
using IModulePtr = std::unique_ptr<IModule>;

bool GetFiles(const std::vector<std::string> &Files, std::vector<StringPair>& DataVector);
bool Preprocess(StringPair& Data, std::vector<IModulePtr>& Modules);


int main(int argc, char* argv[]) {
    if(argc < 2)
    {
        std::cerr << "No arguments!" << std::endl;
        return 1;
    }

    std::vector<StringPair>  WotScriptData;
    std::vector<IModulePtr>  MacroModules;

    MacroModules.emplace_back(std::make_unique<IncludeModule>());
    MacroModules.emplace_back(std::make_unique<DefineModule>());

	std::vector<std::string> InputFiles(argv + 1, argv + argc);

    if(!GetFiles(InputFiles, WotScriptData)) {
        std::cerr << "Failed to get wotscript files!\n";
        return 1;
    }

    for(auto& Data : WotScriptData) {
        if(!Preprocess(Data, MacroModules)) {
            std::cerr << "Failed to preprocess: \"" << Data.first << "\"!\n";
            return 2;
        }
    }

	for(size_t i = 0; i < WotScriptData.size(); i++) {
		std::string& Contents = WotScriptData[i].second;
		for(size_t j = 0; j < Contents.size(); j++) {
			size_t LineBegin = j;
			j = Contents.find('\n', j);
			if(j == std::string::npos) {
				continue;
			}
			ws::module::pipeln(Contents.substr(LineBegin, j - LineBegin));
		}
	}
    return 0;
}


bool GetFiles(const std::vector<std::string> &Files, std::vector<StringPair>& DataVector) {
    std::ifstream File;
    std::stringstream FileInput;
    for(auto &Path : Files) {
        File.open(Path);
        if(!File.is_open()) {
            ws::module::errorln("Could not open file: \"", Path, "\"!");
            return false;
        }
        FileInput << File.rdbuf();
        DataVector.emplace_back(Path, FileInput.str());
    }
    return true;
}

bool Preprocess(StringPair& Data, std::vector<IModulePtr>& Modules) {

    bool Reprocess = false;

    std::unordered_map<std::string, std::vector<MacroInformation>> Macros;

    std::vector<std::vector<MacroInformation>> MacroCommandsList(Modules.size());

    std::string& Content = Data.second;
    for(size_t i = 0; i < Content.size();) {

        i = Content.find(MACRO_IDENTIFIER, i);
        if(i == std::string::npos) {
            continue;
        }
        i++;

        size_t MacroStart  = Content.find(MACRO_START, i);
        size_t MacroEnd    = Content.find(MACRO_END, MacroStart);
        MacroStart++;
        size_t MacroLength = MacroEnd - MacroStart;

        if(MacroStart == std::string::npos || MacroEnd == std::string::npos) {
            ws::module::errorln("Expected a macro value at index:", i, " in: \"", Data.first, "\"!");
            return false;
        }

        for(size_t j = 0; j < Modules.size(); j++) {
            if(std::find(
                Modules[j]->MacroCommands.begin(),
                Modules[j]->MacroCommands.end(),
                Content.substr(i, MacroStart - i - 1))
                != Modules[j]->MacroCommands.end()
            ) {

                MacroCommandsList[j].emplace_back(
                    Content.substr(i, MacroStart - i - 1),
                    Content.substr(MacroStart, MacroLength),
                    (i - 2)
                );

                Content.erase(i - 1, (MacroEnd - i) + 2);
                i = 0;
            }
        }
    }

    for(size_t i = 0; i < Modules.size(); i++) {
        if(!Modules[i]->PushCommandList(MacroCommandsList[i], Data.first)) {
            ws::module::errorln("Error pushing macro list to the \"", Modules[i]->Name, "\" module!");
            return false;
        }
        if(!Modules[i]->Proccess(Content, Data.first)) {
            ws::module::errorln("Error proccessing macro list for the \"", Modules[i]->Name, "\" module");
            return false;
        }
    }

    size_t TestForMoreMacros = Content.find(MACRO_IDENTIFIER);
    if(TestForMoreMacros != std::string::npos) {
        Reprocess = true;
    }

    if(Reprocess) {
        return Preprocess(Data, Modules);
    }
    else {
        for(auto& Module : Modules) {
            if(!Module->ClearCommandList(Data.first)) {
                ws::module::errorln("Error proccessing macro list for the \"", Module->Name, "\" module");
                return false;
            }
        }
    }

    return true;
}
