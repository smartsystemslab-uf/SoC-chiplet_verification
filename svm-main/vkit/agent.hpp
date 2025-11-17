// vkit/agent.hpp
#pragma once
#include "base.hpp"
#include <tlm>
#include <optional>
namespace vkit {
struct sequence_item { virtual ~sequence_item() = default; };

struct driver_if {
  virtual ~driver_if() = default;
  virtual void drive(const sequence_item& it) = 0;
};

struct monitor_if {
  virtual ~monitor_if() = default;
  virtual void start() = 0; // fork a SC_THREAD or poll
};

struct scoreboard_if {
  virtual ~scoreboard_if() = default;
  virtual void push_observation(const sequence_item& it) = 0;
  virtual void finalize() = 0;
};

struct agent : component {
  using component::component;
  virtual driver_if*    driver()    = 0;
  virtual monitor_if*   monitor()   = 0; // may be null until post-verify
  virtual scoreboard_if*scoreboard()= 0; // may be null until post-verify
};
}
