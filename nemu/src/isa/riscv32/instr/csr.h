def_EHelper(csrrs) {
  rtl_mv(s, s0, dsrc2);
  rtl_or(s, dsrc2, s0, dsrc1);
  rtl_mv(s, ddest, s0);
}

def_EHelper(csrrw) {
  rtl_mv(s, s0, dsrc2);
  rtl_mv(s, dsrc2, dsrc1);
  rtl_mv(s, ddest, s0);
}

def_EHelper(ecall) {
  rtl_j(s, isa_raise_intr(gpr(17), cpu.pc));
}

def_EHelper(mret) {
  rtl_j(s, csr(3));
}