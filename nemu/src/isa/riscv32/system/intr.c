#include <isa.h>

word_t isa_raise_intr(word_t NO, vaddr_t epc) {
    /* Trigger an interrupt/exception with ``NO''.
     * Then return the address of the interrupt/exception vector.
     */

    cpu.csr[3]._32 = cpu.pc;    // mepc
    cpu.csr[1]._32 = NO;        // mcause
    return cpu.csr[0]._32;      // mtvec
}

word_t isa_query_intr() { return INTR_EMPTY; }
