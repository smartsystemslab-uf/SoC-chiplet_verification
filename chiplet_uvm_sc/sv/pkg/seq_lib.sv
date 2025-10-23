package seq_pkg;
  import uvm_pkg::*; `include "uvm_macros.svh"
  import lane_pkg::*;

  class lane_seq extends uvm_sequence #(lane_txn);
    `uvm_object_utils(lane_seq)
    rand int unsigned n_ops = 64;
    constraint c_n { n_ops inside {[16:256]}; }
    function new(string name="lane_seq"); super.new(name); endfunction

    task body();
      lane_txn t;
      repeat (n_ops) begin
        t = lane_txn::type_id::create("t");
        assert(t.randomize() with {
          chiplet_id inside {0,1,2,3};
          write dist {1:=50, 0:=50};
          addr[1:0]==0;
        });
        start_item(t); finish_item(t);
      end
    endtask
  endclass

  class config_and_test_gen extends uvm_component;
    `uvm_component_utils(config_and_test_gen)
    uvm_sequencer #(lane_txn) seqr;
    function new(string n, uvm_component p); super.new(n,p); endfunction
    function void build_phase(uvm_phase p);
      if(!uvm_config_db#(uvm_sequencer#(lane_txn))::get(this, "", "seqr", seqr))
        `uvm_fatal("CFG", "sequencer handle not set")
    endfunction
    task run_phase(uvm_phase p);
      lane_seq s = lane_seq::type_id::create("s");
      p.raise_objection(this);
      s.start(seqr);
      p.drop_objection(this);
    endtask
  endclass
endpackage
