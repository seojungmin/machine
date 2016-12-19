##################################################################################

## TESTS

# ---[ LRU TEST
add_executable(lru_test lru_test.cpp)
target_link_libraries(lru_test machine ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
add_test(NAME LRUTest COMMAND lru_test)

# ---[ LFU TEST
add_executable(lfu_test lfu_test.cpp)
target_link_libraries(lfu_test machine ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
add_test(NAME LFUTest COMMAND lfu_test)

# ---[ FIFO TEST
add_executable(fifo_test fifo_test.cpp)
target_link_libraries(fifo_test machine ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
add_test(NAME FIFOTest COMMAND fifo_test)
