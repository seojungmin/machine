// FIFO HEADER

#pragma once

#include <list>

#include "macros.h"
#include "cache_policy.h"

namespace machine {

template <typename Key>
class FIFOCachePolicy : public ICachePolicy<Key> {
 public:
  FIFOCachePolicy() = default;
  ~FIFOCachePolicy() = default;

  void Insert(const Key& key) override {

    fifo_queue.emplace_front(key);

  }

  // handle request to the key-element in a cache
  void Touch(UNUSED_ATTRIBUTE const Key& key) override {

    // nothing to do here in the FIFO strategy

  }

  // handle element deletion from a cache
  void Erase(UNUSED_ATTRIBUTE const Key& key) override {

    fifo_queue.pop_back();

  }

  // return a key of a replacement candidate
  const Key& Victim() const override {

    return fifo_queue.back();

  }

 private:

  std::list<Key> fifo_queue;

};

}  // End machine namespace
