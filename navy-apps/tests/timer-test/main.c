#include <stdio.h>
#include <libndl/NDL.h>
#include <assert.h>

#define true 1

int main() {
  long lst_msec = NDL_GetTicks();
  long cur_msec = lst_msec;
  while (true) {
    cur_msec = NDL_GetTicks();
    if (cur_msec - lst_msec >= 500) {
        printf("Hello, world!\n");
        lst_msec = cur_msec;
    }
  }
  return 0;
}
