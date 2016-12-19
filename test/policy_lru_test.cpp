// LRU TEST

#include <gtest/gtest.h>

#include <map>
#include <unordered_map>
#include <mutex>

#include "policy_lru.h"
#include "cache.h"

namespace machine {

template <typename Key>
using lru_t = LRUCachePolicy<Key>;
template <typename Key, typename Value>
using lru_cache_t = Cache<Key, Value, LRUCachePolicy<Key>>;

TEST(LRUCache, SimplePut) {
  size_t cache_capacity = 1;
  lru_cache_t<std::string, int> cache(cache_capacity,
                                      lru_t<std::string>(cache_capacity));

  cache.Put("test", 666);

  EXPECT_EQ(cache.Get("test"), 666);
}

TEST(LRUCache, MissingValue) {
  size_t cache_capacity = 1;
  lru_cache_t<std::string, int> cache(cache_capacity,
                                      lru_t<std::string>(cache_capacity));

  EXPECT_THROW(cache.Get("test"), std::range_error);
}

TEST(LRUCache, KeepsAllValuesWithinCapacity) {
  constexpr int CACHE_CAPACITY = 50;
  const int TEST_RECORDS = 100;
  lru_cache_t<int, int> cache(CACHE_CAPACITY,
                              lru_t<int>(CACHE_CAPACITY));

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