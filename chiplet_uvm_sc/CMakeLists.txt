cmake_minimum_required(VERSION 3.16)
project(chiplet_uvm_sc LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(NOT DEFINED ENV{SYSTEMC_HOME})
  message(FATAL_ERROR "SYSTEMC_HOME not set")
endif()
set(SystemC_ROOT $ENV{SYSTEMC_HOME})
find_package(SystemC REQUIRED PATHS ${SystemC_ROOT} NO_DEFAULT_PATH)

if(NOT DEFINED ENV{UVMC_HOME})
  message(FATAL_ERROR "UVMC_HOME not set")
endif()

include_directories(
  ${SystemC_INCLUDE_DIRS}
  $ENV{UVMC_HOME}/src/connect
  $ENV{UVMC_HOME}/src/common
  ${CMAKE_CURRENT_SOURCE_DIR}/sc
)

link_directories($ENV{UVMC_HOME}/lib)

add_library(dut_sc SHARED
  sc/sc_top.cpp
)

target_link_libraries(dut_sc PRIVATE SystemC)

set_target_properties(dut_sc PROPERTIES OUTPUT_NAME "libdut_sc")
