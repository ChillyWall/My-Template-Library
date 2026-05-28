add_library(mtl STATIC)
file(GLOB MTL_MODULE_SOURCES "src/*.cppm")
target_sources(
    mtl
    PUBLIC FILE_SET CXX_MODULES FILES ${MTL_MODULE_SOURCES}
)
target_compile_features(mtl PUBLIC cxx_std_23)

add_subdirectory(tests)

install(
    TARGETS mtl
    EXPORT mtl-targets
    COMPONENT mtl
    FILE_SET CXX_MODULES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mtl
)

install(
    EXPORT mtl-targets
    FILE mtl-targets.cmake
    NAMESPACE mtl::
    COMPONENT mtl
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mtl
)
