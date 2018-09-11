#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#define ERROR_OUTPUT_STREAM stderr
#define PIPE_OUTPUT_STREAM stdout

void* malloc(size_t);

const char* PreprocessData(lua_State* L, const char* File);

int main (int argc, char* argv[]) {

    char**  Files     = NULL;
    size_t* FileSizes = NULL;
    size_t  FileCount = argc - 1;
    if(argc > 1) {
        Files     = malloc(sizeof(char*)  * (argc - 1));
        FileSizes = malloc(sizeof(size_t) * (argc - 1));
    }
    else {
        fprintf(ERROR_OUTPUT_STREAM,"No arguments!\n");
        return -1;
    }

    FILE* F = NULL;
    for(int i = 1; i < argc; i++) {
        F = fopen(argv[i], "r");
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

    luaL_dofile(L, "script.lua");

    for(size_t i = 0; i < FileCount; i++) {
        //fprintf(PIPE_OUTPUT_STREAM, "Ouput:\n%s\n",PreprocessData(L, Files[i]));
        PreprocessData(L, Files[i]);
    }

    lua_close(L);

    for(size_t i = 0; i < FileCount; i++) {
        free(Files[i]);
    }
    free(Files);
    free(FileSizes);

    return 0;
}

const char* PreprocessData(lua_State* L, const char* Data) {
    const char* rstr = NULL;

    lua_getglobal(L, "PreprocessData");
    lua_pushstring(L, Data);

    if(lua_pcall(L, 1, 1, 0)) {
        fprintf(ERROR_OUTPUT_STREAM, "Error: Failed to execute \"PreprocessData\" function from lua!\n");
    }
    if(!lua_isstring(L, -1)) {
        fprintf(ERROR_OUTPUT_STREAM, "Error: Function \"PreprocessData\" from lua did not return a string!\n");
    }
    rstr = lua_tostring(L, -1);
    lua_pop(L, 1);

    return rstr;
}