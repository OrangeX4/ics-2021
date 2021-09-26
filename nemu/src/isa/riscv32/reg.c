#include "local-include/reg.h"

#include <isa.h>

const char *regs[] = {"$0", "ra", "sp",  "gp",  "tp", "t0", "t1", "t2",
                      "s0", "s1", "a0",  "a1",  "a2", "a3", "a4", "a5",
                      "a6", "a7", "s2",  "s3",  "s4", "s5", "s6", "s7",
                      "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"};

void isa_reg_display() {
    // riscv32
    for (int i = 0; i < 32; ++i) {
        if (cpu.gpr[i]._32) {
            printf("%-16s%-#16x%-16o\n", regs[i], cpu.gpr[i]._32,
                   cpu.gpr[i]._32);
        }
    }
    printf("%-16s%-#16x%-16o\n", "pc", cpu.pc, cpu.pc);
    printf("\nRegisters equal to zero have been omitted. They are");
    for (int i = 0; i < 32; ++i) {
        if (!cpu.gpr[i]._32) {
            printf(" %s", regs[i]);
        }
    }
    printf(".\n\n");
}

word_t isa_reg_str2val(const char *s, bool *success) {
    if (!strcmp(s, regs[0])) {
        *success = true;
        return cpu.gpr[0]._32;
    } else if (!strcmp(s, "$pc")) {
        *success = true;
        return cpu.pc;
    }
    for (int i = 1; i < 32; ++i) {
        if (!strcmp(s + 1, regs[i])) {
            *success = true;
            return cpu.gpr[i]._32;
        }
    }
    *success = false;
    return 0;
}
