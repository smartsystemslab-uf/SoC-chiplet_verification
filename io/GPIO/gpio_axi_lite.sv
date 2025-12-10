module gpio_axi_lite #(
    parameter integer C_S_AXI_DATA_WIDTH = 32,
    parameter integer C_S_AXI_ADDR_WIDTH = 4,
    parameter integer GPIO_WIDTH         = 8
)(
    // AXI4-Lite signals
    input  logic                          S_AXI_ACLK,
    input  logic                          S_AXI_ARESETN,
    input  logic [C_S_AXI_ADDR_WIDTH-1:0]  S_AXI_AWADDR,
    input  logic                           S_AXI_AWVALID,
    output logic                           S_AXI_AWREADY,
    input  logic [C_S_AXI_DATA_WIDTH-1:0]  S_AXI_WDATA,
    input  logic [C_S_AXI_DATA_WIDTH/8-1:0]S_AXI_WSTRB,
    input  logic                           S_AXI_WVALID,
    output logic                           S_AXI_WREADY,
    output logic [1:0]                     S_AXI_BRESP,
    output logic                           S_AXI_BVALID,
    input  logic                           S_AXI_BREADY,
    input  logic [C_S_AXI_ADDR_WIDTH-1:0]  S_AXI_ARADDR,
    input  logic                           S_AXI_ARVALID,
    output logic                           S_AXI_ARREADY,
    output logic [C_S_AXI_DATA_WIDTH-1:0]  S_AXI_RDATA,
    output logic [1:0]                     S_AXI_RRESP,
    output logic                           S_AXI_RVALID,
    input  logic                           S_AXI_RREADY,

    // GPIO pins
    input  logic [GPIO_WIDTH-1:0]          gpio_in,
    output logic [GPIO_WIDTH-1:0]          gpio_out,
    output logic [GPIO_WIDTH-1:0]          gpio_oe   // 1=drive output
);
   
logic [GPIO_WIDTH-1:0] reg_data;  // Output values
logic [GPIO_WIDTH-1:0] reg_dir;   // Direction bits

//Write Logic
if (AWVALID && WVALID && AWREADY && WREADY) begin
    if (AWADDR == 4'h0) begin
        // Data register
        for (int i = 0; i < C_S_AXI_DATA_WIDTH/8; i++) begin
            if (WSTRB[i]) begin
                reg_data[i*8 +: 8] <= WDATA[i*8 +: 8];
            end
        end
    end else if (AWADDR == 4'h4) begin
        // Direction register
        for (int i = 0; i < C_S_AXI_DATA_WIDTH/8; i++) begin
            if (WSTRB[i]) begin
                reg_dir[i*8 +: 8] <= WDATA[i*8 +: 8];
            end
        end
    end
end

//Read Logic
if (ARVALID && ARREADY) begin
    if (ARADDR == 4'h0) begin
        RDATA <= { {(C_S_AXI_DATA_WIDTH-GPIO_WIDTH){1'b0}}, gpio_in };
    end else if (ARADDR == 4'h4) begin
        RDATA <= { {(C_S_AXI_DATA_WIDTH-GPIO_WIDTH){1'b0}}, reg_dir };
    end else begin
        RDATA <= {C_S_AXI_DATA_WIDTH{1'b0}};
    end
end