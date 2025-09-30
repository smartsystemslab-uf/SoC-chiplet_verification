// Simple testbench for Uart_wrapper
// Loopback TX->RX, demonstrates valid/ready handshake and prints received bytes

module tb_uart;
    parameter DATA_WIDTH = 8;
    parameter BAUD_RATE  = 115200;
    parameter CLOCK_FREQ = 50_000_000;

    logic clk = 0;
    always #10 clk = ~clk; // 50 MHz -> period 20 ns

    logic rst_n = 0;

    // helper variables for the testbench
    string s;
    int i;

    // wrapper I/O
    logic uart_rx;
    logic [DATA_WIDTH-1:0] rx_data;
    logic rx_valid;
    logic rx_ready;

    logic uart_tx;
    logic [DATA_WIDTH-1:0] tx_data;
    logic tx_valid;
    logic tx_ready;

    // instantiate wrapper
    Uart_core #(.DATA_WIDTH(DATA_WIDTH), .BAUD_RATE(BAUD_RATE), .CLOCK_FREQ(CLOCK_FREQ))
        dut(
            .clk(clk), .rst_n(rst_n),
            .uart_rx(uart_rx), .rx_data(rx_data), .rx_valid(rx_valid), .rx_ready(rx_ready),
            .uart_tx(uart_tx), .tx_data(tx_data), .tx_valid(tx_valid), .tx_ready(tx_ready)
        );

    // simple loopback: connect tx to rx
    // use always_comb to drive the logic net from the tx signal
    always_comb begin
        uart_rx = uart_tx;
    end

    // --- Testbench helper tasks ------------------------------------------------
    // Send a byte using the host-side interface (valid/ready handshake)
    task automatic send_byte(input logic [7:0] b);
    begin
        // wait until transmitter indicates it can accept data
        wait (tx_ready == 1);
        @(posedge clk);
        tx_data  = b;
        tx_valid = 1;
        @(posedge clk);
        tx_valid = 0;
    end
    endtask

    // Wait for rx_valid and check the received byte matches expected. Fatal on timeout or mismatch.
    task automatic recv_and_check(input logic [7:0] expected, input int timeout_cycles = 2000000);
    begin : wait_loop
        for (int t = 0; t < timeout_cycles; t++) begin
            @(posedge clk);
            if (rx_valid) begin
                // ensure we accept the data
                if (!rx_ready) rx_ready = 1;
                @(posedge clk);
                if (rx_data !== expected) begin
                    $fatal(1, "TB ERROR: received mismatch: expected 0x%0h got 0x%0h", expected, rx_data);
                end else begin
                    $display("TB: verified received byte: %c (0x%0h)", expected, expected);
                end
                disable wait_loop;
            end
        end
        $fatal(1, "TB ERROR: timeout waiting for rx_valid for expected 0x%0h", expected);
    end
    endtask

    initial begin
        $display("TB: start");
        // reset
        rst_n = 0;
        tx_valid = 0;
        rx_ready = 0;
        #100;
        rst_n = 1;

        // small delay for stability
        #1000;

    // send bytes: "HELLO"
    s = "HELLO";
    for (i = 0; i < s.len(); i++) begin
            tx_data = s[i];
            // wait until tx_ready is asserted
            wait (tx_ready == 1);
            @(posedge clk);
            tx_valid = 1;
            @(posedge clk);
            tx_valid = 0;
            // give transmitter time to start
            #10000; // wait enough for a couple bits (coarse)
        end

        // start consuming RX data
        #1000;
        rx_ready = 1; // allow host to accept bytes

        // wait for a short while to receive data
        #2000000;

        $display("TB: done — simulation paused (use 'run' to continue or 'quit' to exit)");
        // Use $stop to pause the simulator so you can inspect waves/variables.
        $stop;
    end

    // monitor rx_valid and print received bytes
    always @(posedge clk) begin
        if (rx_valid && rx_ready) begin
            $display("TB: Received byte: %c (0x%0h)", rx_data, rx_data);
        end
    end

endmodule
