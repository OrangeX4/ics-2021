#include <am.h>
#include <riscv/riscv.h>
#include <klib.h>

static Context* (*user_handler)(Event, Context*) = NULL;
void __am_get_cur_as(Context *c);
void __am_switch(Context *c);

Context* __am_irq_handle(Context *c) {
  __am_get_cur_as(c);
  if (user_handler) {
    Event ev = {0};
    switch (c->mcause) {
      case 11: {
        if (c->GPR1 == -1) {
          ev.event = EVENT_YIELD;
        } else {
          ev.event = EVENT_SYSCALL;
        }
        break;
      }
      default: ev.event = EVENT_ERROR; break;
    }

    // 通过软件进行 +4 操作
    c->mepc += 4;

    c = user_handler(ev, c);

    assert(c != NULL);
  }

  __am_switch(c);
  // uintptr_t _sp;
  // asm volatile("mv %0, sp" : "=r"(_sp));
  // printf("sp: %x\n", _sp);
  return c;
}

extern void __am_asm_trap(void);

bool cte_init(Context*(*handler)(Event, Context*)) {
  // initialize exception entry
  asm volatile("li t0, 0x1800; csrw mstatus, t0; csrw mtvec, %0" : : "r"(__am_asm_trap) : "t0");
//   asm volatile("csrw mtvec, %0" : : "r"(__am_asm_trap));

  // register event handler
  user_handler = handler;

  return true;
}

Context *kcontext(Area kstack, void (*entry)(void *), void *arg) {
  Context *c = (Context*)kstack.end - 1;
  
  c->pdir = NULL;
  // 给 a0 即第一个参数传参
  c->GPR2 = (uintptr_t) arg;
  // 给 sp 设值
  // c->gpr[2] = (uintptr_t) kstack.end;
  c->mstatus = (uintptr_t) 0x1800;
  c->mcause = (uintptr_t) 11;
  c->mepc = (uintptr_t)entry;
  return c;
}

void yield() {
  asm volatile("li a7, -1; ecall");
}

bool ienabled() {
  return false;
}

void iset(bool enable) {
}
