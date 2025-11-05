#pragma once
#include <systemc>
#include <tlm>
#include <tlm_utils/simple_initiator_socket.h>
#include <random>
#include <cstdint>

struct RandomGen : sc_core::sc_module {
  tlm_utils::simple_initiator_socket<RandomGen> isock{"isock"};

  SC_HAS_PROCESS(RandomGen);

  RandomGen(sc_core::sc_module_name n,
            unsigned num_items,
            uint32_t seed = 0xC0FFEEu,
            sc_core::sc_time issue_period = sc_core::sc_time(20, sc_core::SC_NS))
    : sc_module(n),
      num_items_(num_items),
      rng_(seed),
      period_(issue_period) {
    SC_THREAD(generate_thread);
  }

private:
  void generate_thread() {
    std::uniform_int_distribution<uint32_t> dist(0, 0xFFFFFFFFu);

    tlm::tlm_generic_payload trans;
    sc_core::sc_time delay = sc_core::SC_ZERO_TIME;

    for (unsigned i = 0; i < num_items_; ++i) {
      uint32_t data = dist(rng_);

      // Prepare a blocking WRITE
      trans.set_command(tlm::TLM_WRITE_COMMAND);
      trans.set_address(static_cast<sc_dt::uint64>(i * 4)); // word address
      trans.set_data_length(4);
      trans.set_streaming_width(4);
      trans.set_byte_enable_ptr(nullptr);
      trans.set_dmi_allowed(false);
      trans.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);

      auto* dp = reinterpret_cast<unsigned char*>(&data);
      trans.set_data_ptr(dp);

      delay = sc_core::SC_ZERO_TIME;
      isock->b_transport(trans, delay);

      if (trans.get_response_status() != tlm::TLM_OK_RESPONSE) {
        SC_REPORT_ERROR("/RandomGen", "Transaction failed");
      }

      wait(period_);
    }

    std::cout << "\n[RandomGen] Finished generating " << num_items_
              << " items at " << sc_core::sc_time_stamp() << "\n";
  }

  unsigned           num_items_;
  std::mt19937       rng_;
  sc_core::sc_time   period_;
};
