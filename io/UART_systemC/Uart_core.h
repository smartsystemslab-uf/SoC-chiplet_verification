#ifndef UART_CORE_H
#define UART_CORE_H

#include <systemc.h>
#include "Uart_tx.h"
#include "Uart_rx.h"

SC_MODULE(Uart_core) {
    int DATA_WIDTH;
    int BAUD_RATE;
    int CLOCK_FREQ;

    sc_in<bool> clk;
    sc_in<bool> rst_n;

    // RX interface
    sc_in<bool> rx;
    sc_out< sc_uint<8> > rx_data;
    sc_out<bool> rx_valid;
    sc_in<bool> rx_ready;

    // TX interface
    sc_out<bool> tx;
    sc_in< sc_uint<8> > tx_data;
    sc_in<bool> tx_valid;
    sc_out<bool> tx_ready;

    // Instances
    Uart_tx *tx_inst;
    Uart_rx *rx_inst;

    SC_CTOR(Uart_core) : DATA_WIDTH(8), BAUD_RATE(115200), CLOCK_FREQ(50000000) {
        tx_inst = new Uart_tx("uart_tx");
        rx_inst = new Uart_rx("uart_rx");

        // connect ports
        tx_inst->clk(clk);
        tx_inst->rst_n(rst_n);
        tx_inst->tx(tx);
        tx_inst->data_in(tx_data);
        tx_inst->valid(tx_valid);
        tx_inst->ready(tx_ready);
        tx_inst->set_params(DATA_WIDTH, BAUD_RATE, CLOCK_FREQ);

        rx_inst->clk(clk);
        rx_inst->rst_n(rst_n);
        rx_inst->rx(rx);
        rx_inst->data_out(rx_data);
        rx_inst->valid(rx_valid);
        rx_inst->ready(rx_ready);
        rx_inst->set_params(DATA_WIDTH, BAUD_RATE, CLOCK_FREQ);
    }

    ~Uart_core() {
        if (tx_inst) delete tx_inst;
        if (rx_inst) delete rx_inst;
    }
};

#endif // UART_CORE_H
