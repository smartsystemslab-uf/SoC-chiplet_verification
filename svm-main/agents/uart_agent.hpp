#pragma once

#include "vkit/agent.hpp"
#include <systemc>
#include <vector>
#include <string>

namespace items {
struct uart_tx : vkit::sequence_item {
  std::uint32_t baud{115200};
  std::vector<std::uint8_t> payload;
  bool parity{false};
};
} // namespace items

struct uart_driver : vkit::driver_if {
  void drive(const vkit::sequence_item& base) override {
    auto& it = static_cast<const items::uart_tx&>(base);
    std::string s(it.payload.begin(), it.payload.end());
    std::string msg = "UART TX: baud=" + std::to_string(it.baud) +
                      " len=" + std::to_string(it.payload.size()) +
                      " data=\"" + s + "\"";
    SC_REPORT_INFO("uart_driver", msg.c_str());
  }
};

struct uart_monitor : vkit::monitor_if {
  void start() override {
    // Stub: in a real design, you would observe RX activity here
    SC_REPORT_INFO("uart_monitor", "monitor stub started");
  }
};

struct uart_scoreboard : vkit::scoreboard_if {
  void push_observation(const vkit::sequence_item&) override {
    // Stub: collect and compare against golden here
  }
  void finalize() override {
    SC_REPORT_INFO("uart_scoreboard", "UART scoreboard finalize (stub)");
  }
};

struct uart_agent : vkit::agent {
  std::unique_ptr<uart_driver>      d;
  std::unique_ptr<uart_monitor>     m;
  std::unique_ptr<uart_scoreboard>  s;

  explicit uart_agent(sc_core::sc_module_name nm)
    : vkit::agent(nm)
  {
    d = std::make_unique<uart_driver>();
    m = std::make_unique<uart_monitor>();
    s = std::make_unique<uart_scoreboard>();
  }

  vkit::driver_if*    driver()    override { return d.get(); }
  vkit::monitor_if*   monitor()   override { return m.get(); }
  vkit::scoreboard_if*scoreboard()override { return s.get(); }
};
