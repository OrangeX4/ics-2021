#include <common.h>

void do_syscall(Context *c);

static Context* do_event(Event e, Context* c) {
  switch (e.event) {
    case EVENT_YIELD: printf("EVENT_YIELD, GPR1: %d\n", c->GPR1); break;
    case EVENT_SYSCALL: {
        do_syscall(c);
        break;
    }
    default: panic("Unhandled event ID = %d, GPR1: %d\n", e.event, c->GPR1);
  }

  return c;
}


void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  cte_init(do_event);
}
