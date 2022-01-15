#include <isa.h>

void dev_raise_intr() {
  printf("dev_raise_intr: %x\n", cpu.gpr[0]._32);
  cpu.INTR = true;
}
