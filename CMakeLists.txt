cmake_minimum_required(VERSION 3.0.2)

project(Scanner-Preprocessor CXX)
option(BUILD_TESTING "Should Test targets be generated?" OFF)
include(CTest)

add_executable(${CMAKE_PROJECT_NAME}
src/preprocessor.cpp
src/DefineModule.cpp
src/IncludeModule.cpp
)

file(COPY test/ DESTINATION ${CMAKE_BINARY_DIR})

add_test(NAME SanityCheck COMMAND ${CMAKE_PROJECT_NAME} Test.ws)
