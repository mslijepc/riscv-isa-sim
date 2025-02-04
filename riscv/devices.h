#ifndef _RISCV_DEVICES_H
#define _RISCV_DEVICES_H

#include "decode.h"
#include "abstract_device.h"
#include "abstract_interrupt_controller.h"
#include "platform.h"
#include <map>
#include <queue>
#include <vector>
#include <utility>
#include <cassert>

class processor_t;
class simif_t;

class bus_t : public abstract_device_t {
 public:
  bool load(reg_t addr, size_t len, uint8_t* bytes) override;
  bool store(reg_t addr, size_t len, const uint8_t* bytes) override;
  void add_device(reg_t addr, abstract_device_t* dev);

  std::pair<reg_t, abstract_device_t*> find_device(reg_t addr);

 private:
  std::map<reg_t, abstract_device_t*> devices;
};

class rom_device_t : public abstract_device_t {
 public:
  rom_device_t(std::vector<char> data);
  bool load(reg_t addr, size_t len, uint8_t* bytes) override;
  bool store(reg_t addr, size_t len, const uint8_t* bytes) override;
  const std::vector<char>& contents() { return data; }
 private:
  std::vector<char> data;
};

class abstract_mem_t : public abstract_device_t {
 public:
  virtual ~abstract_mem_t() = default;

  virtual char* contents(reg_t addr) = 0;
  virtual reg_t size() = 0;
  virtual void dump(std::ostream& o) = 0;
};

class mem_t : public abstract_mem_t {
 public:
  mem_t(reg_t size);
  mem_t(const mem_t& that) = delete;
  ~mem_t() override;

  bool load(reg_t addr, size_t len, uint8_t* bytes) override { return load_store(addr, len, bytes, false); }
  bool store(reg_t addr, size_t len, const uint8_t* bytes) override { return load_store(addr, len, const_cast<uint8_t*>(bytes), true); }
  char* contents(reg_t addr) override;
  reg_t size() override { return sz; }
  void dump(std::ostream& o) override;

 private:
  bool load_store(reg_t addr, size_t len, uint8_t* bytes, bool store);

  std::map<reg_t, char*> sparse_memory_map;
  reg_t sz;
};



class tty_mem_t : public abstract_mem_t {
 public:
  tty_mem_t(reg_t size, simif_t* sim = nullptr);
  tty_mem_t(const tty_mem_t& that) = delete;
  ~tty_mem_t() override;

  bool load(reg_t addr, size_t len, uint8_t* bytes) override { return false;}
  bool store(reg_t addr, size_t len, const uint8_t* bytes) override { return false; }
  char* contents(reg_t addr) override;
  reg_t size() override { return sz; }
  void dump(std::ostream& o) override;

 private:
  bool load_store(reg_t addr, size_t len, uint8_t* bytes, bool store);

  simif_t* sim = nullptr;

  std::map<reg_t, char*> sparse_memory_map;
  reg_t sz;
};

class clint_t : public abstract_device_t {
 public:
  clint_t(const simif_t*, uint64_t freq_hz, bool real_time);
  bool load(reg_t addr, size_t len, uint8_t* bytes) override;
  bool store(reg_t addr, size_t len, const uint8_t* bytes) override;
  size_t size() { return CLINT_SIZE; }
  void tick(reg_t rtc_ticks) override;
  uint64_t get_mtimecmp(reg_t hartid) { return mtimecmp[hartid]; }
  uint64_t get_mtime() { return mtime; }
 private:
  typedef uint64_t mtime_t;
  typedef uint64_t mtimecmp_t;
  typedef uint32_t msip_t;
  const simif_t* sim;
  uint64_t freq_hz;
  bool real_time;
  uint64_t real_time_ref_secs;
  uint64_t real_time_ref_usecs;
  mtime_t mtime;
  std::map<size_t, mtimecmp_t> mtimecmp;
};

#define PLIC_MAX_DEVICES 1024

struct plic_context_t {
  plic_context_t(processor_t* proc, bool mmode)
    : proc(proc), mmode(mmode)
  {}

  processor_t *proc;
  bool mmode;

  uint8_t priority_threshold {};
  uint32_t enable[PLIC_MAX_DEVICES/32] {};
  uint32_t pending[PLIC_MAX_DEVICES/32] {};
  uint8_t pending_priority[PLIC_MAX_DEVICES] {};
  uint32_t claimed[PLIC_MAX_DEVICES/32] {};
};

class plic_t : public abstract_device_t, public abstract_interrupt_controller_t {
 public:
  plic_t(const simif_t*, uint32_t ndev);
  bool load(reg_t addr, size_t len, uint8_t* bytes) override;
  bool store(reg_t addr, size_t len, const uint8_t* bytes) override;
  void set_interrupt_level(uint32_t id, int lvl) override;
  size_t size() { return PLIC_SIZE; }
 private:
  std::vector<plic_context_t> contexts;
  uint32_t num_ids;
  uint32_t num_ids_word;
  uint32_t max_prio;
  uint8_t priority[PLIC_MAX_DEVICES];
  uint32_t level[PLIC_MAX_DEVICES/32];
  uint32_t context_best_pending(const plic_context_t *c);
  void context_update(const plic_context_t *context);
  uint32_t context_claim(plic_context_t *c);
  bool priority_read(reg_t offset, uint32_t *val);
  bool priority_write(reg_t offset, uint32_t val);
  bool pending_read(reg_t offset, uint32_t *val);
  bool context_enable_read(const plic_context_t *context,
                           reg_t offset, uint32_t *val);
  bool context_enable_write(plic_context_t *context,
                            reg_t offset, uint32_t val);
  bool context_read(plic_context_t *context,
                    reg_t offset, uint32_t *val);
  bool context_write(plic_context_t *context,
                     reg_t offset, uint32_t val);
};

