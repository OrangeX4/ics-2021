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
    rtl_li(s, ddest, s->pc);
    rtl_addi(s, ddest, ddest, 4);
    //   rtl_j(s, (*dsrc1 + id_src2->imm) & ~1);
    rtl_li(s, s0, ~1);
    rtl_mv(s, s1, dsrc1);
    rtl_addi(s, s1, s1, id_src2->imm);
    rtl_and(s, s0, s0, s1);
    rtl_jr(s, s0);
}