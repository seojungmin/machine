// WORKLOAD SOURCE

#include <chrono>
#include <ctime>
#include <iostream>
#include <fstream>
#include <iomanip>

#include "macros.h"
#include "workload.h"
#include "distribution.h"

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

  printf("Total slots: %lu\n", total_slots);

  size_t upper_bound = total_slots;
  double theta = 1.5;
  size_t sample_count = 10;
  size_t sample_itr;

  ZipfDistribution zipf_generator(upper_bound, theta);

  for(sample_itr = 0; sample_itr < sample_count; sample_itr++){
    auto sample = zipf_generator.GetNextNumber();
    printf("sample %lu : %lu\n", sample_itr, sample);
  }


}

void RunMachineTest() {

  // Run the benchmark once
  MachineHelper();

}

}  // namespace machine

