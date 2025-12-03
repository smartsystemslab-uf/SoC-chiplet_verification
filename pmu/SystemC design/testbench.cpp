// Code your testbench here
// or browse Examples
// Testbench File
// File: testbench.cpp

#include <systemc.h>
#include "design.cpp"

SC_MODULE(testbench) {
    // Clock and Reset
    sc_clock clk;
    sc_signal<bool> rst_n;
    
    // APB Interface signals
    sc_signal<sc_uint<8>> paddr;
    sc_signal<bool> psel;
    sc_signal<bool> penable;
    sc_signal<bool> pwrite;
    sc_signal<sc_uint<32>> pwdata;
    sc_signal<sc_uint<32>> prdata;
    sc_signal<bool> pready;
    
    // Analog Interface (Monitor Inputs)
    sc_signal<sc_uint<12>> mem_voltage;
    sc_signal<sc_uint<12>> mem_current;
    sc_signal<sc_uint<12>> io_voltage;
    sc_signal<sc_uint<12>> io_current;
    sc_signal<sc_uint<8>> temp_memory;
    sc_signal<sc_uint<8>> temp_ambient;
    
    // Alert Outputs
    sc_signal<bool> voltage_alert;
    sc_signal<bool> power_alert;
    sc_signal<bool> temp_alert;
    
    // Clock Outputs
    sc_signal<bool> mem_clk_out;
    sc_signal<bool> io_clk_out;
    
    // DUT instance
    power_monitoring_unit *dut;
    
    // Constructor
    SC_CTOR(testbench) : clk("clk", 10, SC_NS) {
        // Instantiate DUT
        dut = new power_monitoring_unit("dut");
        
        // Connect signals
        dut->clk(clk);
        dut->rst_n(rst_n);
        dut->paddr(paddr);
        dut->psel(psel);
        dut->penable(penable);
        dut->pwrite(pwrite);
        dut->pwdata(pwdata);
        dut->prdata(prdata);
        dut->pready(pready);
        dut->mem_voltage(mem_voltage);
        dut->mem_current(mem_current);
        dut->io_voltage(io_voltage);
        dut->io_current(io_current);
        dut->temp_memory(temp_memory);
        dut->temp_ambient(temp_ambient);
        dut->voltage_alert(voltage_alert);
        dut->power_alert(power_alert);
        dut->temp_alert(temp_alert);
        dut->mem_clk_out(mem_clk_out);
        dut->io_clk_out(io_clk_out);
        
        // Register stimulus process
        SC_THREAD(stimulus);
        sensitive << clk.posedge_event();
        
        // Register monitor process
        SC_THREAD(monitor);
        sensitive << clk.posedge_event();
    }
    
    // APB Write Task
    void apb_write(sc_uint<8> addr, sc_uint<32> data) {
        wait(clk.posedge_event());
        paddr.write(addr);
        pwdata.write(data);
        pwrite.write(true);
        psel.write(true);
        penable.write(false);
        
        wait(clk.posedge_event());
        penable.write(true);
        
        wait(clk.posedge_event());
        psel.write(false);
        penable.write(false);
        pwrite.write(false);
    }
    
    // APB Read Task
    sc_uint<32> apb_read(sc_uint<8> addr) {
        wait(clk.posedge_event());
        paddr.write(addr);
        pwrite.write(false);
        psel.write(true);
        penable.write(false);
        
        wait(clk.posedge_event());
        penable.write(true);
        
        wait(clk.posedge_event());
        sc_uint<32> read_data = prdata.read();
        psel.write(false);
        penable.write(false);
        
        return read_data;
    }
    
    // Stimulus process - Test Cases
    void stimulus() {
        sc_uint<32> read_val;
        
        // Initialize all signals
        rst_n.write(false);
        psel.write(false);
        penable.write(false);
        pwrite.write(false);
        paddr.write(0);
        pwdata.write(0);
        
        mem_voltage.write(0);
        mem_current.write(0);
        io_voltage.write(0);
        io_current.write(0);
        temp_memory.write(0);
        temp_ambient.write(0);
        
        cout << "\n========================================" << endl;
        cout << "Power Monitoring Unit - Test Suite" << endl;
        cout << "========================================\n" << endl;
        
        // Wait for some cycles
        for(int i = 0; i < 5; i++) {
            wait(clk.posedge_event());
        }
        
        // Release reset
        rst_n.write(true);
        cout << "[" << sc_time_stamp() << "] Reset released" << endl;
        wait(clk.posedge_event());
        
        // ==========================================
        // TEST CASE 1: Basic Configuration
        // ==========================================
        cout << "\n[TEST 1] Basic Configuration & Enable" << endl;
        cout << "--------------------------------------" << endl;
        
        // Set thresholds: voltage=1200, power=2000, temp=85
        apb_write(0x08, (85 << 24) | (2000 << 12) | 1200);
        cout << "[" << sc_time_stamp() << "] Configured thresholds" << endl;
        
        // Enable monitoring
        apb_write(0x00, 0x7); // Enable all: monitoring, mem, io
        cout << "[" << sc_time_stamp() << "] Enabled monitoring" << endl;
        
        // Verify configuration
        read_val = apb_read(0x00);
        cout << "[" << sc_time_stamp() << "] Control Register = 0x" << hex << read_val << dec << endl;
        
        // Set normal operating values (below thresholds)
        mem_voltage.write(1000);
        mem_current.write(500);
        io_voltage.write(1100);
        io_current.write(300);
        temp_memory.write(70);
        temp_ambient.write(65);
        
        for(int i = 0; i < 10; i++) {
            wait(clk.posedge_event());
        }
        cout << "[" << sc_time_stamp() << "] Normal operation - No alerts expected" << endl;
        
        // ==========================================
        // TEST CASE 2: Voltage Alert
        // ==========================================
        cout << "\n[TEST 2] Voltage Threshold Violation" << endl;
        cout << "--------------------------------------" << endl;
        
        mem_voltage.write(1300); // Exceed threshold of 1200
        cout << "[" << sc_time_stamp() << "] Set mem_voltage = 1300 (threshold = 1200)" << endl;
        
        for(int i = 0; i < 5; i++) {
            wait(clk.posedge_event());
        }
        
        // Check voltage alert
        if(voltage_alert.read()) {
            cout << "[" << sc_time_stamp() << "] PASS: Voltage alert triggered" << endl;
        } else {
            cout << "[" << sc_time_stamp() << "] FAIL: Voltage alert not triggered" << endl;
        }
        
        // Read voltage register
        read_val = apb_read(0x10);
        cout << "[" << sc_time_stamp() << "] Memory Voltage Register = " << read_val << endl;
        
        // Return to normal
        mem_voltage.write(1000);
        for(int i = 0; i < 5; i++) {
            wait(clk.posedge_event());
        }
        
        // ==========================================
        // TEST CASE 3: Power Alert
        // ==========================================
        cout << "\n[TEST 3] Power Threshold Violation" << endl;
        cout << "--------------------------------------" << endl;
        
        mem_voltage.write(1500);
        mem_current.write(1500); // Power = V*I (simulated)
        cout << "[" << sc_time_stamp() << "] Set high voltage and current for power alert" << endl;
        
        for(int i = 0; i < 5; i++) {
            wait(clk.posedge_event());
        }
        
        // Read power register
        read_val = apb_read(0x18);
        cout << "[" << sc_time_stamp() << "] Memory Power Register = " << read_val << endl;
        
        if(power_alert.read()) {
            cout << "[" << sc_time_stamp() << "] PASS: Power alert triggered" << endl;
        } else {
            cout << "[" << sc_time_stamp() << "] FAIL: Power alert not triggered" << endl;
        }
        
        // Return to normal
        mem_voltage.write(1000);
        mem_current.write(500);
        for(int i = 0; i < 5; i++) {
            wait(clk.posedge_event());
        }
        
        // ==========================================
        // TEST CASE 4: Temperature Alert
        // ==========================================
        cout << "\n[TEST 4] Temperature Threshold Violation" << endl;
        cout << "--------------------------------------" << endl;
        
        temp_memory.write(90); // Exceed threshold of 85
        temp_ambient.write(88);
        cout << "[" << sc_time_stamp() << "] Set temp_memory = 90C, temp_ambient = 88C (threshold = 85C)" << endl;
        
        for(int i = 0; i < 5; i++) {
            wait(clk.posedge_event());
        }
        
        if(temp_alert.read()) {
            cout << "[" << sc_time_stamp() << "] PASS: Temperature alert triggered" << endl;
        } else {
            cout << "[" << sc_time_stamp() << "] FAIL: Temperature alert not triggered" << endl;
        }
        
        // Read temperature registers
        read_val = apb_read(0x30);
        cout << "[" << sc_time_stamp() << "] Memory Temperature = " << read_val << "C" << endl;
        read_val = apb_read(0x34);
        cout << "[" << sc_time_stamp() << "] Ambient Temperature = " << read_val << "C" << endl;
        
        // Return to normal
        temp_memory.write(70);
        temp_ambient.write(65);
        for(int i = 0; i < 5; i++) {
            wait(clk.posedge_event());
        }
        
        // ==========================================
        // TEST CASE 5: Multiple Simultaneous Alerts
        // ==========================================
        cout << "\n[TEST 5] Multiple Simultaneous Violations" << endl;
        cout << "--------------------------------------" << endl;
        
        mem_voltage.write(1400);  // Voltage alert
        io_current.write(2500);   // Power alert
        temp_memory.write(95);     // Temperature alert
        cout << "[" << sc_time_stamp() << "] Triggering all alerts simultaneously" << endl;
        
        for(int i = 0; i < 5; i++) {
            wait(clk.posedge_event());
        }
        
        cout << "[" << sc_time_stamp() << "] Alert Status:" << endl;
        cout << "  Voltage Alert: " << (voltage_alert.read() ? "ACTIVE" : "INACTIVE") << endl;
        cout << "  Power Alert: " << (power_alert.read() ? "ACTIVE" : "INACTIVE") << endl;
        cout << "  Temperature Alert: " << (temp_alert.read() ? "ACTIVE" : "INACTIVE") << endl;
        
        // Return to normal
        mem_voltage.write(1000);
        io_current.write(300);
        temp_memory.write(70);
        for(int i = 0; i < 5; i++) {
            wait(clk.posedge_event());
        }
        
        // ==========================================
        // TEST CASE 6: Clock Control & Gating
        // ==========================================
        cout << "\n[TEST 6] Clock Control and Auto-Gating" << endl;
        cout << "--------------------------------------" << endl;
        
        // Enable auto clock gating
        apb_write(0x44, 0x7); // mem_clk=1, io_clk=1, auto_gate=1
        cout << "[" << sc_time_stamp() << "] Enabled auto clock gating" << endl;
        
        for(int i = 0; i < 5; i++) {
            wait(clk.posedge_event());
        }
        
        // Trigger alert to test auto-gating
        mem_voltage.write(1500);
        cout << "[" << sc_time_stamp() << "] Triggered alert to test clock gating" << endl;
        
        for(int i = 0; i < 5; i++) {
            wait(clk.posedge_event());
        }
        
        cout << "[" << sc_time_stamp() << "] Clock Status:" << endl;
        cout << "  Memory Clock: " << (mem_clk_out.read() ? "RUNNING" : "GATED") << endl;
        cout << "  IO Clock: " << (io_clk_out.read() ? "RUNNING" : "GATED") << endl;
        
        mem_voltage.write(1000);
        for(int i = 0; i < 5; i++) {
            wait(clk.posedge_event());
        }
        
        // ==========================================
        // TEST CASE 7: Disable/Enable Channels
        // ==========================================
        cout << "\n[TEST 7] Channel Enable/Disable" << endl;
        cout << "--------------------------------------" << endl;
        
        // Disable memory channel
        apb_write(0x00, 0x5); // monitoring=1, mem=0, io=1
        cout << "[" << sc_time_stamp() << "] Disabled memory channel" << endl;
        
        // Try to trigger alert on disabled channel
        mem_voltage.write(1500);
        for(int i = 0; i < 5; i++) {
            wait(clk.posedge_event());
        }
        
        if(!voltage_alert.read()) {
            cout << "[" << sc_time_stamp() << "] PASS: No alert on disabled channel" << endl;
        } else {
            cout << "[" << sc_time_stamp() << "] FAIL: Alert triggered on disabled channel" << endl;
        }
        
        // Re-enable memory channel
        apb_write(0x00, 0x7);
        cout << "[" << sc_time_stamp() << "] Re-enabled memory channel" << endl;
        
        for(int i = 0; i < 5; i++) {
            wait(clk.posedge_event());
        }
        
        // ==========================================
        // TEST CASE 8: I/O Channel Monitoring
        // ==========================================
        cout << "\n[TEST 8] I/O Channel Monitoring" << endl;
        cout << "--------------------------------------" << endl;
        
        mem_voltage.write(1000); // Return mem to normal first
        io_voltage.write(1300);
        io_current.write(800);
        cout << "[" << sc_time_stamp() << "] Set I/O channel values" << endl;
        
        for(int i = 0; i < 5; i++) {
            wait(clk.posedge_event());
        }
        
        read_val = apb_read(0x20);
        cout << "[" << sc_time_stamp() << "] I/O Voltage = " << read_val << endl;
        read_val = apb_read(0x24);
        cout << "[" << sc_time_stamp() << "] I/O Current = " << read_val << endl;
        read_val = apb_read(0x28);
        cout << "[" << sc_time_stamp() << "] I/O Power = " << read_val << endl;
        
        // ==========================================
        // TEST CASE 9: Status Register Check
        // ==========================================
        cout << "\n[TEST 9] Status Register Verification" << endl;
        cout << "--------------------------------------" << endl;
        
        read_val = apb_read(0x04);
        cout << "[" << sc_time_stamp() << "] Status Register = 0x" << hex << read_val << dec << endl;
        
        read_val = apb_read(0x38);
        cout << "[" << sc_time_stamp() << "] Clock Status = 0x" << hex << read_val << dec << endl;
        
        read_val = apb_read(0x3C);
        cout << "[" << sc_time_stamp() << "] Power Status = 0x" << hex << read_val << dec << endl;
        
        // ==========================================
        // TEST CASE 10: Dynamic Threshold Changes
        // ==========================================
        cout << "\n[TEST 10] Dynamic Threshold Changes" << endl;
        cout << "--------------------------------------" << endl;
        
        io_voltage.write(1000); // Return IO to normal
        mem_voltage.write(1100);
        cout << "[" << sc_time_stamp() << "] Set voltage = 1100" << endl;
        
        // Set threshold to 1000 (should trigger alert)
        apb_write(0x08, (85 << 24) | (2000 << 12) | 1000);
        cout << "[" << sc_time_stamp() << "] Lowered voltage threshold to 1000" << endl;
        
        for(int i = 0; i < 5; i++) {
            wait(clk.posedge_event());
        }
        
        if(voltage_alert.read()) {
            cout << "[" << sc_time_stamp() << "] PASS: Alert after threshold change" << endl;
        }
        
        // Raise threshold again (alert should clear)
        apb_write(0x08, (85 << 24) | (2000 << 12) | 1200);
        cout << "[" << sc_time_stamp() << "] Raised voltage threshold to 1200" << endl;
        
        for(int i = 0; i < 5; i++) {
            wait(clk.posedge_event());
        }
        
        if(!voltage_alert.read()) {
            cout << "[" << sc_time_stamp() << "] PASS: Alert cleared after threshold raise" << endl;
        }
        
        // ==========================================
        // Test Complete
        // ==========================================
        cout << "\n========================================" << endl;
        cout << "All Test Cases Completed!" << endl;
        cout << "========================================\n" << endl;
        
        for(int i = 0; i < 10; i++) {
            wait(clk.posedge_event());
        }
        
        sc_stop();
    }
    
    // Monitor process
    void monitor() {
        while(true) {
            wait(clk.posedge_event());
            
            // Monitor alerts with detailed logging when they change
            static bool prev_volt = false, prev_pwr = false, prev_temp = false;
            
            if(voltage_alert.read() != prev_volt) {
                cout << "[" << sc_time_stamp() << "] ALERT CHANGE: Voltage = " 
                     << (voltage_alert.read() ? "HIGH" : "LOW") << endl;
                prev_volt = voltage_alert.read();
            }
            
            if(power_alert.read() != prev_pwr) {
                cout << "[" << sc_time_stamp() << "] ALERT CHANGE: Power = " 
                     << (power_alert.read() ? "HIGH" : "LOW") << endl;
                prev_pwr = power_alert.read();
            }
            
            if(temp_alert.read() != prev_temp) {
                cout << "[" << sc_time_stamp() << "] ALERT CHANGE: Temperature = " 
                     << (temp_alert.read() ? "HIGH" : "LOW") << endl;
                prev_temp = temp_alert.read();
            }
        }
    }
    
    ~testbench() {
        delete dut;
    }
};

