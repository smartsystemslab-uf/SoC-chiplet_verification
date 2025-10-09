## Power Monitoring Unit  

The Power Monitoring Unit (PMU) is a hardware module that monitors power consumption, voltage levels, current draw, and temperature across different sections of a System-on-Chip (SoC). It provides real-time visibility into system health and generates alerts when thresholds are exceeded.

## Key Features  

- **2-Channel Power Monitoring:**  
  Independently tracks **Memory** and **I/O** domains.  

- **Real-Time Measurements:**  
  Captures **12-bit voltage** and **12-bit current** values every clock cycle.  

- **Automatic Power Calculation:**  
  Computes instantaneous **P = V × I** for each channel (24-bit result).  

- **Temperature Monitoring:**  
  Monitors both **Memory temperature** and **ambient temperature** (8-bit sensors).  

- **Configurable Alerts:**  
  Programmable threshold registers for **temperature** and **power**.  

- **Clock Gating Control:**  
  Supports both **manual** and **automatic clock gating** to reduce dynamic power.  

- **APB-Lite Interface:**  
  Industry-standard APB interface for easy CPU/software access.  

- **System Status Tracking:**  
  Captures and reports clock enable states, power states, and sequencer status.  

---

## Specifications  

### Measurement Ranges  
| Parameter | Range | Resolution | Notes |
|------------|--------|-------------|-------|
| **Voltage** | 0–4095 | 12-bit | Represents 0–3.3 V |
| **Current** | 0–4095 | 12-bit | Represents 0–4 A |
| **Temperature** | 0–255 | 8-bit | °C scale |
| **Power (V×I)** | 0–16,777,215 | 24-bit | Computed internally |

### Timing  
| Parameter | Description |
|------------|-------------|
| **Clock Frequency** | Up to 100 MHz |
| **Update Rate** | Measurements updated every clock cycle (when enabled) |
| **APB Latency** | 2 clock cycles per read/write transaction |

### Alerts  
- **Temperature Alert (`temp_alert`):** Asserted when **memory** or **ambient** temperature exceeds threshold.  
- **Power Alert (`power_alert`):** Asserted when **memory** or **I/O** channel’s power exceeds threshold.  
- Alerts are **registered outputs** and synchronized to the system clock.  

### Clock Gating  
- **Manual Gating:** Controlled through `clock_control_reg` bits for memory and I/O independently.  
- **Auto Gating:** Automatically disables I/O clock during power or temperature alerts when `auto_gate_enable = 1`.  
- **Default Mode:** All clocks enabled (`clock_enable = 2'b11`).  


## File Structure  

- **pmu.sv** — Main RTL design (Power Monitoring Unit)  
- **pmu_tb.sv** — SystemVerilog testbench for PMU verification  
- **pmu_sim.png** — Simulation waveform / output dump file  
- **README.md** — Project documentation and overview  
- **docs/register_map.pdf** — Detailed register specifications  
- **docs/block_diagram.png** — High-level architecture diagram  
