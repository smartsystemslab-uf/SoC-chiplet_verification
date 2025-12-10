#include "gpio_axi_lite.h"

gpio_axi_lite::gpio_axi_lite(sc_module_name name) : sc_module(name) {
    // Initialize AXI handshake signals
    S_AXI_AWREADY.write(true);
    S_AXI_WREADY.write(true);
    S_AXI_ARREADY.write(true);
    S_AXI_BRESP.write(0);  // OKAY response
    S_AXI_RRESP.write(0);  // OKAY response
    S_AXI_BVALID.write(false);
    S_AXI_RVALID.write(false);

    // Initialize GPIO registers
    reg_data.write(0);
    reg_dir.write(0);
    gpio_out.write(0);
    gpio_oe.write(0);

    // Register processes with clock
    SC_METHOD(write_process);
    sensitive << S_AXI_ACLK.pos();

    SC_METHOD(read_process);
    sensitive << S_AXI_ACLK.pos();

    SC_METHOD(output_process);
    sensitive << reg_data << reg_dir;
}

gpio_axi_lite::~gpio_axi_lite() {
    // Cleanup if needed
}

void gpio_axi_lite::write_process() {
    // Reset check
    if (!S_AXI_ARESETN.read()) {
        reg_data.write(0);
        reg_dir.write(0);
        S_AXI_BVALID.write(false);
        return;
    }

    // Write transaction: AWVALID & WVALID & AWREADY & WREADY
    bool aw_valid = S_AXI_AWVALID.read();
    bool w_valid  = S_AXI_WVALID.read();
    bool aw_ready = S_AXI_AWREADY.read();
    bool w_ready  = S_AXI_WREADY.read();

    if (aw_valid && w_valid && aw_ready && w_ready) {
        sc_uint<C_S_AXI_ADDR_WIDTH> addr = S_AXI_AWADDR.read();
        sc_uint<C_S_AXI_DATA_WIDTH> data = S_AXI_WDATA.read();
        sc_uint<C_S_AXI_DATA_WIDTH/8> strb = S_AXI_WSTRB.read();

        if (addr == 0x0) {
            // Write to data register (gpio_out)
            sc_uint<GPIO_WIDTH> current_data = reg_data.read();
            for (int i = 0; i < C_S_AXI_DATA_WIDTH / 8; i++) {
                if (strb[i]) {
                    // Replace 8 bits at position i*8
                    for (int j = 0; j < 8; j++) {
                        if (i * 8 + j < GPIO_WIDTH) {
                            current_data[i * 8 + j] = data[i * 8 + j];
                        }
                    }
                }
            }
            reg_data.write(current_data);
        } else if (addr == 0x4) {
            // Write to direction register
            sc_uint<GPIO_WIDTH> current_dir = reg_dir.read();
            for (int i = 0; i < C_S_AXI_DATA_WIDTH / 8; i++) {
                if (strb[i]) {
                    // Replace 8 bits at position i*8
                    for (int j = 0; j < 8; j++) {
                        if (i * 8 + j < GPIO_WIDTH) {
                            current_dir[i * 8 + j] = data[i * 8 + j];
                        }
                    }
                }
            }
            reg_dir.write(current_dir);
        }

        // Assert write response valid
        S_AXI_BVALID.write(true);
    } else if (S_AXI_BREADY.read()) {
        // Clear response when master is ready
        S_AXI_BVALID.write(false);
    }
}

void gpio_axi_lite::read_process() {
    // Reset check
    if (!S_AXI_ARESETN.read()) {
        S_AXI_RVALID.write(false);
        S_AXI_RDATA.write(0);
        return;
    }

    // Read transaction: ARVALID & ARREADY
    bool ar_valid = S_AXI_ARVALID.read();
    bool ar_ready = S_AXI_ARREADY.read();

    if (ar_valid && ar_ready) {
        sc_uint<C_S_AXI_ADDR_WIDTH> addr = S_AXI_ARADDR.read();
        sc_uint<C_S_AXI_DATA_WIDTH> rdata;

        if (addr == 0x0) {
            // Read from data register (returns gpio_in on read)
            sc_uint<GPIO_WIDTH> gpio_in_val = gpio_in.read();
            rdata = gpio_in_val;
        } else if (addr == 0x4) {
            // Read from direction register
            sc_uint<GPIO_WIDTH> dir = reg_dir.read();
            rdata = dir;
        } else {
            rdata = 0;
        }

        S_AXI_RDATA.write(rdata);
        S_AXI_RVALID.write(true);
    } else if (S_AXI_RREADY.read()) {
        // Clear response when master is ready
        S_AXI_RVALID.write(false);
    }
}

void gpio_axi_lite::output_process() {
    // Combinational logic: drive gpio_out and gpio_oe based on registers
    sc_uint<GPIO_WIDTH> data = reg_data.read();
    sc_uint<GPIO_WIDTH> dir  = reg_dir.read();

    gpio_out.write(data);
    gpio_oe.write(dir);
}
