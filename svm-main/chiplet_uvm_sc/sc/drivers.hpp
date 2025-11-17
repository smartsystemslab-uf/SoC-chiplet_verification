#pragma once
#include <systemc>
#include <tlm>
#include <uvmc.h>
#include "lane_txn.hpp"

// Driver endpoint that receives UVMC TLM2 transactions from SV
struct lane_driver_sc : sc_core::sc_module {
  tlm::tlm_initiator_socket<>  i_skt; // to mp_lane
  tlm::tlm_target_socket<>     t_skt; // from SV via UVMC

  SC_CTOR(lane_driver_sc) : i_skt("i_skt"), t_skt("t_skt") {
    t_skt.register_b_transport(this, &lane_driver_sc::b_transport);
  }

  void b_transport(tlm::tlm_generic_payload& gp, sc_core::sc_time& delay) {
    i_skt->b_transport(gp, delay);
  }
};
