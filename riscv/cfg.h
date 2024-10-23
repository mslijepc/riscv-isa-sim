// See LICENSE for license details.
#ifndef _RISCV_CFG_H
#define _RISCV_CFG_H

#include <optional>
#include <vector>
#include "decode.h"
#include <cassert>
#include <string>
#include "platform.h"

typedef enum {
  endianness_little,
  endianness_big
} endianness_t;

template <typename T>
class cfg_arg_t {
public:
  cfg_arg_t(T default_val)
    : value(default_val), was_set(false) {}

  bool overridden() const { return was_set; }

  T operator()() const { return value; }

  T operator=(const T v) {
    value = v;
    was_set = true;
    return value;
  }

private:
  T value;
  bool was_set;
};

// Configuration that describes a memory region
class mem_cfg_t
{
  // mslijepc workaround
  #define PGSHIFT_MEM 12
  static constexpr reg_t PGSIZE_MEM = 1 << PGSHIFT_MEM;
  static constexpr reg_t PGMASK_MEM = ~(PGSIZE_MEM-1);
  // end workaround

public:
  static bool check_if_supported(reg_t base, reg_t size) {
  return (size % PGSIZE_MEM == 0) &&
         (base % PGSIZE_MEM == 0) &&
         (size_t(size) == size) &&
         (size > 0) &&
         ((base + size > base) || (base + size == 0));
  }

  mem_cfg_t(reg_t base, reg_t size) : base(base), size(size) {
      assert(mem_cfg_t::check_if_supported(base, size));
  }

  reg_t get_base() const {
    return base;
  }

  reg_t get_size() const {
    return size;
  }

  reg_t get_inclusive_end() const {
    return base + size - 1;
  }

private:
  reg_t base;
  reg_t size;
};

class cfg_t
{
  // mslijepc workaround
  #define PMP_SHIFT_CFG 2
  const char* s76_isa;
  // end workaround
public:
  cfg_t() {
    // The default system configuration
    initrd_bounds    = std::make_pair((reg_t)0, (reg_t)0);
    bootargs         = nullptr;
    isa              = "rv64imafdc_zicntr_zihpm";
    priv             = "MSU";
    misaligned       = false;
    endianness       = endianness_little;
    pmpregions       = 16;
    pmpgranularity   = (1 << PMP_SHIFT_CFG);
    mem_layout       = std::vector<mem_cfg_t>({mem_cfg_t(reg_t(DRAM_BASE), (size_t)2048 << 20)});
    hartids          = std::vector<size_t>({0});
    explicit_hartids = false;
    real_time_clint  = false;
    trigger_count    = 4;

    // s76 workaround
    s76_isa = "rv64imafc_zicsr_zifencei_zihintpause_zihpm_zba_zbb_zcb_zcmp_zcmt_zkt_sscofpmf";
    // end workaround
  }

  void set_s76_isa() {
    isa = s76_isa;
  }

