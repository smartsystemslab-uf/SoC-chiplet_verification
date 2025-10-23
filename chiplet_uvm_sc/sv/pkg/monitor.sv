package mon_pkg;
  import uvm_pkg::*; `include "uvm_macros.svh"
  import lane_pkg::*; import uvmc_pkg::*;

  class lane_monitor extends uvm_component;
    `uvm_component_utils(lane_monitor)
    uvm_analysis_port #(lane_txn) ap;
    uvm_tlm_b_target_socket #(lane_txn) tport;

    function new(string n, uvm_component p); super.new(n,p); ap=new("ap",this); tport=new("tport",this); endfunction
    function void connect_phase(uvm_phase p);
      string ch;
      if(!uvm_config_db#(string)::get(this, "", "uvmc_mon_channel", ch)) ch="mon";
      uvmc_tlm2#(lane_txn)::connect(tport, ch);
    endfunction

    virtual function void b_transport(ref lane_txn t, uvm_tlm_time delay);
      ap.write(t);
    endfunction
  endclass
endpackage
