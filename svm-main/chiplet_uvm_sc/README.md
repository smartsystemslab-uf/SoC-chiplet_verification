# UVM–SystemC Chiplet Verification Starter Kit

This is a plug‑and‑play scaffold that matches the provided diagram: a UVM (SystemVerilog) verifier drives a SystemC/TLM DUT with four *chiplets* over a *multi‑purpose lane*. The bridge is **UVM‑Connect (UVMC)**.

## Prerequisites
- SystemC 2.3.x headers and libraries (`SYSTEMC_HOME`)
- UVMC installed (`UVMC_HOME`)
- One supported simulator (Questa/ModelSim, VCS, or Xcelium) with UVM and UVMC
- CMake ≥ 3.16 and a C++17 compiler for the SystemC side

## Quick Start
```bash
# 0) Set environment
cp scripts/env.example.sh scripts/env.sh
$EDITOR scripts/env.sh
source scripts/env.sh

# 1) Build SystemC shared library
cmake -S . -B build -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build build -j

# 2) Run (Questa by default)
cd sim
make questa   # or: make vcs
```

## Layout
```
chiplet_uvm_sc/
├─ scripts/            # environment setup
├─ sim/                # simulator wrappers
├─ sc/                 # SystemC/TLM DUT + UVMC endpoints
├─ sv/                 # UVM testbench (SV)
├─ cmake/              # helpers (if needed later)
├─ CMakeLists.txt      # builds libdut_sc.so
└─ README.md
```

## Notes
- Change `uvmc_channel` names if you replicate multiple UVM agents; default here uses `lane0` for driver and `mon` for monitor.
- The SystemC side binds four UVMC target sockets (`lane0..lane3`). The default test uses only `lane0` for a smoke run.
- Extend the scoreboard/coverage as you grow tests.
