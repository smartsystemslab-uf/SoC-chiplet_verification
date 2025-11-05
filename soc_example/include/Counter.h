#pragma once
#include <systemc>
#include <tlm>

// Implement the writer interface and expose it via sc_export.
// This binds cleanly to tlm_analysis_port<uint32_t> on the Memory side.
struct Counter : sc_core::sc_module, tlm::tlm_write_if<uint32_t> {
  sc_core::sc_export<tlm::tlm_write_if<uint32_t>> analysis_export{"analysis_export"};

  SC_HAS_PROCESS(Counter);

  explicit Counter(sc_core::sc_module_name n) : sc_module(n) {
    analysis_export.bind(*this);
  }

  // Exact signature required by tlm_write_if<T>
  void write(const uint32_t& /*value*/) override {
    ++count_;
  }

  void end_of_simulation() override {
    std::cout << "\n[Counter] Total items observed: " << count_ << "\n";
  }

private:
  uint64_t count_{0};
};

