def_EHelper(jal) {
  rtl_li(s, ddest, s->pc + 4);
  printf("imm: %d\n", id_src1->imm);
  rtl_j(s, s->pc + id_src1->imm);
}