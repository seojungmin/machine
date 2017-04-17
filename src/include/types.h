// TYPES HEADER

#pragma once

#include <string>

namespace machine {

enum HierarchyType {
  HIERARCHY_TYPE_INVALID = 0,

  HIERARCHY_TYPE_NVM = 1,
  HIERARCHY_TYPE_DRAM_NVM = 2,
  HIERARCHY_TYPE_DRAM_SSD = 3,
  HIERARCHY_TYPE_DRAM_NVM_SSD = 4

};

enum SizeType {
  SIZE_TYPE_INVALID = 0,

  SIZE_TYPE_1 = 1,
  SIZE_TYPE_2 = 2,
  SIZE_TYPE_3 = 3,
  SIZE_TYPE_4 = 4

};

enum CachingType {
  CACHING_TYPE_INVALID = 0,

  CACHING_TYPE_FIFO = 1,
  CACHING_TYPE_LRU = 2,
  CACHING_TYPE_LFU = 3,
  CACHING_TYPE_ARC = 4

};

enum DeviceType {
  DEVICE_TYPE_INVALID = 0,

  DEVICE_TYPE_DRAM = 1,
  DEVICE_TYPE_NVM = 2,
  DEVICE_TYPE_SSD = 3

};

enum LoggingType {
  LOGGING_TYPE_INVALID = 0,

  LOGGING_TYPE_WAL = 1,
  LOGGING_TYPE_WBL = 2

};

DeviceType GetLastDevice(const HierarchyType& hierarchy_type);

std::string HierarchyTypeToString(const HierarchyType& hierarchy_type);

std::string SizeTypeToString(const SizeType& size_type);

std::string CachingTypeToString(const CachingType& caching_type);

std::string DeviceTypeToString(const DeviceType& device_type);

std::string LoggingTypeToString(const LoggingType& logging_type);


}  // End machine namespace
