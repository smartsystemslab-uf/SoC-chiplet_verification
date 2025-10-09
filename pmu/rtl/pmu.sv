module power_monitoring_unit (

    input  logic        clk,
    input  logic        rst_n,
    
    
    // APB Interface
   
    input  logic [7:0]  paddr,
    input  logic        pwrite,
    input  logic        psel,
    input  logic        penable,
    input  logic [31:0] pwdata,
    output logic [31:0] prdata,
    output logic        pready,
    
  
    // Channel 0: Memory Monitoring
  
    input  logic [11:0] mem_voltage,
    input  logic [11:0] mem_current,
    // Channel 1: I/O Monitoring
    
    input  logic [11:0] io_voltage,
    input  logic [11:0] io_current,
    
    // Temperature Monitoring
  input  logic [7:0]  temp_memory,   // Memory temperature
    input  logic [7:0]  temp_ambient,  // Ambient temperature
     // System Status Inputs
   
    input  logic [1:0]  clock_status,  // Clock active: [1]=I/O, [0]=MEM
    input  logic [1:0]  power_status,  // Power on: [1]=I/O, [0]=MEM
    input  logic [2:0]  seq_state,     // Sequencer state
    
    
    // Clock Control Outputs
    output logic [1:0]  clock_enable,  // Clock enable: [1]=I/O, [0]=MEM
    
    
    // Alert Outputs
    output logic        temp_alert,    // Temperature too high
    output logic        power_alert    // Power threshold exceeded
);


// Register Address Map

typedef enum logic [7:0] {
    // Control & Status
    CONTROL_REG       = 8'h00,
    STATUS_REG        = 8'h04,
    THRESHOLD_REG     = 8'h08,
    
    // Memory Channel (Channel 0)
    MEM_VOLTAGE       = 8'h10,
    MEM_CURRENT       = 8'h14,
    MEM_POWER         = 8'h18,
    
    // I/O Channel (Channel 1)
    IO_VOLTAGE        = 8'h20,
    IO_CURRENT        = 8'h24,
    IO_POWER          = 8'h28,
    
    // Temperature & System
    TEMP_MEMORY_REG   = 8'h30,
    TEMP_AMBIENT_REG  = 8'h34,
    CLOCK_STATUS_REG  = 8'h38,
    POWER_STATUS_REG  = 8'h3C,
    SEQ_STATUS_REG    = 8'h40,
    CLOCK_CONTROL_REG = 8'h44
} reg_addr_t;

// Register Structures


// Control Register Structure
typedef struct packed {
    logic [29:0] reserved;
    logic        enable_io;
    logic        enable_mem;
    logic        enable_monitoring;
} control_reg_t;

// Threshold Register Structure
typedef struct packed {
    logic [7:0]  temp_threshold;
    logic [11:0] power_threshold;
    logic [11:0] voltage_threshold;
} threshold_reg_t;

// Clock Control Register Structure
typedef struct packed {
    logic [29:0] reserved;
    logic        auto_gate_enable;  // Automatic clock gating on alert
    logic        io_clk_enable;     // I/O clock enable
    logic        mem_clk_enable;    // Memory clock enable
} clock_control_reg_t;


// Internal Registers


// Configuration Registers
control_reg_t       control_reg;
threshold_reg_t     threshold_reg;
clock_control_reg_t clock_control_reg;

// Channel Data Registers
logic [11:0] mem_voltage_reg, io_voltage_reg;
logic [11:0] mem_current_reg, io_current_reg;
logic [23:0] mem_power_reg, io_power_reg;

// Temperature Registers
logic [7:0] temp_memory_reg, temp_ambient_reg;

// System Status Registers
logic [1:0] clock_status_reg, power_status_reg;
logic [2:0] seq_state_reg;



assign pready = 1'b1;  // Always ready

// APB Write Logic
always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        control_reg   <= '{enable_monitoring: 1'b1, 
                          enable_mem: 1'b1,
                          enable_io: 1'b1,
                          default: '0};
        threshold_reg <= '{temp_threshold: 8'd80,      // 80°C
                          power_threshold: 12'd2048,   // 2048W
                          voltage_threshold: 12'd3072, // 3.072V
                          default: '0};
        clock_control_reg <= '{mem_clk_enable: 1'b1,   // All clocks enabled by default
                              io_clk_enable: 1'b1,
                              auto_gate_enable: 1'b0,  // Auto-gating disabled
                              default: '0};
    end else if (psel && penable && pwrite) begin
        unique case (paddr)
            CONTROL_REG:       control_reg       <= pwdata;
            THRESHOLD_REG:     threshold_reg     <= pwdata;
            CLOCK_CONTROL_REG: clock_control_reg <= pwdata;
            default: ;
        endcase
    end
