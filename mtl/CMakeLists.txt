add_library(mtl INTERFACE)
target_include_directories(
    mtl
    INTERFACE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:include>"
)
target_compile_features(mtl INTERFACE cxx_std_20)

add_subdirectory(tests)

install(
    TARGETS mtl
    EXPORT mtl-targets
    COMPONENT mtl
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(DIRECTORY include/mtl DESTINATION include)
install(
    EXPORT mtl-targets
    FILE mtl-targets.cmake
    NAMESPACE mtl::
    COMPONENT mtl
    DESTINATION lib/cmake/mtl
)
