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

void BootstrapMachine(const size_t& total_slots) {

  auto last_device = state.devices.back();
  //std::cout << "Bootstrap device: " << DeviceTypeToString(last_device_type) << "\n";

  for(size_t slot_itr = 0; slot_itr < total_slots; slot_itr++){
    last_device.cache.Put(slot_itr, slot_itr);
  }

  std::cout << last_device.cache;

}

DeviceType LocateMemoryDevice(const size_t& block_id){

  for(auto device : state.devices){

    DeviceType device_type = device.device_type;
    if(device_type != DeviceType::DEVICE_TYPE_DRAM &&
        device_type != DeviceType::DEVICE_TYPE_NVM){
      continue;
    }

    std::cout << "Check memory device: " << DeviceTypeToString(device_type) << "\n";
    std::cout << device.cache;

    // Check cache
    try{
      auto location = device.cache.Get(block_id);
      std::cout << "Found at location: " << location;
      std::cout << " in " << DeviceTypeToString(device_type) << "\n";
      return device_type;
    }
    catch(const std::range_error& not_found){
      // Nothing to do here!
    }

  }

  return DeviceType::DEVICE_TYPE_INVALID;
}

DeviceType LocateStorageDevice(const size_t& block_id){

  for(auto device : state.devices){

    DeviceType device_type = device.device_type;
    if(device_type != DeviceType::DEVICE_TYPE_SSD){
      continue;
    }

    std::cout << "Check storage device: " << DeviceTypeToString(device_type) << "\n";
    std::cout << device.cache;

    // Check cache
    try{
      auto location = device.cache.Get(block_id);
      std::cout << "Found at location: " << location;
      std::cout << " in " << DeviceTypeToString(device_type) << "\n";
      return device_type;
    }
    catch(const std::range_error& not_found){
      // Nothing to do here!
    }

  }

  return DeviceType::DEVICE_TYPE_INVALID;
}

void CopyToDRAM(DeviceType device_type,
                const size_t& block_id){

  UNUSED_ATTRIBUTE size_t victim_block;

  // Copy to DRAM
  for(auto device : state.devices){

    DeviceType device_type = device.device_type;
    if(device_type == DeviceType::DEVICE_TYPE_DRAM){
      victim_block = device.cache.Put(block_id, 0);
      total_duration += write_dram_latency;
    }

  }

  if(device_type == DeviceType::DEVICE_TYPE_SSD){
    total_duration += read_ssd_latency;
  }

  if(device_type == DeviceType::DEVICE_TYPE_NVM){
    total_duration += read_nvm_latency;
  }

  // Move victim if needed

}

void ReadBlock(const size_t& block_id){
  std::cout << "READ  " << block_id << "\n";

  auto memory_device_type = LocateMemoryDevice(block_id);
  auto storage_device_type = LocateStorageDevice(block_id);

  std::cout << "Memory  Device: " << DeviceTypeToString(memory_device_type) << "\n";
  std::cout << "Storage Device: " << DeviceTypeToString(storage_device_type) << "\n";

  // Not found on DRAM & NVM
  if(memory_device_type == DeviceType::DEVICE_TYPE_INVALID){
    CopyToDRAM(storage_device_type, block_id);
  }

  memory_device_type = LocateMemoryDevice(block_id);

  // Found on DRAM
  if(memory_device_type == DeviceType::DEVICE_TYPE_DRAM) {
    total_duration += read_dram_latency;
  }
  // Found on NVM
  else if(memory_device_type == DeviceType::DEVICE_TYPE_NVM){
    // TODO: Migrate to DRAM
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

    std::cout << "Operation : " << sample_itr << " :: ";
    if(operation_sample < update_ratio) {
      WriteBlock(block_id);
    }
    else {
      ReadBlock(block_id);
    }

    std::cout << "Duration : " << total_duration << "\n";
    std::cout << "-------------------------";

  }

}

void RunMachineTest() {

  // Run the benchmark once
  MachineHelper();

}

}  // namespace machine

