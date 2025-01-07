cmake_minimum_required(VERSION 3.16)
project(mtl LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_SOURCE_DIR .)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
add_subdirectory(mtl)
add_subdirectory(test_mtl)
