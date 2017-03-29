// DEVICE SOURCE

#include "device.h"

namespace machine {

size_t scale_factor = 4;

size_t dram_device_size = 2 * scale_factor;
size_t nvm_device_size = 32 * scale_factor;
size_t ssd_device_size = 128 * scale_factor;
size_t hdd_device_size = 1024 * scale_factor;

size_t dram_read_latency = 1;
size_t nvm_read_latency = 5;
size_t ssd_read_latency = 10;
size_t hdd_read_latency = 50;

size_t dram_write_latency = 1;
size_t nvm_write_latency = 10;
size_t ssd_write_latency = 20;
size_t hdd_write_latency = 50;

// GET READ & WRITE LATENCY

size_t GetWriteLatency(DeviceType device_type){

  DLOG(INFO) << "WRITE :: " << DeviceTypeToString(device_type) << "\n";

  switch(device_type){
    case DEVICE_TYPE_DRAM:
      return dram_write_latency;

    case DEVICE_TYPE_NVM:
      return nvm_write_latency;

    case DEVICE_TYPE_SSD:
      return ssd_write_latency;

    default:
    case DEVICE_TYPE_INVALID:
      exit(EXIT_FAILURE);
  }
}

size_t GetReadLatency(DeviceType device_type){

  DLOG(INFO) << "READ :: " << DeviceTypeToString(device_type) << "\n";

  switch(device_type){
    case DEVICE_TYPE_DRAM:
      return dram_read_latency;

    case DEVICE_TYPE_NVM:
      return nvm_read_latency;

    case DEVICE_TYPE_SSD:
      return ssd_read_latency;

    default:
    case DEVICE_TYPE_INVALID:
      exit(EXIT_FAILURE);
  }
}

// LOCATE IN DEVICE

bool LocateInDevice(Device device,
                    const size_t& block_id){

  // Check device cache
  try{
    device.cache.Get(block_id);
    return true;
  }
  catch(const std::range_error& not_found){
    // Nothing to do here!
  }

  return false;
}

DeviceType LocateInDevices(std::vector<Device> devices,
                           const size_t& block_id){

  for(auto device : devices){
    auto found = LocateInDevice(device, block_id);
    if(found == true){
      return device.device_type;
    }
  }

  return DeviceType::DEVICE_TYPE_INVALID;
}

// GET DEVICE OFFSET

size_t GetDeviceOffset(std::vector<Device>& devices,
                       const DeviceType& device_type){

  size_t device_itr = 0;
  for(auto device : devices){
    if(device.device_type == device_type){
      return device_itr;
    }
    device_itr++;
  }

  std::cout << "Did not find device of type : " <<
      DeviceTypeToString(device_type);
  exit(EXIT_FAILURE);
}

// DEVICE EXISTS?

bool DeviceExists(std::vector<Device>& devices,
                  const DeviceType& device_type){
  for(auto device : devices){
    if(device.device_type == device_type){
      return true;
    }
  }
  return false;
}

// GET DEVICE LOWER IN THE HIERARCHY

DeviceType GetLowerDevice(std::vector<Device>& devices,
                          DeviceType source){
  DeviceType destination = DeviceType::DEVICE_TYPE_INVALID;
  auto nvm_exists = DeviceExists(devices, DeviceType::DEVICE_TYPE_NVM);

  switch(source){
    case DEVICE_TYPE_DRAM: {
      if(nvm_exists == true) {
        destination = DEVICE_TYPE_NVM;
      }
      else {
        destination = DEVICE_TYPE_SSD;
      }
      break;
    }

    case DEVICE_TYPE_NVM: {
      destination = DEVICE_TYPE_SSD;
      break;
    }

    default:
    case DEVICE_TYPE_INVALID:
      exit(EXIT_FAILURE);
  }

  return destination;
}

// COPY + MOVE VICTIM

void MoveVictim(std::vector<Device>& devices,
                DeviceType source,
                const size_t& block_id,
                const bool& is_clean,
                double& total_duration);

void Copy(std::vector<Device>& devices,
          DeviceType destination,
          DeviceType source,
          const size_t& block_id,
          double& total_duration){

  if(destination == DeviceType::DEVICE_TYPE_INVALID){
    exit(EXIT_FAILURE);
  }

  std::cout << DeviceTypeToString(source) << " ";
  std::cout << "---> " << DeviceTypeToString(destination) << "\n";

  // Write to destination device
  auto device_offset = GetDeviceOffset(devices, destination);
  auto device_cache = devices[device_offset].cache;
  auto victim = device_cache.Put(block_id, CLEAN_BLOCK);

  total_duration += GetReadLatency(source);
  total_duration += GetWriteLatency(destination);

  // Move victim
  auto victim_key = victim.block_id;
  auto victim_block_type = victim.block_type;
  MoveVictim(devices,
             destination,
             victim_key,
             (victim_block_type == CLEAN_BLOCK),
             total_duration);

}

void MoveVictim(std::vector<Device>& devices,
                DeviceType source,
                const size_t& block_id,
                const bool& is_clean,
                double& total_duration){

  bool victim_exists = (block_id != INVALID_KEY);
  bool volatile_device = (source == DeviceType::DEVICE_TYPE_DRAM);
  bool is_dirty = (is_clean == false);

  // Check if we have a dirty victim in DRAM
  if(victim_exists && volatile_device && is_dirty){
    auto destination = GetLowerDevice(devices, source);

    DLOG(INFO) << "Move victim : " << block_id << "\n";

    // Copy to device
    Copy(devices, destination, source, block_id, total_duration);
  }

}

// DEVICE FACTORY

Device DeviceFactory::GetDevice(const DeviceType& device_type,
                                const CachingType& caching_type,
                                const size_t& machine_size,
                                const DeviceType& last_device_type){

  switch (device_type){
    case DEVICE_TYPE_DRAM:
      return Device(DEVICE_TYPE_DRAM,
                    caching_type,
                    dram_device_size,
                    dram_read_latency,
                    dram_write_latency
      );

    case DEVICE_TYPE_NVM: {
      // Check for last device
      auto device_size = nvm_device_size;
      if(last_device_type == DEVICE_TYPE_NVM){
        device_size = machine_size;
      }
      return Device(DEVICE_TYPE_NVM,
                    caching_type,
                    device_size,
                    nvm_read_latency,
                    nvm_write_latency
      );
    }

    case DEVICE_TYPE_SSD:  {
      // Check for last device
      auto device_size = ssd_device_size;
      if(last_device_type == DEVICE_TYPE_SSD){
        device_size = machine_size;
      }
      return Device(DEVICE_TYPE_SSD,
                    caching_type,
                    device_size,
                    ssd_read_latency,
                    ssd_write_latency
      );
    }

    default:
      exit(EXIT_FAILURE);
  }

}


}  // End machine namespace

