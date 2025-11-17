#include <systemc>
#include "tb_top.cpp"

int sc_main(int argc, char* argv[]) {
  (void)argc;
  (void)argv;

  tb_top tb("tb");

  // Call vkit-style phases *before* the simulation starts,
  // so any agents (which are sc_modules) are created during elaboration.
  SC_REPORT_INFO("sc_main", "Calling build/connect phases");

  tb.e->build_phase();
  tb.e->connect_phase();
  tb.seq->build_phase();
  tb.seq->connect_phase();

  SC_REPORT_INFO("sc_main", "Running sequencer run_phase");
  tb.seq->run_phase();  // drives all agents based on JSON tests

  // We don't actually need time-based simulation for the current stub setup,
  // but we can still let SystemC run for 0 time to be well-formed.
  sc_core::sc_start(sc_core::SC_ZERO_TIME);

  SC_REPORT_INFO("sc_main", "Simulation done");
  return 0;
}
