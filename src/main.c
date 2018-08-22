#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[]) {

    char WotScriptFileExtension[4] = ".ws";
    char** WotScriptFiles = NULL;
    int FileCount = 0;
    int FileCounter = 0;
    if(argc > 0) {
        for(int i = 0; i < argc; i++) {
			//printf("Input: %i: %s\n", i, argv[i]);
            if(strstr(argv[i], WotScriptFileExtension) != NULL) {
                FileCount++;
            }
        }
        if(FileCount > 0) {
            WotScriptFiles = malloc(sizeof(char*) * FileCount);
			if (WotScriptFiles == NULL) {
				printf("Error allocating WotScriptFiles!\n");
				return -1;
			}
            for(int i = 0; i < argc; i++) {
                if(strstr(argv[i], WotScriptFileExtension)) {
                    WotScriptFiles[FileCounter] = malloc(sizeof(char) * (strlen(argv[i]) + 1));
					if (WotScriptFiles[FileCounter] == NULL) {
						printf("Error allocating WotScriptFiles[%i]!\n", i);
						return -1;
					}
                    strcpy(WotScriptFiles[FileCounter], argv[i]);
                    FileCounter++;
                }
            }
        }
        else {
			printf("Error, no WotScript files!");
            return -1;
        }
    }
    else {
		printf("Error, No arguments!");
        return -1;
    }

	FILE* Inputfile = NULL;
	long FileSize = 0;

	char** WotScriptFileContents = malloc(sizeof(char*) * FileCount);
	if (WotScriptFileContents == NULL) {
		printf("Error allocating WotScriptFileContents!\n");
		return -1;
	}

	for (int i = 0; i < FileCount; i++) {
		Inputfile = fopen(WotScriptFiles[i], "r");
		if (Inputfile != NULL) {
			fseek(Inputfile, 0, SEEK_END);
			FileSize = ftell(Inputfile);
			//fseek(Inputfile, 0, SEEK_SET);
			rewind(Inputfile);
			WotScriptFileContents[i] = malloc(sizeof(char) * (FileSize + 1));
			WotScriptFileContents[i][FileSize] = '\0';
			if (WotScriptFileContents[i] == NULL) {
				printf("Error allocating WotScriptFileContents[%i]!\n", i);
			}
			fread(WotScriptFileContents[i], sizeof(char), FileSize, Inputfile);
			fclose(Inputfile);
		}
		else {
			printf("Error reading \"%s\"!\n", WotScriptFiles[i]);
			return -1;
		}
	}

	FILE* OutputFile = fopen("ScannerOutput.json", "w");
	fprintf(OutputFile, "{\n");
	for (int i = 0; i < FileCount; i++) {
		if (i == FileCount - 1) {
			fprintf(OutputFile, "\t\"%s\" : \"%s\"\n", WotScriptFiles[i], WotScriptFileContents[i]);
		}
		else {
			fprintf(OutputFile, "\t\"%s\" : \"%s\",\n", WotScriptFiles[i], WotScriptFileContents[i]);
		}
	}
	fprintf(OutputFile, "}\n");
	fclose(OutputFile);

    for(int i = 0; i < FileCount; i++) {
		free(WotScriptFiles[i]);
		free(WotScriptFileContents[i]);
    }
	free(WotScriptFiles);
	free(WotScriptFileContents);

    return 0;

}