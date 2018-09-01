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

    virtual bool MacroValue(const std::string& Value) = 0;
    virtual bool Proccess(std::string& Data, std::ostream& ErrorOutputStream) = 0;
};

class Definer : IMacro {

    virtual bool MacroValue(const std::string& Value) override {
        return false;
    }
    virtual bool Proccess(std::string& Data, std::ostream& ErrorOutputStream) override {
        return false;
    }

};

std::unordered_map<std::string, std::shared_ptr<IMacro>> Macros;

bool GetFileNames(int argc, char** argv, std::vector<std::pair<std::string, std::string>>& Output, std::ostream& ErrorOutputStream);
bool Preprocess(std::vector<std::pair<std::string, std::string>>& Output, std::ostream& ErrorOutputStream);

int main(int argc, char* argv[]) {

    std::ostream& ErrorOutputStream = std::cerr;
    std::vector<std::pair<std::string, std::string>> WotScriptFiles;

    if(!GetFileNames(argc, argv, WotScriptFiles, ErrorOutputStream)) {
        ErrorOutputStream << "Fail to get and read files!\n";
        return 1;
    }

}

bool GetFileNames(int argc, char** argv, std::vector<std::pair<std::string, std::string>>& Output, std::ostream& ErrorOutputStream) {

    if(argc > 1) {
        Output.resize(argc - 1);
        std::ifstream File;
        for(size_t i = 0; i < (argc - 1); i++) {
            Output[i].first = argv[i];
            File.open(argv[i]);
            if(!File.is_open()) {
                ErrorOutputStream << "Error Opening \"" << argv[i] << "\"\n";
                return false; 
            }
            File >> Output[i].second;
            File.close();
        }
    }
    else {
        ErrorOutputStream << "There were no input files!\n";
        return false;
    }

    return
 true;
}

bool Preprocess(std::vector<std::pair<std::string, std::string>>& Output, std::ostream& ErrorOutputStream) {
    std::unordered_map<std::string, std::vector<MacroInformation>> Macros;

    std::string WorkingString;
    size_t LastIndex = 0;
    for(auto& Data : Output) {
        size_t MacroDefinition = Data.second.find(MACRO_IDENTIFIER);
        LastIndex += MacroDefinition;
        if(MacroDefinition == Data.second.size() - 1) {
            continue;
        }
        else {
            WorkingString = Data.second.substr(MacroDefinition, Data.second.size());
            size_t MacroStart = WorkingString.find(MACRO_START);
            size_t MacroEnd   = WorkingString.find(MACRO_END);
            size_t MacroLength = MacroEnd - MacroStart;

            Macros[WorkingString.substr(0, MacroStart)].emplace_back(
                WorkingString.substr(0, MacroStart), 
                WorkingString.substr(MacroStart, MacroLength),
                LastIndex
            );

        }
    }

    return true;
}