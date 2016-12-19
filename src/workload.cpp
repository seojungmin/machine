// WORKLOAD SOURCE

#include <chrono>
#include <ctime>
#include <iostream>
#include <fstream>
#include <iomanip>

#include "macros.h"
#include "workload.h"

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

}

void RunMachineTest() {

  // Run the benchmark once
  MachineHelper();

}

}  // namespace machine

