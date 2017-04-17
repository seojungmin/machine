#!/usr/bin/python
# encoding: utf-8

###################################################################################
# MACHINE EXPERIMENTS
###################################################################################

import time
import argparse
import csv
import logging
import math
import os
import subprocess
import pprint
import matplotlib
import numpy as np
import re
import shutil
import datetime

matplotlib.use('Agg')
import pylab
import matplotlib.pyplot as plot
from matplotlib.backends.backend_pdf import PdfPages
from matplotlib.font_manager import FontProperties
from matplotlib.ticker import LinearLocator

###################################################################################
# LOGGING CONFIGURATION
###################################################################################
LOG = logging.getLogger(__name__)
LOG_handler = logging.StreamHandler()
LOG_formatter = logging.Formatter(
    fmt='%(asctime)s [%(funcName)s:%(lineno)03d] %(levelname)-5s: %(message)s',
    datefmt='%m-%d-%Y %H:%M:%S'
)
LOG_handler.setFormatter(LOG_formatter)
LOG.addHandler(LOG_handler)
LOG.setLevel(logging.INFO)

MAJOR_STRING = "++++++++++++++++++++++++++++\n\n\n"
MINOR_STRING = "----------------\n\n"
SUB_MINOR_STRING = "******\n"

###################################################################################
# OUTPUT CONFIGURATION
###################################################################################

BASE_DIR = os.path.dirname(__file__)
OPT_FONT_NAME = 'Helvetica'
OPT_GRAPH_HEIGHT = 150
OPT_GRAPH_WIDTH = 400

# Make a list by cycling through the colors you care about
# to match the length of your data.
NUM_COLORS = 5
COLOR_MAP = ( '#418259', '#bd5632', '#e1a94c', '#7d6c5b', '#364d38', '#c4e1c6')
COLOR_MAP_2 = ( '#a8b2c1', '#9EC9E9', '#80CA86', '#F58A87', '#D89761', '#FED113' )
COLOR_MAP_3 = ( '#2A363B', '#FECEA8', '#99B898')
COLOR_MAP_4 = ( '#262626', '#FECEA8', '#67abb8', '#f15b40')
COLOR_MAP_5 = ( '#262626', '#f15b40', '#67abb8')
COLOR_MAP_6 = ( '#262626', '#67abb8', '#FECEA8', '#f15b40')

OPT_COLORS = COLOR_MAP

OPT_GRID_COLOR = 'gray'
OPT_LEGEND_SHADOW = False
OPT_MARKERS = (['o', 's', 'v', "^", "h", "v", ">", "x", "d", "<", "|", "", "|", "_"])
OPT_PATTERNS = ([ "////", "o", "\\\\" , ".", "\\\\\\"])

OPT_STACK_COLORS = ('#2b3742', '#c9b385', '#610606', '#1f1501')
OPT_LINE_STYLES= ('-', ':', '--', '-.')

# SET FONT

OPT_LABEL_WEIGHT = 'bold'
OPT_LINE_COLORS = COLOR_MAP
OPT_LINE_WIDTH = 6.0
OPT_MARKER_SIZE = 10.0

AXIS_LINEWIDTH = 1.3
BAR_LINEWIDTH = 1.2

LABEL_FONT_SIZE = 16
TICK_FONT_SIZE = 14
TINY_FONT_SIZE = 10
LEGEND_FONT_SIZE = 18

SMALL_LABEL_FONT_SIZE = 10
SMALL_LEGEND_FONT_SIZE = 10

XAXIS_MIN = 0.25
XAXIS_MAX = 3.75

# SET TYPE1 FONTS
matplotlib.rcParams['ps.useafm'] = True
matplotlib.rcParams['pdf.use14corefonts'] = True
matplotlib.rcParams['text.usetex'] = True
matplotlib.rcParams['text.latex.preamble']= [
    r'\usepackage{helvet}',    # set the normal font here
    r'\usepackage{sansmath}',  # load up the sansmath so that math -> helvet
    r'\sansmath'               # <- tricky! -- gotta actually tell tex to use!
]

