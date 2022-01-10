#include <proc.h>

#define MAX_NR_PROC 4

void naive_uload(PCB *pcb, const char *filename);
uintptr_t pcb_uload(PCB *pcb, const char *filename);

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;

void switch_boot_pcb() {
  current = &pcb_boot;
}

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    Log("Hello World from Nanos-lite with string '%s' for the %dth time!", (uintptr_t)arg, j);
    j ++;
    yield();
  }
}

void context_kload(PCB* pcb, void (*entry)(void *), void *arg) {
  Area kstack = { (void *) pcb, (void *) pcb + sizeof(PCB) };
  pcb->cp = kcontext(kstack, entry, arg);
}

void context_uload(PCB* pcb, const char *filename) {
  Area kstack = { (void *) pcb, (void *) pcb + sizeof(PCB) };
  Context *c = (Context*)kstack.end - 1;
  c->GPRx = (uintptr_t)heap.end;
  printf("addr: %p\n", c->GPRx);
  pcb->cp = ucontext(&pcb->as, kstack, (void *) pcb_uload(pcb, filename));
}

void init_proc() {

  Log("Initializing processes...");

  context_uload(&pcb[0], "/bin/hello");
  // context_kload(&pcb[0], hello_fun, "&pcb[0]");
  // context_kload(&pcb[1], hello_fun, "&pcb[1]");
  // context_uload(&pcb[1], "/bin/pal");
  // context_uload(&pcb[1], "/bin/hello");
  switch_boot_pcb();

  // load program here
  // naive_uload(NULL, ENTRY_PROGRAM);
}

Context* schedule(Context *prev) {
  // save the context pointer
  current->cp = prev;

  current = &pcb[0];
  // current = (current == &pcb[0] ? &pcb[1] : &pcb[0]);

  // then return the new context
  return current->cp;
}
