add_library(mtl INTERFACE)
target_include_directories(
    mtl
    INTERFACE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:include>"
)
target_compile_features(mtl INTERFACE cxx_std_20)

install(
    TARGETS mtl
    EXPORT mtl-targets
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(DIRECTORY include/mtl DESTINATION include)
install(
    EXPORT mtl-targets
    FILE mtl-targets.cmake
    NAMESPACE mtl::
    DESTINATION lib/cmake/mtl
)

include(CMakePackageConfigHelpers)
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/mtl-config.cmake"
    INSTALL_DESTINATION lib/cmake/mtl
)
install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/mtl-config.cmake"
    DESTINATION lib/cmake/mtl
)
