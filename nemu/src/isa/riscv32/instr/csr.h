def_EHelper(csrrs) {
  rtl_mv(s, s0, dsrc2);
  rtl_or(s, dsrc2, s0, dsrc1);
  rtl_mv(s, ddest, s0);
}