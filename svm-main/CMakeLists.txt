cmake_minimum_required(VERSION 3.20)
project(svm LANGUAGES CXX)

# Match the C++ standard used to build SystemC (your working standalone example).
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Pick up SYSTEMC_HOME from the environment.
set(SYSTEMC_HOME $ENV{SYSTEMC_HOME} CACHE PATH "Path to SystemC installation")

if (NOT SYSTEMC_HOME)
  message(FATAL_ERROR "SYSTEMC_HOME is not set. Please export SYSTEMC_HOME before configuring.")
endif()

message(STATUS "Using SYSTEMC_HOME = ${SYSTEMC_HOME}")

set(SYSTEMC_INCLUDE_DIR "${SYSTEMC_HOME}/include")
set(SYSTEMC_LIBRARY_DIR "${SYSTEMC_HOME}/lib-linux64")

# Main executable
add_executable(uvm_lite_sim
  sim/main.cpp
  sim/tb_top.cpp
  soc/soc_top.cpp
  vkit/sequencer.cpp       # <-- implementation of ag_deserialize
)

# Includes: project + SoC + framework + agents + SystemC
target_include_directories(uvm_lite_sim PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/soc
  ${CMAKE_CURRENT_SOURCE_DIR}/vkit
  ${CMAKE_CURRENT_SOURCE_DIR}/agents
  ${SYSTEMC_INCLUDE_DIR}
)

# Link against SystemC like your working command
target_link_directories(uvm_lite_sim PRIVATE
  ${SYSTEMC_LIBRARY_DIR}
)

target_link_libraries(uvm_lite_sim PRIVATE
  systemc
  m
)