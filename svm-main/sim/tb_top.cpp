#include <systemc>
#include <filesystem>

#include "soc/soc_top.h"
#include "vkit/env.hpp"
#include "vkit/sequencer.hpp"

struct tb_top : sc_core::sc_module {
  soc_top*         soc{};
  env*             e{};
  vkit::sequencer* seq{};

  // No SC_HAS_PROCESS, no SC_THREAD — this is a pure container.
  explicit tb_top(sc_core::sc_module_name nm)
  : sc_core::sc_module(nm)
  {
    // Instantiate DUT
    soc = new soc_top("dut");

    // Sequencer: consumes tests from tests/generated
    seq = new vkit::sequencer("sequencer",
                              std::filesystem::path("tests/generated"));

    // Env: reads manifest, creates agents, registers them with sequencer
    e   = new env("env", "soc/manifest.json");
    e->seq = seq;
    e->soc = soc;
  }
};


// sim/tb_top.cpp
/*
#include <systemc>
#include "soc/soc_top.h"
#include "vkit/env.hpp"

struct tb_top : sc_core::sc_module {
  soc_top* soc{}; env* e{}; vkit::sequencer* seq{};
  SC_CTOR(tb_top) {
    soc = new soc_top("dut");
    seq = new vkit::sequencer("sequencer", std::filesystem::path("tests/generated"));
    e   = new env("env", "soc/manifest.json");
    e->seq = seq; e->soc = soc;
  }
};

*/

/*
#include <systemc>
#include <filesystem>

#include "soc/soc_top.h"
#include "vkit/env.hpp"
#include "vkit/sequencer.hpp"

struct tb_top : sc_core::sc_module {
  soc_top*         soc{};
  env*             e{};
  vkit::sequencer* seq{};

  SC_HAS_PROCESS(tb_top);

  explicit tb_top(sc_core::sc_module_name nm)
  : sc_core::sc_module(nm)
  {
    // Instantiate DUT
    soc = new soc_top("dut");

    // Sequencer: looks for JSON test suites here
    seq = new vkit::sequencer("sequencer",
                              std::filesystem::path("tests/generated"));

    // Env: reads manifest, creates agents, registers them with the sequencer
    e   = new env("env", "soc/manifest.json");
    e->seq = seq;
    e->soc = soc;

    // Main test thread
    SC_THREAD(run);
  }

  void run() {
    SC_REPORT_INFO("tb_top", "Starting build/connect phases");

    // Call UVM-like phases manually (SystemC doesn't do this for us)
    e->build_phase();
    e->connect_phase();
    seq->build_phase();
    seq->connect_phase();

    SC_REPORT_INFO("tb_top", "Starting sequencer run_phase");
    seq->run_phase();  // This will drive all IP agents using generated JSON tests
    SC_REPORT_INFO("tb_top", "Sequencer run_phase complete, stopping simulation");

    sc_core::sc_stop();
  }
  
};
*/
