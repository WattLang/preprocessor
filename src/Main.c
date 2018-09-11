#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#define ERROR_OUTPUT_STREAM stderr
#define PIPE_OUTPUT_STREAM stdout

static const char* PreprocessData(lua_State* L, const char* Name, const char* Data);
static int lua_StringErase(lua_State* L);
static int lua_StringInsert(lua_State* L);

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
    lua_pushcfunction(L, lua_StringErase);
    lua_setglobal(L, "StringErase");
    lua_pushcfunction(L, lua_StringInsert);
    lua_setglobal(L, "StringInsert");

    luaL_dofile(L, "script.lua");

    for(size_t i = 0; i < FileCount; i++) {
        fprintf(PIPE_OUTPUT_STREAM, "Ouput:\n%s\n", PreprocessData(L, FileNames[i],  Files[i]));
        //PreprocessData(L, FileNames[i],  Files[i]);
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

static int lua_StringErase(lua_State* L) {

    const char* str = lua_tostring(L, 1);
    size_t Index1 = lua_tonumber(L, 2);
    size_t Index2 = lua_tonumber(L, 3);

    memset((void*)((size_t)str + Index1 - 1), ' ', Index2 - Index1 + 1);

    lua_pushstring(L, str);

    return 1;
}

static int lua_StringInsert(lua_State* L) {

    const char* MainString = lua_tostring(L, 1);
    const char* Insertee   = lua_tostring(L, 2);
    size_t Index           = lua_tonumber(L, 3);

    size_t MainStrLen     = strlen(MainString) + 1;
    size_t InserteeStrLen = strlen(Insertee)   + 1;

    fprintf(PIPE_OUTPUT_STREAM, "Inserting: %s\n into %s\n at index %zu\n", Insertee, MainString, Index);

    MainString = realloc((void*)MainString, MainStrLen + InserteeStrLen - 1);

    memshift((void*)MainString + Index, 1, MainStrLen - Index, 1);

    memcpy((void*)((size_t)MainString + Index - 1), Insertee, InserteeStrLen - 1);

    lua_pushstring(L, MainString);

    return 1;
}