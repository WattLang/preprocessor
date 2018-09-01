#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <cstring>

#define MACRO_IDENTIFIER "@"
#define MACRO_START "["
#define MACRO_END "]"

struct MacroInformation {
    MacroInformation(const std::string& Type, const std::string& Data, size_t DefinedOn) : Type(Type), Data(Data), DefinedOnIndex(DefinedOn) {}
    std::string Type;
    std::string Data;
    size_t DefinedOnIndex;
};

class IMacro {
public:
    IMacro() {};
    virtual ~IMacro() {}

    virtual bool Proccess(const std::vector<MacroInformation>& Macros, std::string& Data, std::ostream& ErrorOutputStream) = 0;
};

std::vector<std::pair<std::string, std::string>>           WotScriptFiles;  // Name then contents
std::vector<std::string>                                   ModuleNames;     // Names of the modules
std::unordered_map<std::string, std::shared_ptr<IMacro>>   MacroModules;    // The key is the macro, and then a pointer to the module

bool GetFiles(int argc, char** argv, std::ostream& ErrorOutputStream);
bool Preprocess();
std::vector<char> ReadFile(const std::string& Filename);

int main(int argc, char* argv[]) {

    if(!GetFiles(argc, argv, std::cerr)) {
        std::cerr << "Failed to get wotscript files!\n";
        return 1; 
    }


    for(size_t i = 0; i < WotScriptFiles.size(); i++) {
        std::cout << WotScriptFiles[i].first << "      :      " << WotScriptFiles[i].second << std::endl;
    }

    if(!Preprocess()) {
        std::cerr << "Failed to preprocess!\n";
        return 2;
    }

    return 0;
}

bool GetFiles(int argc, char** argv, std::ostream& ErrorOutputStream) {

    if(argc > 1) {
        WotScriptFiles.resize(argc - 1);
        for(size_t i = 1; i < argc; i++) {
            WotScriptFiles[i - 1].first = argv[i];
            std::vector<char> Data = ReadFile(WotScriptFiles[i - 1].first);
            WotScriptFiles[i - 1].second = std::string(Data.begin(), Data.end());
            std::cout << WotScriptFiles[i - 1].first << "      :      " << WotScriptFiles[i - 1].second << std::endl;
        }
        return true;
    }
    else {
        ErrorOutputStream << "There were no input files!\n";
        return false;
    }

    return true;
}

bool Preprocess() {

    for(size_t j = 0; j < WotScriptFiles.size(); j++) {

        std::unordered_map<std::string, std::vector<MacroInformation>> Macros;
        std::string& Content = WotScriptFiles[j].second;
        for(size_t i = 0; i < Content.size();) {

            i = Content.find(MACRO_IDENTIFIER, i);
            i++;

            if(i == std::string::npos) {
                continue;
            }

            size_t MacroStart  = Content.find(MACRO_START, i);
            size_t MacroEnd    = Content.find(MACRO_END, MacroStart);
            size_t MacroLength = MacroEnd - MacroStart;

            Macros[Content.substr(i, MacroStart - i)].emplace_back(

                Content.substr(i, MacroStart - i),
                Content.substr(MacroStart, MacroLength),
                i

            );

            std::cout << Content.substr(i, MacroStart - i) << "    " << Content.substr(MacroStart, MacroLength) << "         " << i << '\n';

        }

    }

    return true;
}

std::vector<char> ReadFile(const std::string& Filename) {
    std::ifstream file(Filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}
