cmake_minimum_required(VERSION 4.1.1)

set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "d0edc3af-4c50-42ea-a356-e2862fe7a444")

project(mtl LANGUAGES CXX)
set(CMAKE_CXX_MODULE_STD ON)
set(CMAKE_CXX_STANDARD 23)

enable_testing()

add_subdirectory(mtl)

# add_subdirectory(ts_mtl)

# include(CMakePackageConfigHelpers)
# configure_package_config_file(
#     ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in
#     "${CMAKE_CURRENT_BINARY_DIR}/mtl-config.cmake"
#     INSTALL_DESTINATION lib/cmake/mtl
# )
# install(
#     FILES "${CMAKE_CURRENT_BINARY_DIR}/mtl-config.cmake"
#     DESTINATION lib/cmake/mtl
# )
