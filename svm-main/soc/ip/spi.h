#pragma once

#include <systemc>
#include <vector>
#include <cstdint>

struct spi : sc_core::sc_module {
  unsigned mode{0};

  SC_HAS_PROCESS(spi);

  explicit spi(sc_core::sc_module_name nm)
  : sc_core::sc_module(nm)
  {
    // Stub implementation: no TLM sockets yet.
    // Add processes / interfaces later when you model real SPI behavior.
  }

  // Simple behavioral hook you can call later if you want:
  void xfer(const std::vector<std::uint8_t>& tx, std::vector<std::uint8_t>& rx) {
    // For now, just loop back.
    rx = tx;
  }
};
