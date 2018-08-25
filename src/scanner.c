#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define OUTPUT_JSON 1

#if defined(BIT64) 
	#define SIZE_PRINT "%zu"
#else
	#define SIZE_PRINT "%u"
#endif

int GetFileNames  (int argc, const char** argv, char*** Files, size_t* FileCount);
int ReadFiles     (size_t FileCount, const char** Files,       char*** FileContents, size_t** FileSizes);
int Output        (size_t FileCount, const char** Files, const char** FileContents);
int Preprocess    (size_t FileCount, const char** Processees, const size_t* ProcesseesLengths,  char*** ProcessedOutputContents, size_t** ProcessedOutputLengths);
int FormatForJSON (size_t FileCount, const char** Files, char*** FileContents, size_t** FileSizes);

void* memshift(void* src, size_t byteoffset, size_t size, int direction) {
	if (direction > 0) {
		return memmove((void*)((size_t)src + byteoffset), src, size);
	}
	else if(direction < 0) {
		return memmove((void*)((size_t)src - byteoffset), src, size);
	}
	else {
		return NULL;
	}
}

int main(int argc, char* argv[]) {

	char WotScriptFileExtension[4] = ".ws";
	char** WotScriptFiles = NULL;
	size_t FileCount = 0;

	if (GetFileNames(argc, argv, &WotScriptFiles, &FileCount) != 0) {
		fprintf( stderr, "Error reading file names!\n");
		return 1;
	}

	char** WotScriptFileContents = NULL;
	size_t* WotScriptFileLengths = NULL;

	if (ReadFiles(FileCount, WotScriptFiles, &WotScriptFileContents, &WotScriptFileLengths) != 0) {
		fprintf( stderr, "Error reading files!\n");
		return 2;
	}

	char**  WotScriptProcessedContents = NULL;
	size_t* WotScriptProcessedLengths  = NULL;

	if (Preprocess(FileCount, WotScriptFileContents, WotScriptFileLengths, &WotScriptProcessedContents, &WotScriptProcessedLengths) != 0) {
		fprintf( stderr, "Error preprocessing!");
		return 3;
	}

#if OUTPUT_JSON

	if (FormatForJSON(FileCount, WotScriptFiles, &WotScriptProcessedContents, &WotScriptProcessedLengths) != 0) {
		fprintf(stderr, "Error formating into JSON!\n");
		return 4;
	}

#endif

	if (Output(FileCount, WotScriptFiles, WotScriptProcessedContents) != 0) {
		fprintf( stderr, "Error outputing scanner results!");
		return 4;
	}


	for (size_t i = 0; i < FileCount; i++) {
		free(WotScriptFiles[i]);
		free(WotScriptFileContents[i]);
		free(WotScriptProcessedContents[i]);
	}
	free(WotScriptFiles);
	free(WotScriptFileLengths);
	free(WotScriptProcessedContents);
	free(WotScriptProcessedLengths);

	return 0;
}

// ===================================================================================================
// Implementations
// ===================================================================================================

int GetFileNames(int argc, const char** argv, char*** Files, size_t* FileCount) {
	if(argc > 1) {
		*FileCount = argc - 1;
		(*Files) = malloc(sizeof(char*) * (*FileCount));
		if ((*Files) == NULL) {
			fprintf( stderr, "Error allocating File array!");
			return 1;
		}
		for (size_t i = 0; i < *FileCount; i++) {
			(*Files)[i] = malloc(strlen(argv[i + 1]) + 1);
			if ((*Files)[i] == NULL) {
				fprintf( stderr, "Error allocating File["SIZE_PRINT"]\n", i);
				return 1;
			}
			strcpy((*Files)[i], argv[i + 1]);
		}
		return 0;
	}
	else {
		fprintf( stderr, "No input files!\n");
		return 1;
	}
	return 0;
}

