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
  if(memory_device_type == DeviceType::DEVICE_TYPE_INVALID &&
      storage_device_type != DeviceType::DEVICE_TYPE_INVALID){
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

  auto source = LocateInMemoryDevices(block_id);
  auto nvm_exists = DeviceExists(state.devices, DeviceType::DEVICE_TYPE_NVM);
  auto last_device_type = state.devices.back().device_type;
  auto nvm_last = (last_device_type == DeviceType::DEVICE_TYPE_NVM);
  auto nvm_status = block_status;
  if(nvm_last == true){
    nvm_status = CLEAN_BLOCK;
  }

  // Check if it is on DRAM
  if(source == DeviceType::DEVICE_TYPE_DRAM){
    // Copy to NVM first if it exists in hierarchy
    if(nvm_exists == true) {
      Copy(state.devices,
           DeviceType::DEVICE_TYPE_NVM,
           source,
           block_id,
           nvm_status,
           total_duration);
    }
    else {
      Copy(state.devices,
           DeviceType::DEVICE_TYPE_SSD,
           source,
           block_id,
           CLEAN_BLOCK,
           total_duration);
    }

    // Mark block as clean
    auto device_offset = GetDeviceOffset(state.devices, source);
    auto device_cache = state.devices[device_offset].cache;
    auto victim = device_cache.Put(block_id, CLEAN_BLOCK);
    if(victim.block_id != INVALID_KEY){
      exit(EXIT_FAILURE);
    }

    // Update duration
    total_duration += GetWriteLatency(state.devices, source, block_id);
  }

}

void WriteBlock(const size_t& block_id) {

  // Bring block to memory if needed
  BringBlockToMemory(block_id);

  auto destination = LocateInMemoryDevices(block_id);

  // CASE 1: New block
  if(destination == DeviceType::DEVICE_TYPE_INVALID){
    //std::cout << "WRITE " << block_id << "\n";
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

    return;
  }

  // CASE 2: Existing block
  //std::cout << "UPDATE " << block_id << "\n";

  // Mark block as dirty
  if(destination == DeviceType::DEVICE_TYPE_DRAM){
    auto device_offset = GetDeviceOffset(state.devices, destination);
    auto device_cache = state.devices[device_offset].cache;
    auto victim = device_cache.Put(block_id, DIRTY_BLOCK);
    if(victim.block_id != INVALID_KEY){
      exit(EXIT_FAILURE);
    }
  }

  // Update duration
  total_duration += GetWriteLatency(state.devices, destination, block_id);

}

void ReadBlock(const size_t& block_id){
  //std::cout << "READ  " << block_id << "\n";

  // Bring block to memory if needed
  BringBlockToMemory(block_id);

  // Update duration
  auto source = LocateInMemoryDevices(block_id);
  total_duration += GetReadLatency(state.devices, source, block_id);

  if(source == DeviceType::DEVICE_TYPE_INVALID){
    std::cout << "Could not read block : " << block_id << "\n";
    exit(EXIT_FAILURE);
  }

}

void FlushBlock(const size_t& block_id) {
  //std::cout << "FLUSH " << block_id << "\n";

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

  // Machine size
  BootstrapMachine(total_slots);

  // Reinit duration
  total_duration = 0;

  // Go through trace file
  std::unique_ptr<std::istream> input;
  char operation_type;
  size_t fork_number;
  size_t block_number;

  if (state.file_name.empty()) {
    return;
  }
  else {
    std::cout << "Running trace " << state.file_name << "...\n";
    input.reset(new std::ifstream(state.file_name.c_str()));
  }

  size_t fragment_size = 4096;
  char buffer[fragment_size];

  size_t operation_itr = 0;

  // Go over the input stream
  while(!input->eof()){
    operation_itr++;

    // Get a line from the input stream
    input->getline(buffer, fragment_size);

    // Check statement
    sscanf(buffer, "%c%lu%lu",
           &operation_type,
           &fork_number,
           &block_number);

    auto global_block_number = fork_number * 10 + block_number;

    if(global_block_number > state.machine_size){
      std::cout << "Operation " << operation_itr << " :: " <<
          operation_type << " " << global_block_number << " "
          << fork_number << " " << block_number << "\n";
      continue;
    }

    switch(operation_type){
      case 'r':
        ReadBlock(global_block_number);
        break;

      case 'w':
        WriteBlock(global_block_number);
        break;

      case 'f':
        FlushBlock(global_block_number);
        break;

      default:
        std::cout << "Invalid operation type: " << operation_type << "\n";
        break;
    }

    if(operation_itr % 1000 == 0){
      std::cout << "Operation " << operation_itr << " :: " <<
          operation_type << " " << global_block_number << " "
          << fork_number << " " << block_number << "\n";
    }

  }

  std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
  std::cout << "Duration : " << total_duration/1000 << " (s) \n";
  std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++\n";

  // Get machine size
  auto machine_size = GetMachineSize();
  std::cout << "Machine size  : " << machine_size << "\n";

  // Print machine caches
  PrintMachine();

}

void RunMachineTest() {

  // Run the benchmark once
  MachineHelper();

}

}  // namespace machine

