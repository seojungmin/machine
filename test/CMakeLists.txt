## TESTS

add_executable(test_foo test_foo.cpp)
target_link_libraries(test_foo machine ${GTEST_BOTH_LIBRARIES})

add_test(NAME FooTest COMMAND test_foo)
