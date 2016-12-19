##################################################################################

## TESTS

# ---[ LRU TEST
add_executable(policy_lru_test policy_lru_test.cpp)
target_link_libraries(policy_lru_test machine 
${GTEST_BOTH_LIBRARIES} 
${GLOG_LIBRARIES} 
${CMAKE_THREAD_LIBS_INIT}
)
add_test(NAME LRUTest COMMAND policy_lru_test)

# ---[ LFU TEST
add_executable(policy_lfu_test policy_lfu_test.cpp)
target_link_libraries(policy_lfu_test machine 
${GTEST_BOTH_LIBRARIES} 
${GLOG_LIBRARIES} 
${CMAKE_THREAD_LIBS_INIT}
)
add_test(NAME LFUTest COMMAND policy_lfu_test)

# ---[ FIFO TEST
add_executable(policy_fifo_test policy_fifo_test.cpp)
target_link_libraries(policy_fifo_test machine
${GTEST_BOTH_LIBRARIES} 
${GLOG_LIBRARIES} 
${CMAKE_THREAD_LIBS_INIT}
)
add_test(NAME FIFOTest COMMAND policy_fifo_test)

# ---[ ARC TEST
add_executable(policy_arc_test policy_arc_test.cpp)
target_link_libraries(policy_arc_test machine 
${GTEST_BOTH_LIBRARIES} 
${GLOG_LIBRARIES} 
${CMAKE_THREAD_LIBS_INIT}
)
add_test(NAME ARCTest COMMAND policy_arc_test)
