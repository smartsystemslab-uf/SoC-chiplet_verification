module Uart_core #(
    parameter DATA_WIDTH = 8,
    parameter BAUD_RATE = 115200,
    parameter CLOCK_FREQ = 50000000
)(
    input  logic                  clk,
    input  logic                  rst_n,
    // UART RX interface
    input  logic                  uart_rx,
    output logic [DATA_WIDTH-1:0] rx_data,
    output logic                  rx_valid,
    input  logic                  rx_ready,
    // UART TX interface
    output logic                  uart_tx,
    input  logic [DATA_WIDTH-1:0] tx_data,
    input  logic                  tx_valid,
    output logic                  tx_ready
);

    // RX instance
    Uart_rx #(
        .DATA_WIDTH(DATA_WIDTH),
        .BAUD_RATE(BAUD_RATE),
        .CLOCK_FREQ(CLOCK_FREQ)
    ) u_rx (
        .clk      (clk),
        .rst_n    (rst_n),
        .rx       (uart_rx),
        .data_out (rx_data),
        .valid    (rx_valid),
        .ready    (rx_ready)
    );

    // TX instance
    Uart_tx #(
        .DATA_WIDTH(DATA_WIDTH),
        .BAUD_RATE(BAUD_RATE),
        .CLOCK_FREQ(CLOCK_FREQ)
    ) u_tx (
        .clk      (clk),
        .rst_n    (rst_n),
        .tx       (uart_tx),
        .data_in  (tx_data),
        .valid    (tx_valid),
        .ready    (tx_ready)
    );

endmodule