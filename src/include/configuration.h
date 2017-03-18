// CONFIGURATION HEADER

#pragma once

#include <getopt.h>
#include <sys/time.h>
#include <iostream>
#include <string>
#include <vector>

namespace machine {

enum HierarchyType {
  HIERARCHY_TYPE_INVALID = 0,

  HIERARCHY_TYPE_DRAM_NVM = 1,
  HIERARCHY_TYPE_DRAM_NVM_SSD = 2,
  HIERARCHY_TYPE_DRAM_NVM_SSD_HDD = 3

};

enum DeviceType {
  DEVICE_TYPE_INVALID = 0,

  DEVICE_TYPE_DRAM = 1,
  DEVICE_TYPE_NVM = 2,
  DEVICE_TYPE_SSD = 3,
  DEVICE_TYPE_HDD = 4
};

struct Device {

  Device(const DeviceType& device_type,
         const size_t& device_size,
         const size_t& read_latency,
         const size_t& write_latency)
  : device_type(device_type),
    device_size(device_size),
    read_latency(read_latency),
    write_latency(write_latency) {
    // Nothing to do here!
  }

  // type of the device
  DeviceType device_type = DEVICE_TYPE_INVALID;

  // size of the device (in pages)
  size_t device_size = 0;

  // read latency
  size_t read_latency = 0;

  // write latency
  size_t write_latency = 0;

};

enum LoggingType {
  LOGGING_TYPE_INVALID = 0,

  LOGGING_TYPE_WAL = 1,
  LOGGING_TYPE_WBL = 2

};

enum MigrationType {
  MIGRATION_TYPE_INVALID = 0,

  MIGRATION_TYPE_DOWNWARDS = 1,
  MIGRATION_TYPE_BOTHWAYS = 2

};

static const int generator_seed = 50;

class configuration {
 public:

  // hierarchy type
  HierarchyType hierarchy_type;

  // list of devices in hierarchy
  std::vector<Device> devices;

  // directly access nvm ?
  bool direct_nvm;

  // logging type
  LoggingType logging_type;

  // migration type
  MigrationType migration_type;

  // Verbose output
  bool verbose;

};

void Usage(FILE *out);

void ParseArguments(int argc, char *argv[], configuration &state);

}  // namespace machine