LABEL_FP = FontProperties(style='normal', size=LABEL_FONT_SIZE, weight='bold')
TICK_FP = FontProperties(style='normal', size=TICK_FONT_SIZE)
TINY_FP = FontProperties(style='normal', size=TINY_FONT_SIZE)
LEGEND_FP = FontProperties(style='normal', size=LEGEND_FONT_SIZE, weight='bold')

SMALL_LABEL_FP = FontProperties(style='normal', size=SMALL_LABEL_FONT_SIZE, weight='bold')
SMALL_LEGEND_FP = FontProperties(style='normal', size=SMALL_LEGEND_FONT_SIZE, weight='bold')

YAXIS_TICKS = 5
YAXIS_ROUND = 1000.0

EXIT_FAILURE = 1

###################################################################################
# CONFIGURATION
###################################################################################

## PROGRAM DIRS
BASE_DIR = os.path.dirname(os.path.realpath(__file__))

BUILD_DIR = BASE_DIR + "\\..\\..\\..\\Sources\\deuteronomy\\build\\"
PROGRAM_NAME = "bztree_benchmark.exe"

RELEASE_PROGRAM = BUILD_DIR + "\\Release\\" + PROGRAM_NAME
DEBUG_PROGRAM = BUILD_DIR + "\\Debug\\" + PROGRAM_NAME

OUTPUT_FILE = "outputfile.summary"
LOG_TO_STDERR = 0

## BENCHMARK TYPES

BENCHMARK_TYPE_RANDOMREADRANDOMWRITE = "randomreadrandomwrite"
BENCHMARK_TYPE_MWCAS = "mwcas"

BENCHMARK_TYPES = [BENCHMARK_TYPE_RANDOMREADRANDOMWRITE]

## WORKLOAD TYPES (UPDATE %)
WORKLOAD_TYPE_READ_ONLY  = 0
WORKLOAD_TYPE_READ_MOSTLY  = 10
WORKLOAD_TYPE_READ_HEAVY  = 25
WORKLOAD_TYPE_BALANCED  = 50

WORKLOAD_TYPES_STRINGS = {
    0 : "read-only",
    10 : "read-mostly",
    25 : "read-heavy",
    50 : "balanced",
}

WORKLOAD_TYPES = [
                WORKLOAD_TYPE_READ_ONLY,
                WORKLOAD_TYPE_READ_MOSTLY,
                WORKLOAD_TYPE_READ_HEAVY,
                WORKLOAD_TYPE_BALANCED
]

## KEY DISTRIBUTION TYPES
KEY_DISTRIBUTION_TYPE_UNIFORM  = 1   # Uniform random
KEY_DISTRIBUTION_TYPE_ZIPF = 2       # Zipfian skew
KEY_DISTRIBUTION_TYPE_MONOTONIC = 3  # Monotonically increasing

KEY_DISTRIBUTION_TYPES_STRINGS = {
    1 : "uniform",
    2 : "zipf",
    3 : "monotonic",
}

KEY_DISTRIBUTION_TYPES = [
                  KEY_DISTRIBUTION_TYPE_UNIFORM,
                  KEY_DISTRIBUTION_TYPE_ZIPF,
                  KEY_DISTRIBUTION_TYPE_MONOTONIC
]

## THREAD COUNTS
THREAD_COUNTS = [
    1,
    2,
    3,
    4,
    5,
    6,
    7,
    8
]

## CONTAINER TYPES
CONTAINER_TYPE_BZTREE = "bztree"
CONTAINER_TYPE_BWTREE = "bwtree"

CONTAINER_TYPES = [
    CONTAINER_TYPE_BZTREE,
    CONTAINER_TYPE_BWTREE
]

## OUTPUT

THROUGHPUT_OFFSET = 0

REPEAT_COUNT = 1
RETRY_COUNT = 30

## DEFAULTS

