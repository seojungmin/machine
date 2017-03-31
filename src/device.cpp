// DEVICE SOURCE

#include "macros.h"
#include "device.h"

namespace machine {

size_t scale_factor = 1;

std::map<DeviceType, size_t> device_size;
std::map<DeviceType, size_t> seq_read_latency;
std::map<DeviceType, size_t> seq_write_latency;
std::map<DeviceType, size_t> rnd_read_latency;
std::map<DeviceType, size_t> rnd_write_latency;

void BootstrapDeviceMetrics(){

  device_size[DEVICE_TYPE_DRAM] = 4 * scale_factor;
  device_size[DEVICE_TYPE_NVM] = 32 * scale_factor;
  device_size[DEVICE_TYPE_SSD] = 128 * scale_factor;

  seq_read_latency[DEVICE_TYPE_DRAM] = 1;
  seq_read_latency[DEVICE_TYPE_NVM] = 5;
  seq_read_latency[DEVICE_TYPE_SSD] = 10;

  seq_write_latency[DEVICE_TYPE_DRAM] = 1;
  seq_write_latency[DEVICE_TYPE_NVM] = 10;
  seq_write_latency[DEVICE_TYPE_SSD] = 20;

  rnd_read_latency[DEVICE_TYPE_DRAM] = 1;
  rnd_read_latency[DEVICE_TYPE_NVM] = 5;
  rnd_read_latency[DEVICE_TYPE_SSD] = 10;

  rnd_write_latency[DEVICE_TYPE_DRAM] = 1;
  rnd_write_latency[DEVICE_TYPE_NVM] = 10;
  rnd_write_latency[DEVICE_TYPE_SSD] = 20;

}

// GET READ & WRITE LATENCY

size_t GetWriteLatency(DeviceType device_type){

  DLOG(INFO) << "WRITE :: " << DeviceTypeToString(device_type) << "\n";

  switch(device_type){
    case DEVICE_TYPE_DRAM:
    case DEVICE_TYPE_NVM:
    case DEVICE_TYPE_SSD:
      return seq_write_latency[device_type];

    case DEVICE_TYPE_INVALID:
      return 0;

    default:
      exit(EXIT_FAILURE);
  }
}

size_t GetReadLatency(DeviceType device_type){

  DLOG(INFO) << "READ :: " << DeviceTypeToString(device_type) << "\n";

  switch(device_type){
    case DEVICE_TYPE_DRAM:
    case DEVICE_TYPE_NVM:
    case DEVICE_TYPE_SSD:
      return seq_read_latency[device_type];

    case DEVICE_TYPE_INVALID:
      return 0;

    default:
      exit(EXIT_FAILURE);
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

std::string CleanStatus(const size_t& block_status){
  if(block_status == CLEAN_BLOCK){
    return "";
  }
  else if(block_status == DIRTY_BLOCK){
    return "●";
  }
  else {
    DLOG(INFO) << "Invalid block type: " << block_status;
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

  std::cout << "COPY : " << block_id << " " << " " \
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

  total_duration += GetReadLatency(source);
  total_duration += GetWriteLatency(destination);

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

    default:
      exit(EXIT_FAILURE);
  }

}


}  // End machine namespace

