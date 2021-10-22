#include <cpu/ifetch.h>
#include <isa-all-instr.h>

#include "../local-include/reg.h"

def_all_THelper();

static uint32_t get_instr(Decode *s) { return s->isa.instr.val; }

// decode operand helper
#define def_DopHelper(name)                                             \
    void concat(decode_op_, name)(Decode * s, Operand * op, word_t val, \
                                  bool flag)

static def_DopHelper(i) { op->imm = val; }

static def_DopHelper(r) {
    bool is_write = flag;
    static word_t zero_null = 0;
    op->preg = (is_write && val == 0) ? &zero_null : &gpr(val);
}

static def_DHelper(I) {
    decode_op_r(s, id_src1, s->isa.instr.i.rs1, false);
    decode_op_i(s, id_src2, s->isa.instr.i.simm11_0, false);
    decode_op_r(s, id_dest, s->isa.instr.i.rd, true);
}

static def_DHelper(R) {
    decode_op_r(s, id_src1, s->isa.instr.r.rs1, false);
    decode_op_r(s, id_src2, s->isa.instr.r.rs2, false);
    decode_op_r(s, id_dest, s->isa.instr.r.rd, true);
}

static def_DHelper(B) {
    decode_op_r(s, id_src1, s->isa.instr.b.rs1, false);
    decode_op_r(s, id_src2, s->isa.instr.b.rs2, false);
    decode_op_i(s, id_dest,
                (s->isa.instr.b.simm4_1 << 1) | (s->isa.instr.b.simm10_5 << 5) |
                    (s->isa.instr.b.simm11 << 11) |
                    (s->isa.instr.b.simm12 << 12),
                true);
}

static def_DHelper(U) {
    decode_op_i(s, id_src1, s->isa.instr.u.imm31_12 << 12, true);
    decode_op_r(s, id_dest, s->isa.instr.u.rd, true);
}

static def_DHelper(J) {
    decode_op_i(s, id_src1,
                (s->isa.instr.j.simm10_1 << 1) | (s->isa.instr.j.simm11 << 11) |
                    (s->isa.instr.j.simm19_12 << 12) |
                    (s->isa.instr.j.simm20 << 20),
                true);
    decode_op_r(s, id_dest, s->isa.instr.u.rd, true);
}

static def_DHelper(S) {
    decode_op_r(s, id_src1, s->isa.instr.s.rs1, false);
    sword_t simm = (s->isa.instr.s.simm11_5 << 5) | s->isa.instr.s.imm4_0;
    decode_op_i(s, id_src2, simm, false);
    decode_op_r(s, id_dest, s->isa.instr.s.rs2, false);
}

def_THelper(load3) {
    def_INSTR_TAB("??????? ????? ????? 010 ????? ????? ??", lw);
    return EXEC_ID_inv;
}

def_THelper(store3) {
    def_INSTR_TAB("??????? ????? ????? 010 ????? ????? ??", sw);
    return EXEC_ID_inv;
}

def_THelper(comp7) {
    def_INSTR_TAB("0000000 ????? ????? ??? ????? ????? ??", add);
    def_INSTR_TAB("0100000 ????? ????? ??? ????? ????? ??", sub);
    return EXEC_ID_inv;
}

def_THelper(shift7) {
    def_INSTR_TAB("0000000 ????? ????? ??? ????? ????? ??", srl);
    def_INSTR_TAB("0100000 ????? ????? ??? ????? ????? ??", sra);
    return EXEC_ID_inv;
}

def_THelper(shifti7) {
    def_INSTR_TAB("0000000 ????? ????? ??? ????? ????? ??", srli);
    def_INSTR_TAB("0100000 ????? ????? ??? ????? ????? ??", srai);
    return EXEC_ID_inv;
}

def_THelper(comp3) {
    def_INSTR_TAB("??????? ????? ????? 000 ????? ????? ??", comp7);
    def_INSTR_TAB("??????? ????? ????? 101 ????? ????? ??", shift7);
    def_INSTR_TAB("??????? ????? ????? 001 ????? ????? ??", sll);
    return EXEC_ID_inv;
}

def_THelper(compi3) {
    def_INSTR_TAB("??????? ????? ????? 000 ????? ????? ??", addi);
    def_INSTR_TAB("??????? ????? ????? 101 ????? ????? ??", shifti7);
    def_INSTR_TAB("??????? ????? ????? 001 ????? ????? ??", slli);
    def_INSTR_TAB("??????? ????? ????? 011 ????? ????? ??", sltiu);
    return EXEC_ID_inv;
}

def_THelper(jmp3) {
    def_INSTR_TAB("??????? ????? ????? 000 ????? ????? ??", jalr);
    return EXEC_ID_inv;
}

def_THelper(jmpb3) {
    def_INSTR_TAB("??????? ????? ????? 000 ????? ????? ??", beq);
    def_INSTR_TAB("??????? ????? ????? 001 ????? ????? ??", bne);
    def_INSTR_TAB("??????? ????? ????? 100 ????? ????? ??", blt);
    def_INSTR_TAB("??????? ????? ????? 101 ????? ????? ??", bge);
    def_INSTR_TAB("??????? ????? ????? 010 ????? ????? ??", bltu);
    def_INSTR_TAB("??????? ????? ????? 111 ????? ????? ??", bgeu);
    return EXEC_ID_inv;
}

def_THelper(main) {
    def_INSTR_IDTAB("??????? ????? ????? ??? ????? 00000 11", I, load3);
    def_INSTR_IDTAB("??????? ????? ????? ??? ????? 01000 11", S, store3);
    def_INSTR_IDTAB("??????? ????? ????? ??? ????? 01101 11", U, lui);
    def_INSTR_IDTAB("??????? ????? ????? ??? ????? 00101 11", U, auipc);
    def_INSTR_IDTAB("??????? ????? ????? ??? ????? 01100 11", R, comp3);
    def_INSTR_IDTAB("??????? ????? ????? ??? ????? 00100 11", I, compi3);
    def_INSTR_IDTAB("??????? ????? ????? ??? ????? 11001 11", I, jmp3);
    def_INSTR_IDTAB("??????? ????? ????? ??? ????? 11000 11", B, jmpb3);
    def_INSTR_IDTAB("??????? ????? ????? ??? ????? 11011 11", J, jal);
    def_INSTR_TAB("??????? ????? ????? ??? ????? 11010 11", nemu_trap);
    return table_inv(s);
};

int isa_fetch_decode(Decode *s) {
    s->isa.instr.val = instr_fetch(&s->snpc, 4);
    int idx = table_main(s);
    return idx;
}
