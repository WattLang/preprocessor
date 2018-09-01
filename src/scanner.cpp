#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

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

int main(int argc, char* argv[]) {

    if(!GetFiles(argc, argv, std::cerr)) {
        std::cerr << "Failed to get wotscript files!\n";
        return 1; 
    }

    return 0;
}

bool GetFiles(int argc, char** argv, std::ostream& ErrorOutputStream) {

    if(argc > 1) {
        WotScriptFiles.resize(argc - 1);
        std::ifstream File;
        for(size_t i = 0; i < (argc - 1); i++) {
            WotScriptFiles[i].first = argv[i];
            File.open(argv[i]);
            if(!File.is_open()) {
                ErrorOutputStream << "Error Opening \"" << argv[i] << "\"\n";
                return false; 
            }
            File >> WotScriptFiles[i].second;
            File.close();
        }
    }
    else {
        ErrorOutputStream << "There were no input files!\n";
        return false;
    }

    return true;
}

bool Preprocess() {

    for(auto& File : WotScriptFiles) {

        std::unordered_map<std::string, std::vector<MacroInformation>> Macros;
        std::string& Content = File.second;
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

        }

    }

    return false;
}