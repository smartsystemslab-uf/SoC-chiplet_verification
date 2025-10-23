package drv_pkg;
  import uvm_pkg::*; `include "uvm_macros.svh"
  import lane_pkg::*; import uvmc_pkg::*;

  class lane_driver extends uvm_driver #(lane_txn);
    `uvm_component_utils(lane_driver)
    uvm_tlm_b_initiator_socket #(lane_txn) iport;
    function new(string n, uvm_component p); super.new(n,p); iport=new("iport", this); endfunction

    function void connect_phase(uvm_phase p);
      string ch;
      if(!uvm_config_db#(string)::get(this, "", "uvmc_channel", ch)) ch = "lane0";
      uvmc_tlm2#(lane_txn)::connect(iport, ch);
    endfunction

    task run_phase(uvm_phase p);
      lane_txn t; forever begin
        seq_item_port.get_next_item(t);
        iport.transport(t);
        seq_item_port.item_done();
      end
    endtask
  endclass
endpackage
