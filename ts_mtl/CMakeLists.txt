add_library(ts_mtl INTERFACE)
target_include_directories(
    ts_mtl
    INTERFACE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:include>"
)
target_compile_features(ts_mtl INTERFACE cxx_std_20)
target_link_libraries(ts_mtl INTERFACE mtl)

add_subdirectory(tests)

install(
    TARGETS ts_mtl
    EXPORT ts_mtl-targets
    COMPONENT ts_mtl
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(DIRECTORY include/ts_mtl DESTINATION include)
install(
    EXPORT ts_mtl-targets
    FILE ts_mtl-targets.cmake
    NAMESPACE mtl::
    COMPONENT ts_mtl
    DESTINATION lib/cmake/mtl
)
