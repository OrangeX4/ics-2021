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
  long lst_sec = tv.tv_sec;
  long cur_sec = tv.tv_sec;
  while (true) {
    _gettimeofday(&tv, NULL);
    cur_sec = tv.tv_sec;
    if (cur_sec - lst_sec >= 30) {
        printf("Hello, world!\n");
        lst_sec = cur_sec;
    }
  }
  return 0;
}
