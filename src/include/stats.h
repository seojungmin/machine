// STATS HEADER

#pragma once

#include <map>

#include "types.h"

namespace machine {

// STATS

class Stats{

 public:

  void Reset();

  void IncrementReadCount(DeviceType device_type);

  void IncrementWriteCount(DeviceType device_type);

  friend std::ostream& operator<< (std::ostream& stream, const Stats& stats);

 private:

  // Read op count
  std::map<DeviceType, size_t> read_ops;

  // Write op count
  std::map<DeviceType, size_t> write_ops;

};

}  // End machine namespace
