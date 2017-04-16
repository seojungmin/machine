// DEVICE SOURCE

#include "macros.h"
#include "device.h"

namespace machine {

std::map<DeviceType, size_t> device_size;
std::map<DeviceType, size_t> seq_read_latency;
std::map<DeviceType, size_t> seq_write_latency;
std::map<DeviceType, size_t> rnd_read_latency;
std::map<DeviceType, size_t> rnd_write_latency;

size_t nvm_read_scale_factor = 10;
size_t nvm_write_scale_factor = 10;

void BootstrapDeviceMetrics(){

  // SIZES (4K blocks)

  device_size[DEVICE_TYPE_DRAM] = (128 * 1000)/4;
  device_size[DEVICE_TYPE_NVM] = (1024 * 1000)/4;
  device_size[DEVICE_TYPE_SSD] = (32 * 1024 * 1000)/4;

  // LATENCIES (us)

  seq_read_latency[DEVICE_TYPE_DRAM] = 0.4;
  seq_read_latency[DEVICE_TYPE_NVM] = seq_read_latency[DEVICE_TYPE_DRAM] * nvm_read_scale_factor;
  seq_read_latency[DEVICE_TYPE_SSD] = 100;

  seq_write_latency[DEVICE_TYPE_DRAM] = 0.6;
  seq_write_latency[DEVICE_TYPE_NVM] = seq_write_latency[DEVICE_TYPE_DRAM] * nvm_write_scale_factor;
  seq_write_latency[DEVICE_TYPE_SSD] = 250;

  rnd_read_latency[DEVICE_TYPE_DRAM] = 0.5;
  rnd_read_latency[DEVICE_TYPE_NVM] = rnd_read_latency[DEVICE_TYPE_DRAM] * nvm_read_scale_factor;
  rnd_read_latency[DEVICE_TYPE_SSD] = 100;

  rnd_write_latency[DEVICE_TYPE_DRAM] = 0.7;
  rnd_write_latency[DEVICE_TYPE_NVM] = rnd_write_latency[DEVICE_TYPE_DRAM] * nvm_write_scale_factor;
  rnd_write_latency[DEVICE_TYPE_SSD] = 400;

}

bool IsSequential(std::vector<Device>& devices,
                  const DeviceType& device_type,
                  const size_t& next);

std::string GetPattern(bool is_sequential){
  if(is_sequential == true){
    return "SEQ";
  }
  return "RND";
}

// GET READ & WRITE LATENCY

size_t GetWriteLatency(std::vector<Device>& devices,
                       DeviceType device_type,
                       const size_t& block_id){

  DLOG(INFO) << "WRITE :: " << DeviceTypeToString(device_type) << "\n";
  bool is_sequential = IsSequential(devices, device_type, block_id);
  //if(is_sequential == true) {
  //  std::cout << DeviceTypeToString(device_type) << " " << GetPattern(is_sequential) << "\n";
  //}

  switch(device_type){
    case DEVICE_TYPE_DRAM:
    case DEVICE_TYPE_NVM:
    case DEVICE_TYPE_SSD: {
      if(is_sequential == true){
        return seq_write_latency[device_type];
      }
      else {
        return rnd_write_latency[device_type];
      }
    }

    case DEVICE_TYPE_INVALID:
      return 0;

    default: {
      std::cout << "Get invalid device";
      exit(EXIT_FAILURE);
    }
  }
}

size_t GetReadLatency(std::vector<Device>& devices,
                      DeviceType device_type,
                      const size_t& block_id){

  DLOG(INFO) << "READ :: " << DeviceTypeToString(device_type) << "\n";
  bool is_sequential = IsSequential(devices, device_type, block_id);
  //if(is_sequential == true) {
  //  std::cout << DeviceTypeToString(device_type) << " " << GetPattern(is_sequential) << "\n";
  //}

  switch(device_type){
    case DEVICE_TYPE_DRAM:
    case DEVICE_TYPE_NVM:
    case DEVICE_TYPE_SSD: {
      if(is_sequential == true){
        return seq_read_latency[device_type];
      }
      else {
        return rnd_read_latency[device_type];
      }
    }

    case DEVICE_TYPE_INVALID:
      return 0;

    default: {
      std::cout << "Get invalid device";
      exit(EXIT_FAILURE);
    }
  }
}

// LOCATE IN DEVICE

bool LocateInDevice(Device device,
                    const size_t& block_id){

  // Check device cache
  try{
    device.cache.Get(block_id, true);
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

  std::cout << "Get invalid device";
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

// IS SEQUENTIAL?

bool IsSequential(std::vector<Device>& devices,
                  const DeviceType& device_type,
                  const size_t& next){
  for(auto device : devices){
    if(device.device_type == device_type){
      return device.cache.IsSequential(next);
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
    case DEVICE_TYPE_INVALID: {
      std::cout << "Get invalid device";
      exit(EXIT_FAILURE);
    }
  }

  return destination;
}

std::string CleanStatus(const size_t& block_status){
  if(block_status == CLEAN_BLOCK){
    return "";
  }
  else if(block_status == DIRTY_BLOCK){
    return "●";
  }
  else {
    std::cout << "Invalid block type: " << block_status;
    exit(EXIT_FAILURE);
  }
}

// COPY + MOVE VICTIM

void MoveVictim(std::vector<Device>& devices,
                DeviceType source,
                const size_t& block_id,
                const size_t& block_status,
                double& total_duration);

void Copy(std::vector<Device>& devices,
          DeviceType destination,
          DeviceType source,
          const size_t& block_id,
          const size_t& block_status,
          double& total_duration){

  DLOG(INFO) << "COPY : " << block_id << " " << " " \
      << DeviceTypeToString(source) << " " \
      << "---> " << DeviceTypeToString(destination) << " " \
      << CleanStatus(block_status) << "\n";

  // Write to destination device
  auto device_offset = GetDeviceOffset(devices, destination);
  auto last_device_type = devices.back().device_type;
  auto device_cache = devices[device_offset].cache;
  auto final_block_status = block_status;
  if(last_device_type == destination){
    final_block_status = CLEAN_BLOCK;
  }
  auto victim = device_cache.Put(block_id, final_block_status);

  total_duration += GetReadLatency(devices, source, block_id);
  total_duration += GetWriteLatency(devices, destination, block_id);

  // Move victim
  auto victim_key = victim.block_id;
  auto victim_status = victim.block_type;
  MoveVictim(devices,
             destination,
             victim_key,
             victim_status,
             total_duration);

}

void MoveVictim(std::vector<Device>& devices,
                DeviceType source,
                const size_t& block_id,
                const size_t& block_status,
                double& total_duration){

  bool victim_exists = (block_id != INVALID_KEY);
  bool memory_device = (source == DeviceType::DEVICE_TYPE_DRAM ||
      source == DeviceType::DEVICE_TYPE_NVM);
  bool is_dirty = (block_status == DIRTY_BLOCK);

  if(victim_exists == true) {
    DLOG(INFO) << "Move victim   : " << block_id << "\n";
    DLOG(INFO) << "Memory device : " << memory_device << "\n";
    DLOG(INFO) << CleanStatus(block_status) << "\n";
  }

  // Check if we have a dirty victim in DRAM
  if(victim_exists && memory_device && is_dirty){
    auto destination = GetLowerDevice(devices, source);

    // Copy to device
    Copy(devices,
         destination,
         source,
         block_id,
         block_status,
         total_duration);
  }

}

// DEVICE FACTORY

Device DeviceFactory::GetDevice(const DeviceType& device_type,
                                const CachingType& caching_type,
                                const size_t& machine_size,
                                const DeviceType& last_device_type){

  switch (device_type){
    case DEVICE_TYPE_DRAM:
    case DEVICE_TYPE_NVM:
    case DEVICE_TYPE_SSD:  {
      // Check for last device
      auto size = device_size[device_type];
      if(last_device_type == device_type){
        size = machine_size * 1024;
      }
      return Device(device_type,
                    caching_type,
                    size
      );
    }

    case DEVICE_TYPE_INVALID:
    default: {
      std::cout << "Get invalid device";
      exit(EXIT_FAILURE);
    }
  }

}


}  // End machine namespace