int ReadFiles(size_t FileCount, const char** Files, char*** FileContents, size_t** FileSizes) {
	FILE* InputFile = NULL;
	(*FileContents) = malloc(sizeof(char*)  * FileCount);
	(*FileSizes)    = malloc(sizeof(size_t) * FileCount);
	if ((*FileContents) == NULL) {
		fprintf( stderr, "Error allocating FileConents array!");
		return 1;
	}

	//Read file loop
	for (size_t i = 0; i < FileCount; i++) {
		InputFile = fopen(Files[i], "r");
		if (InputFile == NULL) {
			fprintf( stderr, "Error reading file \"%s\", this file will not be read!\n", Files[i]);
			continue;
		}

		//Get file length
		fseek(InputFile, 0, SEEK_END);
		(*FileSizes)[i] = ftell(InputFile);
		fseek(InputFile, 0, SEEK_SET);

		//Read files
		(*FileContents)[i] = malloc((*FileSizes)[i] + 1);
		if ((*FileContents)[i] == NULL) {
			fprintf( stderr, "Error allocating FileContents["SIZE_PRINT"]!\n", i);
			return 1;
		}
		memset((*FileContents)[i], 0, (*FileSizes)[i] + 1);
		fread((*FileContents)[i], sizeof(char), (*FileSizes)[i], InputFile);
		((*FileContents))[i][((*FileSizes))[i]] = '\0';

		fclose(InputFile);
	}

	return 0;
}

int Output(size_t FileCount, const char** Files, const char** FileContents) {
#if OUTPUT_JSON
	//Outputs JSON file
	FILE* OutputFile = fopen("ScannerOutput.json", "w");
	if (OutputFile == NULL) {
		fprintf( stderr, "Error opening \"ScannerOutput.json\" for writing!");
		return -1;
	}
	fprintf(OutputFile, "{\n");
	for (size_t i = 0; i < FileCount; i++) {
		if (i == FileCount - 1) {
			fprintf(OutputFile, "\t\"%s\" : \"%s\"\n", Files[i], FileContents[i]);
		}
		else {
			fprintf(OutputFile, "\t\"%s\" : \"%s\",\n", Files[i], FileContents[i]);
		}
	}
	fprintf(OutputFile, "}\n");
	fclose(OutputFile);

#else
	// Prints each file line by line
	const char* CurrentLine = NULL;
	for (size_t i = 0; i < FileCount; i++) {
		CurrentLine = FileContents[i];
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
				fprintf( stderr, "Error, malloc failed when allocating line for output");
			}

			CurrentLine = NextLine ? (NextLine + 1) : NULL;
		}
	}

	printf("END\n");

#endif

	return 0;
}

