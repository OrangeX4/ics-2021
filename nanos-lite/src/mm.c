#include <memory.h>
#include <proc.h>

static void *pf = NULL;

void* new_page(size_t nr_page) {
  if (pf == NULL) pf = heap.start;
  pf += nr_page * PGSIZE;
  return pf - nr_page * PGSIZE;
}

#ifdef HAS_VME
static void* pg_alloc(int n) {
  assert(n % PGSIZE == 0);
  void *page = new_page(n / PGSIZE);
  // 清零
  // printf("before page alloc: %p\n", page);
  memset(page, 0, n);
  // printf("after page alloc: %p\n", page);
  return page;
}
#endif

void free_page(void *p) {
  panic("not implement yet");
}

/* The brk() system call handler. */
int mm_brk(uintptr_t brk) {
  printf("brk: %p\n", brk);
  if (brk > current->max_brk) {
    if ((brk & (PGSIZE - 1)) > (current->max_brk & (PGSIZE - 1))) {
      // printf("new page\n");
      void *page = new_page(1);
      map(&current->as, (void *)brk, page, MMAP_READ | MMAP_WRITE);
    }
    current->max_brk = brk;
  }
  return 0;
}

void init_mm() {
  pf = (void *)ROUNDUP(heap.start, PGSIZE);
  Log("free physical pages starting from %p", pf);
#ifdef HAS_VME
  vme_init(pg_alloc, free_page);
#endif
}
