#pragma once

#include "vkit/agent.hpp"
#include <systemc>
#include <cstdint>

namespace items {
struct timer_cmd : vkit::sequence_item {
  bool     start{true};
  unsigned period_us{10};
};
} // namespace items

struct timer_driver : vkit::driver_if {
  // This driver is “logical” — it just logs actions for now.
  void drive(const vkit::sequence_item& base) override {
    auto& it = static_cast<const items::timer_cmd&>(base);
    std::string msg = std::string("Timer cmd: ") +
                      (it.start ? "start" : "stop") +
                      " period_us=" + std::to_string(it.period_us);
    SC_REPORT_INFO("timer_driver", msg.c_str());
  }
};

struct timer_monitor : vkit::monitor_if {
  void start() override {
    SC_REPORT_INFO("timer_monitor", "monitor stub started");
  }
};

struct timer_scoreboard : vkit::scoreboard_if {
  void push_observation(const vkit::sequence_item&) override {}
  void finalize() override {
    SC_REPORT_INFO("timer_scoreboard", "Timer scoreboard finalize (stub)");
  }
};

struct timer_agent : vkit::agent {
  std::unique_ptr<timer_driver>      d;
  std::unique_ptr<timer_monitor>     m;
  std::unique_ptr<timer_scoreboard>  s;

  explicit timer_agent(sc_core::sc_module_name nm)
    : vkit::agent(nm)
  {
    d = std::make_unique<timer_driver>();
    m = std::make_unique<timer_monitor>();
    s = std::make_unique<timer_scoreboard>();
  }

  vkit::driver_if*    driver()    override { return d.get(); }
  vkit::monitor_if*   monitor()   override { return m.get(); }
  vkit::scoreboard_if*scoreboard()override { return s.get(); }
};
