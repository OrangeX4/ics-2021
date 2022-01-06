.globl  _start
_start:
  move s0, zero
  // 设置栈顶
  mv sp, a0
  jal call_main
