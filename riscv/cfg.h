// See LICENSE for license details.
#ifndef _RISCV_CFG_H
#define _RISCV_CFG_H

#include <optional>
#include <vector>
#include "decode.h"
#include <cassert>
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
