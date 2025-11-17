#pragma once

#include <systemc>
#include <cstdint>

struct timer : sc_core::sc_module {
  bool          running{false};
  unsigned      period_us{10};   // tick period in microseconds
  std::uint64_t ticks{0};        // number of ticks since start

  SC_HAS_PROCESS(timer);

  explicit timer(sc_core::sc_module_name nm)
  : sc_module(nm)
  {
    SC_THREAD(run);
  }

  // Called by driver to start the timer with a given period
  void start(unsigned new_period_us) {
    period_us = new_period_us;
    running   = true;
    SC_REPORT_INFO(name(), "Timer started");   // ? no sc_core::
  }

  // Called by driver to stop the timer
  void stop() {
    running = false;
    SC_REPORT_INFO(name(), "Timer stopped");   // ? no sc_core::
  }

private:
  void run() {
    while (true) {
      if (running) {
        ++ticks;
      }
      wait(sc_core::sc_time(period_us, sc_core::SC_US));
    }
  }
};
