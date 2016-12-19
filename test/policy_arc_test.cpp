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
  arc_cache_t<std::string, int> cache(cache_capacity);

  cache.Put("test", 666);

  EXPECT_EQ(cache.Get("test"), 666);
}

TEST(ARCCache, MissingValue) {
  size_t cache_capacity = 1;
  arc_cache_t<std::string, int> cache(cache_capacity);

  EXPECT_THROW(cache.Get("test"), std::range_error);
}

TEST(ARCCache, KeepsAllValuesWithinCapacity) {
  constexpr int CACHE_CAPACITY = 50;
  const int TEST_RECORDS = 100;
  arc_cache_t<int, int> cache(CACHE_CAPACITY);

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

TEST(ARCCache, CheckVictim) {
  size_t cache_capacity = 3;
  arc_cache_t<int, std::string> cache(cache_capacity);

  cache.Put(1,"data1");
  cache.Put(2,"data2");
  cache.Put(3,"data3");
  cache.Put(4,"data4");

  EXPECT_EQ(cache.Size(), 3);

}

TEST(ARCCache, CheckPointerMove) {
  size_t cache_capacity = 3;
  arc_cache_t<int, std::string> cache(cache_capacity);

  cache.Put(1,"data1");
  cache.Put(2,"data2");
  cache.Put(3,"data3");

  // At this stage key4 moved from T1 to T2 and
  // T1 still bigger list and lru entry is key1;
  cache.Put(4, "data4");

  EXPECT_THROW(cache.Get(1), std::range_error);
}

TEST(ARCCache, CheckPointerMoveT2) {
  size_t cache_capacity = 3;
  arc_cache_t<int, std::string> cache(cache_capacity);

  cache.Put(1,"data1");
  cache.Put(2,"data2");
  cache.Put(3,"data3");

  cache.Put(3, "foo");
  cache.Put(3, "foo");

  // T2 list is bigger now and key1 have lower reference count
  cache.Put(1, "foo");

  cache.Put(4,"data4");

  EXPECT_THROW(cache.Get(1), std::range_error);
}

TEST(ARCCache, CheckPointerSaveB1) {
  size_t cache_capacity = 4;
  arc_cache_t<int, std::string> cache(cache_capacity);

  cache.Put(1,"data1");
  cache.Put(2,"data2");
  cache.Put(3,"data3");
  cache.Put(4,"data4");
  cache.Put(5,"data5");
  cache.Put(1,"data3");

  //key1 is restored from B1 and key2 moved to B1 (out of cache)
  EXPECT_THROW(cache.Get(2), std::range_error);
}

TEST(ARCCache, CheckPointerSaveB2) {
  size_t cache_capacity = 4;
  arc_cache_t<int, std::string> cache(cache_capacity);

  cache.Put(1,"data1");
  cache.Put(2,"data2");
  cache.Put(3,"data3");
  cache.Put(4,"data4");

  cache.Put(1, "foo");
  cache.Put(2, "foo");
  cache.Put(2, "foo");
  cache.Put(3, "foo");
  cache.Put(3, "foo");
  cache.Put(4, "foo");
  cache.Put(4, "foo");

  cache.Put(5,"data5");
  cache.Put(1,"data3");

  //key1 is restored from B1 and key2 moved to B1 (out of cache)
  EXPECT_THROW(cache.Get(2), std::range_error);
}


}  // End machine namespace
