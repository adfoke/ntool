cmake_minimum_required(VERSION 3.10)

# Project Name and Version
project(ntool VERSION 1.0.0 DESCRIPTION "A small network tool for the terminal" LANGUAGES C)

# Set C Standard (e.g., C11)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

# Add include directory
include_directories(include)

# Source files
set(SOURCES
    src/main.c
    src/common.c
    src/tcp/tcp.c
    src/udp/udp.c
)

# Add the executable
add_executable(ntool ${SOURCES})

# Optional: Enable warnings
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_C_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Installation rules
install(TARGETS ntool DESTINATION bin)
install(FILES README.md DESTINATION share/doc/ntool)

# Add uninstall target
if(NOT TARGET uninstall)
  configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

  add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif()