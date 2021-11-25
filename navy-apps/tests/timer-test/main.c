#include <stdio.h>
#include <assert.h>
#include <sys/_timeval.h>

#define true 1

// extern struct timeval;

// extern struct timezone;

int _gettimeofday(struct timeval *tv, struct timezone *tz);

int main() {
  struct timeval tv = {};
  _gettimeofday(&tv, NULL);
  long lst_usec = tv.tv_usec;
  long cur_usec = tv.tv_usec;
  while (true) {
    _gettimeofday(&tv, NULL);
    cur_usec = tv.tv_usec;
    if (cur_usec - lst_usec >= 500000) {
        printf("Hello, world!\n");
        lst_usec = cur_usec;
    }
  }
  return 0;
}
