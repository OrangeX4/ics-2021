#include <isa.h>

word_t isa_raise_intr(word_t NO, vaddr_t epc) {
    /* Trigger an interrupt/exception with ``NO''.
     * Then return the address of the interrupt/exception vector.
     */

    cpu.pc = cpu.csr[3]._32;
    cpu.csr[1]._32 = NO;
    return cpu.csr[0]._32;
}

word_t isa_query_intr() { return INTR_EMPTY; }
