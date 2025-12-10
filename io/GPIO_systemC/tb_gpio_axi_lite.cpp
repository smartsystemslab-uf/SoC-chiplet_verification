#include <systemc.h>
#include "gpio_axi_lite.h"

SC_MODULE(tb_gpio) {
    // Clock and reset
    sc_clock                        clk;
    sc_signal<bool>                 reset_n;

    // AXI write channels
    sc_signal<sc_uint<4>>           awaddr;
    sc_signal<bool>                 awvalid;
    sc_signal<bool>                 awready;
    sc_signal<sc_uint<32>>          wdata;
    sc_signal<sc_uint<4>>           wstrb;
    sc_signal<bool>                 wvalid;
    sc_signal<bool>                 wready;
    sc_signal<sc_uint<2>>           bresp;
    sc_signal<bool>                 bvalid;
    sc_signal<bool>                 bready;

    // AXI read channels
    sc_signal<sc_uint<4>>           araddr;
    sc_signal<bool>                 arvalid;
    sc_signal<bool>                 arready;
    sc_signal<sc_uint<32>>          rdata;
    sc_signal<sc_uint<2>>           rresp;
    sc_signal<bool>                 rvalid;
    sc_signal<bool>                 rready;

    // GPIO signals
    sc_signal<sc_uint<8>>           gpio_in;
    sc_signal<sc_uint<8>>           gpio_out;
    sc_signal<sc_uint<8>>           gpio_oe;

    // DUT instance
    gpio_axi_lite *dut;

    void testbench_process() {
        // Initialize signals
        reset_n.write(0);
        awvalid.write(0);
        wvalid.write(0);
        bready.write(1);
        arvalid.write(0);
        rready.write(1);
        gpio_in.write(0);

        // Release reset after a few cycles
        wait(10, SC_NS);
        reset_n.write(1);
        wait(10, SC_NS);

        cout << "@" << sc_time_stamp() << ": Starting GPIO testbench" << endl;

        // Test 1: Write data register
        cout << "@" << sc_time_stamp() << ": Test 1 - Write data register (0xA5)" << endl;
        awaddr.write(0x0);
        awvalid.write(1);
        wdata.write(0xA5);
        wstrb.write(0xF);  // All bytes valid
        wvalid.write(1);
        wait();  // Wait for one clock
        awvalid.write(0);
        wvalid.write(0);
        wait(10, SC_NS);

        // Test 2: Write direction register (all outputs)
        cout << "@" << sc_time_stamp() << ": Test 2 - Write direction register (0xFF)" << endl;
        awaddr.write(0x4);
        awvalid.write(1);
        wdata.write(0xFF);
        wstrb.write(0xF);
        wvalid.write(1);
        wait();
        awvalid.write(0);
        wvalid.write(0);
        wait(10, SC_NS);

        // Test 3: Read data register
        cout << "@" << sc_time_stamp() << ": Test 3 - Read data register" << endl;
        araddr.write(0x0);
        arvalid.write(1);
        wait();
        arvalid.write(0);
        wait(10, SC_NS);

        // Check read data
        cout << "@" << sc_time_stamp() << ": Read data = 0x" << hex << rdata.read() << dec << endl;

        // Test 4: Read direction register
        cout << "@" << sc_time_stamp() << ": Test 4 - Read direction register" << endl;
        araddr.write(0x4);
        arvalid.write(1);
        wait();
        arvalid.write(0);
        wait(10, SC_NS);

        cout << "@" << sc_time_stamp() << ": Read direction = 0x" << hex << rdata.read() << dec << endl;

        // Test 5: Toggle GPIO input and read
        cout << "@" << sc_time_stamp() << ": Test 5 - Set GPIO input to 0x55 and read" << endl;
        gpio_in.write(0x55);
        wait(10, SC_NS);

        araddr.write(0x0);
        arvalid.write(1);
        wait();
        arvalid.write(0);
        wait(10, SC_NS);

        cout << "@" << sc_time_stamp() << ": GPIO input read as 0x" << hex << rdata.read() << dec << endl;

        // Test complete
        wait(50, SC_NS);
        cout << "@" << sc_time_stamp() << ": GPIO testbench complete" << endl;
        sc_stop();
    }

    SC_CTOR(tb_gpio) : clk("clk", 20, SC_NS) {
        dut = new gpio_axi_lite("gpio_dut");

        // Connect DUT ports
        dut->S_AXI_ACLK(clk);
        dut->S_AXI_ARESETN(reset_n);
        dut->S_AXI_AWADDR(awaddr);
        dut->S_AXI_AWVALID(awvalid);
        dut->S_AXI_AWREADY(awready);
        dut->S_AXI_WDATA(wdata);
        dut->S_AXI_WSTRB(wstrb);
        dut->S_AXI_WVALID(wvalid);
        dut->S_AXI_WREADY(wready);
        dut->S_AXI_BRESP(bresp);
        dut->S_AXI_BVALID(bvalid);
        dut->S_AXI_BREADY(bready);
        dut->S_AXI_ARADDR(araddr);
        dut->S_AXI_ARVALID(arvalid);
        dut->S_AXI_ARREADY(arready);
        dut->S_AXI_RDATA(rdata);
        dut->S_AXI_RRESP(rresp);
        dut->S_AXI_RVALID(rvalid);
        dut->S_AXI_RREADY(rready);
        dut->gpio_in(gpio_in);
        dut->gpio_out(gpio_out);
        dut->gpio_oe(gpio_oe);

        SC_THREAD(testbench_process);
        sensitive << clk.posedge_event();
    }

    ~tb_gpio() {
        if (dut) delete dut;
    }
};

int sc_main(int argc, char* argv[]) {
    tb_gpio tb("tb_gpio");

    sc_start();

    return 0;
}
