// WORKLOAD SOURCE

#include <chrono>
#include <ctime>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <map>

#include "macros.h"
#include "workload.h"
#include "distribution.h"
#include "configuration.h"

namespace machine {

const static std::string OUTPUT_FILE = "outputfile.summary";
std::ofstream out(OUTPUT_FILE);

size_t query_itr;

double total_duration = 0;

size_t dram_device_size = 5;
size_t nvm_device_size = 10;
size_t ssd_device_size = 20;

size_t read_dram_latency = 10;
size_t read_nvm_latency = 20;
size_t read_ssd_latency = 100;

size_t write_dram_latency = 10;
size_t write_nvm_latency = 20;
size_t write_ssd_latency = 100;

const size_t CLEAN_BLOCK = 100;
const size_t DIRTY_BLOCK = 101;

UNUSED_ATTRIBUTE static void WriteOutput(double duration) {
  // Convert to ms
  duration *= 1000;

  // Write out output in verbose mode
  if (state.verbose == true) {
    printf("----------------------------------------------------------");
    printf("%lu :: %.1lf ms",
           query_itr,
           duration);
  }

  out << query_itr << " ";
  out << std::fixed << std::setprecision(2) << duration << "\n";

  out.flush();
}

void PrintMachine(){

  std::cout << "\n+++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
  std::cout << "MACHINE\n";
  for(auto device: state.devices){
    std::cout << device.cache;
  }
  std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++\n";

}

void BootstrapMachine(const size_t& total_slots) {

  auto last_device = state.devices.back();
  for(size_t slot_itr = 0; slot_itr < total_slots; slot_itr++){
    last_device.cache.Put(slot_itr, CLEAN_BLOCK);
  }

}

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

DeviceType LocateInMemoryDevices(const size_t& block_id){
  return LocateInDevices(state.memory_devices, block_id);
}

DeviceType LocateInStorageDevices(const size_t& block_id){
  return LocateInDevices(state.storage_devices, block_id);
}

size_t GetDeviceOffset(DeviceType device_type){

  size_t device_itr = 0;
  for(auto device : state.devices){
    if(device.device_type == device_type){
      return device_itr;
    }
    device_itr++;
  }

  std::cout << "Did not find device of type : " <<
      DeviceTypeToString(device_type);
  exit(EXIT_FAILURE);
}

size_t GetWriteLatency(DeviceType device_type){
  switch(device_type){
    case DEVICE_TYPE_DRAM:
      return write_dram_latency;

    case DEVICE_TYPE_NVM:
      return write_nvm_latency;

    case DEVICE_TYPE_SSD:
      return write_ssd_latency;

    default:
    case DEVICE_TYPE_INVALID:
      exit(EXIT_FAILURE);
  }
}

size_t GetReadLatency(DeviceType device_type){
  switch(device_type){
    case DEVICE_TYPE_DRAM:
      return read_dram_latency;

    case DEVICE_TYPE_NVM:
      return read_nvm_latency;

    case DEVICE_TYPE_SSD:
      return read_ssd_latency;

    default:
    case DEVICE_TYPE_INVALID:
      exit(EXIT_FAILURE);
  }
}

void Copy(DeviceType destination_device_type,
          DeviceType source_device_type,
          const size_t& block_id){
  UNUSED_ATTRIBUTE size_t victim_block;

  // Write to destination device
  auto device_offset = GetDeviceOffset(destination_device_type);
  auto device_cache = state.devices[device_offset].cache;
  victim_block = device_cache.Put(block_id, CLEAN_BLOCK);

  total_duration += GetWriteLatency(destination_device_type);
  total_duration += GetReadLatency(source_device_type);

  // TODO: Move victim

}

void ReadBlock(const size_t& block_id){
  std::cout << "READ  " << block_id << "\n";

  auto memory_device_type = LocateInMemoryDevices(block_id);
  auto storage_device_type = LocateInStorageDevices(block_id);

  // Not found on DRAM & NVM
  if(memory_device_type == DeviceType::DEVICE_TYPE_INVALID){
    Copy(DeviceType::DEVICE_TYPE_DRAM,
         storage_device_type,
         block_id);
  }

  memory_device_type = LocateInMemoryDevices(block_id);

  // Found on DRAM
  if(memory_device_type == DeviceType::DEVICE_TYPE_DRAM) {
    total_duration += read_dram_latency;
  }

  // Found on NVM
  if(memory_device_type == DeviceType::DEVICE_TYPE_NVM){
    total_duration += read_nvm_latency;
  }

}

void WriteBlock(const size_t& block_id){
  std::cout << "WRITE " << block_id << "\n";

}

void MachineHelper() {

  // Run workload

  // Determine size of last device
  auto last_device = state.devices.back();
  size_t total_slots = last_device.device_size;
  std::cout << "Total slots : " << total_slots << "\n";

  // Reinit duration
  total_duration = 0;

  // Bootstrap
  BootstrapMachine(total_slots);

  // Print machine caches
  PrintMachine();

  size_t upper_bound = total_slots - 1;
  double theta = 1.5;
  size_t sample_count = 10;
  size_t sample_itr;
  double seed = 23;
  srand(seed);
  int update_ratio = 20;

  ZipfDistribution zipf_generator(upper_bound, theta);
  UniformDistribution uniform_generator(seed);

  for(sample_itr = 0; sample_itr < sample_count; sample_itr++){
    auto block_id = zipf_generator.GetNextNumber();
    auto operation_sample = rand() % 100;

    std::cout << "\nOperation : " << sample_itr << " :: ";
    if(operation_sample < update_ratio) {
      WriteBlock(block_id);
    }
    else {
      ReadBlock(block_id);
    }

    std::cout << "Duration : " << total_duration << "\n";
    std::cout << "-------------------------";

  }

  // Print machine caches
  PrintMachine();

}

void RunMachineTest() {

  // Run the benchmark once
  MachineHelper();

}

}  // namespace machine