int Preprocess(size_t FileCount, const char** Processees, const size_t* ProcesseesLengths, char*** ProcessedOutputContents, size_t** ProcessedOutputLengths) {
	(*ProcessedOutputContents) = malloc(sizeof(char*)  * FileCount);
	(*ProcessedOutputLengths)  = malloc(sizeof(size_t) * FileCount);

	if (memcpy((*ProcessedOutputLengths), ProcesseesLengths, sizeof(size_t) * FileCount) == NULL) {
		fprintf( stderr, "Error copying ProcesseesLengths into (*ProcessedOutputLengths)!\n");
		return 1;
	}

	char    MacroDefinitionToken[] = "#define";
	char**  MacroNames             = NULL;
	char**  MacroDefinitions       = NULL;
	size_t* MacroDefinitionLengths = NULL;
	size_t* MacroNameLengths       = NULL;
	size_t* MacroOccurences        = NULL;
	size_t  MacroCount             = 0;

	// Get macro counts
	for (size_t i = 0; i < FileCount; i++) {
		const char* CurrentDefinition = Processees[i];
		for (;;) {
			CurrentDefinition = strstr(CurrentDefinition, MacroDefinitionToken);
			if (CurrentDefinition == NULL) {
				break;
			}
			else {
				MacroCount++;
				CurrentDefinition += sizeof(MacroDefinitionToken);
			}
		}
	}

	if (MacroCount == 0) {
		return 0;
	}

	MacroDefinitionLengths = malloc(sizeof(size_t) * MacroCount);
	MacroNameLengths       = malloc(sizeof(size_t) * MacroCount);
	MacroOccurences        = malloc(sizeof(size_t) * MacroCount);
	MacroDefinitions       = malloc(sizeof(char*)  * MacroCount);
	MacroNames             = malloc(sizeof(char*)  * MacroCount);

	if (MacroDefinitionLengths == NULL) {
		fprintf( stderr, "Error allocating MacroLengths!\n");
		return 1;
	}
	if (MacroOccurences == NULL) {
		fprintf(stderr, "Error allocating MacroOccurences!\n");
		return 1;
	}
	if (MacroDefinitions == NULL) {
		fprintf(stderr, "Error allocating MacroDefinitions!\n");
		return 1;
	}

	memset(MacroOccurences,        0, sizeof(size_t) * MacroCount);
	memset(MacroDefinitionLengths, 0, sizeof(size_t) * MacroCount);

	// Get macro name lengths
	size_t MacrosPassed = 0;
	for (size_t i = 0; i < FileCount; i++) {
		const char* CurrentDefinition = Processees[i];
		const char* EndName = Processees[i];
		while (MacrosPassed < MacroCount) {
			CurrentDefinition = strstr(CurrentDefinition, MacroDefinitionToken);
			if (CurrentDefinition == NULL) {
				break;
			}
			else {
				CurrentDefinition += sizeof(MacroDefinitionToken);
				EndName = strstr(CurrentDefinition, " ");
				if (EndName == NULL) {
					fprintf(stderr, "Expected a value before EOF!\n");
					return 1;
				}
				MacroNameLengths[MacrosPassed] = EndName - CurrentDefinition;
				MacrosPassed++;
			}
		}
	}

	// Get macro definition lengths
	MacrosPassed = 0;
	for (size_t i = 0; i < FileCount; i++) {
		const char* CurrentDefinition = Processees[i];
		const char* EndLine = Processees[i];
		while (MacrosPassed < MacroCount) {
			CurrentDefinition = strstr(CurrentDefinition, MacroDefinitionToken);
			if (CurrentDefinition == NULL) {
				break;
			}
			else {
				CurrentDefinition += sizeof(MacroDefinitionToken);
				CurrentDefinition = strstr(CurrentDefinition, " ");
				if (CurrentDefinition == NULL) {
					fprintf( stderr, "Expected a value before new line!\n");
					return 1;
				}
				CurrentDefinition++;
				EndLine = strstr(CurrentDefinition, "\n");
				MacroDefinitionLengths[MacrosPassed] = (size_t)(((size_t)EndLine) - ((size_t)CurrentDefinition));
				MacrosPassed++;
			}
		}
	}

	// Allocate each macro name to it's defined length
	for (size_t i = 0; i < MacroCount; i++) {
		MacroNames[i] = malloc(MacroNameLengths[i] + 3);
		if (MacroNames[i] == NULL) {
			fprintf(stderr, "Error allocating MacroNames["SIZE_PRINT"]!\n", i);
			return 1;
		}
		memset(MacroNames[i], 0, MacroNameLengths[i] + 3);
		MacroNames[i][0] = ' ';
		MacroNames[i][MacroNameLengths[i] + 1] = ' ';
		MacroNameLengths[i] += 2;
	}

	// Allocate each macro definition to it's defined length
	for (size_t i = 0; i < MacroCount; i++) {
		MacroDefinitions[i] = malloc(MacroDefinitionLengths[i] + 2);
		if (MacroDefinitions[i] == NULL) {
			fprintf( stderr, "Error allocating MacroDefinitions["SIZE_PRINT"]!\n", i);
			return 1;
		}
		memset(MacroDefinitions[i], 0, MacroDefinitionLengths[i] + 3);
		MacroDefinitions[i][0] = ' ';
		MacroDefinitions[i][MacroDefinitionLengths[i] + 1] = ' ';
		MacroDefinitionLengths[i] += 2;
	}

	// Name each macro
	MacrosPassed = 0;
	for (size_t i = 0; i < FileCount; i++) {
		const char* CurrentDefinition = Processees[i];
		const char* EndMacroName = Processees[i];
		while (MacrosPassed < MacroCount) {
			CurrentDefinition = strstr(CurrentDefinition, MacroDefinitionToken);
			if (CurrentDefinition == NULL) {
				break;
			}
			else {
				CurrentDefinition += sizeof(MacroDefinitionToken);
				EndMacroName = strstr(CurrentDefinition, " ");
				if (EndMacroName == NULL) {
					fprintf(stderr, "Expected a value before EOF!\n");
					return 1;
				}
				if (memcpy(MacroNames[MacrosPassed] + 1, CurrentDefinition, (size_t)(EndMacroName - CurrentDefinition)) == NULL) {
					fprintf( stderr, "Error copying CurrentDefinition into MacroNames["SIZE_PRINT"]!\n", MacrosPassed);
					return 1;
				}
				MacrosPassed++;
			}
		}
	}

	// Define each macro
	MacrosPassed = 0;
	for (size_t i = 0; i < FileCount; i++) {
		const char* CurrentDefinition  = Processees[i];
		const char* EndMacroDefinition = Processees[i];
		while (MacrosPassed < MacroCount) {
			CurrentDefinition = strstr(CurrentDefinition, MacroDefinitionToken);
			if (CurrentDefinition == NULL) {
				break;
			}
			else {
				CurrentDefinition += sizeof(MacroDefinitionToken);
				CurrentDefinition = strstr(CurrentDefinition, " ");
				if (CurrentDefinition == NULL) {
					fprintf(stderr, "Expected a value before EOF!\n");
					return 1;
				}
				EndMacroDefinition = strstr(CurrentDefinition, "\n");
				if (EndMacroDefinition == NULL) {
					fprintf(stderr, "Expected a new line before EOF!\n");
					return 1;
				}
				CurrentDefinition++;
				if (memcpy(MacroDefinitions[MacrosPassed] + 1, CurrentDefinition, (size_t)(EndMacroDefinition - CurrentDefinition)) == NULL) {
					fprintf(stderr, "Error copying CurrentDefinition into MacroNames["SIZE_PRINT"]!\n", MacrosPassed);
					return 1;
				}
				MacrosPassed++;
			}
		}
	}

	// Copies unprocessed char array
	for (size_t i = 0; i < FileCount; i++) {
		for (size_t j = 0; j < MacroCount; j++) {
			(*ProcessedOutputContents)[i] = malloc(ProcesseesLengths[i]);
			if ((*ProcessedOutputContents)[i] == NULL) {
				fprintf(stderr, "Error failed to allocate (*ProcessedOutputContents)["SIZE_PRINT"] with "SIZE_PRINT" bytes!\n", i, ProcesseesLengths[i]);
				return 1;
			}
			if (memcpy((*ProcessedOutputContents)[i], Processees[i], ProcesseesLengths[i]) == NULL) {
				fprintf(stderr, "Error copying Processees["SIZE_PRINT"] into (*ProcessedOutputContents)["SIZE_PRINT"]!\n", i, i);
				return 1;
			}
		}
	}

	// Remove macro definitions
	MacrosPassed = 0;
	for (size_t i = 0; i < FileCount; i++) {
		char* CurrentDefinition  = (*ProcessedOutputContents)[i];
		const char* EndMacroDefinition = (*ProcessedOutputContents)[i];
		while (MacrosPassed < MacroCount) {
			CurrentDefinition = strstr(CurrentDefinition, MacroDefinitionToken);
			if (CurrentDefinition == NULL) {
				break;
			}
			else {
				EndMacroDefinition = strstr(CurrentDefinition, "\n");
				if (EndMacroDefinition == NULL) {
					fprintf(stderr, "Expected a new line before EOF!\n");
					return 1;
				}
				EndMacroDefinition++;
				if (memset(CurrentDefinition, ' ', (size_t)(EndMacroDefinition - CurrentDefinition)) == NULL) {
					fprintf(stderr, "Error clearing the definition for MacroNames["SIZE_PRINT"]!\n", MacrosPassed);
					return 1;
				}
				MacrosPassed++;
			}
		}
	}

	// Find how many times each macro occurs
	size_t MacrosChecked = 0;
	for (size_t i = 0; i < FileCount; i++) {
		while (MacrosChecked < MacroCount) {
			const char* CurrentPosition = (*ProcessedOutputContents)[i];
			for (;;) {
				CurrentPosition = strstr(CurrentPosition, MacroNames[MacrosChecked]);
				if (CurrentPosition == NULL) {
					MacrosChecked++;
					break;
				}
				CurrentPosition += MacroNameLengths[MacrosChecked] + MacroDefinitionLengths[MacrosChecked];
				MacroOccurences[MacrosChecked]++;
			}
		}
	}

	// Calculate how big each processed character ouput should be
	for (size_t i = 0; i < FileCount; i++) {
		for (size_t j = 0; j < MacroCount; j++) {
			(*ProcessedOutputLengths)[i] += (MacroDefinitionLengths[j] * MacroOccurences[j]);
		}
	}

	// Size each processed output char array and copies unprocessed char array
	for (size_t i = 0; i < FileCount; i++) {
		for (size_t j = 0; j < MacroCount; j++) {
			(*ProcessedOutputContents)[i] = realloc((*ProcessedOutputContents)[i], (*ProcessedOutputLengths)[i]);
			if ((*ProcessedOutputContents)[i] == NULL) {
				fprintf( stderr, "Error failed to reallocate (*ProcessedOutputContents)["SIZE_PRINT"] with "SIZE_PRINT" bytes!\n", i, (*ProcessedOutputLengths)[i]);
				return 1;
			}
		}
	}

	// Apply each macro
	size_t MacrosApplied = 0;
	for (size_t i = 0; i < FileCount; i++) {
		size_t CurrentProcessedSize = (ProcesseesLengths[i]);
		char* CurrentUsedMacro = (*ProcessedOutputContents)[i];
		while (MacrosApplied < MacroCount) {
			CurrentUsedMacro = strstr(CurrentUsedMacro, MacroNames[MacrosApplied]);
			if (CurrentUsedMacro == NULL) {
				MacrosApplied++;
				break;
			}
			else {
				fprintf(stdout, SIZE_PRINT, (*ProcessedOutputLengths)[i] - (((size_t)CurrentUsedMacro - (size_t)(*ProcessedOutputContents)[i])));
				if (memshift(CurrentUsedMacro, MacroDefinitionLengths[MacrosApplied] - MacroNameLengths[MacrosApplied], CurrentProcessedSize - (((size_t)CurrentUsedMacro - (size_t)(*ProcessedOutputContents)[i])), 1) == NULL) {
					fprintf( stderr, "Failed to shift ProcessedOutputContents["SIZE_PRINT"] by "SIZE_PRINT" bytes", i, MacroDefinitionLengths[MacrosApplied]);
					return 1;
				}
				if (memcpy(CurrentUsedMacro, MacroDefinitions[MacrosApplied], MacroDefinitionLengths[MacrosApplied]) == NULL) {
					fprintf( stderr, "Failed to copy \"%s\" into CurrentUsedMacro", MacroDefinitions[MacrosApplied]);
					return 1;
				}
				CurrentProcessedSize += MacroDefinitionLengths[MacrosApplied] - MacroNameLengths[MacrosApplied];
				CurrentUsedMacro = strstr(CurrentUsedMacro, "\n");
				if (CurrentUsedMacro == NULL) {
					MacrosApplied++;
					break;
				}
			}
		}
	}


	return 0;
}

