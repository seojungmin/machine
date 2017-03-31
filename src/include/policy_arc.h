// ARC HEADER

#pragma once

#include <deque>
#include <vector>
#include <map>
#include <algorithm>

#include <glog/logging.h>

#include "macros.h"
#include "policy.h"

namespace machine {

#define MAX(a,b) (((a)>(b))?(a):(b))

template <typename Key>
class ARCCachePolicy : public ICachePolicy<Key> {
 public:

  ARCCachePolicy(const size_t& capacity)
 : capacity(capacity),
   p(0) {
    // Nothing to do here!
  }

  ~ARCCachePolicy() = default;

  void Insert(const Key& key) override {

    DLOG(INFO) << "ARC INSERT : " << key << "\n";

    if(Contains(key_cache, key)){
      return;
    }

    key_cache[key] = 1;

    if(Contains(B1, key)){
      size_t size_ratio = B2.size()/B1.size();
      size_t b_ratio = MAX(size_ratio, 1);
      p = std::min(capacity, p + b_ratio);
      Erase(key);
      Erase(B1, key);
      T2.push_front(key);
    }
    else if(Contains(B2, key)){
      size_t size_ratio = B1.size()/B2.size();
      size_t b_ratio = MAX(size_ratio, 1);
      p = MAX(0, p - b_ratio);
      Erase(key);
      Erase(B2, key);
      T2.push_front(key);
    }
    else if(T1.size() + B1.size() == capacity){
      if(T1.size() < capacity){
        B1.pop_back();
        Erase(key);
      }
      else {
        Erase(key_cache,T1.back());
        T1.pop_back();
      }
    }
    else {
      auto total = T1.size() + T2.size() + B1.size() + B2.size();
      if(total >= capacity){
        if(total == 2 * capacity){
          B2.pop_back();
        }
        Erase(key);
      }
    }

    T1.push_front(key);
  }

  void Touch(const Key& key) override {

    DLOG(INFO) << "ARC TOUCH : " << key << "\n";

    if (Contains(T1, key)) {
      Erase(T1, key);
      T2.push_front(key);
    }
    else if (Contains(T2, key)){
      Erase(T2, key);
      T2.push_front(key);
    }

  }

  void Erase(const Key& key) override {

    DLOG(INFO) << "ARC ERASE : " << key << "\n";

    bool T1_not_empty = (T1.empty() == false);
    bool in_B2 = Contains(B2, key);
    bool len_T1_eq_P = (T1.size() == p);
    bool len_T1_gt_P = (T1.size() > p);

    if(T1_not_empty && ((in_B2 && len_T1_eq_P) || len_T1_gt_P)){
      auto victim = T1.back();
      T1.pop_back();
      B1.push_front(victim);
    }
    else {
      auto victim = T2.back();
      T2.pop_back();
      B2.push_front(victim);
    }

    Erase(key_cache, key);

  }

  // return a key of a displacement candidate
  const Key& Victim(const Key& key) const override {

    bool T1_not_empty = (T1.empty() == false);
    bool in_B2 = Contains(B2, key);
    bool len_T1_eq_P = (T1.size() == p);
    bool len_T1_gt_P = (T1.size() > p);

    if(T1_not_empty && ((in_B2 && len_T1_eq_P) || len_T1_gt_P)){
      return T1.back();
    }
    else {
      return T2.back();
    }

  }

  bool Contains(const std::deque<Key>& deque, const Key& key) const {
    if (std::find(deque.begin(), deque.end(), key) != deque.end()) {
      return true;
    }
    return false;
  }

  bool Contains(const std::map<Key,int>& map, const Key& key) const {
    if (map.count(key) != 0) {
      return true;
    }
    return false;
  }

  void Erase(std::deque<Key>& deque, const Key& key){
    std::remove(deque.begin(), deque.end(), key);
  }

  void Erase(std::map<Key,int>& map, const Key& key){
    map.erase(key);
  }

 private:

  std::deque<Key> T1;
  std::deque<Key> B1;

  std::deque<Key> T2;
  std::deque<Key> B2;

  std::map<Key,int> key_cache;

  // capacity of cache
  size_t capacity;

  // marker
  size_t p;

};

}  // End machine namespace
