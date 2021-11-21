#include <isa.h>

word_t isa_raise_intr(word_t NO, vaddr_t epc) {
    /* Trigger an interrupt/exception with ``NO''.
     * Then return the address of the interrupt/exception vector.
     */

    cpu.csr[3]._32 = epc;    // mepc
    cpu.csr[1]._32 = NO;     // mcause

#ifdef CONFIG_ETRACE
log_write("[etrace] mcause: %d, mstatus: %x, mepc: %x\n", cpu.csr[1]._32, cpu.csr[2]._32, cpu.csr[3]._32);
#endif

    return cpu.csr[0]._32;          // mtvec
}

word_t isa_query_intr() { return INTR_EMPTY; }
