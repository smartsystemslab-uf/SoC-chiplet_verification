# tools/dsl2tests.py
import argparse, json, os, sys, pathlib, random, yaml

SCHEMA = {
  "uart": {
    "defaults": {"baud": 115200, "parity": "none"}
  },
  "spi": {
    "defaults": {"mode": 0}
  },
  "axi_dma": {
    "defaults": {}
  },
  "timer": {
    "defaults": {"period_us": 10, "start": True}
  }
}

IP_TYPE_BY_PREFIX = {
  "uart": "uart",
  "spi": "spi",
  "dma": "axi_dma",
  "timer": "timer"
}

def ip_type(ip_name:str)->str:
  for p,t in IP_TYPE_BY_PREFIX.items():
    if ip_name.startswith(p):
      return t
  raise SystemExit(f"Cannot infer IP type for {ip_name}")


def gen_uart(name, spec):
  send = spec.get("send", [])
  baud = spec.get("baud", SCHEMA["uart"]["defaults"]["baud"])
  parity = spec.get("parity", SCHEMA["uart"]["defaults"]["parity"])
  vectors = [{"op":"tx", "baud":baud, "parity":parity, "payload":list(s.encode())} for s in send]
  return {"ip":name, "type":"uart", "vectors":vectors}


def gen_spi(name, spec):
  mode = spec.get("mode", 0)
  transfers = spec.get("transfers", [])
  vectors = [{"op":"xfer", "mode":mode, "tx": t.get("tx", [])} for t in transfers]
  return {"ip":name, "type":"spi", "vectors":vectors}


def gen_dma(name, spec):
  bursts = spec.get("bursts", [])
  vectors = [{"op":"burst", "len":b["len"], "src":b["src"], "dst":b["dst"]} for b in bursts]
  return {"ip":name, "type":"axi_dma", "vectors":vectors}


def gen_timer(name, spec):
  start = spec.get("start", True)
  period = spec.get("period_us", 10)
  vectors = [{"op":"start" if start else "stop", "period_us": period}]
  return {"ip":name, "type":"timer", "vectors":vectors}

GEN_BY_TYPE = {"uart":gen_uart, "spi":gen_spi, "axi_dma":gen_dma, "timer":gen_timer}


def main():
  ap = argparse.ArgumentParser()
  ap.add_argument("dsl", help="DSL YAML file")
  ap.add_argument("--out", default="tests/generated", help="Output dir")
  args = ap.parse_args()

  with open(args.dsl) as f:
    d = yaml.safe_load(f)

  os.makedirs(args.out, exist_ok=True)
  ips = d.get("ips", {})
  for ip_name, spec in ips.items():
    t = ip_type(ip_name)
    gen = GEN_BY_TYPE[t]
    out = gen(ip_name, spec)
    ip_dir = pathlib.Path(args.out) / ip_name
    ip_dir.mkdir(parents=True, exist_ok=True)
    with open(ip_dir/"suite.json", "w") as f:
      json.dump(out, f, indent=2)

  print(f"Generated tests for: {', '.join(ips.keys())}")

if __name__ == "__main__":
  main()
