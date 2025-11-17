// vkit/sequencer.hpp
#pragma once
#include "agent.hpp"
#include "utils/json.hpp"
#include <map>
#include <queue>
#include <filesystem>

namespace vkit {
struct sequencer : component {
  std::map<std::string, agent*> agents; // key: ip_name
  std::filesystem::path tests_root;

  SC_HAS_PROCESS(sequencer);
  sequencer(sc_core::sc_module_name nm, std::filesystem::path root)
    : component(nm), tests_root(std::move(root)) {}

  void register_agent(const std::string& name, agent* a) { agents[name]=a; }

  void run_phase() override {
    // For each IP, read its generated test suite and drive via its driver
    for (auto& [ip, ag] : agents) {
      const auto file = tests_root / ip / "suite.json";
      if (!std::filesystem::exists(file)) {
        SC_REPORT_WARNING(name(), ("No tests for IP " + ip).c_str());
        continue;
      }
      nlohmann::json j;
      std::ifstream ifs(file);
      ifs >> j;
      // Pre-verify: only driver is active
      for (auto& vec : j["vectors"]) {
        // Deserialize to a concrete item understood by the agent
        auto item = ag_deserialize(ip, vec);
        ag->driver()->drive(*item);
      }
      // Post-verify: create monitors & scoreboards and run passive checks
      if (auto m = ag->monitor()) m->start();
      if (auto s = ag->scoreboard()) s->finalize();
    }
  }

  std::unique_ptr<sequence_item> ag_deserialize(const std::string& ip,
                                                const nlohmann::json& v);
};
}
