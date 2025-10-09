module pmu_tb;
    // Clock and Reset
    logic        clk;
    logic        rst_n;
    
    // APB Interface
    logic [7:0]  paddr;
    logic        pwrite;
    logic        psel;
    logic        penable;
    logic [31:0] pwdata;
    logic [31:0] prdata;
    logic        pready;
    
    // Monitoring Inputs
    logic [11:0] mem_voltage;
    logic [11:0] mem_current;
    logic [11:0] io_voltage;
    logic [11:0] io_current;
    logic [7:0]  temp_memory;
    logic [7:0]  temp_ambient;
    logic [1:0]  clock_status;
    logic [1:0]  power_status;
    logic [2:0]  seq_state;
    
    // Outputs
    logic        temp_alert;
    logic        power_alert;
    logic [1:0]  clock_enable;
    
    // Instantiate DUT
    power_monitoring_unit dut (.*);
    
    // Clock generation
    initial clk = 0;
    always #5ns clk = ~clk;  // 100MHz
    
    // APB Write Task
    task automatic apb_write(input logic [7:0] addr, input logic [31:0] data);
        @(posedge clk);
        paddr = addr;
        pwdata = data;
        pwrite = 1'b1;
        psel = 1'b1;
        penable = 1'b0;
        @(posedge clk);
        penable = 1'b1;
        @(posedge clk);
        psel = 1'b0;
        penable = 1'b0;
        $display("[%0t] APB Write: Addr=0x%02X Data=0x%08X", $time, addr, data);
    endtask
    
    // APB Read Task
    task automatic apb_read(input logic [7:0] addr, output logic [31:0] data);
        @(posedge clk);
        paddr = addr;
        pwrite = 1'b0;
        psel = 1'b1;
        penable = 1'b0;
        @(posedge clk);
        penable = 1'b1;
        @(posedge clk);
        data = prdata;
        $display("[%0t] APB Read: Addr=0x%02X Data=0x%08X", $time, addr, data);
        psel = 1'b0;
        penable = 1'b0;
    endtask
    
    // Test stimulus
    initial begin
        logic [31:0] read_data;
        
        // Initialize
        rst_n = 0;
        paddr = '0;
        pwrite = 0;
        psel = 0;
        penable = 0;
        pwdata = '0;
        
        mem_voltage = 12'd1200;  // 1.2V
        mem_current = 12'd500;   // 0.5A
        io_voltage = 12'd1800;   // 1.8V
        io_current = 12'd200;    // 0.2A
        temp_memory = 8'd45;     // 45°C
        temp_ambient = 8'd30;    // 30°C
        clock_status = 2'b11;    // Both clocks active
        power_status = 2'b11;    // Both powered
        seq_state = 3'b011;      // Running state
        
        #100ns;
        rst_n = 1;
        #50ns;
        
        // Wait for clock_enable to stabilize after reset
        #100ns;
        
        $display("\n========================================");
        $display("     PMU 2-Channel Testbench");
        $display("     Channels: Memory + I/O");
        $display("========================================\n");
        
        $display("After Reset: clock_enable = 0b%02b (should be 0b11)", clock_enable);
        #50ns;
        
        $display("\n========================================");
        $display("     PMU 2-Channel Testbench");
        $display("========================================\n");
        
        // Test 1: Read initial status
        $display("Test 1: Read Status Registers");
        apb_read(8'h00, read_data);  // CONTROL_REG
        apb_read(8'h04, read_data);  // STATUS_REG
        apb_read(8'h08, read_data);  // THRESHOLD_REG
        apb_read(8'h44, read_data);  // CLOCK_CONTROL_REG
        
        // Test 2: Read Memory channel
        $display("\nTest 2: Read Memory Channel Data");
        apb_read(8'h10, read_data);  // MEM voltage
        apb_read(8'h14, read_data);  // MEM current
        apb_read(8'h18, read_data);  // MEM power
        
        // Test 3: Read I/O channel
        $display("\nTest 3: Read I/O Channel Data");
        apb_read(8'h20, read_data);  // I/O voltage
        apb_read(8'h24, read_data);  // I/O current
        apb_read(8'h28, read_data);  // I/O power
        
        // Test 4: Read temperature
        $display("\nTest 4: Temperature Monitoring");
        apb_read(8'h30, read_data);  // Memory temp
        apb_read(8'h34, read_data);  // Ambient temp
        
        // Test 5: Trigger temperature alert
        $display("\nTest 5: Temperature Alert Test");
        #100ns;
        temp_memory = 8'd90;  // Exceed threshold
        #200ns;
        apb_read(8'h04, read_data);  // Check status
        $display("Clock Enable: 0b%02b (auto-gate not enabled yet)", clock_enable);
        
        // Test 6: Clock Control
        $display("\nTest 6: Clock Control");
        #100ns;
        $display("Initial clock_enable: 0b%02b", clock_enable);
        
        // Enable automatic clock gating
        apb_write(8'h44, 32'h00000007);  // Auto-gate ON, both clocks enabled
        #100ns;
        $display("After enabling auto-gate: 0b%02b (with temp_alert, should be 0b01)", clock_enable);
        
        // Clear temperature alert
        temp_memory = 8'd50;
        #100ns;
        $display("After clearing temp alert: 0b%02b (should be 0b11)", clock_enable);
        
        // Manually disable I/O clock
        apb_write(8'h44, 32'h00000001);  // Manual: MEM only
        #100ns;
        $display("After manual control (MEM only): 0b%02b (should be 0b01)", clock_enable);
        
        // Re-enable both
        apb_write(8'h44, 32'h00000003);  // Both enabled
        #100ns;
        $display("After re-enabling both: 0b%02b (should be 0b11)", clock_enable);
        
        // Test 7: Power Alert
        $display("\nTest 7: Auto Power Alert Gating");
        apb_write(8'h08, 32'h4B_100_A00);  // Lower power threshold
        apb_read(8'h08, read_data);
        
        apb_write(8'h44, 32'h00000007);  // Re-enable auto-gate
        #100ns;
        
        // Trigger power alert
        mem_current = 12'd4000;  // High current
        #300ns;
        
        apb_read(8'h18, read_data);  // Read MEM power
        $display("Memory Power: 0x%08X (decimal: %0d)", read_data, read_data);
        apb_read(8'h04, read_data);  // Read status
        $display("Status: 0x%08X", read_data);
        $display("After power alert: 0b%02b (should gate I/O → 0b01)", clock_enable);
        
        #1000ns;
        
        $display("\n========================================");
        $display("     Test Completed Successfully");
        $display("     2-Channel PMU (Memory + I/O)");
        $display("========================================\n");
        
        $finish;
    end
    
    // Waveform dumping
    initial begin
        $dumpfile("pmu_2ch.vcd");
        $dumpvars(0, pmu_tb);
    end
    
    // Monitor clock_enable changes
    always @(clock_enable) begin
        $display("[%0t] CLOCK_ENABLE Changed: 0b%02b", $time, clock_enable);
    end
    
endmodule