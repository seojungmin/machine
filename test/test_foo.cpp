// TEST FOO

#include <cstdlib>
#include <iostream>
#include <cstring>

#include "foo.h"

#include "gtest/gtest.h"

namespace machine {

TEST(FOO, FOOTEST1) {
  printf("foo: %d", foo());
}

int main (int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);

    return  RUN_ALL_TESTS();
}

}  // End machine namespace
