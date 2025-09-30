# UART Wrapper and Files README

This directory contains a synthesizable **Universal Asynchronous Receiver/Transmitter (UART)** core,
written in SystemVerilog and designed to be easily integrated into larger FPGA projects.

## Overview
The UART core provides **full-duplex serial communication** with configurable parameters such as
clock frequency and baud rate.  
It is composed of three modules:

| Module        | Description                                                        |
|---------------|--------------------------------------------------------------------|
| `uart_rx.sv`  | Receives serial data from the `uart_rx` pin and outputs 8-bit bytes |
| `uart_tx.sv`  | Transmits 8-bit bytes on the `uart_tx` pin                          |
| `uart_core.sv`| Wrapper that instantiates `uart_rx` and `uart_tx` for a single, clean peripheral interface |

This design is based on a simple Verilog UART (originally by Ben Marshall) and has been updated to **SystemVerilog**.

---

## Features
- **Full-duplex** communication (independent transmit and receive paths)
- Parameterizable:
  - `CLK_FREQ` (default 50 MHz)
  - `BAUD_RATE` (default 115200)
- Clean SystemVerilog interfaces using `logic` and `always_ff/always_comb`
- Synthesis-friendly for FPGAs
- Tested in simulation with ModelSim/Questa

---

## File List
- uart_rx.sv // Receiver module
- uart_tx.sv // Transmitter module
- uart_core.sv // Wrapper module (combines RX and TX)
- tb_uart.sv // Self-checking testbench

---

## References
- UART rx and tx files are derived from Ben Marshall’s original UART implementation.
  - https://github.com/ben-marshall/uart/tree/master
