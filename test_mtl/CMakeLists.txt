include_directories(include)
link_libraries(mtl)

add_executable(test_algorithms src/test_algorithms.cpp)
add_executable(test_vector src/test_vector.cpp)
add_executable(test_list src/test_list.cpp)
add_executable(test_deque src/test_deque.cpp)
add_executable(test_avl src/test_avl.cpp)
