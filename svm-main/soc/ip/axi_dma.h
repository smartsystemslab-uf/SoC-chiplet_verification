#pragma once

#include <systemc>
#include <cstdint>
#include <string>
#include <sstream>

struct axi_dma : sc_core::sc_module {
  // Last command info (for debugging/introspection)
  std::uint64_t last_src{0};
  std::uint64_t last_dst{0};
  std::uint32_t last_len{0};

  SC_HAS_PROCESS(axi_dma);

  explicit axi_dma(sc_core::sc_module_name nm)
  : sc_core::sc_module(nm)
  {
    // No TLM target sockets here anymore.
    // This avoids unbound sc_port/sc_export errors during elaboration.
  }

  void do_burst(std::uint64_t src, std::uint64_t dst, std::uint32_t len) {
    last_src = src;
    last_dst = dst;
    last_len = len;

    std::string msg = "DMA burst: src=0x" + to_hex(src) +
                      " dst=0x" + to_hex(dst) +
                      " len="   + std::to_string(len);
    SC_REPORT_INFO(name(), msg.c_str());
  }

private:
  static std::string to_hex(std::uint64_t v) {
    std::ostringstream oss;
    oss << std::hex << v;
    return oss.str();
  }
};
