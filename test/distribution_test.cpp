// DISTRIBUTION TEST

#include <gtest/gtest.h>

#include "distribution.h"

namespace machine {

TEST(DistributionTest, RangeCheck) {

  size_t upper_bound = 100;
  double theta = 1.5;
  size_t sample_count = 1000;
  size_t sample_itr;

  ZipfDistribution zipf_generator(upper_bound, theta);

  for(sample_itr = 0; sample_itr < sample_count; sample_itr++){
    auto sample = zipf_generator.GetNextNumber();
    //printf("sample %lu : %lu\n", sample_itr, sample);

    // Check range
    EXPECT_TRUE(sample >= 1);
    EXPECT_TRUE(sample <= upper_bound);
  }

}

}  // End machine namespace
