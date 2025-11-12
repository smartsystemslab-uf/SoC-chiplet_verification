#pragma once
#include <systemc>
#include <tlm>
#include "lane_txn.hpp"

// Simple interconnect that routes by chiplet_id to 4 targets
struct mp_lane : sc_core::sc_module {
  tlm::tlm_target_socket<>   t_skt;      // from drivers
  tlm::tlm_initiator_socket<> i_skt[4];  // to chiplets

  SC_CTOR(mp_lane) : t_skt("t_skt") {
    for(int i=0;i<4;++i) {
      std::string n = std::string("i_skt_") + char('0'+i);
      i_skt[i].init(n.c_str());
    }
    t_skt.register_b_transport(this, &mp_lane::b_transport);
  }

  void b_transport(tlm::tlm_generic_payload& gp, sc_core::sc_time& delay) {
    lane_txn* lt = gp.get_extension<lane_txn>();
    unsigned id = lt ? lt->chiplet_id.to_uint() : 0;
    if(id>3) id=0;
    i_skt[id]->b_transport(gp, delay);
  }
};
