// DEVICE HEADER

#pragma once

#include "storage_cache.h"

namespace machine {

extern size_t scale_factor;

size_t GetWriteLatency(DeviceType device_type);

size_t GetReadLatency(DeviceType device_type);

struct Device {


  Device(const DeviceType& device_type,
         const CachingType& caching_type,
         const size_t& device_size)
  : device_type(device_type),
    device_size(device_size),
    cache(device_type, caching_type, device_size){
    // Nothing to do here!
  }

  // type of the device
  DeviceType device_type = DEVICE_TYPE_INVALID;

  // size of the device (in pages)
  size_t device_size = 0;

  // storage cache
  StorageCache cache;

};

void BootstrapDeviceMetrics();

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
