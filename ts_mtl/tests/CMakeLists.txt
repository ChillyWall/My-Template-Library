find_package(GTest REQUIRED)

link_libraries(ts_mtl GTest::GTest GTest::Main)

add_executable(test_ts_stack src/test_ts_stack.cpp)
add_test(NAME test_ts_stack COMMAND test_ts_stack)