end

// APB Read Logic
always_comb begin
    prdata = '0;
    if (psel && !pwrite) begin
        unique case (paddr)
            // Control & Status
            CONTROL_REG:       prdata = control_reg;
            STATUS_REG:        prdata = {28'h0, power_alert, temp_alert, 
                                        1'b0, control_reg.enable_monitoring};
            THRESHOLD_REG:     prdata = threshold_reg;
            CLOCK_CONTROL_REG: prdata = clock_control_reg;
            
            // Memory Channel
            MEM_VOLTAGE:       prdata = {20'h0, mem_voltage_reg};
            MEM_CURRENT:       prdata = {20'h0, mem_current_reg};
            MEM_POWER:         prdata = {8'h0, mem_power_reg};
            
            // I/O Channel
            IO_VOLTAGE:        prdata = {20'h0, io_voltage_reg};
            IO_CURRENT:        prdata = {20'h0, io_current_reg};
            IO_POWER:          prdata = {8'h0, io_power_reg};
            
            // Temperature
            TEMP_MEMORY_REG:   prdata = {24'h0, temp_memory_reg};
            TEMP_AMBIENT_REG:  prdata = {24'h0, temp_ambient_reg};
            
            // System Status
            CLOCK_STATUS_REG:  prdata = {30'h0, clock_status_reg};
            POWER_STATUS_REG:  prdata = {30'h0, power_status_reg};
            SEQ_STATUS_REG:    prdata = {29'h0, seq_state_reg};
            
            default:           prdata = '0;
        endcase
    end
end


// Memory Channel Monitoring (Channel 0)

always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        mem_voltage_reg <= '0;
        mem_current_reg <= '0;
        mem_power_reg   <= '0;
    end else if (control_reg.enable_monitoring && control_reg.enable_mem) begin
        // Capture voltage and current
        mem_voltage_reg <= mem_voltage;
        mem_current_reg <= mem_current;
        // Calculate power: P = V × I
        mem_power_reg   <= mem_voltage * mem_current;
    end
end


// I/O Channel Monitoring (Channel 1)

always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        io_voltage_reg <= '0;
        io_current_reg <= '0;
        io_power_reg   <= '0;
    end else if (control_reg.enable_monitoring && control_reg.enable_io) begin
        io_voltage_reg <= io_voltage;
        io_current_reg <= io_current;
        io_power_reg   <= io_voltage * io_current;
    end
end

// Temperature Monitoring

always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        temp_memory_reg  <= '0;
        temp_ambient_reg <= '0;
    end else if (control_reg.enable_monitoring) begin
        temp_memory_reg  <= temp_memory;
        temp_ambient_reg <= temp_ambient;
    end
end


// System Status Monitoring

always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        clock_status_reg <= '0;
        power_status_reg <= '0;
        seq_state_reg    <= '0;
    end else begin
        clock_status_reg <= clock_status;
        power_status_reg <= power_status;
        seq_state_reg    <= seq_state;
    end
end

// Clock Control Logic

always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        clock_enable <= 2'b11;  // Both clocks enabled on reset
    end else begin
        if (clock_control_reg.auto_gate_enable) begin
            // Automatic clock gating based on alerts
            if (power_alert) begin
                // Power alert: Keep Memory only, gate I/O
                clock_enable <= 2'b01;
            end else if (temp_alert) begin
                // Temperature alert: Gate I/O only
                clock_enable <= 2'b01;
            end else begin
                // No alerts: Use manual settings
                clock_enable <= {clock_control_reg.io_clk_enable,
                                clock_control_reg.mem_clk_enable};
            end
        end else begin
            // Manual clock control (auto-gating disabled)
            clock_enable <= {clock_control_reg.io_clk_enable,
                            clock_control_reg.mem_clk_enable};
        end
    end
end


// Alert Generation


// Temperature Alert
always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        temp_alert <= 1'b0;
    end else begin
        temp_alert <= (temp_memory_reg > threshold_reg.temp_threshold) || 
                      (temp_ambient_reg > threshold_reg.temp_threshold);
    end
end

// Power Alert (any channel exceeds threshold)
always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        power_alert <= 1'b0;
    end else begin
        // Compare full 24-bit power value against threshold (shifted to match scale)
        power_alert <= (mem_power_reg > {threshold_reg.power_threshold, 12'h0}) ||
                       (io_power_reg > {threshold_reg.power_threshold, 12'h0});
    end
end
endmodule