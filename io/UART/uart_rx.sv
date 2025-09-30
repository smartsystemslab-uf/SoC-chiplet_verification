
// 
// Module: uart_rx 
// 
// Notes:
// - UART reciever module.
//

module Uart_rx #(
    parameter DATA_WIDTH = 8,
    parameter BAUD_RATE  = 9600,
    parameter CLOCK_FREQ = 50_000_000
) (
    input  logic                  clk,
    input  logic                  rst_n,
    input  logic                  rx,
    output logic [DATA_WIDTH-1:0] data_out,
    output logic                  valid,
    input  logic                  ready
);

// --------------------------------------------------------------------------- 
// External parameters.
// 

//
// module parameters (passed in via the module header)

//
// Number of stop bits indicating the end of a packet.
parameter   STOP_BITS       = 1;

// -------------------------------------------------------------------------- 
// Internal parameters.
// 

//
// Number of clock cycles per uart bit. Use integer division CLOCK_FREQ/BAUD_RATE
// This matches the calculation in Uart_tx and avoids undefined BIT_P/CLK_P.
localparam int CYCLES_PER_BIT = (CLOCK_FREQ + BAUD_RATE/2) / BAUD_RATE; // rounded

// Size of the registers which store sample counts and bit durations.
localparam       COUNT_REG_LEN      = 1+$clog2(CYCLES_PER_BIT);

// -------------------------------------------------------------------------- 
// Internal registers.
// 

//
// Internally latched value of the uart_rxd line. Helps break long timing
// paths from input pins into the logic.
logic rxd_reg;
logic rxd_reg_0;

//
// Storage for the recieved serial data.
logic [DATA_WIDTH-1:0] recieved_data;

//
// Counter for the number of cycles over a packet bit.
logic [COUNT_REG_LEN-1:0] cycle_counter;

//
// Counter for the number of recieved bits of the packet.
logic [3:0] bit_counter;

//
// Sample of the UART input line whenever we are in the middle of a bit frame.
logic bit_sample;

//
// Current and next states of the internal FSM.
// Use SystemVerilog enum instead of localparam
typedef enum logic [1:0] {
    FSM_IDLE  = 2'd0,
    FSM_START = 2'd1,
    FSM_RECV  = 2'd2,
    FSM_STOP  = 2'd3
} fsm_e;

fsm_e fsm_state;
fsm_e n_fsm_state;

// --------------------------------------------------------------------------- 
// Output assignment and combinational signals
// 
logic next_bit;
logic payload_done;

always_comb begin
    // combinational helper signals
    next_bit = (cycle_counter == CYCLES_PER_BIT) ||
               ((fsm_state == FSM_STOP) && (cycle_counter == CYCLES_PER_BIT/2));
    payload_done = (bit_counter == DATA_WIDTH);

    // outputs
    valid = (fsm_state == FSM_STOP && n_fsm_state == FSM_IDLE);
    // data_out updated in sequential block
end

always_ff @(posedge clk) begin
    if(!rst_n) begin
        data_out  <= {DATA_WIDTH{1'b0}};
    end else if (fsm_state == FSM_STOP) begin
        data_out  <= recieved_data;
    end
end

// --------------------------------------------------------------------------- 
// FSM next state selection.
 
// Handle picking the next state.
always_comb begin : p_n_fsm_state
    case(fsm_state)
    FSM_IDLE : n_fsm_state = rxd_reg      ? FSM_IDLE : FSM_START;
        FSM_START: n_fsm_state = next_bit     ? FSM_RECV : FSM_START;
        FSM_RECV : n_fsm_state = payload_done ? FSM_STOP : FSM_RECV ;
        FSM_STOP : n_fsm_state = next_bit     ? FSM_IDLE : FSM_STOP ;
        default  : n_fsm_state = FSM_IDLE;
    endcase
end

// --------------------------------------------------------------------------- 
// Internal register setting and re-setting.
// 

//
// Handle updates to the recieved data register.
always_ff @(posedge clk) begin : p_recieved_data
    if(!rst_n) begin
        recieved_data <= {DATA_WIDTH{1'b0}};
    end else if(fsm_state == FSM_IDLE             ) begin
        recieved_data <= {DATA_WIDTH{1'b0}};
    end else if(fsm_state == FSM_RECV && next_bit ) begin
        recieved_data[DATA_WIDTH-1] <= bit_sample;
        for (int i = DATA_WIDTH-2; i >= 0; i = i - 1) begin
            recieved_data[i] <= recieved_data[i+1];
        end
    end
end

//
// Increments the bit counter when recieving.
always_ff @(posedge clk) begin : p_bit_counter
    if(!rst_n) begin
        bit_counter <= 4'b0;
    end else if(fsm_state != FSM_RECV) begin
        bit_counter <= {COUNT_REG_LEN{1'b0}};
    end else if(fsm_state == FSM_RECV && next_bit) begin
        bit_counter <= bit_counter + 1'b1;
    end
end

//
// Sample the recieved bit when in the middle of a bit frame.
always_ff @(posedge clk) begin : p_bit_sample
    if(!rst_n) begin
        bit_sample <= 1'b0;
    end else if (cycle_counter == CYCLES_PER_BIT/2) begin
        bit_sample <= rxd_reg;
    end
end


//
// Increments the cycle counter when recieving.
always_ff @(posedge clk) begin : p_cycle_counter
    if(!rst_n) begin
        cycle_counter <= {COUNT_REG_LEN{1'b0}};
    end else if(next_bit) begin
        cycle_counter <= {COUNT_REG_LEN{1'b0}};
    end else if(fsm_state == FSM_START || 
                fsm_state == FSM_RECV  || 
                fsm_state == FSM_STOP   ) begin
        cycle_counter <= cycle_counter + 1'b1;
    end
end


//
// Progresses the next FSM state.
always_ff @(posedge clk) begin : p_fsm_state
    if(!rst_n) begin
        fsm_state <= FSM_IDLE;
    end else begin
        fsm_state <= n_fsm_state;
    end
end


//
// Responsible for updating the internal value of the rxd_reg.
always_ff @(posedge clk) begin : p_rxd_reg
    if(!rst_n) begin
        rxd_reg     <= 1'b1;
        rxd_reg_0   <= 1'b1;
    end else begin
        rxd_reg     <= rxd_reg_0;
        rxd_reg_0   <= rx;
    end
end


endmodule
