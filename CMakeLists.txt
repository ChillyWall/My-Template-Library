cmake_minimum_required(VERSION 3.16)
project(mtl LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

enable_testing()

add_subdirectory(mtl)
add_subdirectory(ts_mtl)

include(CMakePackageConfigHelpers)
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/mtl-config.cmake"
    INSTALL_DESTINATION lib/cmake/mtl
)
install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/mtl-config.cmake"
    DESTINATION lib/cmake/mtl
)
