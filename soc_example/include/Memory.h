#pragma once
#include <systemc>
#include <tlm>
#include <tlm_utils/simple_target_socket.h>
#include <vector>
#include <cstdint>
#include <cstring>

struct Memory : sc_core::sc_module {
  tlm_utils::simple_target_socket<Memory> tsock{"tsock"};
  tlm::tlm_analysis_port<uint32_t>        data_ap{"data_ap"};

  SC_HAS_PROCESS(Memory);

  Memory(sc_core::sc_module_name n,
         size_t words,
         sc_core::sc_time latency = sc_core::sc_time(10, sc_core::SC_NS))
    : sc_module(n), mem_(words, 0), latency_(latency) {
    tsock.register_b_transport(this, &Memory::b_transport);
  }

  void b_transport(tlm::tlm_generic_payload& trans, sc_core::sc_time& delay) {
    auto cmd  = trans.get_command();
    auto addr = trans.get_address();
    auto* ptr = trans.get_data_ptr();
    auto  len = trans.get_data_length();

    // Basic checks (word aligned, 4-byte access, address range)
    if ((addr & 0x3) != 0 || len != 4 || (addr >> 2) >= mem_.size()) {
      trans.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
      return;
    }

    delay += latency_;
    uint32_t& reg = mem_[addr >> 2];

    if (cmd == tlm::TLM_WRITE_COMMAND) {
      uint32_t val;
      std::memcpy(&val, ptr, sizeof(val));
      reg = val;

      // Publish to analysis port for the counter IP
      data_ap.write(val);

      trans.set_response_status(tlm::TLM_OK_RESPONSE);
    } else if (cmd == tlm::TLM_READ_COMMAND) {
      std::memcpy(ptr, &reg, sizeof(reg));
      trans.set_response_status(tlm::TLM_OK_RESPONSE);
    } else {
      trans.set_response_status(tlm::TLM_COMMAND_ERROR_RESPONSE);
    }
  }

private:
  std::vector<uint32_t> mem_;
  sc_core::sc_time      latency_;
};
