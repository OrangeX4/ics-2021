#include <isa.h>
#include <memory/paddr.h>
#include <memory/vaddr.h>

// typedef struct {
//   word_t valid : 1;
//   word_t read : 1;
//   word_t write : 1;
//   word_t execute : 1;
//   word_t other : 6;
//   word_t ppn : 22;
// } RISCV_PTE;

#define VA_PPN1x4(va) (((word_t)va >> 22) << 2)
#define VA_PPN0x4(va) ((((word_t)va & 0x3fffff) >> 12) << 2)
#define VALID(pte) (pte & 1)
#define PPN(pte) (pte >> 10)

#define PGSIZE 4096

// #define __riscv_xlen 32
// static inline void set_satp(void *pdir) {
//   word_t mode = 1ul << (__riscv_xlen - 1);
//   asm volatile("csrw satp, %0" : : "r"(mode | ((word_t)pdir >> 12)));
// }

static inline word_t get_satp() {
  word_t satp = cpu.csr[4]._32; // satp
  return satp << 12;
}

int isa_mmu_check(vaddr_t vaddr, int len, int type) {

  word_t satp = cpu.csr[4]._32; // satp
  word_t mode = 1ul << (32 - 1);
  
  if (satp & mode) {
    return MMU_TRANSLATE;
  } else {
    return MMU_DIRECT;
  }
}

paddr_t isa_mmu_translate(vaddr_t vaddr, int len, int type) {

  printf("vaddr: 0x%x\n", vaddr);
  printf("get_satp(): 0x%x\n", get_satp());

  printf("page_dir_index: 0x%x\n", (word_t)get_satp() + VA_PPN1x4(vaddr));

  word_t page_dir_item = paddr_read((word_t)get_satp() + VA_PPN1x4(vaddr), 4); 

  printf("page_dir_item: 0x%x\n", page_dir_item);

  assert(VALID(page_dir_item));

  printf("page_table_index: 0x%x\n", (PPN(page_dir_item) << 12) + VA_PPN0x4(vaddr));

  word_t page_table_item = paddr_read((PPN(page_dir_item) << 12) + VA_PPN0x4(vaddr), 4);

  printf("page_table_item: 0x%x\n", page_table_item);

  assert(VALID(page_table_item));

  printf("paddr_index: 0x%x\n", (PPN(page_table_item) << 12) + (vaddr & 0xfff));

  paddr_t paddr = (PPN(page_table_item) << 12) + (vaddr & 0xfff);

  printf("paddr: 0x%x\n", paddr);
  
  assert(paddr == vaddr);

  return paddr;
}
