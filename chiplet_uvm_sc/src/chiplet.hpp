#pragma once
#include <systemc>
#include <tlm>
#include "lane_txn.hpp"

struct chiplet : sc_core::sc_module {
  tlm::tlm_target_socket<> t_skt; // receive routed txns
  unsigned id;

  SC_HAS_PROCESS(chiplet);
  chiplet(sc_core::sc_module_name nm, unsigned id_) : sc_module(nm), t_skt("t_skt"), id(id_) {
    t_skt.register_b_transport(this, &chiplet::b_transport);
  }

  void b_transport(tlm::tlm_generic_payload& gp, sc_core::sc_time& delay) {
    lane_txn* lt = gp.get_extension<lane_txn>();
    if(lt && !lt->write) {
      uint32_t val = static_cast<uint32_t>(lt->addr.to_uint()) ^ id;
      unsigned char* d = gp.get_data_ptr();
      if(d) *reinterpret_cast<uint32_t*>(d) = val;
    }
    wait(sc_core::sc_time(10, sc_core::SC_NS));
  }
};
