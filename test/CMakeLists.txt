## TESTS

add_executable(lru_test lru_test.cpp)
target_link_libraries(lru_test machine ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

add_test(NAME LRUTest COMMAND lru_test)
