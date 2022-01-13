#include <isa.h>
#include <memory/paddr.h>
#include <memory/vaddr.h>

typedef struct {
  uintptr_t valid : 1;
  uintptr_t read : 1;
  uintptr_t write : 1;
  uintptr_t execute : 1;
  uintptr_t other : 6;
  uintptr_t ppn : 22;
} RISCV_PTE;

#define VA_PPN1x4(va) (((uintptr_t)va >> 22) << 2)
#define VA_PPN0x4(va) ((((uintptr_t)va & 0x3fffff) >> 12) << 2)

#define PGSIZE 4096

// #define __riscv_xlen 32
// static inline void set_satp(void *pdir) {
//   uintptr_t mode = 1ul << (__riscv_xlen - 1);
//   asm volatile("csrw satp, %0" : : "r"(mode | ((uintptr_t)pdir >> 12)));
// }

static inline uintptr_t get_satp() {
  uintptr_t satp = cpu.csr[4]._32; // satp
  return satp << 12;
}

int isa_mmu_check(vaddr_t vaddr, int len, int type) {

  uintptr_t satp = cpu.csr[4]._32; // satp
  uintptr_t mode = 1ul << (32 - 1);
  
  if (satp & mode) {
    return MMU_TRANSLATE;
  } else {
    return MMU_DIRECT;
  }
}

paddr_t isa_mmu_translate(vaddr_t vaddr, int len, int type) {

  printf("translate: 0x%x\n", vaddr);
  printf("get_satp(): 0x%lx\n", get_satp());
  printf("page_dir_item: 0x%lx\n", VA_PPN1x4(vaddr));

  RISCV_PTE *page_dir_item = (RISCV_PTE *)((uintptr_t)get_satp() + VA_PPN1x4(vaddr)); 


  assert(page_dir_item->valid);

  printf("page_dir_item: %p\n", page_dir_item);

  RISCV_PTE *page_table_item = (RISCV_PTE *)((page_dir_item->ppn << 12) + VA_PPN0x4(vaddr));

  assert(page_table_item->valid);

  paddr_t paddr = (paddr_t)((page_table_item->ppn << 12) + (vaddr & 0xfff));

  assert(paddr == vaddr);

  return paddr;
}
