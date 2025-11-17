// soc/soc_top.cpp
/*
#include "soc_top.h"
using namespace sc_core;

soc_top::soc_top(sc_module_name nm)
: sc_module(nm)
, ctrl_init("ctrl_init") {
  u_uart  = std::make_unique<uart>("uart0");
  u_spi   = std::make_unique<spi>("spi0");
  u_dma   = std::make_unique<axi_dma>("dma0");
  u_timer = std::make_unique<timer>("timer0");

  // Example: connect ctrl_init to a simple register router within each IP
  // (In a real SoC, you'd have a bus fabric. For brevity, we show direct binds.)
  ctrl_init.bind(u_uart->reg_tgt);
  ctrl_init.bind(u_spi->reg_tgt);
  ctrl_init.bind(u_dma->reg_tgt);
  //ctrl_init.bind(u_timer->reg_tgt);
}
*/

#include "soc_top.h"

using namespace sc_core;

soc_top::soc_top(sc_module_name nm)
: sc_module(nm)
{
  // Construct IP instances
  u_uart  = std::make_unique<uart>("uart0");
  u_spi   = std::make_unique<spi>("spi0");
  u_dma   = std::make_unique<axi_dma>("dma0");
  u_timer = std::make_unique<timer>("timer0");

  // NOTE:
  // We no longer have a ctrl_init bus or bind any TLM target sockets here.
  // This avoids sc_export "no interface" errors for stubbed IPs.
  //
  // When you later add a real control bus and proper TLM target implementations
  // (with b_transport or simple_target_socket), you can re-introduce a
  // bus + binding in a controlled way.
}
