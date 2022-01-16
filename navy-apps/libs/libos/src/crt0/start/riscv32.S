.globl  _start
_start:
  // 设置栈顶
  mv sp, a0
  move s0, zero
  jal call_main
