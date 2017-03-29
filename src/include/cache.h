// CACHE HEADER

#pragma once

#include <mutex>

#include "policy.h"

#include "policy_arc.h"
#include "policy_fifo.h"
#include "policy_lfu.h"
#include "policy_lru.h"

namespace machine {

#define INVALID_KEY INT32_MAX

struct Block{
  size_t block_id;
  size_t block_type;
};

// Base class for all caching algorithms
template <typename Key, typename Value, typename Policy>
class Cache {
 public:

  using map = std::unordered_map<Key, Value>;
  using iterator = typename map::iterator;
  using const_iterator = typename map::const_iterator;
  using operation_guard = typename std::lock_guard<std::mutex>;

  Cache(size_t capacity);

  // Returns victim block
  Block Put(const Key& key, const Value& value);

  const Value& Get(const Key& key, bool touch = true) const;

  void Erase(const Key& key);

  size_t CurrentCapacity() const;

  void Print() const;

 protected:

  void Insert(const Key& key, const Value& value);

  void Update(const Key& key, const Value& value);

  const_iterator LocateEntry(const Key& key) const;

 private:
  std::unordered_map<Key, Value> cache_items_map;

  mutable Policy cache_policy_;

  mutable std::mutex cache_mutex_;

  size_t capacity_;

};

}  // End machine namespace
