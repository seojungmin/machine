// TYPES SOURCE

#include "types.h"

namespace machine {

std::string CachingTypeToString(const CachingType& caching_type){

  switch (caching_type){
    case CACHING_TYPE_FIFO:
      return "FIFO";
    case CACHING_TYPE_LRU:
      return "LRU";
    case CACHING_TYPE_LFU:
      return "LFU";
    case CACHING_TYPE_ARC:
      return "ARC";
    default:
      return "INVALID";
  }

}

std::string DeviceTypeToString(const DeviceType& device_type){

  switch (device_type){
    case DEVICE_TYPE_DRAM:
      return "DRAM";
    case DEVICE_TYPE_NVM:
      return "NVM";
    case DEVICE_TYPE_SSD:
      return "SSD";
    default:
      return "INVALID";
  }

}

}  // End machine namespace

