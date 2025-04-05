cmake_minimum_required(VERSION 3.10)

# Project Name
project(ntool C)

# Set C Standard (e.g., C11)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

# Add include directory (optional for now, but good practice)
# include_directories(include)

# Add the executable
add_executable(ntool src/main.c)

# Link libraries if needed later
# target_link_libraries(net_status PRIVATE some_library)

# Optional: Enable warnings
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_C_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()
