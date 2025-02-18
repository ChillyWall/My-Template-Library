find_package(GTest REQUIRED)
find_package(glog REQUIRED)

include_directories(include)
link_libraries(mtl glog GTest::GTest GTest::Main)

add_compile_definitions(GLOG_LOG_DIR=${CMAKE_BINARY_DIR}/test_mtl/log)

add_executable(test_algorithms src/test_algorithms.cpp)
add_executable(test_vector src/test_vector.cpp)
add_executable(test_list src/test_list.cpp)
add_executable(test_deque src/test_deque.cpp)
add_executable(test_avl src/test_avl.cpp)
add_executable(test_priority_queue src/test_priority_queue.cpp)

enable_testing()
add_test(NAME test_algorithms COMMAND test_algorithms)
add_test(NAME test_vector COMMAND test_vector)
add_test(NAME test_list COMMAND test_list)
add_test(NAME test_deque COMMAND test_deque)
add_test(NAME test_avl COMMAND test_avl)
add_test(NAME test_priority_queue COMMAND test_priority_queue)
