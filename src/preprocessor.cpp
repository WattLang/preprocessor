#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <sstream>

#include "IMacro.hpp"
#include "DefineModule.hpp"
#include "IncludeModule.hpp"

#define MACRO_IDENTIFIER "@"
#define MACRO_START "["
#define MACRO_END "]"

std::vector<std::pair<std::string, std::string>>           WotScriptFiles;  // Name then contents
std::unordered_map<std::string, std::shared_ptr<IMacro>>   MacroModules;    // The key is the macro, and then a pointer to the module


bool GetFiles(int argc, char** argv, std::ostream& ErrorOutputStream);
bool Preprocess(std::ostream& ErrorOutputStream);






int main(int argc, char* argv[]) {

    MacroModules["include"]  = std::make_shared<IncludeModule>();
    MacroModules["define"]   = std::make_shared<DefineModule>();
    MacroModules["undefine"] = MacroModules["define"];
    

    if(!GetFiles(argc, argv, std::cerr)) {
        std::cerr << "Failed to get wotscript files!\n";
        return 1; 
    }

    if(!Preprocess(std::cerr)) {
        std::cerr << "Failed to preprocess!\n";
        return 2;
    }

    /*for(size_t i = 0; i < WotScriptFiles.size(); i++) {
        std::cout << WotScriptFiles[i].second << std::endl;
    }*/

    for(size_t i = 0; i < WotScriptFiles.size(); i++) {
        std::string& Contents = WotScriptFiles[i].second;
        for(size_t j = 0; j < Contents.size(); j++) {

            size_t LineBegin = j;
            j = Contents.find('\n', j);
            if(j == std::string::npos) {
                std::cout << Contents;
                continue;
            }

            std::cout << Contents.substr(LineBegin, j - LineBegin) << std::endl;

        }
    }

    return 0;
}





bool GetFiles(int argc, char** argv, std::ostream& ErrorOutputStream) {

    if(argc > 1) {
        WotScriptFiles.reserve(argc - 1);
        std::ifstream File;
        std::stringstream FileInput;
        for(size_t i = 1; i < argc; i++) {

            File.open(argv[i]);
            if(!File.is_open()) {
                ErrorOutputStream << "Could not open file: \"" << argv[i] << "\"!\n";
                return false;
            }

            FileInput << File.rdbuf();

            WotScriptFiles.emplace_back(argv[i], FileInput.str());

            File.close();
            FileInput.str("");
            FileInput.clear();
        }
        return true;
    }
    else {
        ErrorOutputStream << "There were no input files!\n";
        return false;
    }

    return true;
}

bool Preprocess(std::ostream& ErrorOutputStream) {

    bool Reprocess = false;
    for(size_t j = 0; j < WotScriptFiles.size(); j++) {

        std::unordered_map<std::string, std::vector<MacroInformation>> Macros;
        std::string& Content = WotScriptFiles[j].second;
        for(size_t i = 0; i < Content.size();) {

            i = Content.find(MACRO_IDENTIFIER, i);
            if(i == std::string::npos) {
                continue;
            }
            i++;
            if(i == std::string::npos) {
                continue;
            }

            size_t MacroStart  = Content.find(MACRO_START, i);
            size_t MacroEnd    = Content.find(MACRO_END, MacroStart);
            MacroStart++;
            size_t MacroLength = MacroEnd - MacroStart;

            if(MacroStart == std::string::npos || MacroEnd == std::string::npos) {
                ErrorOutputStream << "Expected a macro value!\n";
                return false;
            }

            Macros[Content.substr(i, MacroStart - i - 1)].emplace_back(

                Content.substr(i, MacroStart - i - 1),
                Content.substr(MacroStart, MacroLength),
                (i - 1)

            );

            Content.erase(i - 1, (MacroEnd - i) + 2);

            i = 0;

        }

        for(auto& MacrosKV: Macros) {
            if(MacroModules.count(MacrosKV.first) < 1) {
                ErrorOutputStream << "There isn't a preprocessing module for \"" << MacrosKV.first << "\" macros!\n";
                continue;
            }
            else {
                if(!MacroModules[MacrosKV.first]->PushCommandList(MacrosKV.second, ErrorOutputStream)){
                    ErrorOutputStream << "Error pushing macro list to the \"" << MacrosKV.first << "\" module\n";
                    return false;
                }
            }
        }

        for(auto& MacroModulesKV: MacroModules) {
            if(!MacroModulesKV.second->Proccess(Content, ErrorOutputStream)){
                ErrorOutputStream << "Error processing for the \""<< MacroModulesKV.first<< "\" module!\n";
                return false;
            }
        }

        size_t TestForMoreMacros = Content.find(MACRO_IDENTIFIER);
        if(TestForMoreMacros != std::string::npos) {
            Reprocess = true;
        }

    }

    if(Reprocess) {
        return Preprocess(ErrorOutputStream);
    }

    return true;
}
