#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define OUTPUT_JSON 0

int main(int argc, char* argv[]) {

    char WotScriptFileExtension[4] = ".ws";
    char** WotScriptFiles = NULL;
    int FileCount = 0;
    int FileCounter = 0;
    if(argc > 0) {
        for(int i = 0; i < argc; i++) {
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
			memset(WotScriptFileContents[i], 0, FileSize + 1);
			WotScriptFileContents[i][FileSize] = '\0';
			if (WotScriptFileContents[i] == NULL) {
				printf("Error allocating WotScriptFileContents[%i]!\n", i);
			}
			fread(WotScriptFileContents[i], sizeof(char), FileSize, Inputfile);

#if OUTPUT_JSON

			size_t PassedQuotes = 0;
			for (size_t j = 0; j < FileSize; j++) {
				if (WotScriptFileContents[i][j] == '\"') {
					PassedQuotes++;
				}
			}
			if (PassedQuotes > 0) {
				long NewFileSize = FileSize + PassedQuotes;
				WotScriptFileContents[i] = realloc(WotScriptFileContents[i], sizeof(char) * (NewFileSize + 1));
				WotScriptFileContents[i][NewFileSize] = '\0';
				if (WotScriptFileContents[i] == NULL) {
					printf("Error reallocating WotScriptFileContents[%i]!\n", i);
				}
				size_t QuotesAdded = 0;
				for (size_t j = 0; j < FileSize; j++) {
					if (WotScriptFileContents[i][j] == '\"') {
						if (memmove(WotScriptFileContents[i] + j + sizeof(char), WotScriptFileContents[i] + j, NewFileSize - (j + sizeof(char))) == NULL) {
							printf("Error during memmove to add quotations for WotScriptFileContents[%i]!\n", i);
						}
						WotScriptFileContents[i][j] = '\\';
						QuotesAdded++;
						j++;
					}
				}
			}

#endif

			fclose(Inputfile);
		}
		else {
			printf("Error reading \"%s\"!\n", WotScriptFiles[i]);
			return -1;
		}
	}

#if OUTPUT_JSON

	FILE* OutputFile = fopen("ScannerOutput.json", "w");
	if (OutputFile == NULL) {
		printf("Error opening \"ScannerOutput.json\" for writing!");
		return -1;
	}
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

#else
	const char* CurrentLine = NULL;
	for (int i = 0; i < FileCount; i++) {
		CurrentLine = WotScriptFileContents[i];
		while (CurrentLine) {
			const char* NextLine = strchr(CurrentLine, '\n');
			int CurLineLength = NextLine != NULL ? (NextLine - CurrentLine) : strlen(CurrentLine);
			char* OutputLine = malloc(sizeof(char) * (CurLineLength + 1));
			memset(OutputLine, 0, CurLineLength + 1);
			if (OutputLine) {
				memcpy(OutputLine, CurrentLine, CurLineLength);
				OutputLine[CurLineLength] = '\0';
				printf("%s\n", OutputLine);
				free(OutputLine);
			}
			else {
				printf("Error, malloc failed when allocating line for output");
			}

			CurrentLine = NextLine ? (NextLine + 1) : NULL;
		}
	}

	printf("\n");
	printf("END\n");

#endif

    for(int i = 0; i < FileCount; i++) {
		free(WotScriptFiles[i]);
		free(WotScriptFileContents[i]);
    }
	free(WotScriptFiles);
	free(WotScriptFileContents);

    return 0;

}