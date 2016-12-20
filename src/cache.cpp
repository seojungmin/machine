// CACHE SOURCE

#include <iostream>
#include <cstddef>
#include <limits>
#include <memory>
#include <mutex>
#include <unordered_map>

#include "cache.h"
#include "policy_arc.h"
#include "policy_fifo.h"
#include "policy_lfu.h"
#include "policy_lru.h"

namespace machine {

#define CACHE_TEMPLATE_ARGUMENT \
    template <typename Key, typename Value, typename Policy>

#define CACHE_TEMPLATE_TYPE \
    Cache<Key, Value, Policy>

CACHE_TEMPLATE_ARGUMENT
CACHE_TEMPLATE_TYPE::Cache(size_t capacity)
: cache_policy_(Policy(capacity)),
  capacity_{capacity} {

  PL_ASSERT(capacity_ > 0);

}

CACHE_TEMPLATE_ARGUMENT
void CACHE_TEMPLATE_TYPE::Put(const Key& key,
                              const Value& value) {

  operation_guard{cache_mutex_};
  auto entry_location = LocateEntry(key);

  if (entry_location == cache_items_map.end()) {

    // add new element to the cache
    if (CurrentCapacity() + 1 > capacity_) {
      auto disp_candidate_key = cache_policy_.Victim();
      Erase(disp_candidate_key);
    }

    Insert(key, value);

  }
  else {

    // update previous value
    Update(key, value);

  }

}

CACHE_TEMPLATE_ARGUMENT
const Value& CACHE_TEMPLATE_TYPE::Get(const Key& key) const {

  operation_guard{cache_mutex_};
  auto elem_it = LocateEntry(key);

  if (elem_it == cache_items_map.end()) {
    throw std::range_error{"No such element in the cache"};
  }

  cache_policy_.Touch(key);

  return elem_it->second;

}

CACHE_TEMPLATE_ARGUMENT
size_t CACHE_TEMPLATE_TYPE::CurrentCapacity() const {

  operation_guard{cache_mutex_};

  return cache_items_map.size();
}


CACHE_TEMPLATE_ARGUMENT
void CACHE_TEMPLATE_TYPE::Insert(const Key& key,
                                 const Value& value) {

  cache_policy_.Insert(key);
  cache_items_map.emplace(std::make_pair(key, value));

}

CACHE_TEMPLATE_ARGUMENT
void CACHE_TEMPLATE_TYPE::Erase(const Key& key) {

  cache_policy_.Erase(key);
  cache_items_map.erase(key);

}

CACHE_TEMPLATE_ARGUMENT
void CACHE_TEMPLATE_TYPE::Update(const Key& key,
                                 const Value& value) {

  cache_policy_.Touch(key);
  cache_items_map[key] = value;

}

CACHE_TEMPLATE_ARGUMENT
typename CACHE_TEMPLATE_TYPE::const_iterator
CACHE_TEMPLATE_TYPE::LocateEntry(const Key& key) const {

  return cache_items_map.find(key);

}

// Instantiations

// LRU
template class Cache<int, int, LRUCachePolicy<int>>;
template class Cache<std::string, int, LRUCachePolicy<std::string>>;
template class Cache<int, std::string, LRUCachePolicy<int>>;

// LFU
template class Cache<int, int, LFUCachePolicy<int>>;
template class Cache<std::string, int, LFUCachePolicy<std::string>>;
template class Cache<int, std::string, LFUCachePolicy<int>>;

// FIFO
template class Cache<int, int, FIFOCachePolicy<int>>;
template class Cache<std::string, int, FIFOCachePolicy<std::string>>;
template class Cache<int, std::string, FIFOCachePolicy<int>>;

// ARC
template class Cache<int, int, ARCCachePolicy<int>>;
template class Cache<std::string, int, ARCCachePolicy<std::string>>;
template class Cache<int, std::string, ARCCachePolicy<int>>;


}  // End machine namespace

