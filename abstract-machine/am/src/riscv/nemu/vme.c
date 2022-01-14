#include <am.h>
#include <nemu.h>
#include <klib.h>

typedef struct {
  uintptr_t valid : 1;
  uintptr_t read : 1;
  uintptr_t write : 1;
  uintptr_t execute : 1;
  uintptr_t other : 6;
  uintptr_t ppn : 22;
} RISCV_PTE;

#define VA_PPN1x4(va) (((uintptr_t)va >> 22) << 2)
#define VA_PPN0(va) (((uintptr_t)va & 0x3fffff) >> 12)

static AddrSpace kas = {};
static void* (*pgalloc_usr)(int) = NULL;
static void (*pgfree_usr)(void*) = NULL;
static int vme_enable = 0;

static Area segments[] = {      // Kernel memory mappings
  NEMU_PADDR_SPACE
};

#define USER_SPACE RANGE(0x40000000, 0x80000000)

static inline void set_satp(void *pdir) {
  uintptr_t mode = 1ul << (__riscv_xlen - 1);
  // printf("pdir: %p\n", pdir);
  // printf("size: %d\n", sizeof(uintptr_t));
  asm volatile("csrw satp, %0" : : "r"(mode | ((uintptr_t)pdir >> 12)));
}

static inline uintptr_t get_satp() {
  uintptr_t satp;
  asm volatile("csrr %0, satp" : "=r"(satp));
  return satp << 12;
}

bool vme_init(void* (*pgalloc_f)(int), void (*pgfree_f)(void*)) {
  pgalloc_usr = pgalloc_f;
  pgfree_usr = pgfree_f;

  kas.ptr = pgalloc_f(PGSIZE);

  int i;
  for (i = 0; i < LENGTH(segments); i ++) {
    void *va = segments[i].start;
    for (; va < segments[i].end; va += PGSIZE) {
      map(&kas, va, va, 0);
    }
  }

  set_satp(kas.ptr);
  vme_enable = 1;

  return true;
}

void protect(AddrSpace *as) {
  PTE *updir = (PTE*)(pgalloc_usr(PGSIZE));
  as->ptr = updir;
  as->area = USER_SPACE;
  as->pgsize = PGSIZE;
  // map kernel space
  memcpy(updir, kas.ptr, PGSIZE);
}

void unprotect(AddrSpace *as) {
}

void __am_get_cur_as(Context *c) {
  c->pdir = (vme_enable ? (void *)get_satp() : NULL);
}

void __am_switch(Context *c) {
  if (vme_enable && c->pdir != NULL) {
    set_satp(c->pdir);
  }
}

void map(AddrSpace *as, void *va, void *pa, int prot) {
  // assert(sizeof(RISCV_PTE) == 4);

  if (va != pa) {
    printf("map: 0x%p => 0x%p\n", va, pa);
  }

  // 判断页表是否存在
  RISCV_PTE *page_dir_item = (RISCV_PTE *)((uintptr_t)as->ptr + VA_PPN1x4(va)); 

  RISCV_PTE *page_table;

  // 不存在就创建
  if (!page_dir_item->valid) {

    // 创建新页, 用于存放页表
    page_table = (RISCV_PTE *)pgalloc_usr(PGSIZE);

    // printf("index: %p\n", page_dir_item);
    // printf("page: %p\n", page_table);

    assert(((uintptr_t)page_table & 0xfff) == 0);

    // 填入页目录项
    *page_dir_item = (RISCV_PTE) { .valid = 1, .ppn = (uintptr_t)page_table >> 12, .read = 0, .write = 0, .execute = 0, .other = 0 };

    // printf("item: %p\n", *page_dir_item);

  } else {
    // 不然就直接获取
    page_table = (RISCV_PTE *)(page_dir_item->ppn << 12);
  }

  // 往插入页表内插入 PTE
  *(page_table + VA_PPN0(va)) = (RISCV_PTE) { .valid = 1, .ppn = (uintptr_t)pa >> 12, .read = 1, .write = 1, .execute = 0, .other = 0 };
}

Context *ucontext(AddrSpace *as, Area kstack, void *entry) {
  Context *c = (Context*)kstack.end - 1;

  c->pdir = (void *) as->ptr;
  c->mstatus = (uintptr_t) 0x1800;
  c->mcause = (uintptr_t) 11;
  c->mepc = (uintptr_t) entry;
  return c;
}
