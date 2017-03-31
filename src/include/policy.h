// CACHE POLICY HEADER

#pragma once

#include <unordered_set>

#include "macros.h"

namespace machine {

template <typename Key>
class ICachePolicy {
 public:

  virtual ~ICachePolicy() {}

  // handle element insertion in a cache
  virtual void Insert(const Key& key) = 0;

  // handle request to the key-element in a cache
  virtual void Touch(const Key& key) = 0;

  // handle element deletion from a cache
  virtual void Erase(const Key& key) = 0;

  // return a key of a replacement candidate
  virtual const Key& Victim(const Key& key) const = 0;

};

}  // End machine namespace
