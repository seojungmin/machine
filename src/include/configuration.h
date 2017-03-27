// CONFIGURATION HEADER

#pragma once

#include <getopt.h>
#include <sys/time.h>
#include <iostream>
#include <string>
#include <vector>

#include "types.h"
#include "device.h"

namespace machine {

static const int generator_seed = 50;

class configuration {
 public:

  // hierarchy type
  HierarchyType hierarchy_type;

  // list of devices in hierarchy
  std::vector<Device> devices;

  // list of memory devices in hierarchy
  std::vector<Device> memory_devices;

  // list of storage devices in hierarchy
  std::vector<Device> storage_devices;

  // directly access nvm ?
  bool direct_nvm;

  // logging type
  LoggingType logging_type;

  // migration type
  MigrationType migration_type;

  // caching type
  CachingType caching_type;

  // upwards migration frequency
  size_t migration_frequency;

  // Verbose output
  bool verbose;

};

extern size_t dram_device_size;
extern size_t nvm_device_size;
extern size_t ssd_device_size;
extern size_t hdd_device_size;

extern size_t dram_read_latency;
extern size_t nvm_read_latency;
extern size_t ssd_read_latency;
extern size_t hdd_read_latency;

extern size_t dram_write_latency;
extern size_t nvm_write_latency;
extern size_t ssd_write_latency;
extern size_t hdd_write_latency;

void Usage(FILE *out);

void ParseArguments(int argc, char *argv[], configuration &state);

}  // namespace machine
