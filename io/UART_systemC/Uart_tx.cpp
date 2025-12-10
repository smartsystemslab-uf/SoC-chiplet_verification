#include "Uart_tx.h"

void Uart_tx::tx_thread() {
    // Compute cycles per bit if not set
    CYCLES_PER_BIT = (CLOCK_FREQ + BAUD_RATE/2) / BAUD_RATE;

    // internal state
    sc_uint<8> shift_reg = 0;
    bool idle = true;

    // counters
    int cycle_counter = 0;
    int bit_counter = 0;

    // default line high
    tx.write(true);
    ready.write(true);

    while (true) {
        wait(); // wait for posedge clk

        if (!rst_n.read()) {
            tx.write(true);
            ready.write(true);
            idle = true;
            cycle_counter = 0;
            bit_counter = 0;
            continue;
        }

        if (idle) {
            ready.write(true);
            if (valid.read()) {
                // latch data and start sending
                shift_reg = data_in.read();
                idle = false;
                ready.write(false);
                // start bit
                tx.write(false);
                cycle_counter = 1;
                bit_counter = 0; // start bit is bit_counter==0
            }
        } else {
            // transmitting
            cycle_counter += 1;
            if (cycle_counter >= CYCLES_PER_BIT) {
                cycle_counter = 0;
                if (bit_counter == 0) {
                    // we finished start bit, move to data bits
                    // send LSB first
                    tx.write( (shift_reg & 0x1) ? true : false );
                    shift_reg = shift_reg >> 1;
                    bit_counter = 1; // data bits started
                } else if (bit_counter >=1 && bit_counter <= DATA_WIDTH) {
                    if (bit_counter < DATA_WIDTH) {
                        tx.write( (shift_reg & 0x1) ? true : false );
                        shift_reg = shift_reg >> 1;
                        bit_counter += 1;
                    } else {
                        // completed last data bit, move to stop bits
                        tx.write(true);
                        bit_counter = DATA_WIDTH + 1; // first stop bit
                    }
                } else {
                    // in stop bits
                    if (bit_counter < DATA_WIDTH + STOP_BITS) {
                        // keep tx=1 for stop bits
                        bit_counter += 1;
                    } else {
                        // finished frame
                        idle = true;
                        ready.write(true);
                        tx.write(true);
                        cycle_counter = 0;
                        bit_counter = 0;
                    }
                }
            }
        }
    }
}
