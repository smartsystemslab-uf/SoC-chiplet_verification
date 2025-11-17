package lane_pkg;
  import uvm_pkg::*; `include "uvm_macros.svh"
  import uvmc_pkg::*;

  class lane_txn extends uvm_sequence_item;
    rand bit [31:0] addr;
    rand bit [31:0] data;
    rand bit        write;
    rand bit [3:0]  chiplet_id;

    `uvm_object_utils_begin(lane_txn)
      `uvm_field_int(addr,      UVM_ALL_ON)
      `uvm_field_int(data,      UVM_ALL_ON)
      `uvm_field_int(write,     UVM_ALL_ON)
      `uvm_field_int(chiplet_id,UVM_ALL_ON)
    `uvm_object_utils_end

    function new(string name="lane_txn"); super.new(name); endfunction
  endclass
endpackage
