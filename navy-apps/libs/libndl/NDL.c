#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <assert.h>

int _gettimeofday(struct timeval *tv, struct timezone *tz);

static int evtdev = -1;
static int fbdev = -1;
static int screen_w = 0, screen_h = 0;
static int canvas_w = 0, canvas_h = 0;
static FILE *fp_event;

uint32_t NDL_GetTicks() {
  struct timeval tv = {};
  _gettimeofday(&tv, NULL);
  return tv.tv_usec / 1000;
}

int NDL_PollEvent(char *buf, int len) {
  return read(fp_event, buf, len);
}

// Return the next string after '\n' or NULL after '\0' 
char *parse_kv(const char *str, char *key, char *value) {
  for (; *str == ' '; ++str);
  for (; *str != ' ' && *str != ':'; ++str, ++key) *key = *str; 
  *key = '\0';
  for (; *str == ' '; ++str);
  assert(*str == ':');
  ++str;
  for (; *str == ' '; ++str);
  for (; *str != ' ' && *str != '\n'; ++str, ++value) *value = *str; 
  *value = '\0';
  for (; *str == ' '; ++str);
  assert(*str == '\n' || *str == '\0');
  if (*str == '\n') {
    ++str;
    if (*str != '\0') {
      return str;
    } else {
      return NULL;
    }
  } else if (*str == '\0') {
    return NULL;
  }
}

void NDL_OpenCanvas(int *w, int *h) {
  if (getenv("NWM_APP")) {
    int fbctl = 4;
    fbdev = 5;
    screen_w = *w; screen_h = *h;
    char buf[64];
    int len = sprintf(buf, "%d %d", screen_w, screen_h);
    // let NWM resize the window and create the frame buffer
    write(fbctl, buf, len);
    while (1) {
      // 3 = evtdev
      int nread = read(3, buf, sizeof(buf) - 1);
      if (nread <= 0) continue;
      buf[nread] = '\0';
      if (strcmp(buf, "mmap ok") == 0) break;
    }
    close(fbctl);
  } else {
    FILE *fp_info = open("/proc/dispinfo", "r+");
    char buf[64];
    char *_buf = buf;
    read(fp_info, buf, sizeof(buf));
    close(fp_info);
    // WIDTH : 640
    // HEIGHT:480
    char key[16], value[16];
    _buf = parse_kv(_buf, key, value);
    assert(_buf);
    // printf("buf:\n%s\n", buf);
    // printf("key: [%s]\n", key);
    // printf("value: [%s]\n", value);
    if (strcmp(key, "WIDTH") == 0) {
      screen_w = atoi(value);
      parse_kv(_buf, key, value);
    // printf("key: [%s]\n", key);
    // printf("value: [%s]\n", value);
    // assert(_buf == NULL);
      if (strcmp(key, "HEIGHT") == 0) {
        screen_h = atoi(value);
      } else {
        assert(0);
      }
    } else if (strcmp(key, "HEIGHT") == 0) {
      screen_h = atoi(value);
      parse_kv(_buf, key, value);
    //   assert(_buf == NULL);
      if (strcmp(key, "WIDTH") == 0) {
        screen_w = atoi(value);
      } else {
        assert(0);
      }
    } else {
      assert(0);
    }
    assert(screen_w);
    assert(screen_h);
    printf("screen_w: %d\n", screen_w);
    printf("screen_h: %d\n", screen_h);
    assert(*w <= screen_w);
    assert(*h <= screen_h);
    if (*w == 0 || *h == 0) {
      canvas_w = screen_w;
      canvas_w = screen_h;
    } else {
      canvas_w = *w;
      canvas_w = *h;
    }
  }
}

void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
  FILE *fp = open("/dev/fb", "w+");
  long off_h = (screen_h - canvas_h) / 2;
  long off_w = (screen_w - canvas_w) / 2;
  // printf("screen_h: %d\n", screen_h);
  // printf("screen_w: %d\n", screen_w);
  printf("canvas_h: %d\n", canvas_h);
  assert(0);
  printf("canvas_w: %d\n", canvas_w);
  // printf("off_h: %d\n", off_h);
  // printf("off_w: %d\n", off_w);
  // printf("x: %d\n", x);
  // printf("y: %d\n", y);
  // printf("w: %d\n", w);
  // printf("h: %d\n", h);
  for (int i = 0; i < h; ++i) {
    long offset = (off_h + y + i) * screen_w + off_w + x;
    printf("offset: %d\n", offset);
    fseek(fp, offset, SEEK_SET);
    assert(0);
    write(fp, pixels, w);
  }
  close(fp);
}

void NDL_OpenAudio(int freq, int channels, int samples) {
}

void NDL_CloseAudio() {
}

int NDL_PlayAudio(void *buf, int len) {
  return 0;
}

int NDL_QueryAudio() {
  return 0;
}

int NDL_Init(uint32_t flags) {
  if (getenv("NWM_APP")) {
    evtdev = 3;
  }
  fp_event = open("/dev/events", "r+");
  return 0;
}

void NDL_Quit() {
  close(fp_event);
}
