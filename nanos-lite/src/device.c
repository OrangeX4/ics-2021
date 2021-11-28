#include <common.h>
#include <my_time.h>

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

int my_gettimeofday(struct timeval *tv, struct timezone *tz) {
    assert(tv);
    uint64_t tmp = io_read(AM_TIMER_UPTIME).us;
    tv->tv_usec = tmp % 1000000;
    tv->tv_sec = tmp / 1000000;
    if (tz) {
      tz->tz_minuteswest = tv->tv_sec / 60;
      tz->tz_dsttime = 0;
    }
    return 0;
}

size_t events_read(void *buf, size_t offset, size_t len) {
  AM_INPUT_KEYBRD_T ev = io_read(AM_INPUT_KEYBRD);
  if (ev.keycode == AM_KEY_NONE) return 0;
  return snprintf(buf, len, "%s %s\n", ev.keydown ? "kd" : "ku", keyname[ev.keycode]);
}

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  AM_GPU_CONFIG_T config = io_read(AM_GPU_CONFIG);
  return snprintf(buf, len, "WIDTH:%d\nHEIGHT:%d", config.width, config.height);
}

static int screen_w = 0;
static int screen_h = 0;

size_t get_fb_size() {
  AM_GPU_CONFIG_T config = io_read(AM_GPU_CONFIG);
  screen_w = config.width;
  screen_h = config.height;
  return 4 * screen_w * screen_h;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  assert(offset + len <= 4 * screen_w * screen_h);
  int x = (offset % screen_w) / 4;
  int y = (offset / screen_w) / 4;
  assert(x + len / 4 <= screen_w);
  io_write(AM_GPU_FBDRAW, x, y, (void *) buf, len / 4, 1, false);
  return len;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