  void set_s76_mem_layout() {
    mem_layout = std::vector<mem_cfg_t>({mem_cfg_t(reg_t(DRAM_BASE), (size_t)2048 << 20)});
    mem_layout.push_back(mem_cfg_t(reg_t(0x0), size_t(0x1000)));
    mem_layout.push_back(mem_cfg_t(reg_t(0x1700000), size_t(0x1000)));
    mem_layout.push_back(mem_cfg_t(reg_t(0x2000000), size_t(0x10000)));
    mem_layout.push_back(mem_cfg_t(reg_t(0x2010000), size_t(0x8000)));
    mem_layout.push_back(mem_cfg_t(reg_t(0x2018000), size_t(0x8000)));
    mem_layout.push_back(mem_cfg_t(reg_t(0x2030000), size_t(0x1000)));
    mem_layout.push_back(mem_cfg_t(reg_t(0x2060000), size_t(0x1000)));
    mem_layout.push_back(mem_cfg_t(reg_t(0x2061000), size_t(0x1000)));
    mem_layout.push_back(mem_cfg_t(reg_t(0x3100000), size_t(0x1000)));
    mem_layout.push_back(mem_cfg_t(reg_t(0x3101000), size_t(0x1000)));
    mem_layout.push_back(mem_cfg_t(reg_t(0x4000000), size_t(0x1000)));
    mem_layout.push_back(mem_cfg_t(reg_t(0x4010000), size_t(0x1000)));
    mem_layout.push_back(mem_cfg_t(reg_t(0x4011000), size_t(0x1000)));
    mem_layout.push_back(mem_cfg_t(reg_t(0x4025000), size_t(0x1000)));
    mem_layout.push_back(mem_cfg_t(reg_t(0x4030000), size_t(0x1000)));
    mem_layout.push_back(mem_cfg_t(reg_t(0x4048000), size_t(0x1000)));
    mem_layout.push_back(mem_cfg_t(reg_t(0x4049000), size_t(0x1000)));
    mem_layout.push_back(mem_cfg_t(reg_t(0x4050000), size_t(0x1000)));
    mem_layout.push_back(mem_cfg_t(reg_t(0x4070000), size_t(0x1000)));
    mem_layout.push_back(mem_cfg_t(reg_t(0x4072000), size_t(0x1000)));
    mem_layout.push_back(mem_cfg_t(reg_t(0x4073000), size_t(0x1000)));
    mem_layout.push_back(mem_cfg_t(reg_t(0x4078000), size_t(0x1000)));
    mem_layout.push_back(mem_cfg_t(reg_t(0x407a000), size_t(0x1000)));
    mem_layout.push_back(mem_cfg_t(reg_t(0x4100000), size_t(0x1000)));
    mem_layout.push_back(mem_cfg_t(reg_t(0x4101000), size_t(0x1000)));
    mem_layout.push_back(mem_cfg_t(reg_t(0xc000000), size_t(0x10000)));
    mem_layout.push_back(mem_cfg_t(reg_t(0xd000000), size_t(0x1000)));
    mem_layout.push_back(mem_cfg_t(reg_t(0xd100000), size_t(0x8000)));
    mem_layout.push_back(mem_cfg_t(reg_t(0x10000000), size_t(0x1000)));
    mem_layout.push_back(mem_cfg_t(reg_t(0x10180000), size_t(0x1000)));
    mem_layout.push_back(mem_cfg_t(reg_t(0x17000000), size_t(0x1000)));
    mem_layout.push_back(mem_cfg_t(reg_t(0x17004000), size_t(0x4000)));
    mem_layout.push_back(mem_cfg_t(reg_t(0x20000000), size_t(0x1fff5000)));
    mem_layout.push_back(mem_cfg_t(reg_t(0x3fff5000), size_t(0x1000)));
    mem_layout.push_back(mem_cfg_t(reg_t(0x3fff6000), size_t(0x1000)));
    mem_layout.push_back(mem_cfg_t(reg_t(0x3fff7000), size_t(0x1000)));
    mem_layout.push_back(mem_cfg_t(reg_t(0x3fff8000), size_t(0x1000)));
    mem_layout.push_back(mem_cfg_t(reg_t(0x3fffa000), size_t(0x1000)));
    mem_layout.push_back(mem_cfg_t(reg_t(0x3fffb000), size_t(0x1000)));
    mem_layout.push_back(mem_cfg_t(reg_t(0x3fffc000), size_t(0x1000)));
    mem_layout.push_back(mem_cfg_t(reg_t(0x3fffd000), size_t(0x1000)));
    mem_layout.push_back(mem_cfg_t(reg_t(0x3fffe000), size_t(0x1000)));
    mem_layout.push_back(mem_cfg_t(reg_t(0x3ffff000), size_t(0x1000)));
    mem_layout.push_back(mem_cfg_t(reg_t(0x40000000), size_t(0x20000000)));
    mem_layout.push_back(mem_cfg_t(reg_t(0x7f000000), size_t(0x1000000000)));
    mem_layout.push_back(mem_cfg_t(reg_t(0x4f00000), size_t(0x1000)));
  }

  std::pair<reg_t, reg_t> initrd_bounds;
  const char *            bootargs;
  const char *            isa;
  const char *            priv;
  bool                    misaligned;
  endianness_t            endianness;
  reg_t                   pmpregions;
  reg_t                   pmpgranularity;
  std::vector<mem_cfg_t>  mem_layout;
  std::optional<reg_t>    start_pc;
  std::vector<size_t>     hartids;
  bool                    explicit_hartids;
  bool                    real_time_clint;
  reg_t                   trigger_count;

  size_t nprocs() const { return hartids.size(); }
  size_t max_hartid() const { return hartids.back(); }
};

#endif
