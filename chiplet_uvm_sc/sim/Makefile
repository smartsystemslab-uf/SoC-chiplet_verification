# Simple wrappers for Questa or VCS; assumes you've built ../build/libdut_sc.so
SV_SRC=../sv/uvm_top.sv \
       ../sv/pkg/lane_txn.sv ../sv/pkg/seq_lib.sv \
       ../sv/pkg/driver_proxy.sv ../sv/pkg/monitor.sv \
       ../sv/pkg/env.sv ../sv/pkg/test.sv

QUESTA?=$(QUESTA_HOME)
VCS?=$(VCS_HOME)
UVMC?=$(UVMC_HOME)

.PHONY: all questa vcs clean
all: questa

questa:
	vlib work
	vlog -sv +acc=rn +incdir+$(UVMC)/src/connect +incdir+../sv/pkg $(SV_SRC)
	vsim -c -sv_lib ../build/libdut_sc -do "run -all; quit" uvm_top +UVM_TESTNAME=chiplet_smoke_test

vcs:
	vcs -full64 -sverilog +v2k +lint=all,noVCDE +incdir+$(UVMC)/src/connect +incdir+../sv/pkg $(SV_SRC) \
	    -load ../build/libdut_sc:sc_elab \
	    -o simv
	./simv +UVM_TESTNAME=chiplet_smoke_test

clean:
	rm -rf work transcript vsim.wlf simv simv.daidir csrc ucli.key
