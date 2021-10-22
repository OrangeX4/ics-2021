#include <cpu/decode.h>
#include "../local-include/rtl.h"

#define INSTR_LIST(f) f(auipc) f(addi) f(add) f(srl) f(srli) f(beq) f(bne) f(sltiu) f(jal) f(jalr) f(lui) f(lw) f(sw) f(inv) f(nemu_trap)

def_all_EXEC_ID();
