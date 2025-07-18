#include "rocc.h"
#include "mmu.h"
#include <cstring>
#include <iostream>

class xrockettest_t : public rocc_t
{
 public:
  const char* name() const override { return "rockettest123"; }

  reg_t custom0(processor_t *p, rocc_insn_t insn, reg_t xs1, reg_t xs2) override
  {
    std::cout << "xrockettest custom0 called!" << std::endl;
    switch (insn.funct)
    {
      case 0: // ADD operation
        return xs1 + xs2;
      default:
        illegal_instruction(*p);
    }
    return 0;
  }

  xrockettest_t() {
    std::cout << "xrockettest_t constructor called!" << std::endl;
  }
};

REGISTER_EXTENSION(rockettest, []() { 
  std::cout << "rockettest factory called!" << std::endl;
  return new xrockettest_t; 
})
