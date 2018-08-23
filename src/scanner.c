#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define OUTPUT_JSON 0

int GetFileNames(int argc, const char** argv, char*** Files, size_t* FileCount);
int ReadFiles(size_t FileCount, const char** Files, char*** FileContents, size_t** FileSizes);
int Output(size_t FileCount, const char** Files, const char** FileContents);

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
		return 1;
	}

	if (Output(FileCount, WotScriptFiles, WotScriptFileContents) != 0) {
		fprintf( stderr, "Error outputing scanner results!");
		return 1;
	}

	for (size_t i = 0; i < FileCount; i++) {
		free(WotScriptFiles[i]);
		free(WotScriptFileContents[i]);
	}
	free(WotScriptFiles);
	free(WotScriptFileLengths);

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
				fprintf( stderr, "Error allocating File[%uz]\n", i);
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
			fprintf( stderr, "Error allocating FileContents[%uz]!\n", i);
			return 1;
		}
		memset((*FileContents)[i], 0, (*FileSizes)[i] + 1);
		fread((*FileContents)[i], sizeof(char), (*FileSizes)[i], InputFile);
		((*FileContents))[i][((*FileSizes))[i]] = '\0';

		fclose(InputFile);
	}

#if OUTPUT_JSON

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
					if (memmove((*FileContents)[i] + j + 1, (*FileContents)[i] + j, (*FileSizes)[i] - (j + 1)) == NULL) {
						fprintf( stderr, "Error during memmove to add newline for FileContents[%uz][%uz]\n", i, j);
					}
					(*FileContents)[i][j] = '\\';
					(*FileContents)[i][j + 1] = 'n';
					SpecialCharactersConverted++;
					j++;
					break;
				}
				case '\"': {
					if (memmove((*FileContents)[i] + j + 1, (*FileContents)[i] + j, (*FileSizes)[i] - (j + 1)) == NULL) {
						fprintf(stderr, "Error during memmove to add quotations for FileContents[%uz][%uz]\n", i, j);
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

#endif

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
	for (int i = 0; i < FileCount; i++) {
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