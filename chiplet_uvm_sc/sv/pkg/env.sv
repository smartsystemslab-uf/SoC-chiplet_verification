package env_pkg;
  import uvm_pkg::*; `include "uvm_macros.svh"
  import lane_pkg::*; import seq_pkg::*; import drv_pkg::*; import mon_pkg::*;

  class chiplet_env extends uvm_env;
    `uvm_component_utils(chiplet_env)
    uvm_sequencer #(lane_txn) seqr;
    drv_pkg::lane_driver      drv;
    mon_pkg::lane_monitor     mon;
    uvm_analysis_export #(lane_txn) analysis_export;

    function new(string n, uvm_component p); super.new(n,p); analysis_export=new("analysis_export", this); endfunction

    function void build_phase(uvm_phase p);
      seqr = uvm_sequencer#(lane_txn)::type_id::create("seqr", this);
      drv  = drv_pkg::lane_driver     ::type_id::create("drv",  this);
      mon  = mon_pkg::lane_monitor    ::type_id::create("mon",  this);
      uvm_config_db#(uvm_sequencer#(lane_txn))::set(this, "*.config", "seqr", seqr);
    endfunction

    function void connect_phase(uvm_phase p);
      drv.seq_item_port.connect(seqr.seq_item_export);
      mon.ap.connect(analysis_export);
    endfunction
  endclass

  class scoreboard extends uvm_component;
    `uvm_component_utils(scoreboard)
    uvm_analysis_imp #(lane_txn, scoreboard) imp;
    int unsigned wr_cnt, rd_cnt;
    function new(string n, uvm_component p); super.new(n,p); imp=new("imp", this); endfunction
    function void write(lane_txn t);
      if(t.write) wr_cnt++; else rd_cnt++;
      `uvm_info("SCB", $sformatf("saw txn to chiplet%0d addr=%08h data=%08h write=%0b",
                 t.chiplet_id, t.addr, t.data, t.write), UVM_MEDIUM)
    endfunction
    function void report_phase(uvm_phase p);
      `uvm_info("SCB", $sformatf("WR=%0d RD=%0d", wr_cnt, rd_cnt), UVM_LOW)
    endfunction
  endclass
endpackage
