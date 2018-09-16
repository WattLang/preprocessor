#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#define ERROR_OUTPUT_STREAM stderr
#define PIPE_OUTPUT_STREAM stdout

static const char* PreprocessData(lua_State* L, const char* Name, const char* Data);
static void        PrintLines(lua_State* L, const char* String);
static int lua_StringSpaceReplace(lua_State* L);
static int lua_StringInsertFormat(lua_State* L);

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

int main (int argc, char* argv[]) {

    char**  Files     = NULL;
    char**  FileNames = NULL;
    size_t* FileSizes = NULL;
    size_t  FileCount = argc - 1;
    if(argc > 1) {
        FileNames = malloc(sizeof(char*)  * (argc - 1));
        Files     = malloc(sizeof(char*)  * (argc - 1));
        FileSizes = malloc(sizeof(size_t) * (argc - 1));
    }
    else {
        fprintf(ERROR_OUTPUT_STREAM,"No arguments!\n");
        return -1;
    }

    FILE* F = NULL;
    for(int i = 1; i < argc; i++) {

        FileNames[i - 1] = malloc(strlen(argv[i]) + 1);
        FileNames[i - 1][strlen(FileNames[i - 1])] = '\0';
        strcpy(FileNames[i - 1], argv[i]);

        F = fopen(argv[i], "r");
        if(F == NULL) {
            fprintf(ERROR_OUTPUT_STREAM, "Error: Failed to open file: \"%s\"!\n", FileNames[i - 1]);
            goto ErrorReturn;
        }
        fseek(F, 0, SEEK_END);
        FileSizes[i - 1] = ftell(F);
        fseek(F, 0, SEEK_SET);

        Files[i - 1] = malloc(FileSizes[i - 1] + 1);
        fread(Files[i - 1], FileSizes[i - 1], 1, F);
        fclose(F);

        Files[i - 1][FileSizes[i - 1]] = '\0';
    }

    lua_State *L = luaL_newstate();   /* opens Lua */
    luaL_openlibs(L);
    lua_pushcfunction(L, lua_StringSpaceReplace);
    lua_setglobal(L, "StringSpaceReplace");
    lua_pushcfunction(L, lua_StringInsertFormat);
    lua_setglobal(L, "StringInsertFormat");

    luaL_dofile(L, "script.lua");


    for(size_t i = 0; i < FileCount; i++) {
        //fprintf(PIPE_OUTPUT_STREAM, "Ouput:\n%s\n", PreprocessData(L, FileNames[i],  Files[i]));
        PrintLines(L, PreprocessData(L, FileNames[i],  Files[i]));
    }

    lua_close(L);

    for(size_t i = 0; i < FileCount; i++) {
        free(Files[i]);
        free(FileNames[i]);
    }
    free(Files);
    free(FileSizes);
    free(FileNames);

    return 0;

ErrorReturn:
    for(size_t i = 0; i < FileCount; i++) {
        free(Files[i]);
        free(FileNames[i]);
    }
    free(Files);
    free(FileSizes);
    free(FileNames);

    return -1;
}

static const char* PreprocessData(lua_State* L, const char* Name, const char* Data) {
    const char* rstr = NULL;

    lua_getglobal(L, "PreprocessData");
    lua_pushstring(L, Name);
    lua_pushstring(L, Data);

    if(lua_pcall(L, 2, 1, 0)) {
        fprintf(ERROR_OUTPUT_STREAM, "Error: Failed to execute \"PreprocessData\" function from lua!\n");
    }
    if(!lua_isstring(L, -1)) {
        fprintf(ERROR_OUTPUT_STREAM, "Error: Function \"PreprocessData\" from lua did not return a string!\n");
    }
    rstr = lua_tostring(L, -1);
    lua_pop(L, 1);

    return rstr;
}

static void PrintLines(lua_State* L, const char* String) {
    lua_getglobal(L, "PrintLines");
    lua_pushstring(L, String);

    if(lua_pcall(L, 1, 0, 0)) {
        fprintf(ERROR_OUTPUT_STREAM, "Error: Failed to execute \"PrintLines\" function from lua!\n");
    }
}

static int lua_StringSpaceReplace(lua_State* L) {

    const char* str = lua_tostring(L, 1);
    size_t Index1 = lua_tonumber(L,   2);
    size_t Index2 = lua_tonumber(L,   3);

    memset((void*)((size_t)str + Index1 - 1), ' ', Index2 - Index1 + 1);

    lua_pushstring(L, str);

    return 1;
}

static int lua_StringInsertFormat(lua_State* L) {
    char* MainString = (char*)lua_tostring(L, 1);
    size_t Index     = lua_tonumber(L, 2);


    MainString[Index - 1] = '%';
    MainString[Index    ] = 's';


    lua_pushstring(L, MainString);

    return 1;
}