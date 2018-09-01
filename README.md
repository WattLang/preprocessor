# Wotscript Scanner and Preprocessor

## Details
* Language C/C++
  
## Goals
* Ability to read a file and pass it through the WotScipt pipeline
* Ability for users to define macros

## How to compile (Linux)
1. Download and install premake5, check by typeing `premake5 --help` in the console
2. CD into the download directory
3. Type `premake5 gmake2` into the console
4. If configuration successful, type `make -C ./build32` (build64 if 64 bit) or CD into the build directory and type `make`
5. The compiled binary should be under the build32(build64 if 64 bit)/bin/Scanner-Preprocessor/Debug(Release)/

## How to compile (Windows)
1. Download and install premake5, check by typeing `premake5 --help` in the console
2. CD into the download directory
3. Type `premake5 vs2017` (or whatever version of VS you want to use)
4. Open the build32 (build64 if 64 bit) folder and open the .sln file
5. Compile using Visual Studio
6. The compiled binary should be under the build32(build64 if 64 bit)/bin/Scanner-Preprocessor/Debug(Release)/