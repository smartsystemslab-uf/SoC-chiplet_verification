#ifndef UART_TX_H
#define UART_TX_H

#include <systemc.h>

SC_MODULE(Uart_tx) {
    // Parameters
    int DATA_WIDTH;
    int BAUD_RATE;
    int CLOCK_FREQ;
    int STOP_BITS;

    // Ports
    sc_in<bool> clk;
    sc_in<bool> rst_n;

    sc_out<bool> tx; // serial line

    sc_in< sc_uint<8> > data_in; // using 8-bit width but will use DATA_WIDTH
    sc_in<bool>         valid;
    sc_out<bool>        ready;

    // Internal
    int CYCLES_PER_BIT;

    void tx_thread();

    SC_CTOR(Uart_tx) : DATA_WIDTH(8), BAUD_RATE(9600), CLOCK_FREQ(50000000), STOP_BITS(1) {
        SC_THREAD(tx_thread);
        sensitive << clk.pos();
        dont_initialize();
    }

    // Allow parameter override after construction
    void set_params(int data_width, int baud_rate, int clock_freq, int stop_bits=1) {
        DATA_WIDTH = data_width; BAUD_RATE = baud_rate; CLOCK_FREQ = clock_freq; STOP_BITS = stop_bits;
        CYCLES_PER_BIT = (CLOCK_FREQ + BAUD_RATE/2) / BAUD_RATE;
    }

};

#endif // UART_TX_H