// class aplic_t: public abstract_device_t {
//  public:
//   aplic_t(std::vector<processor_t*>& procs, AplicConfig& config);
//   bool load(reg_t addr, size_t len, uint8_t* bytes);
//   bool store(reg_t addr, size_t len, const uint8_t* bytes);
//   void set_bus_ref(bus_t& bus) {bus_p = &bus;}

//  private:
//   void aplic_domain_init(AplicConfig& config);
//   bool gen_msi_read (uint32_t domain, uint32_t *val);
//   bool gen_msi_write(uint32_t domain, uint32_t val);
//   void aplic_imsic_write(uint32_t hart_id, uint32_t intr, uint32_t di);
//   void aplic_update(uint32_t domain);
//   void array_update(uint32_t *array, enum APLIC_ARRAY_OP op, uint32_t intr);
//   void sourcecfg_update(uint32_t di, uint8_t sm, uint32_t intr);
//   bool read_idc(reg_t idc_addr, uint32_t di, uint32_t &val);
//   bool write_idc(reg_t idc_addr, uint32_t di, uint32_t val);
//   void clear_external_interrupt(AplicDomainCntx& d, uint32_t hart_id);

//   std::vector<processor_t*>& procs;
//   uint32_t max_procs;
//   aplic_context_t context;
//   bus_t* bus_p;
// };

// class imsic_t : public abstract_device_t {
//  public:
//   imsic_t(std::vector<processor_t*>& procs, uint32_t mode);
//   virtual bool load(reg_t addr, size_t len, uint8_t* bytes) override;
//   reg_t read_imsic_reg(uint32_t fi, uint32_t reg_num);
//   void write_imsic_reg(uint32_t fi, uint32_t reg_num, reg_t data);
//   void imsic_clear_intr(uint32_t index, uint32_t intr_num);
//  protected:
//   typedef std::map<uint32_t, reg_t> INTR_FILE;
//   void imsic_reg_init(INTR_FILE& f);
//   void imsic_update(uint32_t index);
//   void get_reg_num_and_pos(uint32_t xlen, uint32_t intr_num, uint32_t *num, uint32_t *pos);

//   std::vector<processor_t*>& procs;
//   uint32_t max_procs;
//   INTR_FILE* intr_f;
//   uint32_t mode;
// };

// class imsic_m_t : public imsic_t {
//  public:
//   imsic_m_t(std::vector<processor_t*>& procs);
//   virtual bool store(reg_t addr, size_t len, const uint8_t* bytes) override;
// };

// class imsic_s_t : public imsic_t {
//  public:
//   imsic_s_t(std::vector<processor_t*>& procs);
//   virtual bool store(reg_t addr, size_t len, const uint8_t* bytes) override;
// };

class ns16550_t : public abstract_device_t {
 public:
  ns16550_t(abstract_interrupt_controller_t *intctrl,
            uint32_t interrupt_id, uint32_t reg_shift, uint32_t reg_io_width);
  bool load(reg_t addr, size_t len, uint8_t* bytes) override;
  bool store(reg_t addr, size_t len, const uint8_t* bytes) override;
  void tick(reg_t rtc_ticks) override;
  size_t size() { return NS16550_SIZE; }
 private:
  abstract_interrupt_controller_t *intctrl;
  uint32_t interrupt_id;
  uint32_t reg_shift;
  uint32_t reg_io_width;
  std::queue<uint8_t> rx_queue;
  uint8_t dll;
  uint8_t dlm;
  uint8_t iir;
  uint8_t ier;
  uint8_t fcr;
  uint8_t lcr;
  uint8_t mcr;
  uint8_t lsr;
  uint8_t msr;
  uint8_t scr;
  void update_interrupt(void);
  uint8_t rx_byte(void);
  void tx_byte(uint8_t val);

  int backoff_counter;
  static const int MAX_BACKOFF = 16;
};

// class sfspi_t : public abstract_device_t {
//  public:
//   sfspi_t(reg_t size);
//   bool load(reg_t addr, size_t len, uint8_t* bytes);
//   bool store(reg_t addr, size_t len, const uint8_t* bytes);
//  private:
//   std::vector<uint8_t> mem;
//   reg_t base_addr;
//   reg_t size;
// };

// class dummy_device_t : public abstract_device_t {
//  public:
//   dummy_device_t(reg_t size);
//   bool load(reg_t addr, size_t len, uint8_t* bytes);
//   bool store(reg_t addr, size_t len, const uint8_t* bytes);
//  private:
//   std::vector<uint8_t> mem;
//   reg_t base_addr;
//   reg_t size;
// };

template<typename T>
void write_little_endian_reg(T* word, reg_t addr, size_t len, const uint8_t* bytes)
{
  assert(len <= sizeof(T));

  for (size_t i = 0; i < len; i++) {
    const int shift = 8 * ((addr + i) % sizeof(T));
    *word = (*word & ~(T(0xFF) << shift)) | (T(bytes[i]) << shift);
  }
}

template<typename T>
void read_little_endian_reg(T word, reg_t addr, size_t len, uint8_t* bytes)
{
  assert(len <= sizeof(T));

  for (size_t i = 0; i < len; i++) {
    const int shift = 8 * ((addr + i) % sizeof(T));
    bytes[i] = word >> shift;
  }
}

#endif
