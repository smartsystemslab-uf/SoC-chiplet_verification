// Code your design here
#include <systemc.h>

// Register Address Map
#define CONTROL_REG       0x00
#define STATUS_REG        0x04
#define THRESHOLD_REG     0x08
#define MEM_VOLTAGE       0x10
#define MEM_CURRENT       0x14
#define MEM_POWER         0x18
#define IO_VOLTAGE        0x20
#define IO_CURRENT        0x24
#define IO_POWER          0x28
#define TEMP_MEMORY_REG   0x30
#define TEMP_AMBIENT_REG  0x34
#define CLOCK_STATUS_REG  0x38
#define POWER_STATUS_REG  0x3C
#define SEQ_STATUS_REG    0x40
#define CLOCK_CONTROL_REG 0x44

// Power Monitoring Unit Module
SC_MODULE(power_monitoring_unit) {
    // Clock and Reset
    sc_in<bool> clk;
    sc_in<bool> rst_n;
    
    // APB Interface
    sc_in<sc_uint<8>> paddr;
    sc_in<bool> psel;
    sc_in<bool> penable;
    sc_in<bool> pwrite;
    sc_in<sc_uint<32>> pwdata;
    sc_out<sc_uint<32>> prdata;
    sc_out<bool> pready;
    
    // Analog Interface (Monitor Inputs)
    sc_in<sc_uint<12>> mem_voltage;
    sc_in<sc_uint<12>> mem_current;
    sc_in<sc_uint<12>> io_voltage;
    sc_in<sc_uint<12>> io_current;
    sc_in<sc_uint<8>> temp_memory;
    sc_in<sc_uint<8>> temp_ambient;
    
    // Alert Outputs
    sc_out<bool> voltage_alert;
    sc_out<bool> power_alert;
    sc_out<bool> temp_alert;
    
    // Clock Outputs
    sc_out<bool> mem_clk_out;
    sc_out<bool> io_clk_out;
    
    // Internal registers
    sc_uint<32> control_reg;
    sc_uint<32> threshold_reg;
    sc_uint<32> clock_control_reg;
    sc_uint<32> status_reg;
    
    sc_uint<12> mem_voltage_reg;
    sc_uint<12> mem_current_reg;
    sc_uint<12> mem_power_reg;
    sc_uint<12> io_voltage_reg;
    sc_uint<12> io_current_reg;
    sc_uint<12> io_power_reg;
    sc_uint<8> temp_memory_reg;
    sc_uint<8> temp_ambient_reg;
    
    bool volt_alert;
    bool pwr_alert;
    bool tmp_alert;
    
    // APB State
    enum apb_state_t {APB_IDLE, APB_SETUP, APB_ACCESS};
    apb_state_t apb_state;
    
    // Constructor
    SC_CTOR(power_monitoring_unit) {
        SC_THREAD(apb_process);
        sensitive << clk.pos();
        
        SC_THREAD(monitor_process);
        sensitive << clk.pos();
        
        SC_THREAD(alert_process);
        sensitive << clk.pos();
        
        SC_THREAD(clock_process);
        sensitive << clk.pos();
    }
    
    // APB Interface Process
    void apb_process() {
        while(true) {
            wait();
            
            if(!rst_n.read()) {
                prdata.write(0);
                pready.write(true);
                control_reg = 0;
                threshold_reg = 0;
                clock_control_reg = 0x3; // Clocks enabled by default
                status_reg = 0;
                apb_state = APB_IDLE;
            } else {
                // APB State Machine
                switch(apb_state) {
                    case APB_IDLE:
                        pready.write(true);
                        if(psel.read() && !penable.read()) {
                            apb_state = APB_SETUP;
                        }
                        break;
                        
                    case APB_SETUP:
                        if(psel.read() && penable.read()) {
                            apb_state = APB_ACCESS;
                            
                            if(pwrite.read()) {
                                // Write operation
                                switch(paddr.read().to_uint()) {
                                    case CONTROL_REG:
                                        control_reg = pwdata.read();
                                        break;
                                    case THRESHOLD_REG:
                                        threshold_reg = pwdata.read();
                                        break;
                                    case CLOCK_CONTROL_REG:
                                        clock_control_reg = pwdata.read();
                                        break;
                                }
                            } else {
                                // Read operation
                                sc_uint<32> read_data = 0;
                                switch(paddr.read().to_uint()) {
                                    case CONTROL_REG:
                                        read_data = control_reg;
                                        break;
                                    case STATUS_REG:
                                        read_data = status_reg;
                                        break;
                                    case THRESHOLD_REG:
                                        read_data = threshold_reg;
                                        break;
                                    case MEM_VOLTAGE:
                                        read_data = mem_voltage_reg;
                                        break;
                                    case MEM_CURRENT:
                                        read_data = mem_current_reg;
                                        break;
                                    case MEM_POWER:
                                        read_data = mem_power_reg;
                                        break;
                                    case IO_VOLTAGE:
                                        read_data = io_voltage_reg;
                                        break;
                                    case IO_CURRENT:
                                        read_data = io_current_reg;
                                        break;
                                    case IO_POWER:
                                        read_data = io_power_reg;
                                        break;
                                    case TEMP_MEMORY_REG:
                                        read_data = temp_memory_reg;
                                        break;
                                    case TEMP_AMBIENT_REG:
                                        read_data = temp_ambient_reg;
                                        break;
                                    case CLOCK_STATUS_REG:
                                        read_data = clock_control_reg;
                                        break;
                                    case POWER_STATUS_REG:
                                        read_data = (pwr_alert << 2) | (volt_alert << 1) | tmp_alert;
                                        break;
                                    case CLOCK_CONTROL_REG:
                                        read_data = clock_control_reg;
                                        break;
                                }
                                prdata.write(read_data);
                            }
                            pready.write(true);
                        }
                        break;
                        
                    case APB_ACCESS:
                        if(!psel.read()) {
                            apb_state = APB_IDLE;
                        }
                        break;
                }
            }
        }
    }
    
    // Monitor Process - Sample analog inputs
    void monitor_process() {
        while(true) {
            wait();
            
            if(!rst_n.read()) {
                mem_voltage_reg = 0;
                mem_current_reg = 0;
                mem_power_reg = 0;
                io_voltage_reg = 0;
                io_current_reg = 0;
                io_power_reg = 0;
                temp_memory_reg = 0;
                temp_ambient_reg = 0;
            } else {
                bool enable_monitoring = control_reg.bit(0);
                bool enable_mem = control_reg.bit(1);
                bool enable_io = control_reg.bit(2);
                
                if(enable_monitoring) {
                    if(enable_mem) {
                        mem_voltage_reg = mem_voltage.read();
                        mem_current_reg = mem_current.read();
                        // Simple power calculation (in real design this would be more complex)
                        mem_power_reg = (mem_voltage_reg * mem_current_reg) >> 10;
                    }
                    
                    if(enable_io) {
                        io_voltage_reg = io_voltage.read();
                        io_current_reg = io_current.read();
                        io_power_reg = (io_voltage_reg * io_current_reg) >> 10;
                    }
                    
                    temp_memory_reg = temp_memory.read();
                    temp_ambient_reg = temp_ambient.read();
                }
            }
        }
    }
    
    // Alert Generation Process
    void alert_process() {
        while(true) {
            wait();
            
            if(!rst_n.read()) {
                volt_alert = false;
                pwr_alert = false;
                tmp_alert = false;
                voltage_alert.write(false);
                power_alert.write(false);
                temp_alert.write(false);
            } else {
                bool enable_monitoring = control_reg.bit(0);
                
                if(enable_monitoring) {
                    // Extract thresholds
                    sc_uint<12> voltage_threshold = threshold_reg.range(11, 0);
                    sc_uint<12> power_threshold = threshold_reg.range(23, 12);
                    sc_uint<8> temp_threshold = threshold_reg.range(31, 24);
                    
                    // Check voltage alert
                    volt_alert = (mem_voltage_reg > voltage_threshold) || 
                                 (io_voltage_reg > voltage_threshold);
                    
                    // Check power alert
                    pwr_alert = (mem_power_reg > power_threshold) || 
                                (io_power_reg > power_threshold);
                    
                    // Check temperature alert
                    tmp_alert = (temp_memory_reg > temp_threshold) || 
                                (temp_ambient_reg > temp_threshold);
                    
                    voltage_alert.write(volt_alert);
                    power_alert.write(pwr_alert);
                    temp_alert.write(tmp_alert);
                    
                    // Update status register
                    status_reg = (tmp_alert << 2) | (pwr_alert << 1) | volt_alert;
                }
            }
        }
    }
    
    // Clock Control Process
    void clock_process() {
        while(true) {
            wait();
            
            if(!rst_n.read()) {
                mem_clk_out.write(false);
                io_clk_out.write(false);
            } else {
                bool mem_clk_enable = clock_control_reg.bit(0);
                bool io_clk_enable = clock_control_reg.bit(1);
                bool auto_gate_enable = clock_control_reg.bit(2);
                
                // Clock gating logic
                bool gate_clocks = auto_gate_enable && (volt_alert || pwr_alert || tmp_alert);
                
                mem_clk_out.write(mem_clk_enable && !gate_clocks);
                io_clk_out.write(io_clk_enable && !gate_clocks);
            }
        }
    }
};
