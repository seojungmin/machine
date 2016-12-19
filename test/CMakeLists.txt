## TESTS

add_executable(test_foo test_foo.cpp)
target_link_libraries(test_foo machine ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

add_test(NAME FooTest COMMAND test_foo)
