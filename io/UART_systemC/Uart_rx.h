#ifndef UART_RX_H
#define UART_RX_H

#include <systemc.h>

SC_MODULE(Uart_rx) {
    int DATA_WIDTH;
    int BAUD_RATE;
    int CLOCK_FREQ;
    int STOP_BITS;

    sc_in<bool> clk;
    sc_in<bool> rst_n;

    sc_in<bool> rx; // serial input

    sc_out< sc_uint<8> > data_out; // use 8-bit but only DATA_WIDTH bits used
    sc_out<bool>         valid;
    sc_in<bool>          ready;

    int CYCLES_PER_BIT;

    void rx_thread();

    SC_CTOR(Uart_rx) : DATA_WIDTH(8), BAUD_RATE(9600), CLOCK_FREQ(50000000), STOP_BITS(1) {
        SC_THREAD(rx_thread);
        sensitive << clk.pos();
        dont_initialize();
    }

    void set_params(int data_width, int baud_rate, int clock_freq, int stop_bits=1) {
        DATA_WIDTH = data_width; BAUD_RATE = baud_rate; CLOCK_FREQ = clock_freq; STOP_BITS = stop_bits;
        CYCLES_PER_BIT = (CLOCK_FREQ + BAUD_RATE/2) / BAUD_RATE;
    }
};

#endif // UART_RX_H
