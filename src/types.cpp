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
    case DEVICE_TYPE_HDD:
      return "HDD";
    default:
      return "INVALID";
  }

}

std::string HierarchyTypeToString(const HierarchyType& hierarchy_type){

  switch (hierarchy_type) {
    case HIERARCHY_TYPE_NVM:
      return "NVM";
    case HIERARCHY_TYPE_DRAM_NVM:
      return "DRAM-NVM";
    case HIERARCHY_TYPE_DRAM_NVM_SSD:
      return "DRAM-NVM-SSD";
    case HIERARCHY_TYPE_DRAM_NVM_SSD_HDD:
      return "DRAM-NVM-SSD-HDD";
    default:
      return "INVALID";
  }

}

std::string LoggingTypeToString(const LoggingType& logging_type){

  switch (logging_type) {
    case LOGGING_TYPE_WAL:
      return "WAL";
    case LOGGING_TYPE_WBL:
      return "WBL";
    default:
      return "INVALID";
  }

}

DeviceType GetLastDevice(const HierarchyType& hierarchy_type){

  switch (hierarchy_type) {
     case HIERARCHY_TYPE_NVM:
       return DeviceType::DEVICE_TYPE_NVM;
     case HIERARCHY_TYPE_DRAM_NVM:
       return DeviceType::DEVICE_TYPE_NVM;
     case HIERARCHY_TYPE_DRAM_NVM_SSD:
       return DeviceType::DEVICE_TYPE_SSD;
     case HIERARCHY_TYPE_DRAM_NVM_SSD_HDD:
       return DeviceType::DEVICE_TYPE_HDD;
     default:
       return DeviceType::DEVICE_TYPE_INVALID;
   }

}


}  // End machine namespace