SCALE_FACTOR = 1000

DEFAULT_TEST_ID = 0
DEFAULT_LOG_TO_STDERR = 0
DEFAULT_DURATION = 30
TIMEOUT_DURATION = DEFAULT_DURATION * 4
DEFAULT_BENCHMARK_TYPE = BENCHMARK_TYPE_RANDOMREADRANDOMWRITE
DEFAULT_WORKLOAD_TYPE = WORKLOAD_TYPE_READ_ONLY
DEFAULT_KEY_DISTRIBUTION_TYPE = KEY_DISTRIBUTION_TYPE_UNIFORM
DEFAULT_CONTAINER_TYPE = CONTAINER_TYPE_BZTREE
DEFAULT_PAGE_SIZE = 2048


## EXPERIMENTS
BENCHMARK_EXPERIMENT = 1

## EVAL DIRS
BENCHMARK_DIR = BASE_DIR + "/results/benchmark"

## PLOT DIRS
LEGEND_PLOT_DIR = BASE_DIR + "/images/legend/"
BENCHMARK_PLOT_DIR = BASE_DIR + "/images/benchmark/"

## BENCHMARK EXPERIMENT

BENCHMARK_EXP_KEY_DISTRIBUTION_TYPES = KEY_DISTRIBUTION_TYPES
BENCHMARK_EXP_WORKLOAD_TYPES = WORKLOAD_TYPES
BENCHMARK_EXP_CONTAINER_TYPES = CONTAINER_TYPES
BENCHMARK_EXP_THREAD_COUNTS = THREAD_COUNTS

## CSV FILES

BENCHMARK_CSV = "benchmark.csv"

###################################################################################
# UTILS
###################################################################################

def chunks(l, n):
    """ Yield successive n-sized chunks from l.
    """
    for i in range(0, len(l), n):
        yield l[i:i + n]

def loadDataFile(path):
    file = open(path, "r")
    reader = csv.reader(file)

    data = []

    row_num = 0
    for row in reader:
        row_data = []
        column_num = 0
        for col in row:
            row_data.append(float(col))
            column_num += 1
        row_num += 1
        data.append(row_data)

    return data

def next_power_of_10(n):
    return (10 ** math.ceil(math.log(n, 10)))

def get_upper_bound(n):
    return (math.ceil(n / YAXIS_ROUND) * YAXIS_ROUND)

## MAKE GRID
def makeGrid(ax):
    axes = ax.get_axes()
    axes.yaxis.grid(True, color=OPT_GRID_COLOR)
    for axis in ['top','bottom','left','right']:
            ax.spines[axis].set_linewidth(AXIS_LINEWIDTH)
    ax.set_axisbelow(True)

## SAVE GRAPH
def saveGraph(fig, output, width, height):
    size = fig.get_size_inches()
    dpi = fig.get_dpi()
    LOG.debug("Current Size Inches: %s, DPI: %d" % (str(size), dpi))

    new_size = (width / float(dpi), height / float(dpi))
    fig.set_size_inches(new_size)
    new_size = fig.get_size_inches()
    new_dpi = fig.get_dpi()
    LOG.debug("New Size Inches: %s, DPI: %d" % (str(new_size), new_dpi))

    pp = PdfPages(output)
    fig.savefig(pp, format='pdf', bbox_inches='tight')
    pp.close()
    LOG.info("OUTPUT: %s", output)

def get_result_file(base_result_dir, result_dir_list, result_file_name):

    # Start with result dir
    final_result_dir = base_result_dir + "/"

    # Add each entry in the list as a sub-dir
    for result_dir_entry in result_dir_list:
        final_result_dir += result_dir_entry + "/"

    # Create dir if needed
    if not os.path.exists(final_result_dir):
        os.makedirs(final_result_dir)

    # Add local file name
    result_file_name = final_result_dir + result_file_name

    #pprint.pprint(result_file_name)
    return result_file_name

###################################################################################
# LEGEND
###################################################################################

