cmake_minimum_required(VERSION 3.0.2)

project(Scanner-Preprocessor CXX)
include(CTest)

add_executable(${CMAKE_PROJECT_NAME}
src/scanner.cpp
src/DefineModule.cpp)

add_test(NAME SanityCheck COMMAND ${CMAKE_PROJECT_NAME} Test.ws)

file(COPY Test.ws DESTINATION ${CMAKE_BINARY_DIR})
