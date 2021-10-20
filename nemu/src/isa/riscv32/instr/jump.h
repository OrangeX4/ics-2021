def_EHelper(jal) {
  rtl_li(s, ddest, s->pc + 4);
  rtl_j(s, s->pc + id_src1->imm);
}