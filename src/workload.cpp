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

// Ephemeral block map
std::map<size_t, DeviceType> ephemeral_block_map;

// Durable block map
std::map<size_t, DeviceType> durable_block_map;

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

  for(size_t slot_itr = 0; slot_itr < total_slots; slot_itr++){
    auto last_device = state.devices.back();
    auto last_device_type = last_device.device_type;
    auto last_device_string = DeviceTypeToString(last_device_type);

    durable_block_map[slot_itr] = last_device_type;
  }

}

DeviceType LocateEphemeralDevice(const size_t& block_id){
  DeviceType device_type = DeviceType::DEVICE_TYPE_INVALID;

  if(ephemeral_block_map.count(block_id) != 0){
    // Found
    device_type = ephemeral_block_map[block_id];
  }

  return device_type;
}

DeviceType LocateDurableDevice(const size_t& block_id){
  DeviceType device_type = DeviceType::DEVICE_TYPE_INVALID;

  if(durable_block_map.count(block_id) != 0){
    // Found
    device_type = durable_block_map[block_id];
  }

  return device_type;
}

void ReadBlock(const size_t& block_id){
  std::cout << "Read  block : " << block_id << "\n";

  auto ephemeral_device_type = LocateEphemeralDevice(block_id);
  auto durable_device_type = LocateDurableDevice(block_id);

  std::cout << "Ephemeral Device: " << DeviceTypeToString(ephemeral_device_type) << "\n";
  std::cout << "Durable   Device: " << DeviceTypeToString(durable_device_type) << "\n";

}

void WriteBlock(const size_t& block_id){
  std::cout << "Write block : " << block_id << "\n";

}

void MachineHelper() {

  // Run workload

  // Determine size of hierarchy
  size_t total_slots = 0;
  for(auto device : state.devices){
    auto device_type = device.device_type;
    if(device_type != DeviceType::DEVICE_TYPE_DRAM){
      total_slots += device.device_size;
    }
  }

  std::cout << "Total slots : " << total_slots << "\n";

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

    std::cout << "Operation : " << sample_itr << " ";
    if(operation_sample < update_ratio) {
      WriteBlock(block_id);
    }
    else {
      ReadBlock(block_id);
    }

  }

}

void RunMachineTest() {

  // Run the benchmark once
  MachineHelper();

}

}  // namespace machine

