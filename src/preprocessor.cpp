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

std::vector<std::pair<std::string, std::string>>           WotScriptFiles;  // Name then contents
std::vector<std::unique_ptr<IModule>>                       MacroModules2;


bool GetFiles(const std::vector<std::string> &Files, std::ostream& ErrorOutputStream);
bool Preprocess(std::ostream& ErrorOutputStream);
void WriteFile(const std::string &OutputFile);


int main(int argc, char* argv[]) {
    if(argc < 2)
    {
        std::cerr << "No arguments!\nUsage: " << argv[0] << " -i <input file> [-i <more input files>] [-o <output file>]\n";
        return 1;
    }
    MacroModules2.emplace_back(std::make_unique<IncludeModule>());
    MacroModules2.emplace_back(std::make_unique<DefineModule>());

    std::vector<std::string> InputFiles;
    std::string OutputFile;

    for(int i = 1; i < argc; ++i) {
        if(!strcmp(argv[i], "-i")) {
            if(++i < argc) {
                InputFiles.push_back(std::string(argv[i]));
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

    if(!GetFiles(InputFiles, std::cerr)) {
        std::cerr << "Failed to get wotscript files!\n";
        return 1;
    }

    if(!Preprocess(std::cerr)) {
        std::cerr << "Failed to preprocess!\n";
        return 2;
    }

    if(OutputFile.empty())
    {
        for(size_t i = 0; i < WotScriptFiles.size(); i++) {
            std::string& Contents = WotScriptFiles[i].second;
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
        WriteFile(OutputFile);
    }
    return 0;
}


bool GetFiles(const std::vector<std::string> &files, std::ostream& ErrorOutputStream) {
    std::ifstream File;
    std::stringstream FileInput;
    for(auto &path : files) {
        File.open(path);
        if(!File.is_open()) {
            ErrorOutputStream << "Could not open file: \"" << path << "\"!\n";
            return false;
        }
        FileInput << File.rdbuf();
        WotScriptFiles.emplace_back(path, FileInput.str());
    }
    return true;
}

bool Preprocess(std::ostream& ErrorOutputStream) {

    bool Reprocess = false;
    for(size_t j = 0; j < WotScriptFiles.size(); j++) {

        std::unordered_map<std::string, std::vector<MacroInformation>> Macros;

        std::vector<std::vector<MacroInformation>> MacroCommandsList(MacroModules2.size());

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

            for(size_t k = 0; k < MacroModules2.size(); k++) {
                if(std::find(
                    MacroModules2[k]->MacroCommands.begin(), 
                    MacroModules2[k]->MacroCommands.end(), 
                    Content.substr(i, MacroStart - i - 1))
                    != MacroModules2[k]->MacroCommands.end()
                ) {

                    MacroCommandsList[k].emplace_back(
                        Content.substr(i, MacroStart - i - 1),
                        Content.substr(MacroStart, MacroLength),
                        (i - 2)
                    );

                    Content.erase(i - 1, (MacroEnd - i) + 2);
                    i = 0;
                }
            }
        }

        for(size_t i = 0; i < MacroModules2.size(); i++) {
            if(!MacroModules2[i]->PushCommandList(MacroCommandsList[i], WotScriptFiles[j].first, ErrorOutputStream)) {
                ErrorOutputStream << "Error pushing macro list to the \"" << MacroModules2[i]->Name << "\" module\n";
                return false;
            }
            if(!MacroModules2[i]->Proccess(Content, WotScriptFiles[j].first, ErrorOutputStream)) {
                ErrorOutputStream << "Error proccessing macro list for the \"" << MacroModules2[i]->Name << "\" module\n";
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

void WriteFile(const std::string& OutputFile)
{
    std::fstream Output(OutputFile, std::ios::out | std::ios::binary);
    for(int i = 0; i < WotScriptFiles.size(); ++i)
    {
        Output << WotScriptFiles[i].second;
    }
}
