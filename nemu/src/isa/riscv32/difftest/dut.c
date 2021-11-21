#include <cpu/difftest.h>
#include <isa.h>

#include "../local-include/reg.h"

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
    if (cpu.pc != ref_r->pc) return false;
    for (int i = 0; i < 32; ++i) {
        if (cpu.gpr[i]._32 != ref_r->gpr[i]._32) return false;
    }
    // for (int i = 0; i < 4; ++i) {
    //     if (cpu.csr[i]._32 != ref_r->csr[i]._32) return false;
    // }
    return true;
}

void isa_difftest_attach() {}