def create_legend_container_type():
    fig = pylab.figure()
    ax1 = fig.add_subplot(111)

    LOG.info("Creating legend container type");

    LEGEND_VALUES = CONTAINER_TYPES

    figlegend = pylab.figure(figsize=(7, 0.5))
    idx = 0
    lines = [None] * (len(LEGEND_VALUES) + 1)
    data = [1]
    x_values = [1]

    TITLE = "CONTAINER:"
    LABELS = [TITLE, "BZTREE", "BWTREE"]

    lines[idx], = ax1.plot(x_values, data, linewidth = 0)
    idx = 1

    for group in range(len(LEGEND_VALUES)):
        lines[idx], = ax1.plot(x_values, data,
                               color=OPT_LINE_COLORS[idx - 1],
                               linewidth=OPT_LINE_WIDTH,
                               marker=OPT_MARKERS[idx - 1],
                               markersize=OPT_MARKER_SIZE)
        idx = idx + 1

    # LEGEND
    figlegend.legend(lines, LABELS, prop=LEGEND_FP,
                     loc=1, ncol=3,
                     mode="expand", shadow=OPT_LEGEND_SHADOW,
                     frameon=False, borderaxespad=0.0,
                     handleheight=1, handlelength=3)

    figlegend.savefig(LEGEND_PLOT_DIR + 'legend_container_type.pdf')
    
###################################################################################
# PLOT
###################################################################################

def get_label(label):
    bold_label = "\\textbf{" + label + "}"
    return bold_label

def create_benchmark_line_chart(datasets, key_distribution_type):
    fig = plot.figure()
    ax1 = fig.add_subplot(111)

    # X-AXIS
    x_values = [str(i) for i in BENCHMARK_EXP_THREAD_COUNTS]
    N = len(x_values)
    ind = np.arange(N)

    idx = 0
    for group in range(len(datasets)):
        # GROUP
        y_values = []
        for line in  range(len(datasets[group])):
            for col in  range(len(datasets[group][line])):
                if col == 1:
                    y_values.append(datasets[group][line][col]/1000)
        LOG.info("group_data = %s", str(y_values))
        ax1.plot(ind + 0.5, y_values,
                 color=OPT_COLORS[idx],
                 linewidth=OPT_LINE_WIDTH,
                 marker=OPT_MARKERS[idx],
                 markersize=OPT_MARKER_SIZE,
                 label=str(group))
        idx = idx + 1

    # GRID
    makeGrid(ax1)

    # Y-AXIS
    ax1.yaxis.set_major_locator(LinearLocator(YAXIS_TICKS))
    ax1.minorticks_off()
    ax1.set_ylabel(get_label('Throughput (K)'), fontproperties=LABEL_FP)

    YAXIS_MIN = 0
    ax1.set_ylim(bottom=YAXIS_MIN)

    YAXIS_MAX = 0
    if key_distribution_type == KEY_DISTRIBUTION_TYPE_UNIFORM:
        YAXIS_MAX = 8 * 1000
    elif key_distribution_type == KEY_DISTRIBUTION_TYPE_ZIPF:
        YAXIS_MAX = 16 * 1000
    elif key_distribution_type == KEY_DISTRIBUTION_TYPE_MONOTONIC:
        YAXIS_MAX = 24 * 1000

    ax1.set_ylim(top=YAXIS_MAX)

    # X-AXIS
    ax1.set_xticks(ind + 0.5)
    ax1.set_xlabel(get_label('Thread counts'), fontproperties=LABEL_FP)
    ax1.set_xticklabels(x_values)
    #ax1.set_xlim([XAXIS_MIN, XAXIS_MAX])

    for label in ax1.get_yticklabels() :
        label.set_fontproperties(TICK_FP)
    for label in ax1.get_xticklabels() :
        label.set_fontproperties(TICK_FP)

    return fig


###################################################################################
# PLOT HELPERS
###################################################################################

