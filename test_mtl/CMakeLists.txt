add_executable(test_vector src/test_vector.cpp)
target_include_directories(test_vector PRIVATE include)
target_link_libraries(test_vector PRIVATE mtl)

add_executable(test_algorithms src/test_algorithms.cpp)
target_include_directories(test_algorithms PRIVATE include)
target_link_libraries(test_algorithms PRIVATE mtl)

add_executable(test_list src/test_list.cpp)
target_include_directories(test_list PRIVATE include)
target_link_libraries(test_list PRIVATE mtl)
