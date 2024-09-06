#ifndef _RISCV_SYSC_SIM_H
#define _RISCV_SYSC_SIM_H

#include "sim.h"
#include "zmq_data.h"

#include <zmq.h>

#include <cassert>
#include <iostream>
#include <string>
using namespace std;

#include <cassert>
#include <chrono>
#include <cstddef>
#include <thread>

using namespace std::this_thread;     // sleep_for, sleep_until
using namespace std::chrono_literals; // ns, us, ms, s, h, etc.

class sysc_sim_t : public sim_t {
public:
  void *context{nullptr};
  void *socket{nullptr};
  bool dbg{false};

  sysc_sim_t(const cfg_t *cfg, bool halted,
             std::vector<std::pair<reg_t, abstract_mem_t *>> mems,
             const std::vector<std::pair<reg_t, abstract_mem_t *>> remote_mems,
             const std::vector<device_factory_sargs_t> &plugin_device_factories,
             const std::vector<std::string> &args,
             const debug_module_config_t &dm_config, const char *log_path,
             bool dtb_enabled, const char *dtb_file, bool socket_enabled,
             FILE *cmd_file)
      : sim_t(cfg, halted, mems, plugin_device_factories, args, dm_config,
              log_path, dtb_enabled, dtb_file, socket_enabled, cmd_file) {

    if (dbg) {
      std::cout << "sysc_sim_t::sysc_sim_t" << std::endl;
    }
    context = zmq_ctx_new();
    socket = zmq_socket(context, ZMQ_PAIR);
    zmq_connect(socket, "ipc:///tmp/zmq");
    
    if (dbg) {
      std::cout << "sysc_sim_t::sysc_sim_t: connected" << std::endl;
    }

    if (!socket) {
      std::cout << "zmq_socket failed" << std::endl;
    } else if (!context) {
      std::cout << "zmq_ctx_new failed" << std::endl;
    }
    for (auto& x : remote_mems)
      get_bus()->add_device(x.first, x.second);
  } // end constructor

  // void step(size_t n) override {
  //     if (dbg) std::cout << "sysc_sim_t::step" << std::endl;
  //     const size_t msg_size = sizeof(struct x280msg);
  //     x280_recv(socket, &msg_recv, msg_size); // this is blocking call,
  //     needed for clock sync (CLK message expected) if (dbg) std::cout <<
  //     "passed recv " << std::endl; sim_t::step(n); if (dbg) std::cout <<
  //     "sending END_CLK" << std::endl; x280_send_end_clk(socket, &msg_send,
  //     msg_size);

  // }
};
#endif