#include <cpu/decode.h>
#include "../local-include/rtl.h"

#define INSTR_LIST(f) f(auipc) f(mul) f(mulh) f(divu) f(div) f(remu) f(rem) f(addi) f(add) f(andi) f(and) f(ori) f(or) f(xori) f(xor) f(sub) f(sra) f(srai) f(srl) f(srli) f(sll) f(slli) f(bge) f(bgeu) f(blt) f(bltu) f(beq) f(bne) f(slt) f(slti) f(sltu) f(sltiu) f(jal) f(jalr) f(lui) f(lw) f(lbu) f(sw) f(sh) f(sb) f(inv) f(nemu_trap)

def_all_EXEC_ID();
