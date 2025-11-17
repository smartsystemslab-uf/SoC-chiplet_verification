package test_pkg;
  import uvm_pkg::*; `include "uvm_macros.svh"
  import env_pkg::*; import seq_pkg::*;

  class chiplet_base_test extends uvm_test;
    `uvm_component_utils(chiplet_base_test)
    chiplet_env    env;
    scoreboard     scb;
    function new(string n, uvm_component p); super.new(n,p); endfunction

    function void build_phase(uvm_phase p);
      env = chiplet_env::type_id::create("env", this);
      scb = scoreboard ::type_id::create("scb", this);
      env.analysis_export.connect(scb.imp);
      uvm_config_db#(string)::set(this, "env.drv", "uvmc_channel", "lane0");
      uvm_config_db#(string)::set(this, "env.mon", "uvmc_mon_channel", "mon");
    endfunction

    task run_phase(uvm_phase p);
      p.raise_objection(this);
      seq_pkg::config_and_test_gen gen = new("config", env);
      gen.starting_phase = p;
      #1us;
      p.drop_objection(this);
    endtask
  endclass

  class chiplet_smoke_test extends chiplet_base_test;
    `uvm_component_utils(chiplet_smoke_test)
  endclass
endpackage
