#include "local-include/reg.h"

#include <isa.h>

const char *regs[] = {"$0", "ra", "sp",  "gp",  "tp", "t0", "t1", "t2",
                      "s0", "s1", "a0",  "a1",  "a2", "a3", "a4", "a5",
                      "a6", "a7", "s2",  "s3",  "s4", "s5", "s6", "s7",
                      "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"};
const char *csrs[] = {"mtvec", "mcause", "mstatus", "mepc"};

void _isa_reg_display(CPU_state *_cpu, char *msg) {
    printf("--------------------------\n%s\n", msg);
    for (int i = 0; i < 32; ++i) {
        if (_cpu->gpr[i]._32) {
            printf("%-16s%-#16x%-16u\n", regs[i], _cpu->gpr[i]._32,
                   _cpu->gpr[i]._32);
        }
    }
    for (int i = 0; i < 4; ++i) {
        if (_cpu->csr[i]._32) {
            printf("%-16s%-#16x%-16u\n", csrs[i], _cpu->csr[i]._32,
                   _cpu->csr[i]._32);
        }
    }
    printf("%-16s%-#16x%-16u\n", "pc", _cpu->pc, _cpu->pc);
    printf("\nRegisters equal to zero have been omitted. They are");
    for (int i = 0; i < 32; ++i) {
        if (!_cpu->gpr[i]._32) {
            printf(" %s", regs[i]);
        }
    }
    for (int i = 0; i < 4; ++i) {
        if (!_cpu->csr[i]._32) {
            printf(" %s", csrs[i]);
        }
    }
    printf(".\n\n");
}

void isa_reg_display() {
    // riscv32
    _isa_reg_display(&cpu, "[DUT]:");
}

word_t isa_reg_str2val(const char *s, bool *success) {
    
    if (!strcmp(s, "$pc")) {
        *success = true;
        return cpu.pc;
    }
    for (int i = 0; i < 32; ++i) {
        if (!strcmp(s + 1, regs[i])) {
            *success = true;
            return gpr(i);
        }
    }
    for (int i = 0; i < 4; ++i) {
        if (!strcmp(s + 1, csrs[i])) {
            *success = true;
            return csr(i);
        }
    }
    *success = false;
    return 0;
}
