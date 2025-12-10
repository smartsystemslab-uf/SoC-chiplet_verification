#include <systemc.h>
#include "Uart_core.h"

int sc_main(int argc, char* argv[]) {
    sc_clock clk("clk", 20, SC_NS); // 50 MHz
    sc_signal<bool> rst_n;

    // signals
    sc_signal<bool> uart_tx;
    sc_signal<bool> uart_rx;
    sc_signal< sc_uint<8> > tx_data;
    sc_signal<bool> tx_valid;
    sc_signal<bool> tx_ready;

    sc_signal< sc_uint<8> > rx_data;
    sc_signal<bool> rx_valid;
    sc_signal<bool> rx_ready;

    // instantiate core
    Uart_core core("uart_core");
    core.clk(clk);
    core.rst_n(rst_n);
    core.rx(uart_rx);
    core.rx_data(rx_data);
    core.rx_valid(rx_valid);
    core.rx_ready(rx_ready);
    core.tx(uart_tx);
    core.tx_data(tx_data);
    core.tx_valid(tx_valid);
    core.tx_ready(tx_ready);

    // loopback: connect tx -> rx
    // use small delay via an SC_THREAD
    sc_spawn([&]{
        while (true) {
            wait(clk.posedge_event());
            uart_rx.write(uart_tx.read());
        }
    });

    // test sequence thread
    sc_spawn([&]{
        // reset
        rst_n.write(false);
        tx_valid.write(false);
        rx_ready.write(false);
        wait(100, SC_NS);
        rst_n.write(true);
        wait(100, SC_NS);

        std::string s = "HELLO";
        for (size_t i = 0; i < s.size(); ++i) {
            // wait until tx_ready
            while (!tx_ready.read()) wait(clk.posedge_event());
            tx_data.write((uint8_t)s[i]);
            tx_valid.write(true);
            wait(clk.posedge_event());
            tx_valid.write(false);
            // wait some time for frame to be sent
            wait(5000, SC_NS);
        }

        // start receiving
        rx_ready.write(true);
        int received = 0;
        sc_time timeout = sc_time(5, SC_MS);
        sc_time start = sc_time_stamp();
        while (received < 5 && sc_time_stamp() - start < timeout) {
            if (rx_valid.read() && rx_ready.read()) {
                std::cout << "TB: Received byte: " << (char)rx_data.read() << " (0x" << std::hex << rx_data.read() << std::dec << ") at " << sc_time_stamp() << std::endl;
                received++;
            }
            wait(clk.posedge_event());
        }

        std::cout << "TB: Done" << std::endl;
        sc_stop();
    });

    sc_start();
    return 0;
}
