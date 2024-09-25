cmake_minimum_required(VERSION 3.16)
project(mtl LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_BUILD_TYPE "Debug")

set(CMAKE_SOURCE_DIR .)
set(CMAKE_BINARY_DIR build)

add_subdirectory(mtl)
add_subdirectory(test_mtl)
