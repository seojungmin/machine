// CONFIGURATION SOURCE

#include <algorithm>
#include <iomanip>

#include "configuration.h"

namespace machine {

void Usage() {
  std::cout <<
      "\n"
      "Command line options : machine <options>\n"
      "   -a --hierarchy_type                 :  hierarchy type\n"
      "   -l --logging_type                   :  logging type\n"
      "   -m --migration_type                 :  migration type\n"
      "   -n --direct_nvm                     :  direct nvm\n"
      "   -v --verbose                        :  verbose\n";

  exit(EXIT_FAILURE);
}

static struct option opts[] = {
    {"hierarchy_type", optional_argument, NULL, 'a'},
    {"logging_type", optional_argument, NULL, 'l'},
    {"migration_type", optional_argument, NULL, 'm'},
    {"direct_nvm", optional_argument, NULL, 'n'},
    {"verbose", optional_argument, NULL, 'v'},
    {NULL, 0, NULL, 0}
};

std::string DeviceTypeToString(const DeviceType& device_type){

  switch (device_type){
    case DEVICE_TYPE_DRAM:
      return "DRAM";
    case DEVICE_TYPE_NVM:
      return "NVM";
    case DEVICE_TYPE_SSD:
      return "SSD";
    case DEVICE_TYPE_HDD:
      return "HDD";
    default:
      return "INVALID";
  }

}


static void ValidateHierarchyType(const configuration &state) {
  if (state.hierarchy_type < 1 || state.hierarchy_type > 3) {
    printf("Invalid hierarchy_type :: %d\n", state.hierarchy_type);
    exit(EXIT_FAILURE);
  }
  else {
    switch (state.hierarchy_type) {
      case HIERARCHY_TYPE_DRAM_NVM:
        printf("%30s : %s\n", "hierarchy_type", "HIERARCHY_TYPE_DRAM_NVM");
        break;
      case HIERARCHY_TYPE_DRAM_NVM_SSD:
        printf("%30s : %s\n", "hierarchy_type", "HIERARCHY_TYPE_DRAM_NVM_SSD");
        break;
      case HIERARCHY_TYPE_DRAM_NVM_SSD_HDD:
        printf("%30s : %s\n", "hierarchy_type", "HIERARCHY_TYPE_DRAM_NVM_SSD_HDD");
        break;
      default:
        break;
    }
  }
}

static void ValidateLoggingType(const configuration &state) {
  if (state.logging_type < 1 || state.logging_type > 2) {
    printf("Invalid logging_type :: %d\n", state.logging_type);
    exit(EXIT_FAILURE);
  }
  else {
    switch (state.logging_type) {
      case LOGGING_TYPE_WAL:
        printf("%30s : %s\n", "logging_type", "LOGGING_TYPE_WAL");
        break;
      case LOGGING_TYPE_WBL:
        printf("%30s : %s\n", "logging_type", "LOGGING_TYPE_WBL");
        break;
      default:
        break;
    }
  }
}

static void ValidateMigrationType(const configuration &state) {
  if (state.migration_type < 1 || state.migration_type > 3) {
    printf("Invalid migration_type :: %d\n", state.migration_type);
    exit(EXIT_FAILURE);
  }
  else {
    switch (state.migration_type) {
      case MIGRATION_TYPE_DOWNWARDS:
        printf("%30s : %s\n", "migration_type", "MIGRATION_TYPE_DOWNWARDS");
        break;
      case MIGRATION_TYPE_BOTHWAYS:
        printf("%30s : %s\n", "migration_type", "MIGRATION_TYPE_BOTHWAYS");
        break;
      default:
        break;
    }
  }
}

static void ValidateDirectNVM(const configuration &state) {
  printf("%30s : %d\n", "direct_nvm", state.direct_nvm);
}

static void ConstructDeviceList(configuration &state){

  // TODO: fix size of all devices to 1000 slots
  const size_t dram_device_size = 5;
  const size_t nvm_device_size = 10;
  const size_t ssd_device_size = 20;
  const size_t hdd_device_size = 50;

  const size_t read_dram_latency = 10;
  const size_t read_nvm_latency = 20;
  const size_t read_ssd_latency = 100;
  const size_t read_hdd_latency = 1000;

  const size_t write_dram_latency = 10;
  const size_t write_nvm_latency = 20;
  const size_t write_ssd_latency = 100;
  const size_t write_hdd_latency = 1000;

  Device dram_device(DEVICE_TYPE_DRAM,
                     dram_device_size,
                     read_dram_latency,
                     write_dram_latency
  );
  Device nvm_device(DEVICE_TYPE_NVM,
                    nvm_device_size,
                    read_nvm_latency,
                    write_nvm_latency
  );
  Device ssd_device(DEVICE_TYPE_SSD,
                    ssd_device_size,
                    read_ssd_latency,
                    write_ssd_latency
  );
  Device hdd_device(DEVICE_TYPE_HDD,
                    hdd_device_size,
                    read_hdd_latency,
                    write_hdd_latency
  );

  switch (state.hierarchy_type) {
    case HIERARCHY_TYPE_DRAM_NVM: {
      state.devices = {dram_device, nvm_device};
    }
    break;
    case HIERARCHY_TYPE_DRAM_NVM_SSD: {
      state.devices = {dram_device, nvm_device, ssd_device};
    }
    break;
    case HIERARCHY_TYPE_DRAM_NVM_SSD_HDD: {
      state.devices = {dram_device, nvm_device, ssd_device, hdd_device};
    }
    break;
    default:
      break;
  }
}


void ParseArguments(int argc, char *argv[], configuration &state) {

  // Default Values
  state.verbose = false;

  state.hierarchy_type = HIERARCHY_TYPE_DRAM_NVM_SSD_HDD;
  state.logging_type = LOGGING_TYPE_WBL;
  state.migration_type = MIGRATION_TYPE_DOWNWARDS;

  // Parse args
  while (1) {
    int idx = 0;
    int c = getopt_long(argc, argv,
                        "a:l:m:n:v",
                        opts, &idx);

    if (c == -1) break;

    switch (c) {
      case 'a':
        state.hierarchy_type = (HierarchyType)atoi(optarg);
        break;
      case 'l':
        state.logging_type = (LoggingType)atoi(optarg);
        break;
      case 'm':
        state.migration_type = (MigrationType)atoi(optarg);
        break;
      case 'n':
        state.direct_nvm = atoi(optarg);
        break;
      case 'v':
        state.verbose = atoi(optarg);
        break;

      default:
        printf("Unknown option: -%c-\n", c);
        Usage();
    }
  }

  // Run validators
  printf("//===----------------------------------------------------------------------===//\n");
  printf("//                               MACHINE                                      //\n");
  printf("//===----------------------------------------------------------------------===//\n");

  ValidateHierarchyType(state);
  ValidateLoggingType(state);
  ValidateMigrationType(state);
  ValidateDirectNVM(state);
  printf("//===----------------------------------------------------------------------===//\n");

  // Construct device list
  ConstructDeviceList(state);

}

}  // namespace machine
