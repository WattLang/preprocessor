# Wotscript Scanner and Preprocessor
[![pipeline status](https://gitlab.com/WotScript/WotScript-scanner-preproccessor/badges/master/pipeline.svg)](https://gitlab.com/WotScript/WotScript-scanner-preproccessor/commits/master)
## Details
* Language C/C++

## Goals
* Ability to read a file and pass it through the WotScipt pipeline
* Ability for users to define macros

## How to compile (Linux)
### Premake5:
1. Download and install premake5, check by typeing `premake5 --help` in the console
2. CD into the download directory
3. Type `premake5 gmake2` into the console
4. If configuration successful, type `make -C ./build32` (build64 if 64 bit) or CD into the build directory and type `make`
5. The compiled binary should be under the build32(build64 if 64 bit)/bin/Scanner-Preprocessor/Debug(Release)/

### CMake:
1. Install CMake, you'll need at least version 3.8.2 (Ubuntu 18.04 has 3.10.2 in the official repositories)
2. CD into the repository
3. Type `cmake -H. -Bbuild`, you may specify a build system of your own liking, i.e. `-GNinja` for ninja-build
4. CD into build and run `make`, or whatever build system you specified

## How to compile (Windows)
### Premake5:
1. Download and install premake5, check by typeing `premake5 --help` in the console
2. CD into the download directory
3. Type `premake5 vs2017` (or whatever version of VS you want to use)
4. Open the build32 (build64 if 64 bit) folder and open the .sln file
5. Compile using Visual Studio
6. The compiled binary should be under the build32(build64 if 64 bit)/bin/Scanner-Preprocessor/Debug(Release)/

### CMake:
1. Install and IDE of your choice
2. Download and install CMake
3. Open up CMake, type in the path to repository in the top input box, point it to a preferably empty directory in where to build binaries
4. Configure and Generate the project
5. Open the project in the IDE of your choice and run build
