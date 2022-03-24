def_EHelper(jal) {
    //   rtl_li(s, ddest, s->pc + 4);
    rtl_li(s, ddest, s->pc);
    rtl_addi(s, ddest, ddest, 4);
    //   rtl_j(s, s->pc + id_src1->imm);
    rtl_li(s, s0, s->pc);
    rtl_addi(s, s0, s0, id_src1->imm);
    rtl_jr(s, s0);
}

def_EHelper(jalr) {
    //   rtl_li(s, ddest, s->pc + 4);
    rtl_li(s, s0, s->pc);
    rtl_addi(s, s0, s0, 4);
    //   rtl_j(s, (*dsrc1 + id_src2->imm) & ~1);
    rtl_li(s, s1, ~1);
    rtl_mv(s, s2, dsrc1);
    rtl_addi(s, s2, s2, id_src2->simm);
    rtl_and(s, s1, s1, s2);
    rtl_mv(s, ddest, s0);
    rtl_jr(s, s1);
}

static inline void jump_helper(Decode *s, uint32_t relop) {
    rtl_li(s, s0, s->pc);
    rtl_addi(s, s0, s0, id_dest->imm);
    rtl_jrelopr(s, relop, dsrc1, dsrc2, s0);
}

def_EHelper(beq) {
    jump_helper(s, RELOP_EQ);
}

def_EHelper(bne) {
    jump_helper(s, RELOP_NE);
}

def_EHelper(bge) {
    jump_helper(s, RELOP_GE);
}

def_EHelper(bgeu) {
    jump_helper(s, RELOP_GEU);
}

def_EHelper(blt) {
    jump_helper(s, RELOP_LT);
}

def_EHelper(bltu) {
    jump_helper(s, RELOP_LTU);
}