int FormatForJSON(size_t FileCount, const char** Files, char*** FileContents, size_t** FileSizes) {
	// Reformats special characters ( Ones that require a '\' to be used ) into ascii characters for JSON output
	for (size_t i = 0; i < FileCount; i++) {

		size_t PassedSpecialChars = 0;
		// Count special characters
		for (size_t j = 0; j < (*FileSizes)[i]; j++) {
			if (
				(*FileContents)[i][j] == '\n' ||
				(*FileContents)[i][j] == '\"'
				) {
				PassedSpecialChars++;
			}
		}

		if (PassedSpecialChars > 0) {
			// Reallocates conents to fit the new file size
			(*FileSizes)[i] += PassedSpecialChars;
			(*FileContents)[i] = realloc((*FileContents)[i], (*FileSizes)[i] + 1);
			(*FileContents)[i][(*FileSizes)[i]] = '\0';

			// Replaces special chars with ascii representation
			size_t SpecialCharactersConverted = 0;
			for (size_t j = 0; j < (*FileSizes)[i]; j++) {
				switch ((*FileContents)[i][j]) {
				case '\n': {
					if (memshift((*FileContents)[i] + j, 1, (*FileSizes)[i] - (j + 1), 1) == NULL) {
						fprintf(stderr, "Error during memmove to add newline for FileContents["SIZE_PRINT"]["SIZE_PRINT"]\n", i, j);
						return 1;
					}
					(*FileContents)[i][j] = '\\';
					(*FileContents)[i][j + 1] = 'n';
					SpecialCharactersConverted++;
					j++;
					break;
				}
				case '\"': {
					if (memshift((*FileContents)[i] + j, 1, (*FileSizes)[i] - (j + 1), 1) == NULL) {
						fprintf(stderr, "Error during memmove to add quotations for FileContents["SIZE_PRINT"]["SIZE_PRINT"]\n", i, j);
						return 1;
					}
					(*FileContents)[i][j] = '\\';
					SpecialCharactersConverted++;
					j++;
					break;
				}
				default: { break; }
				}
			}
		}
	}
	return 0;
}