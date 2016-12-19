// LRU POLICY HEADER

#pragma once

#include "policy.h"

namespace machine {

// Base class for all caching algorithms
template <typename Key, typename Value, typename Policy>
class Cache {
 public:

  using map = std::unordered_map<Key, Value>;
  using iterator = typename map::iterator;
  using const_iterator = typename map::const_iterator;
  using operation_guard = typename std::lock_guard<std::mutex>;

  Cache(size_t capacity);

  void Put(const Key& key, const Value& value);

  const Value& Get(const Key& key) const;

  size_t Size() const;

 protected:

  void Insert(const Key& key, const Value& value);

  void Erase(const Key& key);

  void Update(const Key& key, const Value& value);

  const_iterator FindElem(const Key& key) const;

 private:
  std::unordered_map<Key, Value> cache_items_map;

  mutable Policy cache_policy_;

  mutable std::mutex cache_mutex_;

  size_t capacity_;

};

}  // End machine namespace
