#include "devices.h"
#include "mmu.h"
#include <stdexcept>

mmio_device_map_t& mmio_device_map()
{
  static mmio_device_map_t device_map;
  return device_map;
}

void bus_t::add_device(reg_t addr, abstract_device_t* dev)
{
  // Searching devices via lower_bound/upper_bound
  // implicitly relies on the underlying std::map 
  // container to sort the keys and provide ordered
  // iteration over this sort, which it does. (python's
  // SortedDict is a good analogy)
  devices[addr] = dev;
}

bool bus_t::load(reg_t addr, size_t len, uint8_t* bytes)
{
  // Find the device with the base address closest to but
  // less than addr (price-is-right search)
  auto it = devices.upper_bound(addr);
  if (devices.empty() || it == devices.begin()) {
    // Either the bus is empty, or there weren't 
    // any items with a base address <= addr
    return false;
  }
  // Found at least one item with base address <= addr
  // The iterator points to the device after this, so
  // go back by one item.
  it--;
  return it->second->load(addr - it->first, len, bytes);
}

bool bus_t::store(reg_t addr, size_t len, const uint8_t* bytes)
{
  // See comments in bus_t::load
  auto it = devices.upper_bound(addr);
  if (devices.empty() || it == devices.begin()) {
    return false;
  }
  it--;
  return it->second->store(addr - it->first, len, bytes);
}

std::pair<reg_t, abstract_device_t*> bus_t::find_device(reg_t addr)
{
  // See comments in bus_t::load
  auto it = devices.upper_bound(addr);
  if (devices.empty() || it == devices.begin()) {
    return std::make_pair((reg_t)0, (abstract_device_t*)NULL);
  }
  it--;
  return std::make_pair(it->first, it->second);
}

mem_t::mem_t(reg_t size)
  : sz(size)
{
  if (size == 0 || size % PGSIZE != 0)
    throw std::runtime_error("memory size must be a positive multiple of 4 KiB");
}

mem_t::~mem_t()
{
  for (auto& entry : sparse_memory_map)
    free(entry.second);
}

bool mem_t::load_store(reg_t addr, size_t len, uint8_t* bytes, bool store)
{
  if (addr + len < addr || addr + len > sz)
    return false;

  while (len > 0) {
    auto n = std::min(PGSIZE - (addr % PGSIZE), reg_t(len));

    if (store)
      memcpy(this->contents(addr), bytes, n);
    else
      memcpy(bytes, this->contents(addr), n);

    addr += n;
    bytes += n;
    len -= n;
  }

  return true;
}

char* mem_t::contents(reg_t addr) {
  reg_t ppn = addr >> PGSHIFT, pgoff = addr % PGSIZE;
  auto search = sparse_memory_map.find(ppn);
  if (search == sparse_memory_map.end()) {
    auto res = (char*)calloc(PGSIZE, 1);
    if (res == nullptr)
      throw std::bad_alloc();
    sparse_memory_map[ppn] = res;
    return res + pgoff;
  }
  return search->second + pgoff;
}

void mem_t::dump(std::ostream& o) {
  const char empty[PGSIZE] = {0};
  for (reg_t i = 0; i < sz; i += PGSIZE) {
    reg_t ppn = i >> PGSHIFT;
    auto search = sparse_memory_map.find(ppn);
    if (search == sparse_memory_map.end()) {
      o.write(empty, PGSIZE);
    } else {
      o.write(sparse_memory_map[ppn], PGSIZE);
    }
  }
}


sysc_mem_t::sysc_mem_t(reg_t size, simif_t* sim) 
  : sz(size), sim(sim)
{
  if (size == 0 || size % PGSIZE != 0) {
    fprintf(stderr, "Memory size is %" PRIu64 "\n", size);
    throw std::runtime_error("memory size must be a positive multiple of 4 KiB");
  }
}

sysc_mem_t::~sysc_mem_t()
{

}

const std::string sysc_mem_t::name() {
  return "sysc_mem";
}

char* sysc_mem_t::contents(reg_t addr) {
  if (!sim)
    throw std::runtime_error("sysc_mem_t::contents called before simif_t set");
  // printf("acessing DMI for addr %lx\n", addr);
  return sim->send_dmi(addr);
}

reg_t sysc_mem_t::size() {
  return sz;
}

void sysc_mem_t::dump(std::ostream& o) {
  // TODO:
}






tty_mem_t::tty_mem_t(reg_t size, simif_t* sim)
  : sz(size), sim(sim)
{
  if (size == 0 || size % PGSIZE != 0)
    throw std::runtime_error("memory size must be a positive multiple of 4 KiB");
}

tty_mem_t::~tty_mem_t()
{
  for (auto& entry : sparse_memory_map)
    free(entry.second);
}

char* tty_mem_t::contents(reg_t addr) {

  printf("Acessing TTY MEM %lx\n", addr);
  // if (addr == 0x5fffb008 || addr == 0x5fffb030) /
  // assert(0);
  
  reg_t ppn = addr >> PGSHIFT, pgoff = addr % PGSIZE;
  auto search = sparse_memory_map.find(ppn);
  if (search == sparse_memory_map.end()) {
    auto res = (char*)calloc(PGSIZE, 1);
    if (res == nullptr)
      throw std::bad_alloc();
    sparse_memory_map[ppn] = res;
    return res + pgoff;
  }
  return search->second + pgoff;
}

void tty_mem_t::dump(std::ostream& o) {
  const char empty[PGSIZE] = {0};
  for (reg_t i = 0; i < sz; i += PGSIZE) {
    reg_t ppn = i >> PGSHIFT;
    auto search = sparse_memory_map.find(ppn);
    if (search == sparse_memory_map.end()) {
      o.write(empty, PGSIZE);
    } else {
      o.write(sparse_memory_map[ppn], PGSIZE);
    }
  }
}
