##################################################################################

# --[ Machine library

# Create our library
add_library (machine_library cache.cpp configuration.cpp workload.cpp)

# Make sure the compiler can find include files for our machine library
# when other libraries or executables link to machine
target_include_directories (machine_library PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)