add_executable(test_vector src/test_vector.cpp)
target_include_directories(test_vector PUBLIC include)
target_include_directories(test_vector PUBLIC ${CMAKE_SOURCE_DIR}/mtl/include)