# BENCHMARK -- PLOT
def benchmark_plot():

    # CLEAN UP RESULT DIR
    clean_up_dir(BENCHMARK_PLOT_DIR)

    for key_distribution_type in BENCHMARK_EXP_KEY_DISTRIBUTION_TYPES:
        print(MAJOR_STRING)

        for workload_type in BENCHMARK_EXP_WORKLOAD_TYPES:

            datasets = []
            for container_type in BENCHMARK_EXP_CONTAINER_TYPES:

                # Get result file
                result_dir_list = [KEY_DISTRIBUTION_TYPES_STRINGS[key_distribution_type],
                                   WORKLOAD_TYPES_STRINGS[workload_type],
                                   container_type]
                result_file = get_result_file(BENCHMARK_DIR, result_dir_list, BENCHMARK_CSV)

                dataset = loadDataFile(result_file)
                datasets.append(dataset)

            fig = create_benchmark_line_chart(datasets, key_distribution_type)

            file_name = BENCHMARK_PLOT_DIR + "benchmark" + "-" + \
                        KEY_DISTRIBUTION_TYPES_STRINGS[key_distribution_type] + "-" + \
                        WORKLOAD_TYPES_STRINGS[workload_type] + ".pdf"

            saveGraph(fig, file_name, width=OPT_GRAPH_WIDTH, height=OPT_GRAPH_HEIGHT)

###################################################################################
# UTILITIES
###################################################################################

# COLLECT STATS

# Collect stat
def collect_stat(stat_offset):

    # Collect stats
    with open(OUTPUT_FILE) as fp:
        for line in fp:
            line = line.strip()
            line_data = line.split(" ")
            stat = float(line_data[stat_offset])

    return stat

# Write result to a given file that already exists
def write_stat(result_file_name,
               independent_variable,
               stat):

    # Open result file in append mode
    result_file = open(result_file_name, "a")

    # Write out stat
    result_file.write(str(independent_variable) + " , " + str(stat) + "\n")

    result_file.close()

# Print ETA
def print_eta(*args):
    eta_seconds = 1
    for arg in args:
        eta_seconds = eta_seconds * arg

    # Factor in repeat count and default duration
    eta_seconds = eta_seconds * DEFAULT_DURATION
    eta_seconds = eta_seconds * REPEAT_COUNT

    LOG.info("EXPECTED TIME TO COMPLETE (HH:MM::SS): " +
             str(datetime.timedelta(seconds=eta_seconds)))

###################################################################################
# EVAL
###################################################################################

# BENCHMARK -- EVAL
def benchmark_eval():

    # CLEAN UP RESULT DIR
    clean_up_dir(BENCHMARK_DIR)
    LOG.info("BENCHMARK EVAL")

    # ETA
    l1 = len(BENCHMARK_EXP_KEY_DISTRIBUTION_TYPES)
    l2 = len(BENCHMARK_EXP_WORKLOAD_TYPES)
    l3 = len(BENCHMARK_EXP_CONTAINER_TYPES)
    l4 = len(BENCHMARK_EXP_THREAD_COUNTS)
    print_eta(l1, l2, l3, l4)

    for key_distribution_type in BENCHMARK_EXP_KEY_DISTRIBUTION_TYPES:
        LOG.info(MAJOR_STRING)

        for workload_type in BENCHMARK_EXP_WORKLOAD_TYPES:
            LOG.info(MINOR_STRING)

            for container_type in BENCHMARK_EXP_CONTAINER_TYPES:
                LOG.info(SUB_MINOR_STRING)

                for thread_count in BENCHMARK_EXP_THREAD_COUNTS:
                    LOG.info(" > key_distribution_type: " + KEY_DISTRIBUTION_TYPES_STRINGS[key_distribution_type] +
                          " workload_type: " + WORKLOAD_TYPES_STRINGS[workload_type] +
                          " container_type: " + container_type +
                          " thread_count: " + str(thread_count) +
                          "\n"
                    )

                    # Get result file
                    result_dir_list = [KEY_DISTRIBUTION_TYPES_STRINGS[key_distribution_type],
                                       WORKLOAD_TYPES_STRINGS[workload_type],
                                       container_type]
                    result_file = get_result_file(BENCHMARK_DIR, result_dir_list, BENCHMARK_CSV)

                    # Run experiment
                    stat = run_experiment(stat_offset=THROUGHPUT_OFFSET,
                                          key_distribution_type=key_distribution_type,
                                          workload_type=workload_type,
                                          container_type=container_type,
                                          thread_count=thread_count)

                    # Write stat
                    write_stat(result_file, thread_count, stat)

