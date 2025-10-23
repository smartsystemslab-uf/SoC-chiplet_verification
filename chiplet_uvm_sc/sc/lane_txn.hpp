#pragma once
#include <systemc>
#include <tlm>
#include <uvmc.h>

struct lane_txn {
  sc_dt::sc_uint<32> addr{0};
  sc_dt::sc_uint<32> data{0};
  bool               write{false};
  sc_dt::sc_uint<4>  chiplet_id{0};

  template <typename PACKER> void do_pack(PACKER& p) const {
    p << addr << data << write << chiplet_id;
  }
  template <typename PACKER> void do_unpack(PACKER& p) {
    p >> addr >> data >> write >> chiplet_id;
  }
};

UVMC_UTILS_4(lane_txn, addr, data, write, chiplet_id)
