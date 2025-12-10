#include "Uart_rx.h"

void Uart_rx::rx_thread() {
    CYCLES_PER_BIT = (CLOCK_FREQ + BAUD_RATE/2) / BAUD_RATE;

    bool sampling = false;
    int cycle_counter = 0;
    int bit_counter = 0;
    sc_uint<8> shift_reg = 0;

    valid.write(false);
    data_out.write(0);

    while (true) {
        wait(); // posedge clk

        if (!rst_n.read()) {
            sampling = false;
            cycle_counter = 0;
            bit_counter = 0;
            shift_reg = 0;
            valid.write(false);
            continue;
        }

        // Simple input synchronization: shift two-stage
        static bool rx_sync_0 = true;
        static bool rx_sync_1 = true;
        rx_sync_0 = rx.read();
        rx_sync_1 = rx_sync_0;

        if (!sampling) {
            // detect start bit (line goes low)
            if (rx_sync_1 == false) {
                sampling = true;
                cycle_counter = 1;
                bit_counter = 0; // start bit
            }
        } else {
            cycle_counter += 1;
            if (cycle_counter >= (CYCLES_PER_BIT/2)) {
                // sample in middle of bit
                if (bit_counter == 0) {
                    // sample start bit; if not low, abort
                    if (rx_sync_1 != false) {
                        sampling = false; // false start
                    } else {
                        cycle_counter = 0;
                        bit_counter = 1; // move to first data bit on next milestone
                    }
                } else if (bit_counter >=1 && bit_counter <= DATA_WIDTH) {
                    // sample data bits LSB first
                    bool b = rx_sync_1;
                    shift_reg = (shift_reg >> 1) | ( (b?1:0) << (DATA_WIDTH-1) );
                    bit_counter += 1;
                    cycle_counter = 0;
                } else if (bit_counter > DATA_WIDTH && bit_counter <= DATA_WIDTH + STOP_BITS) {
                    // sample stop bit(s) - expecting high
                    // if stop bit valid, finish
                    bool b = rx_sync_1;
                    if (b == true) {
                        // produce data (note shift_reg contains MSB at position DATA_WIDTH-1)
                        // convert to LSB-first ordering
                        sc_uint<8> out = 0;
                        for (int k = 0; k < DATA_WIDTH; ++k) {
                            out[k] = shift_reg[DATA_WIDTH-1-k];
                        }
                        data_out.write(out);
                        valid.write(true);
                        // keep valid for one cycle; match original which asserts valid at stop
                        // we'll clear valid when host accepts (ready)
                        // reset sampling
                        sampling = false;
                        cycle_counter = 0;
                        bit_counter = 0;
                    } else {
                        // framing error - ignore
                        sampling = false;
                    }
                }
            }
        }

        // clear valid if host ready
        if (valid.read() && ready.read()) {
            valid.write(false);
        }
    }
}