###################################################################################
# TEST
###################################################################################

#  RUN PARTICULAR TEST
def run_test(test_id):
    LOG.info("Running test : " + str(test_id))

    if test_id == 0 or test_id not in TEST_STRINGS:
        LOG.error("INVALID TEST ID: " + str(test_id))
        return True

    # Run experiment
    return run_experiment(test_id=test_id,
                          log_to_stderr=1)

# RUN ALL TESTS
def run_all_tests():
    LOG.info("Running all tests")

    for test in TEST_STRINGS:
        run_test(test)

    return True

###################################################################################
# EVAL HELPERS
###################################################################################

# CLEAN UP RESULT DIR
def clean_up_dir(result_directory):

    subprocess.call(['rm', '-rf', result_directory])
    if not os.path.exists(result_directory):
        os.makedirs(result_directory)

# RUN EXPERIMENT
def run_experiment(
    program=RELEASE_PROGRAM,
    log_to_stderr=DEFAULT_LOG_TO_STDERR,
    array_size=DEFAULT_ARRAY_SIZE,
    benchmark_type=DEFAULT_BENCHMARK_TYPE,
    container_type=DEFAULT_CONTAINER_TYPE,
    clflush_type=DEFAULT_CLFLUSH_TYPE,
    database_size=DEFAULT_DATABASE_SIZE,
    inner_node_fraction=DEFAULT_INNER_NODE_FRACTION,
    key_size=DEFAULT_KEY_SIZE,
    key_distribution_type=DEFAULT_KEY_DISTRIBUTION_TYPE,
    prefill_size=DEFAULT_PREFILL_SIZE,
    page_size=DEFAULT_PAGE_SIZE,
    duration=DEFAULT_DURATION,
    thread_count=DEFAULT_THREAD_COUNT,
    unsorted_fraction=DEFAULT_UNSORTED_FRACTION,
    workload_type=DEFAULT_WORKLOAD_TYPE,
    write_delay=DEFAULT_WRITE_DELAY,
    test_id=DEFAULT_TEST_ID,
    verbosity=DEFAULT_VERBOSITY,
    function_retry_count=1,
    function_repeat_count=1,
    stat_offset=THROUGHPUT_OFFSET,
    sum_stat=0):

    # subprocess.call(["rm -f " + OUTPUT_FILE], shell=True)
    PROGRAM_OUTPUT_FILE_NAME = "bztree.txt"
    PROGRAM_OUTPUT_FILE = open(PROGRAM_OUTPUT_FILE_NAME, "w")
    arg_list = [program,
                    "--logtostderr=" + str(log_to_stderr), "",
                    "-array_size", str(array_size),
                    "-benchmarks", str(benchmark_type),
                    "-container", container_type,
                    "-database_size", str(database_size),
                    "-inner_node_fraction", str(inner_node_fraction),
                    "-key_size", str(key_size),
                    "-prefill_size", str(prefill_size),
                    "-page_size", str(page_size),
                    "-seconds", str(duration),
                    "-threads", str(thread_count),
                    "-unsorted_fraction", str(unsorted_fraction),
                    "-update_percent", str(workload_type),
                    "-use_clflush", str(clflush_type),
                    "-random_number_distribution", KEY_DISTRIBUTION_TYPES_STRINGS[key_distribution_type],
                    "-test_id", str(test_id),
                    "-write_delay_ns", str(write_delay),
                ]
    arg_string = ' '.join(arg_list[0:])

    if function_repeat_count == 1:
        LOG.info(arg_string)

    ## Run and check return status
    run_status = True
    try:
        subprocess.check_call(arg_list,
                              stdout=PROGRAM_OUTPUT_FILE,
                              timeout = TIMEOUT_DURATION)
    except subprocess.TimeoutExpired as e:
        LOG.error("TIMED OUT: " + PROGRAM_NAME)
        run_status = False
    except subprocess.CalledProcessError as e:
        LOG.error("FAILED: " + PROGRAM_NAME)
        run_status = False

    ## Check output file
    PROGRAM_OUTPUT_FILE.close()
    PROGRAM_OUTPUT_FILE = open(PROGRAM_OUTPUT_FILE_NAME, "r")
    for line in PROGRAM_OUTPUT_FILE:
        if re.search("error", line):
              LOG.error("FAILED: " + PROGRAM_NAME + "\nERROR :: " + line)
              run_status = False
        if re.search("fail", line):
              LOG.error("FAILED: " + PROGRAM_NAME + "\nERROR :: " + line)
              run_status = False

    ## Clean up output file
    PROGRAM_OUTPUT_FILE.close()
    os.remove(PROGRAM_OUTPUT_FILE_NAME)

    ## Retry if needed
    if function_retry_count > RETRY_COUNT:
        LOG.error("MAX RETRY COUNT EXCEEDED: " + arg_string)
        exit(EXIT_FAILURE)
    elif run_status == False:
        function_retry_count = function_retry_count + 1
        LOG.info("RETRY ATTEMPT: " + str(function_retry_count))
    elif function_repeat_count < REPEAT_COUNT:
        # Sum stat
        stat = collect_stat(stat_offset)
        sum_stat = sum_stat + stat
        function_repeat_count = function_repeat_count + 1
        LOG.info("REPEAT ATTEMPT: " + str(function_repeat_count))
    elif function_repeat_count == REPEAT_COUNT:
        # Sum stat
        stat = collect_stat(stat_offset)
        sum_stat = sum_stat + stat
        average_stat = sum_stat/REPEAT_COUNT
        LOG.info("Average stat: " + str(average_stat))
        return average_stat

    # Retry/Repeat if needed
    average_stat = run_experiment(
        program=program,
        log_to_stderr=log_to_stderr,
        array_size=array_size,
        benchmark_type=benchmark_type,
        container_type=container_type,
        clflush_type=clflush_type,
        database_size=database_size,
        inner_node_fraction=inner_node_fraction,
        key_size=key_size,
        key_distribution_type=key_distribution_type,
        prefill_size=prefill_size,
        page_size=page_size,
        duration=duration,
        thread_count=thread_count,
        unsorted_fraction=unsorted_fraction,
        workload_type=workload_type,
        write_delay=write_delay,
        test_id=test_id,
        verbosity=verbosity,
        function_retry_count=function_retry_count,
        function_repeat_count=function_repeat_count,
        stat_offset=stat_offset,
        sum_stat=sum_stat
    )

    return average_stat


## ==============================================
##  MAIN
## ==============================================
if __name__ == '__main__':

    parser = argparse.ArgumentParser(description='Run Machine Experiments')

    ## EVALUATION GROUP
    evaluation_group = parser.add_argument_group('evaluation_group')
    evaluation_group.add_argument("-a", "--benchmark_eval", help="eval benchmark", action='store_true')

    ## PLOTTING GROUP
    plotting_group = parser.add_argument_group('plotting_group')
    plotting_group.add_argument("-m", "--benchmark_plot", help="plot benchmark", action='store_true')

    args = parser.parse_args()

    ## EVALUATION GROUP

    if args.benchmark_eval:
        benchmark_eval()

    ## PLOTTING GROUP

    if args.benchmark_plot:
        benchmark_plot()

    ## LEGEND GROUP

    #create_legend_latency_type()

