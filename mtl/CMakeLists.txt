add_library(mtl STATIC)
file(GLOB MTL_MODULE_SOURCES "src/*.cppm")
target_sources(
    mtl
    PUBLIC FILE_SET CXX_MODULES FILES ${MTL_MODULE_SOURCES}
)

add_subdirectory(tests)

# install(
#     TARGETS mtl
#     EXPORT mtl-targets
#     COMPONENT mtl
#     INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
# )
# install(DIRECTORY include/mtl DESTINATION include)
# install(
#     EXPORT mtl-targets
#     FILE mtl-targets.cmake
#     NAMESPACE mtl::
#     COMPONENT mtl
#     DESTINATION lib/cmake/mtl
# )
