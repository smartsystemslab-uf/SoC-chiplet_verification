// vkit/base.hpp
#pragma once
#include <systemc>
#include <string>
#include <memory>
#include <functional>
namespace vkit {
struct component : sc_core::sc_module {
  using sc_module::sc_module;
  virtual void build_phase() {}
  virtual void connect_phase() {}
  virtual void start_of_simulation() {}
  virtual void run_phase() {}
  virtual void extract_phase() {}
  virtual void check_phase() {}
  virtual void report_phase() {}
};
}
