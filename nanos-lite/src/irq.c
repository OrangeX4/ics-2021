#include <common.h>

static Context* do_event(Event e, Context* c) {
  switch (e.event) {
    case EVENT_YIELD: printf("EVENT_YIELD, cause: %d, ref: %d, GPR1: %d\n", e.cause, e.ref, c->GPR1); break;
    default: panic("Unhandled event ID = %d, cause: %d, ref: %d, GPR1: %d\n", e.event, e.cause, e.ref, c->GPR1);
  }

  return c;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  cte_init(do_event);
}
