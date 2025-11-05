#pragma once
#include "RandomGen.h"
#include "Memory.h"
#include "Counter.h"

struct Top : sc_core::sc_module {
  RandomGen gen;
  Memory    mem;
  Counter   cnt;

  explicit Top(sc_core::sc_module_name n)
    : sc_module(n),
      gen("gen", /*num_items=*/32, /*seed=*/0x1234u),
      mem("mem", /*words=*/1024),
      cnt("counter")
  {
    // Generator writes to memory
    gen.isock.bind(mem.tsock);
    // Memory publishes each written datum to the counter
    mem.data_ap.bind(cnt.analysis_export);
  }
};
