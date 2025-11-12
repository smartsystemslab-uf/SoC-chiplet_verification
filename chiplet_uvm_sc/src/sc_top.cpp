#include <systemc>
#include <uvmc.h>
#include "lane_txn.hpp"
#include "mp_lane.hpp"
#include "chiplet.hpp"
#include "drivers.hpp"

using namespace sc_core;

SC_MODULE(sc_top) {
  lane_driver_sc drv0, drv1, drv2, drv3;
  mp_lane        lane;
  chiplet        c0, c1, c2, c3;

  SC_CTOR(sc_top)
  : drv0("drv0"), drv1("drv1"), drv2("drv2"), drv3("drv3"),
    lane("lane"),
    c0("chiplet0",0), c1("chiplet1",1), c2("chiplet2",2), c3("chiplet3",3)
  {
    // Bind drivers upstream into lane
    drv0.i_skt.bind(lane.t_skt);
    drv1.i_skt.bind(lane.t_skt);
    drv2.i_skt.bind(lane.t_skt);
    drv3.i_skt.bind(lane.t_skt);

    // Bind lane outputs to chiplets
    lane.i_skt[0].bind(c0.t_skt);
    lane.i_skt[1].bind(c1.t_skt);
    lane.i_skt[2].bind(c2.t_skt);
    lane.i_skt[3].bind(c3.t_skt);

    // Bind UVMC channels (from SV driver proxies)
    uvmc_connect(drv0.t_skt, "lane0");
    uvmc_connect(drv1.t_skt, "lane1");
    uvmc_connect(drv2.t_skt, "lane2");
    uvmc_connect(drv3.t_skt, "lane3");
  }
};

int sc_main(int argc, char* argv[]) {
  sc_top top("top");
  sc_start();
  return 0;
}
