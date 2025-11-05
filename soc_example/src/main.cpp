#include <systemc>
#include "Top.h"

int sc_main(int argc, char* argv[]) {
  (void)argc; (void)argv;
  Top top("Top");
  sc_core::sc_start(sc_core::sc_time(1, sc_core::SC_MS));
  std::cout << "\n[SC_MAIN] Finished at " << sc_core::sc_time_stamp() << "\n";
  return 0;
}
