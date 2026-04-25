
cmake_minimum_required(VERSION 3.10)
project(VexobenOrangeJuice)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add executable
add_executable(code solution.cpp)

# Set compiler flags for optimization
target_compile_options(code PRIVATE -O3 -march=native)

# Link math library if needed
target_link_libraries(code m)
