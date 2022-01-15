#include <isa.h>

#define IRQ_TIMER 0x80000007  // for riscv32

word_t isa_raise_intr(word_t NO, vaddr_t epc) {
    /* Trigger an interrupt/exception with ``NO''.
     * Then return the address of the interrupt/exception vector.
     */
    cpu.csr[3]._32 = epc;    // mepc
    cpu.csr[1]._32 = NO;     // mcause

    if (NO == IRQ_TIMER) {
      printf("isa_raise_intr\n");
    }

    // 将 mstatus.MIE 保存到 mstatus.MPIE 中,
    // 然后将 mstatus.MIE 位置为 0
    // printf("mstatus before: 0x%x", cpu.csr[2]._32);
    cpu.csr[2]._32 = (cpu.csr[2]._32 & ~(1 << 7)) & ((cpu.csr[2]._32 & (1 << 3)) << 4); 
    cpu.csr[2]._32 = cpu.csr[2]._32 & ~(1 << 3);
    // printf("mstatus after: 0x%x", cpu.csr[2]._32);

#ifdef CONFIG_ETRACE
log_write("[etrace] mcause: %d, mstatus: %x, mepc: %x\n", cpu.csr[1]._32, cpu.csr[2]._32, cpu.csr[3]._32);
#endif

    // printf("mtvec: 0x%x", cpu.csr[0]._32);
    return cpu.csr[0]._32;          // mtvec
}

word_t isa_query_intr() { 
  if (cpu.INTR && cpu.gpr[0]._32 != 0) {
    cpu.INTR = false;
    return IRQ_TIMER;
  }
  return INTR_EMPTY;
}
