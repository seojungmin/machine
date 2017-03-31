// ARC HEADER

#pragma once

#include <set>

#include <glog/logging.h>

#include "macros.h"
#include "policy.h"
#include "policy_lfu.h"
#include "policy_lru.h"

namespace machine {

template <typename Key>
class ARCCachePolicy : public ICachePolicy<Key> {
 public:
  using arc_iterator = typename std::list<Key>::const_iterator;

  ARCCachePolicy(const size_t& capacity)
  : T1(LRUCachePolicy<Key>(capacity)),
    B1(LRUCachePolicy<Key>(capacity)),
    T2(LFUCachePolicy<Key>(capacity)),
    B2(LFUCachePolicy<Key>(capacity)),
    capacity(capacity) {

    // Nothing to do here!
  }

  ~ARCCachePolicy() = default;

  void Insert(const Key& key) override {

    DLOG(INFO) << "ARC INSERT : " << key << "\n";

    if (B1Entries.find(key) != B1Entries.end()) {

      //Check whether we are outsized
      B1Entries.erase(key);
      B1.Erase(key);

      T1Entries.insert(key);
      T1.Insert(key);

    }
    else if (B2Entries.find(key) != B2Entries.end()) {

      B2.Erase(key);
      B2Entries.erase(key);

      T2Entries.insert(key);
      T2.Insert(key);

    }
    else {

      //Completely new!
      T1Entries.insert(key);
      T1.Insert(key);

    }

  }

  void Touch(const Key& key) override {

    DLOG(INFO) << "ARC TOUCH : " << key << "\n";

    if (T1Entries.find(key) != T1Entries.end()) {

      T1Entries.erase(key);
      DLOG(INFO) << "ERASE FROM LRU : " << key;
      T1.Erase(key);

      T2Entries.insert(key);
      T2.Insert(key);

    }
    else {
      T2.Touch(key);
    }

  }

  void Erase(UNUSED_ATTRIBUTE const Key& key) override {

    DLOG(INFO) << "ARC ERASE : " << key << "\n";

    if (T1Entries.find(key) != T1Entries.end()) {
      DLOG(INFO) << "LRU FOUND : " << key;

      B1.Insert(key);
      B1Entries.insert(key);

      if (B1Entries.size() >= capacity/2) {
        auto victim = B1.Victim();
        B1.Erase(victim);
        B1Entries.erase(victim);
      }

      T1Entries.erase(key);
      DLOG(INFO) << "ERASE FROM LRU : " << key;
      T1.Erase(key);
    }
    else if (T2Entries.find(key) != T2Entries.end()) {
      DLOG(INFO) << "LFU FOUND : " << key;

      B2.Insert(key);
      B2Entries.insert(key);

      if (B2Entries.size() >= capacity/2) {
        auto victim = B2.Victim();
        B2.Erase(victim);
        B2Entries.erase(victim);
      }

      T2Entries.erase(key);
      DLOG(INFO) << "ERASE FROM LFU : " << key;
      T2.Erase(key);
    }

  }

  // return a key of a displacement candidate
  const Key& Victim() const override {

    if (T1Entries.size() > T2Entries.size()) {
      DLOG(INFO) << "ARC-LRU VICTIM : " << T1.Victim();
      return T1.Victim();
    } else {
      DLOG(INFO) << "ARC-LFU VICTIM : " << T2.Victim();
      return T2.Victim();
    }

  }

 private:

  LRUCachePolicy<Key> T1;
  std::set<Key> T1Entries;
  LRUCachePolicy<Key> B1;
  std::set<Key> B1Entries;

  LFUCachePolicy<Key> T2;
  std::set<Key> T2Entries;
  LFUCachePolicy<Key> B2;
  std::set<Key> B2Entries;

  // capacity of cache
  size_t capacity;

};

}  // End machine namespace
