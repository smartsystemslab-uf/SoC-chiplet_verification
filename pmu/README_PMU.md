## Power Monitoring Unit  

The Power Monitoring Unit (PMU) is a hardware module that monitors power consumption, voltage levels, current draw, and temperature across different sections of a System-on-Chip (SoC). It provides real-time visibility into system health and generates alerts when thresholds are exceeded.


## Key Features

3-Channel Power Monitoring: Monitors CPU, Memory, and I/O domains independently
Real-time Measurements: Captures voltage (12-bit) and current (12-bit) every clock cycle
Automatic Power Calculation: Computes P = V × I for each channel
Temperature Monitoring: Tracks CPU and ambient temperatures
Configurable Alerts: Programmable thresholds for voltage, power, and temperature
APB Interface: Industry-standard bus for easy CPU access
System Status Tracking: Monitors clock states, power states, and sequencer status


## Specifications

### Measurement Ranges

Voltage: 0 to 4095 (12-bit ADC), typically 0-3.3V
Current: 0 to 4095 (12-bit ADC), represents 0-4A
Temperature: 0 to 255°C (8-bit sensor)
Power: 0 to 16,777,215 (24-bit calculated value)

### Timing

Clock Frequency: Supports up to 100MHz
Update Rate: Measurements updated every clock cycle when enabled
APB Latency: 2 clock cycles per transaction

### Alerts

Temperature Alert: Triggers when any temperature sensor exceeds threshold
Power Alert: Triggers when any channel power exceeds threshold
Alerts are registered outputs (synchronized to clock)

## File Structure
pmu/
pmu.sv    # Main RTL design 
pmu.tb    # Main Testbench 
pmu_sim.png #simulation output
├── README.md                   
└── docs/
    ├── register_map.pdf        # Detailed register specifications
    └── block_diagram.png       # Architecture diagram