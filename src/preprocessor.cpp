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

#define MACRO_IDENTIFIER "@"
#define MACRO_START "["
#define MACRO_END "]"

bool GetFiles(const std::vector<std::string> &files, std::vector<std::pair<std::string, std::string>>& DataVector, std::ostream& ErrorOutputStream);
bool Preprocess(std::pair<std::string, std::string>& Data, std::vector<std::unique_ptr<IModule>>& Modules, std::ostream& ErrorOutputStream);
void WriteFile(const std::vector<std::pair<std::string, std::string>>& DataVector,const std::string &OutputFile);


int main(int argc, char* argv[]) {
    if(argc < 2)
    {
        std::cerr << "No arguments!\nUsage: " << argv[0] << " -i <input file> [-i <more input files>] [-o <output file>]\n";
        return 1;
    }

    std::vector<std::pair<std::string, std::string>>  WotScriptData;
    std::vector<std::unique_ptr<IModule>>              MacroModules;

    MacroModules.emplace_back(std::make_unique<IncludeModule>());
    MacroModules.emplace_back(std::make_unique<DefineModule>());

    std::vector<std::string> InputFiles;
    std::string OutputFile;

    for(int i = 1; i < argc; ++i) {
        if(!strcmp(argv[i], "-i")) {
            if(++i < argc) {
                InputFiles.emplace_back(argv[i]);
            }
            else {
                std::cerr << "Missing argument after -i\n";
                return 1;
            }
        }

        else if(!strcmp(argv[i], "-o")) {
            if(++i < argc) {
                OutputFile = std::string(argv[i]);
            }
            else {
                std::cerr << "Missing argument after -o\n";
                return 1;
            }
        }

        else if(!strcmp(argv[i], "-h"))
        {
            std::cout << "Usage: " << argv[0] << " -i <input file> [-i <more input files>] [-o <output file>]\n";
            return 0;
        }
    }

    if(!GetFiles(InputFiles,WotScriptData, std::cerr)) {
        std::cerr << "Failed to get wotscript files!\n";
        return 1;
    }

    for(auto& Data : WotScriptData) {
        if(!Preprocess(Data, MacroModules, std::cerr)) {
            std::cerr << "Failed to preprocess: \"" << Data.first << "\"!\n";
            return 2;
        }
    }

    if(OutputFile.empty())
    {
        for(size_t i = 0; i < WotScriptData.size(); i++) {
            std::string& Contents = WotScriptData[i].second;
            for(size_t j = 0; j < Contents.size(); j++) {
                size_t LineBegin = j;
                j = Contents.find('\n', j);
                if(j == std::string::npos) {
                    //std::cout << Contents;
                    ws::pipe(Contents);
                    continue;
                }
                std::cout << Contents.substr(LineBegin, j - LineBegin) << std::endl;
            }
        }
    }
    else
    {
        WriteFile(WotScriptData, OutputFile);
    }
    return 0;
}


bool GetFiles(const std::vector<std::string> &Files, std::vector<std::pair<std::string, std::string>>& DataVector, std::ostream& ErrorOutputStream) {
    std::ifstream File;
    std::stringstream FileInput;
    for(auto &Path : Files) {
        File.open(Path);
        if(!File.is_open()) {
            ErrorOutputStream << "Could not open file: \"" << Path << "\"!\n";
            return false;
        }
        FileInput << File.rdbuf();
        DataVector.emplace_back(Path, FileInput.str());
    }
    return true;
}

bool Preprocess(std::pair<std::string, std::string>& Data, std::vector<std::unique_ptr<IModule>>& Modules, std::ostream& ErrorOutputStream) {

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
        if(!Modules[i]->PushCommandList(MacroCommandsList[i], Data.first, ErrorOutputStream)) {
            ErrorOutputStream << "Error pushing macro list to the \"" << Modules[i]->Name << "\" module\n";
            return false;
        }
        if(!Modules[i]->Proccess(Content, Data.first, ErrorOutputStream)) {
            ErrorOutputStream << "Error proccessing macro list for the \"" << Modules[i]->Name << "\" module\n";
            return false;
        }
    }

    size_t TestForMoreMacros = Content.find(MACRO_IDENTIFIER);
    if(TestForMoreMacros != std::string::npos) {
        Reprocess = true;
    }

    if(Reprocess) {
        return Preprocess(Data, Modules, ErrorOutputStream);
    }

    return true;
}

void WriteFile(const std::vector<std::pair<std::string, std::string>>& DataVector, const std::string& OutputFile)
{
    std::fstream Output(OutputFile, std::ios::out | std::ios::binary);
    for(auto &data : DataVector)
    {
        Output << data.second;
    }
}
