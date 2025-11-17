#pragma once

#include <systemc>

struct uart : sc_core::sc_module {
  // You can add internal state here later (FIFOs, config, etc.)

  SC_HAS_PROCESS(uart);

  explicit uart(sc_core::sc_module_name nm)
  : sc_core::sc_module(nm)
  {
    // No TLM sockets for now, purely a placeholder module.
    // Framework agents currently just log activity and don't touch the DUT.
  }
};
