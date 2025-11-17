#pragma once

#include "factory.hpp"
#include "sequencer.hpp"
#include "utils/json.hpp"

#include <fstream>
#include <string>

// Include agent types so we can register them
#include "agents/uart_agent.hpp"
#include "agents/spi_agent.hpp"
#include "agents/axi_dma_agent.hpp"
#include "agents/timer_agent.hpp"

struct env : vkit::component {
  vkit::sequencer* seq{};                  // Provided by tb_top
  factory<vkit::agent> agent_factory;
  std::string manifest_path;
  sc_core::sc_module* soc{};               // Provided by tb_top

  env(sc_core::sc_module_name nm, const std::string& manifest)
    : vkit::component(nm)
    , manifest_path(manifest)
  {}

  void build_phase() override {
    // Register known agents with explicit std::string parameter
    agent_factory.reg("uart",
      [](const std::string& n) -> vkit::agent* {
        return new uart_agent(n.c_str());
      });

    agent_factory.reg("spi",
      [](const std::string& n) -> vkit::agent* {
        return new spi_agent(n.c_str());
      });

    agent_factory.reg("axi_dma",
      [](const std::string& n) -> vkit::agent* {
        return new axi_dma_agent(n.c_str());
      });

    agent_factory.reg("timer",
      [](const std::string& n) -> vkit::agent* {
        return new timer_agent(n.c_str());
      });
  }

  void connect_phase() override {
    // Parse manifest, instantiate agents, connect sockets (later)
    json j;
    std::ifstream ifs(manifest_path);
    if (!ifs.is_open()) {
      SC_REPORT_ERROR(name(), "Failed to open SoC manifest");
      return;
    }
    ifs >> j;

    for (auto& ip : j["ips"]) {
      const auto ip_name = ip["name"].get<std::string>();
      const auto ip_type = ip["type"].get<std::string>();

      vkit::agent* ag = agent_factory.make(ip_type, ip_name + "_agent");
      if (!ag) {
        std::string msg = "Unknown IP type in manifest: " + ip_type;
        SC_REPORT_ERROR(name(), msg.c_str());
        continue;
      }

      // TODO: connect agent driver/monitor sockets to SoC here
      // For now, just register with the sequencer so tests can run.
      if (seq) {
        seq->register_agent(ip_name, ag);
      } else {
        SC_REPORT_ERROR(name(), "Sequencer pointer is null in env");
      }
    }
  }
};
