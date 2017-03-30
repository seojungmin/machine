// DEVICE HEADER

#pragma once

#include "storage_cache.h"

namespace machine {

const size_t CLEAN_BLOCK = 100;
const size_t DIRTY_BLOCK = 101;

extern size_t scale_factor;

size_t GetWriteLatency(DeviceType device_type);

size_t GetReadLatency(DeviceType device_type);

struct Device {


  Device(const DeviceType& device_type,
         const CachingType& caching_type,
         const size_t& device_size,
         const size_t& read_latency,
         const size_t& write_latency)
  : device_type(device_type),
    device_size(device_size),
    read_latency(read_latency),
    write_latency(write_latency),
    cache(device_type, caching_type, device_size){
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

  // storage cache
  StorageCache cache;

};


void Copy(std::vector<Device>& devices,
          DeviceType destination,
          DeviceType source,
          const size_t& block_id,
          const size_t& block_status,
          double& total_duration);

DeviceType LocateInDevices(std::vector<Device> devices,
                           const size_t& block_id);

bool DeviceExists(std::vector<Device>& devices,
                  const DeviceType& device_type);

size_t GetDeviceOffset(std::vector<Device>& devices,
                       const DeviceType& device_type);

class DeviceFactory {
 public:
  DeviceFactory();
  virtual ~DeviceFactory();

  static Device GetDevice(const DeviceType& device_type,
                          const CachingType& caching_type,
                          const size_t& machine_size,
                          const DeviceType& last_device_type);

};

}  // End machine namespace
