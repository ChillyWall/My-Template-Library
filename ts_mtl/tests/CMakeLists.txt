find_package(GTest REQUIRED)

link_libraries(ts_mtl GTest::GTest GTest::Main)

add_executable(test_ts_queue src/test_ts_queue.cpp)
add_test(NAME test_ts_queue COMMAND test_ts_queue)

add_executable(test_ts_stack src/test_ts_stack.cpp)
add_test(NAME test_ts_stack COMMAND test_ts_stack)

add_executable(test_ts_deque src/test_ts_deque.cpp)
add_test(NAME test_ts_deque COMMAND test_ts_deque)

add_executable(test_ts_hash_set src/test_ts_hash_set.cpp)
add_test(NAME test_ts_hash_set COMMAND test_ts_hash_set)

add_executable(test_ts_hash_map src/test_ts_hash_map.cpp)
add_test(NAME test_ts_hash_map COMMAND test_ts_hash_map)
