// ARC TEST

#include <gtest/gtest.h>

#include <map>
#include <unordered_map>
#include <mutex>

#include "policy_arc.h"
#include "cache.h"

namespace machine {

template <typename Key>
using arc_t = ARCCachePolicy<Key>;
template <typename Key, typename Value>
using arc_cache_t = Cache<Key, Value, ARCCachePolicy<Key>>;

TEST(ARCCache, SimplePut) {
  size_t cache_capacity = 1;
  arc_cache_t<std::string, int> cache(cache_capacity,
                                      arc_t<std::string>(cache_capacity));

  cache.Put("test", 666);

  EXPECT_EQ(cache.Get("test"), 666);
}

TEST(ARCCache, MissingValue) {
  size_t cache_capacity = 1;
  arc_cache_t<std::string, int> cache(cache_capacity,
                                      arc_t<std::string>(cache_capacity));

  EXPECT_THROW(cache.Get("test"), std::range_error);
}

TEST(ARCCache, KeepsAllValuesWithinCapacity) {
  constexpr int CACHE_CAPACITY = 50;
  const int TEST_RECORDS = 100;
  arc_cache_t<int, int> cache(CACHE_CAPACITY, arc_t<int>(CACHE_CAPACITY));

  for (int i = 0; i < TEST_RECORDS; ++i) {
    cache.Put(i, i);
  }

  for (int i = 0; i < TEST_RECORDS - CACHE_CAPACITY; ++i) {
    EXPECT_THROW(cache.Get(i), std::range_error);
  }

  for (int i = TEST_RECORDS - CACHE_CAPACITY; i < TEST_RECORDS; ++i) {
    EXPECT_EQ(i, cache.Get(i));
  }
}

}  // End machine namespace
