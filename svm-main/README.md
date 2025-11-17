// System-Level Verification Methodology
// High-level system Description 
// Author: Peter Mbua

1. Reads a system-level SoC (SystemC TLM) with at least 2 IPs
2. Autogenerates and attaches verification agents for each IP 
3. USes a central sequencer to orchestrate tests across IPs, driving tests via each IP's driver.
4. Builds monitors and scoreboards after IP verification to perform checking/coverage 
5. Consumes a high-level DSL and produces per-IP test suits (JSON) via python 
Targets usability for non-technical users via a clear CLI and sensible defualts. 
