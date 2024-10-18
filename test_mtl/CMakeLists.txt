add_executable(test_vector src/test_vector.cpp)
target_include_directories(test_vector PUBLIC include)
target_include_directories(test_vector PUBLIC ${MTL_INCLUDE})

add_executable(test_algorithms src/test_algorithms.cpp)
target_include_directories(test_algorithms PUBLIC include)
target_include_directories(test_algorithms PUBLIC &{MTL_INCLUDE})

add_executable(test_list src/test_list.cpp)
target_include_directories(test_list PUBLIC include)
target_include_directories(test_list PUBLIC ${MTL_INCLUDE})
