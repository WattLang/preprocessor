#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <sstream>
#include <algorithm>
#include <cstring>

#include "module.h"

constexpr auto MACRO_IDENTIFIER = "@";
constexpr auto MACRO_START      = "[";
constexpr auto MACRO_END        = "]";

using StringPair = std::pair<std::string, std::string>;

bool GetFiles(const std::vector<std::string> &Files, std::vector<StringPair>& DataVector);
bool Preprocess(StringPair& Data);


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

bool Preprocess(StringPair& Data) {

    std::vector<std::string> IncludedFiles;
    std::ifstream File;
    std::stringstream StringStream;

    std::string& Content = Data.second;
    for(size_t i = 0; i < Content.size();) {

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


        if(MacroType == "include") {
            if(std::find(IncludedFiles.begin(), IncludedFiles.end(), MacroValue) == IncludedFiles.end()) {  // Check to see if file is already included
                //TODO: ADD INCLUDE MACRO LOGIC

                std::string AbsoluteFileName;
                if(MacroValue[0] == '/') {
                    AbsoluteFileName = MacroValue;
                    AbsoluteFileName.erase(0, 1);    // Removes the '/'
                }
                else {
                    size_t Folder = Data.first.rfind('/');
                    AbsoluteFileName = Data.first.substr(0, Folder + 1).append(MacroValue);
                }
                File.open(AbsoluteFileName);
                if(!File.is_open()) {
                    ws::module::errorln("Failed to open file: \"", AbsoluteFileName, "\"!");
                    return false;
                }
                StringStream << File.rdbuf();
                Content.insert(i, StringStream.str());

                StringStream.str("");
                StringStream.clear();

                File.close();
                IncludedFiles.emplace_back(MacroValue);
            } else {
                ws::module::warnln("File: \"", MacroValue, "\" already included in \"", Data.first, "\"");
            }
        }
        else if(MacroType == "force_include") {
            if(std::find(IncludedFiles.begin(), IncludedFiles.end(), MacroValue) == IncludedFiles.end()) {  // Check to see if file is already included
                IncludedFiles.emplace_back(MacroValue);
            } else {
                ws::module::warnln("File: \"", MacroValue, "\" already included in \"", Data.first, "\" force including may cause problems and is not advised!");
            }
            std::string AbsoluteFileName;
            if(MacroValue[0] == '/') {
                AbsoluteFileName = MacroValue;
                AbsoluteFileName.erase(0, 1);    // Removes the '/'
            }
            else {
                size_t Folder = Data.first.rfind('/');
                AbsoluteFileName = Data.first.substr(0, Folder + 1).append(MacroValue);
            }
            File.open(AbsoluteFileName);
            if(!File.is_open()) {
                ws::module::errorln("Failed to open file: \"", AbsoluteFileName, "\"!");
                return false;
            }
            StringStream << File.rdbuf();
            Content.insert(i, StringStream.str());

            StringStream.str("");
            StringStream.clear();

            File.close();
        }
        else if(MacroType == "define") {
            
        }
        else if(MacroType == "undefine") {

        }
        else {
            ws::module::errorln("Macro: \"", MacroType, "\" does not exist!");
        }

        i = 0;  // Sets i to 0 to hopefully fix some bugs
    }


    return true;
}
