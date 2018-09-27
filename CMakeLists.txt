cmake_minimum_required(VERSION 3.8.2)

project(Scanner-Preprocessor CXX)
option(BUILD_TESTING "Should Test targets be generated?" OFF)
include(CTest)

add_executable(${CMAKE_PROJECT_NAME}
src/preprocessor.cpp)

set_property(TARGET ${CMAKE_PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET ${CMAKE_PROJECT_NAME} PROPERTY CXX_STANDARD 17)

file(COPY test/ DESTINATION ${CMAKE_BINARY_DIR}/test)

add_test(NAME SanityCheck COMMAND ${CMAKE_PROJECT_NAME} test/Test.ws)

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC lib/module/)
