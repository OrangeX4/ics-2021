#include <cpu/difftest.h>
#include <isa.h>

#include "../local-include/reg.h"

const char *regs[] = {"$0", "ra", "sp",  "gp",  "tp", "t0", "t1", "t2",
                      "s0", "s1", "a0",  "a1",  "a2", "a3", "a4", "a5",
                      "a6", "a7", "s2",  "s3",  "s4", "s5", "s6", "s7",
                      "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"};

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
    if (cpu.pc != ref_r->pc) {
        printf("pc is expected to be [%#x] but got [%#x].\n", ref_r->pc,
               cpu.pc);
        return false;
    }
    for (int i = 0; i < 32; ++i) {
        if (cpu.gpr[i]._32 != ref_r->gpr[i]._32) {
            printf("%s is expected to be [%#x] but got [%#x].\n", regs[i],
                   ref_r->gpr[i]._32, cpu.gpr[i]._32);
            return false;
        }
    }
    return true;
}

void isa_difftest_attach() {}
