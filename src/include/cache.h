// LRU POLICY HEADER

#pragma once

#include <mutex>

#include "policy.h"

#include "policy_arc.h"
#include "policy_fifo.h"
#include "policy_lfu.h"
#include "policy_lru.h"

namespace machine {

#define INVALID_KEY INT32_MAX

enum CachingType {
  CACHING_TYPE_INVALID = 0,

  CACHING_TYPE_FIFO = 1,
  CACHING_TYPE_LRU = 2,
  CACHING_TYPE_LFU = 3,
  CACHING_TYPE_ARC = 4

};

enum DeviceType {
  DEVICE_TYPE_INVALID = 0,

  DEVICE_TYPE_DRAM = 1,
  DEVICE_TYPE_NVM = 2,
  DEVICE_TYPE_SSD = 3

};

std::string CachingTypeToString(const CachingType& caching_type);

std::string DeviceTypeToString(const DeviceType& device_type);

// Base class for all caching algorithms
template <typename Key, typename Value, typename Policy>
class Cache {
 public:

  using map = std::unordered_map<Key, Value>;
  using iterator = typename map::iterator;
  using const_iterator = typename map::const_iterator;
  using operation_guard = typename std::lock_guard<std::mutex>;

  Cache(size_t capacity);

  Key Put(const Key& key, const Value& value);

  const Value& Get(const Key& key) const;

  size_t CurrentCapacity() const;

  void Print();

 protected:

  void Insert(const Key& key, const Value& value);

  void Erase(const Key& key);

  void Update(const Key& key, const Value& value);

  const_iterator LocateEntry(const Key& key) const;

 private:
  std::unordered_map<Key, Value> cache_items_map;

  mutable Policy cache_policy_;

  mutable std::mutex cache_mutex_;

  size_t capacity_;

};

class StorageCache {

 public:

  StorageCache(DeviceType device_type,
               CachingType caching_type,
               size_t capacity);

  int Put(const int& key, const int& value);

  const int& Get(const int& key) const;

  size_t CurrentCapacity() const;

  friend std::ostream& operator<< (std::ostream& stream,
                                   const StorageCache& cache);

  DeviceType device_type_ = DeviceType::DEVICE_TYPE_INVALID;

  CachingType caching_type_ = CachingType::CACHING_TYPE_INVALID;

  Cache<int, int, FIFOCachePolicy<int>>* fifo_cache = nullptr;

  Cache<int, int, LRUCachePolicy<int>>* lru_cache = nullptr;

  Cache<int, int, LFUCachePolicy<int>>* lfu_cache = nullptr;

  Cache<int, int, ARCCachePolicy<int>>* arc_cache = nullptr;

};


}  // End machine namespace
