`timescale 1ns/1ps
import uvm_pkg::*; `include "uvm_macros.svh"
import uvmc_pkg::*;

import lane_pkg::*;
import seq_pkg::*;
import drv_pkg::*;
import mon_pkg::*;
import env_pkg::*;
import test_pkg::*;

module uvm_top;
  initial begin
    run_test(); // +UVM_TESTNAME=chiplet_smoke_test
  end
endmodule
