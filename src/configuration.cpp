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

static void ValidateHierarchyType(const configuration &state) {
  if (state.hierarchy_type < 1 || state.hierarchy_type > 3) {
    printf("Invalid hierarchy_type :: %d", state.hierarchy_type);
    exit(EXIT_FAILURE);
  }
  else {
    switch (state.hierarchy_type) {
      case HIERARCHY_TYPE_DRAM_NVM:
        printf("%s : HIERARCHY_TYPE_DRAM_NVM", "hierarchy_type ");
        break;
      case HIERARCHY_TYPE_DRAM_NVM_SSD:
        printf("%s : HIERARCHY_TYPE_DRAM_NVM_SSD", "hierarchy_type ");
        break;
      case HIERARCHY_TYPE_DRAM_NVM_SSD_HDD:
        printf("%s : HIERARCHY_TYPE_DRAM_NVM_SSD_HDD", "hierarchy_type ");
        break;
      default:
        break;
    }
  }
}

static void ValidateLoggingType(const configuration &state) {
  if (state.logging_type < 1 || state.logging_type > 2) {
    printf("Invalid logging_type :: %d", state.logging_type);
    exit(EXIT_FAILURE);
  }
  else {
    switch (state.logging_type) {
      case LOGGING_TYPE_WAL:
        printf("%s : LOGGING_TYPE_WAL", "logging_type ");
        break;
      case LOGGING_TYPE_WBL:
        printf("%s : LOGGING_TYPE_WBL", "logging_type ");
        break;
      default:
        break;
    }
  }
}

static void ValidateMigrationType(const configuration &state) {
  if (state.migration_type < 1 || state.migration_type > 3) {
    printf("Invalid migration_type :: %d", state.migration_type);
    exit(EXIT_FAILURE);
  }
  else {
    switch (state.migration_type) {
      case MIGRATION_TYPE_DOWNWARDS:
        printf("%s : MIGRATION_TYPE_DOWNWARDS", "migration_type ");
        break;
      case MIGRATION_TYPE_BOTHWAYS:
        printf("%s : MIGRATION_TYPE_BOTHWAYS", "migration_type ");
        break;
      default:
        break;
    }
  }
}

static void ValidateDirectNVM(const configuration &state) {
  printf("Direct NVM : %d", state.direct_nvm);
}

static void ConstructDeviceList(configuration &state){

  // TODO: fix size of all devices to 1000 slots
  const size_t device_size = 1000;

  Device dram_device(DEVICE_TYPE_DRAM, device_size);
  Device nvm_device(DEVICE_TYPE_NVM, device_size);
  Device ssd_device(DEVICE_TYPE_SSD, device_size);
  Device hdd_device(DEVICE_TYPE_HDD, device_size);

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
        printf("Unknown option: -%c-", c);
        Usage();
    }
  }

  // Run validators
  ValidateHierarchyType(state);
  ValidateLoggingType(state);
  ValidateMigrationType(state);
  ValidateDirectNVM(state);

  // Construct device list
  ConstructDeviceList(state);

}

}  // namespace machine