int sc_main(int argc, char* argv[]) {
    testbench tb("tb");
    
    // VCD trace
    sc_trace_file *wf = sc_create_vcd_trace_file("power_monitor");
    wf->set_time_unit(1, SC_NS);
    
    sc_trace(wf, tb.clk, "clk");
    sc_trace(wf, tb.rst_n, "rst_n");
    sc_trace(wf, tb.mem_voltage, "mem_voltage");
    sc_trace(wf, tb.mem_current, "mem_current");
    sc_trace(wf, tb.io_voltage, "io_voltage");
    sc_trace(wf, tb.io_current, "io_current");
    sc_trace(wf, tb.temp_memory, "temp_memory");
    sc_trace(wf, tb.temp_ambient, "temp_ambient");
    sc_trace(wf, tb.voltage_alert, "voltage_alert");
    sc_trace(wf, tb.power_alert, "power_alert");
    sc_trace(wf, tb.temp_alert, "temp_alert");
    sc_trace(wf, tb.mem_clk_out, "mem_clk_out");
    sc_trace(wf, tb.io_clk_out, "io_clk_out");
    sc_trace(wf, tb.paddr, "paddr");
    sc_trace(wf, tb.pwrite, "pwrite");
    sc_trace(wf, tb.pwdata, "pwdata");
    sc_trace(wf, tb.prdata, "prdata");
    
    sc_start();
    
    sc_close_vcd_trace_file(wf);
    
    return 0;
}
