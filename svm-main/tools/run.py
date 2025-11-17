import argparse, subprocess, os, sys

ap = argparse.ArgumentParser()
ap.add_argument("dsl")
args = ap.parse_args()

subprocess.check_call([sys.executable, "tools/dsl2tests.py", args.dsl])
# Build if needed
if not os.path.exists("build/sim"):
  os.makedirs("build", exist_ok=True)
  subprocess.check_call(["cmake", "-S", ".", "-B", "build", "-DCMAKE_BUILD_TYPE=Release"])
  subprocess.check_call(["cmake", "--build", "build", "-j"])
# Run
subprocess.check_call(["build/sim/uvm_lite_sim"])
