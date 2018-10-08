#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <sstream>
#include <algorithm>
#include <tuple>

#include "module.h"

constexpr auto MACRO_IDENTIFIER = "@";
constexpr auto MACRO_START      = "[";
constexpr auto MACRO_END        = "]";

constexpr auto INCLUDE_MACRO       = "include";
constexpr auto FORCE_INCLUDE_MACRO = "force_include";
constexpr auto DEFINE_MACRO        = "define";
constexpr auto UNDEFINE_MACRO      = "undefine";

using StringPair = std::pair<std::string, std::string>;

bool GetFiles(const std::vector<std::string> &Files, std::vector<StringPair>& DataVector);
bool Preprocess(StringPair& Data);
bool IncludeFile(const std::string& Includer, const std::string& Filepath, std::string& Data, size_t Index); 


int main(int argc, char* argv[]) {
    if(argc < 2)
    {
        std::cerr << "No arguments!" << std::endl;
        return 1;
    }

    std::vector<StringPair>  WotScriptData;

	std::vector<std::string> InputFiles(argv + 1, argv + argc);

    if(!GetFiles(InputFiles, WotScriptData)) {
        std::cerr << "Failed to get wotscript files!\n";
        return 1;
    }

    for(auto& Data : WotScriptData) {
        if(!Preprocess(Data)) {
            std::cerr << "Failed to preprocess: \"" << Data.first << "\"!\n";
            return 2;
        }
    }

	for(size_t i = 0; i < WotScriptData.size(); i++) {
		std::string& Contents = WotScriptData[i].second;
		for(size_t j = 0; j < Contents.size();) {
			size_t LineBegin = j;
			j = Contents.find('\n', j);
			if(j == std::string::npos) {
				continue;
			}
			ws::module::pipeln(Contents.substr(LineBegin, j - LineBegin));
			j++;
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

bool Preprocess(StringPair& Data) {

    std::vector<std::string> IncludedFiles;
    std::vector<std::tuple<std::string, std::string>> Defines; // Replaces, Value
    std::ifstream File;
    std::stringstream StringStream;

    std::string& Content = Data.second;
    for(size_t i = 0; i < Content.size();) { // Scans for macros and completes include macros

        i = Content.find(MACRO_IDENTIFIER, i);                     //Find where a macro is delcared using @
        if(i == std::string::npos) {
            continue;
        }
        i++;

        size_t MacroStart  = Content.find(MACRO_START, i);         //Find the [ right after the declaration
        size_t MacroEnd    = Content.find(MACRO_END, MacroStart);  //Find the ] right after the [
        MacroStart++;
        size_t MacroLength = MacroEnd - MacroStart;

        if(MacroStart == std::string::npos || MacroEnd == std::string::npos) { //Check to see if macro declaration has a value
            ws::module::errorln("Expected a macro value at index:", i, " in: \"", Data.first, "\"!");
            return false;
        }

        std::string MacroType  = Content.substr(i, MacroStart - i - 1);          // The type of macro it is, include, define, etc.
        std::string MacroValue = Content.substr(MacroStart, MacroLength);        // The value of the macro, what's in between the []
        std::fill(Content.begin() + i - 1, Content.begin() + MacroEnd + 1, ' '); // Repace the macro declaration with spaces


        if(MacroType == INCLUDE_MACRO) {
            if(std::find(IncludedFiles.begin(), IncludedFiles.end(), MacroValue) == IncludedFiles.end()) {  // Check to see if file is already included
                IncludeFile(Data.first, MacroValue, Content, i);
                IncludedFiles.emplace_back(MacroValue);
            } else {
                ws::module::warnln("File: \"", MacroValue, "\" already included in \"", Data.first, "\"");
            }
        }
        else if(MacroType == FORCE_INCLUDE_MACRO) {
            if(std::find(IncludedFiles.begin(), IncludedFiles.end(), MacroValue) == IncludedFiles.end()) {  // Check to see if file is already included
                IncludedFiles.emplace_back(MacroValue);
            } else {
                ws::module::warnln("File: \"", MacroValue, "\" already included in \"", Data.first, "\" force including may cause problems and is not advised!");
            }
            IncludeFile(Data.first, MacroValue, Content, i);
        }
        else if(MacroType == DEFINE_MACRO) {

            size_t SeperatorIndex = MacroValue.find(':');
            if(SeperatorIndex == std::string::npos) {
                ws::module::errorln("Expected \':\' in define definition! ", MacroType, " : ", MacroValue);
                return false;
            }

            std::string DefineName  = MacroValue.substr(0, SeperatorIndex);
            std::string DefineValue = MacroValue.substr(   SeperatorIndex + 1);

            for(auto& Define : Defines) { // Check to see if already defined
                if(std::get<0>(Define) == DefineName) {
                    ws::module::errorln("Define: \"", DefineName, "\" already defined!");
                    return false;
                }
            }

            Defines.emplace_back(DefineName, DefineValue);
        }
        else if(MacroType == UNDEFINE_MACRO) {
            for(size_t i = 0; i < Defines.size(); i++) {
                //ws::module::noticeln(std::get<0>(Defines[i]), "   ", MacroValue);
                if(std::get<0>(Defines[i]) == MacroValue) {
                    Defines.erase(Defines.begin() + i);
                }
            }
        }
        else {
            ws::module::errorln("Macro: \"", MacroType, "\" does not exist!");
        }

        for(auto& Define : Defines) { // Scan for defines and replace them before the next macro
            for(;;) {
                size_t NextMacro       = Content.find(MACRO_IDENTIFIER);
                size_t NextDefineIndex = Content.find(std::string( ' ' + std::get<0>(Define) + ' '), i);
                if(NextDefineIndex == std::string::npos || NextDefineIndex >= NextMacro) { // Tries to find a define that is before the next macro
                    NextDefineIndex = Content.find(std::string( ' ' + std::get<0>(Define) + '\n'), i);
                }
                if(NextDefineIndex == std::string::npos || NextDefineIndex >= NextMacro) { // Tries to find a define that is before the next macro
                    NextDefineIndex = Content.find(std::string( ' ' + std::get<0>(Define) + '\r'), i);
                }
                if(NextDefineIndex < NextMacro && std::string::npos) {
                    Content.replace(Content.begin() + NextDefineIndex + 1, Content.begin() + NextDefineIndex + std::get<0>(Define).size() + 1, std::get<1>(Define));
                }
                else {
                    break;
                }
            }
        }

    }


    return true;
}

bool IncludeFile(const std::string& Includer, const std::string& Filepath, std::string& Data, size_t Index) {
    std::ifstream File;
    std::stringstream StringStream;
    std::string AbsoluteFileName;
    if(Filepath[0] == '/') {
        AbsoluteFileName = Filepath;
        AbsoluteFileName.erase(0, 1);    // Removes the '/'
    }
    else {
        size_t Folder    = Includer.rfind('/');
        AbsoluteFileName = Includer.substr(0, Folder + 1).append(Filepath);
    }
    File.open(AbsoluteFileName);
    if(!File.is_open()) {
        ws::module::errorln("Failed to open file: \"", AbsoluteFileName, "\"!");
        return false;
    }
    StringStream << File.rdbuf();
    Data.insert(Index - 1, StringStream.str());

    StringStream.str("");
    StringStream.clear();

    File.close();

    return true;
}
