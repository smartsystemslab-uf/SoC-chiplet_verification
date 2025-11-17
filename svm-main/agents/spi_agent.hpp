#pragma once

#include "vkit/agent.hpp"
#include <systemc>
#include <vector>
#include <string>

namespace items {
struct spi_xfer : vkit::sequence_item {
  unsigned mode{0};
  std::vector<std::uint8_t> tx;
};
} // namespace items

struct spi_driver : vkit::driver_if {
  void drive(const vkit::sequence_item& base) override {
    auto& it = static_cast<const items::spi_xfer&>(base);
    std::string msg = "SPI XFER: mode=" + std::to_string(it.mode) +
                      " len=" + std::to_string(it.tx.size());
    SC_REPORT_INFO("spi_driver", msg.c_str());
  }
};

struct spi_monitor : vkit::monitor_if {
  void start() override {
    SC_REPORT_INFO("spi_monitor", "monitor stub started");
  }
};

struct spi_scoreboard : vkit::scoreboard_if {
  void push_observation(const vkit::sequence_item&) override {}
  void finalize() override {
    SC_REPORT_INFO("spi_scoreboard", "SPI scoreboard finalize (stub)");
  }
};

struct spi_agent : vkit::agent {
  std::unique_ptr<spi_driver>      d;
  std::unique_ptr<spi_monitor>     m;
  std::unique_ptr<spi_scoreboard>  s;

  explicit spi_agent(sc_core::sc_module_name nm)
    : vkit::agent(nm)
  {
    d = std::make_unique<spi_driver>();
    m = std::make_unique<spi_monitor>();
    s = std::make_unique<spi_scoreboard>();
  }

  vkit::driver_if*    driver()    override { return d.get(); }
  vkit::monitor_if*   monitor()   override { return m.get(); }
  vkit::scoreboard_if*scoreboard()override { return s.get(); }
};
