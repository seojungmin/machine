// LFU HEADER

#pragma once

#include <cstddef>
#include <unordered_map>
#include <map>
#include <iostream>

#include "macros.h"
#include "policy.h"

namespace machine {

template <typename Key>
class LFUCachePolicy : public ICachePolicy<Key> {
 public:
  using lfu_iterator = typename std::multimap<std::size_t, Key>::iterator;

  LFUCachePolicy(UNUSED_ATTRIBUTE const size_t& capacity){
    // Nothing to do here!
  }

  ~LFUCachePolicy() override = default;

  void Insert(const Key& key) override {

    constexpr std::size_t INIT_VAL = 1;

    DLOG(INFO) << "LFU INSERT: " << key << "\n";

    // all new value initialized with the frequency 1
    lfu_storage[key] = frequency_storage.emplace_hint(frequency_storage.cbegin(),
                                                      INIT_VAL,
                                                      key);

  }

  void Touch(const Key& key) override {

    DLOG(INFO) << "LFU TOUCH: " << key << "\n";

    if(lfu_storage.count(key) == 0){
      DLOG(INFO) << "KEY NOT FOUND: " << key << "\n";
      return;
    }

    // get the previous frequency value of a key
    auto elem_for_update = lfu_storage[key];
    auto updated_elem = std::make_pair(elem_for_update->first + 1,
                                       elem_for_update->second);

    // update the previous value
    frequency_storage.erase(elem_for_update);
    lfu_storage[key] = frequency_storage.emplace_hint(frequency_storage.cend(),
                                                      std::move(updated_elem));

  }

  void Erase(const Key& key) override {

    DLOG(INFO) << "LFU ERASE : " << key << "\n";

    if(lfu_storage.count(key) == 0){
      DLOG(INFO) << "KEY NOT FOUND: " << key << "\n";
      return;
    }

    frequency_storage.erase(lfu_storage[key]);
    lfu_storage.erase(key);

  }

  const Key& Victim() const override {

    // at the beginning of the frequency_storage we have the
    // least frequency used value
    auto& victim = frequency_storage.cbegin()->second;
    DLOG(INFO) << "LFU VICTIM: " << victim << "\n";

    return victim;
  }

 private:

  std::multimap<std::size_t, Key> frequency_storage;

  std::unordered_map<Key, lfu_iterator> lfu_storage;

};

}  // End machine namespace
