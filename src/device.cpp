// DEVICE SOURCE

#include "device.h"

namespace machine {

size_t dram_device_size = 50/scale_factor;
size_t nvm_device_size = 200/scale_factor;
size_t ssd_device_size = 1000/scale_factor;
size_t hdd_device_size = 5000/scale_factor;

size_t dram_read_latency = 10;
size_t nvm_read_latency = 50;
size_t ssd_read_latency = 100;
size_t hdd_read_latency = 500;

size_t dram_write_latency = 10;
size_t nvm_write_latency = 50;
size_t ssd_write_latency = 100;
size_t hdd_write_latency = 500;

size_t GetWriteLatency(DeviceType device_type){
  switch(device_type){
    case DEVICE_TYPE_DRAM:
      return dram_write_latency;

    case DEVICE_TYPE_NVM:
      return nvm_write_latency;

    case DEVICE_TYPE_SSD:
      return ssd_write_latency;

    case DEVICE_TYPE_HDD:
      return hdd_write_latency;

    default:
    case DEVICE_TYPE_INVALID:
      exit(EXIT_FAILURE);
  }
}

size_t GetReadLatency(DeviceType device_type){
  switch(device_type){
    case DEVICE_TYPE_DRAM:
      return dram_read_latency;

    case DEVICE_TYPE_NVM:
      return nvm_read_latency;

    case DEVICE_TYPE_SSD:
      return ssd_read_latency;

    case DEVICE_TYPE_HDD:
      return hdd_read_latency;

    default:
    case DEVICE_TYPE_INVALID:
      exit(EXIT_FAILURE);
  }
}

Device DeviceFactory::GetDevice(const DeviceType& device_type,
                                const CachingType& caching_type){

  switch (device_type){
    case DEVICE_TYPE_DRAM:
      return Device(DEVICE_TYPE_DRAM,
                    caching_type,
                    dram_device_size,
                    dram_read_latency,
                    dram_write_latency
      );

    case DEVICE_TYPE_NVM:
      return Device(DEVICE_TYPE_NVM,
                    caching_type,
                    nvm_device_size,
                    nvm_read_latency,
                    nvm_write_latency
      );

    case DEVICE_TYPE_SSD:
      return Device(DEVICE_TYPE_SSD,
                    caching_type,
                    ssd_device_size,
                    ssd_read_latency,
                    ssd_write_latency
      );

    case DEVICE_TYPE_HDD:
      return Device(DEVICE_TYPE_HDD,
                    caching_type,
                    hdd_device_size,
                    hdd_read_latency,
                    hdd_write_latency
      );

    default:
      exit(EXIT_FAILURE);
  }

}


}  // End machine namespace

