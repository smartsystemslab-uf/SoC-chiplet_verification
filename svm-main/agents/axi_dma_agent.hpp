#pragma once

#include "vkit/agent.hpp"
#include <systemc>
#include <cstdint>
#include <string>

namespace items {
struct dma_burst : vkit::sequence_item {
  std::uint64_t src{0};
  std::uint64_t dst{0};
  std::uint32_t len{0};
};
} // namespace items

struct axi_dma_driver : vkit::driver_if {
  void drive(const vkit::sequence_item& base) override {
    auto& it = static_cast<const items::dma_burst&>(base);
    std::string msg = "DMA burst: src=0x" + to_hex(it.src) +
                      " dst=0x" + to_hex(it.dst) +
                      " len="   + std::to_string(it.len);
    SC_REPORT_INFO("axi_dma_driver", msg.c_str());
  }

private:
  static std::string to_hex(std::uint64_t v) {
    std::ostringstream oss;
    oss << std::hex << v;
    return oss.str();
  }
};

struct axi_dma_monitor : vkit::monitor_if {
  void start() override {
    SC_REPORT_INFO("axi_dma_monitor", "monitor stub started");
  }
};

struct axi_dma_scoreboard : vkit::scoreboard_if {
  void push_observation(const vkit::sequence_item&) override {}
  void finalize() override {
    SC_REPORT_INFO("axi_dma_scoreboard", "DMA scoreboard finalize (stub)");
  }
};

struct axi_dma_agent : vkit::agent {
  std::unique_ptr<axi_dma_driver>      d;
  std::unique_ptr<axi_dma_monitor>     m;
  std::unique_ptr<axi_dma_scoreboard>  s;

  explicit axi_dma_agent(sc_core::sc_module_name nm)
    : vkit::agent(nm)
  {
    d = std::make_unique<axi_dma_driver>();
    m = std::make_unique<axi_dma_monitor>();
    s = std::make_unique<axi_dma_scoreboard>();
  }

  vkit::driver_if*    driver()    override { return d.get(); }
  vkit::monitor_if*   monitor()   override { return m.get(); }
  vkit::scoreboard_if*scoreboard()override { return s.get(); }
};
