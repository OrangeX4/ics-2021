#include <isa.h>
#include <memory/paddr.h>

word_t vaddr_ifetch(vaddr_t addr, int len) {
  if (isa_mmu_check(addr, len, 0) == MMU_TRANSLATE) {
    assert(addr != 0x400657d8);
    return paddr_read(isa_mmu_translate(addr, len, 0), len);
  } else {
    return paddr_read(addr, len);
  }
}

word_t vaddr_read(vaddr_t addr, int len) {
  if (isa_mmu_check(addr, len, 0) == MMU_TRANSLATE) {
    assert(addr != 0x400657d8);
    return paddr_read(isa_mmu_translate(addr, len, 0), len);
  } else {
    return paddr_read(addr, len);
  }
}

void vaddr_write(vaddr_t addr, int len, word_t data) {
  assert(len <= 4096);
  if (isa_mmu_check(addr, len, 0) == MMU_TRANSLATE) {
    assert(addr != 0x400657d8);
    paddr_write(isa_mmu_translate(addr, len, 0), len, data);
  } else {
    paddr_write(addr, len, data);
  }
}
