// ARC TEST

#include <gtest/gtest.h>

#include <map>
#include <unordered_map>
#include <mutex>

#include "cache.h"
#include "arc_policy.h"

namespace machine {

template <typename Key>
using arc_t = ARCCachePolicy<Key>;
template <typename Key, typename Value>
using arc_cache_t = Cache<Key, Value, ARCCachePolicy<Key>>;

TEST(ARCCache, SimplePut) {
  arc_cache_t<std::string, int> cache(1, arc_t<std::string>(1));

  cache.Put("test", 666);

  EXPECT_EQ(cache.Get("test"), 666);
}

TEST(ARCCache, MissingValue) {
  arc_cache_t<std::string, int> cache(1, arc_t<std::string>(1));

  EXPECT_THROW(cache.Get("test"), std::range_error);
}

TEST(ARCCache, KeepsAllValuesWithinCapacity) {
  constexpr int CACHE_CAP = 50;
  const int TEST_RECORDS = 100;
  arc_cache_t<int, int> cache(CACHE_CAP, arc_t<int>(CACHE_CAP));

  for (int i = 0; i < TEST_RECORDS; ++i) {
    cache.Put(i, i);
  }

  for (int i = 0; i < TEST_RECORDS - CACHE_CAP; ++i) {
    EXPECT_THROW(cache.Get(i), std::range_error);
  }

  for (int i = TEST_RECORDS - CACHE_CAP; i < TEST_RECORDS; ++i) {
    EXPECT_EQ(i, cache.Get(i));
  }
}

}  // End machine namespace
