cmake_minimum_required(VERSION 3.12)
project(soc_example CXX)

# Set these to your SystemC install
# Example: -DSYSTEMC_HOME=/opt/systemc-2.3.4
if(NOT DEFINED ENV{SYSTEMC_HOME})
  message(FATAL_ERROR "Please set SYSTEMC_HOME environment variable")
endif()

set(SYSTEMC_HOME $ENV{SYSTEMC_HOME})
include_directories(${SYSTEMC_HOME}/include)
link_directories(${SYSTEMC_HOME}/lib ${SYSTEMC_HOME}/lib-linux64)

add_executable(soc_example
  src/main.cpp
)

target_include_directories(soc_example PRIVATE include)
target_compile_features(soc_example PRIVATE cxx_std_17)
target_link_libraries(soc_example systemc)
