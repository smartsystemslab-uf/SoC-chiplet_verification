
// 
// Module: uart_tx 
// 
// Notes:
// - UART transmitter module.
//

module Uart_tx #(
    parameter DATA_WIDTH = 8,
    parameter BAUD_RATE  = 9600,
    parameter CLOCK_FREQ = 50_000_000
) (
    input  logic                      clk,
    input  logic                      rst_n,
    output logic                      tx,
    input  logic [DATA_WIDTH-1:0]     data_in,
    input  logic                      valid,
    output logic                      ready
);

// --------------------------------------------------------------------------- 
// External parameters.
// 

//
// module parameters (passed in via the module header)

// Number of clock cycles per uart bit. Use integer division CLOCK_FREQ/BAUD_RATE
// This is simpler and avoids nanosecond integer-division ordering issues.
localparam int CYCLES_PER_BIT = (CLOCK_FREQ + BAUD_RATE/2) / BAUD_RATE; // rounded

//
// Number of stop bits indicating the end of a packet.
parameter   STOP_BITS       = 1;

// --------------------------------------------------------------------------- 
// Internal parameters.
// 

//
// Number of clock cycles per uart bit.
// Size of the registers which store sample counts and bit durations.
localparam       COUNT_REG_LEN      = 1+$clog2(CYCLES_PER_BIT);

// --------------------------------------------------------------------------- 
// Internal registers.
// 

//
// Internally latched value of the uart_txd line. Helps break long timing
// paths from the logic to the output pins.
logic txd_reg;

//
// Storage for the serial data to be sent.
logic [DATA_WIDTH-1:0] data_to_send;

//
// Counter for the number of cycles over a packet bit.
logic [COUNT_REG_LEN-1:0] cycle_counter;

//
// Counter for the number of sent bits of the packet.
logic [3:0] bit_counter;

//
// Current and next states of the internal FSM.
// FSM state type (use SystemVerilog enum instead of localparam)
typedef enum logic [1:0] {
    FSM_IDLE  = 2'd0,
    FSM_START = 2'd1,
    FSM_SEND  = 2'd2,
    FSM_STOP  = 2'd3
} fsm_e;

fsm_e fsm_state;
fsm_e n_fsm_state;


// --------------------------------------------------------------------------- 
// FSM next state selection.
// 

// combinational signals driven by always_comb
logic next_bit;
logic payload_done;
logic stop_done;

// drive outputs and combinational signals
always_comb begin : p_outputs_and_comb
    // ready is high when not busy
    ready        = (fsm_state == FSM_IDLE);
    tx           = txd_reg;

    next_bit     = (cycle_counter == CYCLES_PER_BIT);
    payload_done = (bit_counter   == DATA_WIDTH);
    stop_done    = (bit_counter   == STOP_BITS) && (fsm_state == FSM_STOP);
end

// Handle picking the next state.
always_comb begin : p_n_fsm_state
    case(fsm_state)
    FSM_IDLE : n_fsm_state = valid         ? FSM_START: FSM_IDLE ;
        FSM_START: n_fsm_state = next_bit     ? FSM_SEND : FSM_START;
        FSM_SEND : n_fsm_state = payload_done ? FSM_STOP : FSM_SEND ;
        FSM_STOP : n_fsm_state = stop_done    ? FSM_IDLE : FSM_STOP ;
        default  : n_fsm_state = FSM_IDLE;
    endcase

end

// --------------------------------------------------------------------------- 
// Internal register setting and re-setting.
// 

//
// Handle updates to the sent data register.
always_ff @(posedge clk) begin : p_data_to_send
    if(!rst_n) begin
        data_to_send <= {DATA_WIDTH{1'b0}};
    end else if(fsm_state == FSM_IDLE && valid) begin
        data_to_send <= data_in;
    end else if(fsm_state       == FSM_SEND       && next_bit ) begin
        for (int i = DATA_WIDTH-2; i >= 0; i = i - 1) begin
            data_to_send[i] <= data_to_send[i+1];
        end
    end
end


//
// Increments the bit counter each time a new bit frame is sent.
always_ff @(posedge clk) begin : p_bit_counter
    if(!rst_n) begin
        bit_counter <= 4'b0;
    end else if(fsm_state != FSM_SEND && fsm_state != FSM_STOP) begin
        bit_counter <= {COUNT_REG_LEN{1'b0}};
    end else if(fsm_state == FSM_SEND && n_fsm_state == FSM_STOP) begin
        bit_counter <= {COUNT_REG_LEN{1'b0}};
    end else if(fsm_state == FSM_STOP&& next_bit) begin
        bit_counter <= bit_counter + 1'b1;
    end else if(fsm_state == FSM_SEND && next_bit) begin
        bit_counter <= bit_counter + 1'b1;
    end
end


//
// Increments the cycle counter when sending.
always_ff @(posedge clk) begin : p_cycle_counter
    if(!rst_n) begin
        cycle_counter <= {COUNT_REG_LEN{1'b0}};
    end else if(next_bit) begin
        cycle_counter <= {COUNT_REG_LEN{1'b0}};
    end else if(fsm_state == FSM_START || 
                fsm_state == FSM_SEND  || 
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
// Responsible for updating the internal value of the txd_reg.
always_ff @(posedge clk) begin : p_txd_reg
    if(!rst_n) begin
        txd_reg <= 1'b1;
    end else if(fsm_state == FSM_IDLE) begin
        txd_reg <= 1'b1;
    end else if(fsm_state == FSM_START) begin
        txd_reg <= 1'b0;
    end else if(fsm_state == FSM_SEND) begin
        txd_reg <= data_to_send[0];
    end else if(fsm_state == FSM_STOP) begin
        txd_reg <= 1'b1;
    end
end

endmodule
