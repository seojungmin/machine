# Create a library called "machine" which includes the desired source files
# The extension is already found. Any number of sources could be listed here.
add_library (machine foo.cpp)

# Make sure the compiler can find include files for our machine library
# when other libraries or executables link to machine
target_include_directories (machine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)