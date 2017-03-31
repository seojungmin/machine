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
#include "device.h"
#include "cache.h"

namespace machine {

const static std::string OUTPUT_FILE = "outputfile.summary";
std::ofstream out(OUTPUT_FILE);

size_t query_itr;

double total_duration = 0;

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

size_t GetMachineSize(){

  size_t machine_size = 0;
  for(auto device: state.devices){
    auto device_size = device.cache.CurrentCapacity();
    machine_size += device_size;
  }

  return machine_size;
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

DeviceType LocateInMemoryDevices(const size_t& block_id){
  return LocateInDevices(state.memory_devices, block_id);
}

DeviceType LocateInStorageDevices(const size_t& block_id){
  return LocateInDevices(state.storage_devices, block_id);
}

void BringBlockToMemory(const size_t& block_id){

  auto memory_device_type = LocateInMemoryDevices(block_id);
  auto storage_device_type = LocateInStorageDevices(block_id);
  auto nvm_exists = DeviceExists(state.devices, DeviceType::DEVICE_TYPE_NVM);

  // Not found on DRAM & NVM
  if(memory_device_type == DeviceType::DEVICE_TYPE_INVALID){
    // Copy to NVM first if it exists in hierarchy
    if(nvm_exists == true) {
      Copy(state.devices,
           DeviceType::DEVICE_TYPE_NVM,
           storage_device_type,
           block_id,
           CLEAN_BLOCK,
           total_duration);
    }
    else {
      Copy(state.devices,
           DeviceType::DEVICE_TYPE_DRAM,
           storage_device_type,
           block_id,
           CLEAN_BLOCK,
           total_duration);
    }
  }

  // NVM to DRAM migration
  memory_device_type = LocateInMemoryDevices(block_id);
  if(memory_device_type == DeviceType::DEVICE_TYPE_NVM){
    auto dram_exists = DeviceExists(state.devices, DeviceType::DEVICE_TYPE_DRAM);
    bool migrate_to_dram = (rand() % state.migration_frequency == 0);
    if(dram_exists == true){
      if(migrate_to_dram == true){
        Copy(state.devices,
             DeviceType::DEVICE_TYPE_DRAM,
             DeviceType::DEVICE_TYPE_NVM,
             block_id,
             CLEAN_BLOCK,
             total_duration);
      }
    }
  }

}

void BringBlockToStorage(const size_t& block_id,
                         const size_t& block_status){

  auto memory_device_type = LocateInMemoryDevices(block_id);
  auto nvm_exists = DeviceExists(state.devices, DeviceType::DEVICE_TYPE_NVM);
  auto last_device_type = state.devices.back().device_type;
  auto nvm_last = (last_device_type == DeviceType::DEVICE_TYPE_NVM);
  auto nvm_status = block_status;
  if(nvm_last == true){
    nvm_status = CLEAN_BLOCK;
  }

  // Check if it is on DRAM
  if(memory_device_type == DeviceType::DEVICE_TYPE_DRAM){
    // Copy to NVM first if it exists in hierarchy
    if(nvm_exists == true) {
      Copy(state.devices,
           DeviceType::DEVICE_TYPE_NVM,
           memory_device_type,
           block_id,
           nvm_status,
           total_duration);
    }
    else {
      Copy(state.devices,
           DeviceType::DEVICE_TYPE_SSD,
           memory_device_type,
           block_id,
           CLEAN_BLOCK,
           total_duration);
    }

    // Mark block as clean
    auto device_offset = GetDeviceOffset(state.devices, memory_device_type);
    auto device_cache = state.devices[device_offset].cache;
    auto victim = device_cache.Put(block_id, CLEAN_BLOCK);
    if(victim.block_id != INVALID_KEY){
      exit(EXIT_FAILURE);
    }

    // Update duration
    total_duration += GetWriteLatency(memory_device_type);
  }

}


void ReadBlock(const size_t& block_id){
  std::cout << "READ  " << block_id << "\n";

  // Bring block to memory if needed
  BringBlockToMemory(block_id);

  // Update duration
  auto memory_device_type = LocateInMemoryDevices(block_id);
  total_duration += GetReadLatency(memory_device_type);

}

void UpdateBlock(const size_t& block_id) {
  std::cout << "UPDATE " << block_id << "\n";

  // Bring block to memory if needed
  BringBlockToMemory(block_id);

  // Mark block as dirty
  auto memory_device_type = LocateInMemoryDevices(block_id);
  if(memory_device_type == DeviceType::DEVICE_TYPE_DRAM){
    auto device_offset = GetDeviceOffset(state.devices, memory_device_type);
    auto device_cache = state.devices[device_offset].cache;
    auto victim = device_cache.Put(block_id, DIRTY_BLOCK);
    if(victim.block_id != INVALID_KEY){
      exit(EXIT_FAILURE);
    }
  }

  // Update duration
  total_duration += GetWriteLatency(memory_device_type);

}

void WriteBlock(size_t& block_id) {
  std::cout << "WRITE " << block_id << "\n";

  auto dram_exists = DeviceExists(state.devices, DeviceType::DEVICE_TYPE_DRAM);

  // Mark block as dirty if written to DRAM
  if(dram_exists){
    Copy(state.devices,
         DeviceType::DEVICE_TYPE_DRAM,
         DeviceType::DEVICE_TYPE_INVALID,
         block_id,
         DIRTY_BLOCK,
         total_duration);
  }
  // Mark block as clean if written to NVM
  else {
    Copy(state.devices,
         DeviceType::DEVICE_TYPE_NVM,
         DeviceType::DEVICE_TYPE_INVALID,
         block_id,
         CLEAN_BLOCK,
         total_duration);
  }

  // Update write block id
  block_id++;

}

void FlushBlock(const size_t& block_id) {
  std::cout << "FLUSH " << block_id << "\n";

  // Check if dirty in DRAM
  auto memory_device_type = LocateInMemoryDevices(block_id);
  if(memory_device_type == DeviceType::DEVICE_TYPE_DRAM){
    auto device_offset = GetDeviceOffset(state.devices, memory_device_type);
    auto device_cache = state.devices[device_offset].cache;
    auto block_status = device_cache.Get(block_id, true);
    if(block_status != CLEAN_BLOCK){
      BringBlockToStorage(block_id, block_status);
    }
  }

}

void MachineHelper() {

  // Run workload

  // Determine size of last device
  size_t total_slots = state.machine_size;
  std::cout << "Total slots : " << total_slots << "\n";

  // Reinit duration
  total_duration = 0;

  // Bootstrap
  BootstrapMachine(total_slots);

  // Print machine caches
  //PrintMachine();

  size_t upper_bound = total_slots - 1;
  double theta = 0.5;
  size_t operation_count = state.operation_count;
  size_t operation_itr;
  double seed = 23;
  srand(seed);
  int update_ratio = 40;
  int flush_ratio = 20;
  int write_ratio = 15;

  ZipfDistribution zipf_generator(upper_bound, theta);
  UniformDistribution uniform_generator(seed);
  size_t current_block_id = total_slots;

  for(operation_itr = 0; operation_itr < operation_count; operation_itr++){
    auto block_id = zipf_generator.GetNextNumber();
    auto operation_sample = rand() % 100;

    std::cout << "\nOperation : " << operation_itr << " :: ";
    if(operation_sample < write_ratio) {
      WriteBlock(current_block_id);
    }
    else if(operation_sample < flush_ratio) {
      UpdateBlock(block_id);
      std::cout << "-------------------------";
      FlushBlock(block_id);
    }
    else if(operation_sample < update_ratio) {
      UpdateBlock(block_id);
    }
    else {
      ReadBlock(block_id);
    }

    LOG(INFO) << "Duration : " << total_duration << "\n";
    std::cout << "-------------------------";

    // Get machine size
    auto machine_size = GetMachineSize();
    auto expected_size = current_block_id;
    if(machine_size < expected_size || machine_size > 2 * expected_size){
      LOG(INFO) << "Machine size  : " << machine_size;
      LOG(INFO) << "Expected size : " << expected_size;
      exit(EXIT_FAILURE);
    }

  }

  std::cout << "Duration : " << total_duration << "\n";

  // Get machine size
  auto machine_size = GetMachineSize();
  auto expected_size = current_block_id;
  std::cout << "Machine size  : " << machine_size;
  std::cout << "Expected size : " << expected_size;

  // Print machine caches
  PrintMachine();

}

void RunMachineTest() {

  // Run the benchmark once
  MachineHelper();

}

}  // namespace machine

