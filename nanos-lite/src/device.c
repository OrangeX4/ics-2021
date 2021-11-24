#include <common.h>
#include <time.h>

#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
# define MULTIPROGRAM_YIELD() yield()
#else
# define MULTIPROGRAM_YIELD()
#endif

#define NAME(key) \
  [AM_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [AM_KEY_NONE] = "NONE",
  AM_KEYS(NAME)
};

size_t serial_write(const void *buf, size_t offset, size_t len) {
  for (size_t i = 0; i < len; ++i) putch(((char *) buf)[i]);
  return len;
}

int gettimeofday(struct timeval *tv, struct timezone *tz) {
    assert(tv);
    tv->tv_usec = io_read(AM_TIMER_UPTIME).us;
    tv->tv_sec = tv->tv_usec / 1000000;
    if (tz) {
        tz->tz_minuteswest = tv->tv_sec / 60;
        tz->tz_dsttime = 0;
    }
    return 0;
}

size_t events_read(void *buf, size_t offset, size_t len) {
  return 0;
}

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  return 0;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  return 0;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
