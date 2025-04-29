add_library(mtl INTERFACE)
target_include_directories(mtl INTERFACE include)

install(DIRECTORY include/ DESTINATION include)
