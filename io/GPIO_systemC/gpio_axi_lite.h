#ifndef GPIO_AXI_LITE_H
#define GPIO_AXI_LITE_H

#include <systemc.h>

SC_MODULE(gpio_axi_lite) {
public:
    // Parameters
    static const int C_S_AXI_DATA_WIDTH = 32;
    static const int C_S_AXI_ADDR_WIDTH = 4;
    static const int GPIO_WIDTH = 8;

    // AXI4-Lite slave interface ports
    sc_in<bool>                              S_AXI_ACLK;
    sc_in<bool>                              S_AXI_ARESETN;
    
    // Write address channel
    sc_in<sc_uint<C_S_AXI_ADDR_WIDTH>>     S_AXI_AWADDR;
    sc_in<bool>                              S_AXI_AWVALID;
    sc_out<bool>                             S_AXI_AWREADY;
    
    // Write data channel
    sc_in<sc_uint<C_S_AXI_DATA_WIDTH>>     S_AXI_WDATA;
    sc_in<sc_uint<C_S_AXI_DATA_WIDTH/8>>   S_AXI_WSTRB;
    sc_in<bool>                              S_AXI_WVALID;
    sc_out<bool>                             S_AXI_WREADY;
    
    // Write response channel
    sc_out<sc_uint<2>>                      S_AXI_BRESP;
    sc_out<bool>                             S_AXI_BVALID;
    sc_in<bool>                              S_AXI_BREADY;
    
    // Read address channel
    sc_in<sc_uint<C_S_AXI_ADDR_WIDTH>>     S_AXI_ARADDR;
    sc_in<bool>                              S_AXI_ARVALID;
    sc_out<bool>                             S_AXI_ARREADY;
    
    // Read data channel
    sc_out<sc_uint<C_S_AXI_DATA_WIDTH>>    S_AXI_RDATA;
    sc_out<sc_uint<2>>                      S_AXI_RRESP;
    sc_out<bool>                             S_AXI_RVALID;
    sc_in<bool>                              S_AXI_RREADY;

    // GPIO pins
    sc_in<sc_uint<GPIO_WIDTH>>              gpio_in;
    sc_out<sc_uint<GPIO_WIDTH>>             gpio_out;
    sc_out<sc_uint<GPIO_WIDTH>>             gpio_oe;  // 1=drive output

    // Internal register state
    sc_signal<sc_uint<GPIO_WIDTH>>          reg_data;  // Output values
    sc_signal<sc_uint<GPIO_WIDTH>>          reg_dir;   // Direction bits

    // Process declarations
    void write_process();
    void read_process();
    void output_process();

    SC_CTOR(gpio_axi_lite);
    ~gpio_axi_lite();
};

#endif // GPIO_AXI_LITE_H
