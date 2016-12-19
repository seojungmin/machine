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
CACHE_TEMPLATE_TYPE::Cache(size_t max_size,
                           const Policy& policy)
: cache_policy(policy),
  max_cache_size{max_size} {

  if (max_cache_size == 0) {
    max_cache_size = std::numeric_limits<size_t>::max();
  }

}

CACHE_TEMPLATE_ARGUMENT
void CACHE_TEMPLATE_TYPE::Put(const Key& key,
                              const Value& value) {

  operation_guard{safe_op};
  auto elem_it = FindElem(key);

  if (elem_it == cache_items_map.end()) {
    // add new element to the cache
    if (Size() + 1 > max_cache_size) {
      auto disp_candidate_key = cache_policy.Victim();

      Erase(disp_candidate_key);
    }

    Insert(key, value);
  } else {
    // update previous value
    Update(key, value);
  }

}

CACHE_TEMPLATE_ARGUMENT
const Value& CACHE_TEMPLATE_TYPE::Get(const Key& key) const {

  operation_guard{safe_op};
  auto elem_it = FindElem(key);

  if (elem_it == cache_items_map.end()) {
    throw std::range_error{"No such element in the cache"};
  }
  cache_policy.Touch(key);

  return elem_it->second;

}

CACHE_TEMPLATE_ARGUMENT
size_t CACHE_TEMPLATE_TYPE::Size() const {

  operation_guard{safe_op};

  return cache_items_map.size();
}


CACHE_TEMPLATE_ARGUMENT
void CACHE_TEMPLATE_TYPE::Insert(const Key& key,
                                 const Value& value) {

  cache_policy.Insert(key);
  cache_items_map.emplace(std::make_pair(key, value));

}

CACHE_TEMPLATE_ARGUMENT
void CACHE_TEMPLATE_TYPE::Erase(const Key& key) {

  cache_policy.Erase(key);
  cache_items_map.erase(key);

}

CACHE_TEMPLATE_ARGUMENT
void CACHE_TEMPLATE_TYPE::Update(const Key& key,
                                 const Value& value) {

  cache_policy.Touch(key);
  cache_items_map[key] = value;

}

CACHE_TEMPLATE_ARGUMENT
typename CACHE_TEMPLATE_TYPE::const_iterator
CACHE_TEMPLATE_TYPE::FindElem(const Key& key) const {

  return cache_items_map.find(key);

}

// Instantiations

// LRU
template class Cache<int, int, LRUCachePolicy<int>>;
template class Cache<std::string, int, LRUCachePolicy<std::string>>;

// LFU
template class Cache<int, int, LFUCachePolicy<int>>;
template class Cache<std::string, int, LFUCachePolicy<std::string>>;

// FIFO
template class Cache<int, int, FIFOCachePolicy<int>>;
template class Cache<std::string, int, FIFOCachePolicy<std::string>>;

// ARC
template class Cache<int, int, ARCCachePolicy<int>>;
template class Cache<std::string, int, ARCCachePolicy<std::string>>;


}  // End machine namespace

