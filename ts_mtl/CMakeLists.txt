add_library(ts_mtl STATIC)
file(GLOB TS_MTL_MODULE_SOURCES "src/*.cppm")
target_sources(
    ts_mtl
    PUBLIC FILE_SET CXX_MODULES FILES ${TS_MTL_MODULE_SOURCES}
)
target_compile_features(ts_mtl PUBLIC cxx_std_23)
target_link_libraries(ts_mtl PUBLIC mtl)

add_subdirectory(tests)

install(
    TARGETS ts_mtl
    EXPORT ts_mtl-targets
    COMPONENT ts_mtl
    FILE_SET CXX_MODULES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ts_mtl
)

install(
    EXPORT ts_mtl-targets
    FILE ts_mtl-targets.cmake
    NAMESPACE mtl::
    COMPONENT ts_mtl
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mtl
)
