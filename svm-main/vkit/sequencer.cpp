#include "sequencer.hpp"

#include "agents/uart_agent.hpp"
#include "agents/spi_agent.hpp"
#include "agents/axi_dma_agent.hpp"
#include "agents/timer_agent.hpp"

using nlohmann::json; // from vkit/utils/json.hpp via sequencer.hpp

std::unique_ptr<vkit::sequence_item>
vkit::sequencer::ag_deserialize(const std::string& ip, const json& v) {
  // UART
  if (ip.rfind("uart", 0) == 0) {
    auto p = std::make_unique<items::uart_tx>();
    p->baud   = v.value("baud", 115200u);
    // parity in JSON is stored as string ("none", etc.), we keep a bool stub
    p->parity = false;
    if (v.contains("payload")) {
      for (auto b : v["payload"]) {
        p->payload.push_back(static_cast<std::uint8_t>(b.get<int>()));
      }
    }
    return p;
  }

  // SPI
  if (ip.rfind("spi", 0) == 0) {
    auto p = std::make_unique<items::spi_xfer>();
    p->mode = v.value("mode", 0u);
    if (v.contains("tx")) {
      for (auto b : v["tx"]) {
        p->tx.push_back(static_cast<std::uint8_t>(b.get<int>()));
      }
    }
    return p;
  }

  // DMA
  if (ip.rfind("dma", 0) == 0) {
    auto p = std::make_unique<items::dma_burst>();
    p->len = v.value("len", 0u);
    p->src = v.at("src").get<std::uint64_t>();
    p->dst = v.at("dst").get<std::uint64_t>();
    return p;
  }

  // TIMER
  if (ip.rfind("timer", 0) == 0) {
    auto p = std::make_unique<items::timer_cmd>();
    const std::string op = v.value("op", std::string("start"));
    p->start     = (op == "start");
    p->period_us = v.value("period_us", 10u);
    return p;
  }

  // Unknown IP type
  SC_REPORT_WARNING("sequencer", ("ag_deserialize: unknown IP " + ip).c_str());
  return {};
}
