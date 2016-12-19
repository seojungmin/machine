// MACHINE SOURCE

#include <iostream>
#include <fstream>

#include "configuration.h"
#include "workload.h"

namespace machine {

configuration state;

// Main Entry Point
void RunBenchmark() {

  // Run a single machine test
  RunMachineTest();

}

}  // namespace machine

int main(int argc, char **argv) {

  machine::ParseArguments(
      argc, argv, machine::state);

  machine::RunBenchmark();

  return 0;
}
