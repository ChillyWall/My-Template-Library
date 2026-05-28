cmake_minimum_required(VERSION 4.1.1)

set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "d0edc3af-4c50-42ea-a356-e2862fe7a444")

project(mtl_demo LANGUAGES CXX)

set(CMAKE_CXX_MODULE_STD ON)
set(CMAKE_CXX_STANDARD 23)

find_package(mtl REQUIRED COMPONENTS mtl ts_mtl)

add_executable(mtl_demo src/main.cpp)
target_link_libraries(mtl_demo PRIVATE mtl::mtl mtl::ts_mtl)
