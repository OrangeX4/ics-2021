#include <NDL.h>
#include <SDL.h>
#include <string.h>
#include <assert.h>

#define keyname(k) #k,
#define LENGTH(arr) (sizeof(arr) / sizeof((arr)[0]))

static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

// #define SDL_KEYS_STATUS_ZERO(k) keys_status[SDLK_##k] = 0;
static uint8_t keys_status[256] = {};

int SDL_PushEvent(SDL_Event *ev) {
  assert(0);
  return 0;
}

static void clean_line_break(char *buf) {
  for (; *buf != '\n' && *buf != '\0'; ++buf);
  if (*buf == '\n') *buf = '\0';
}

static void parse_event_buf(char *buf, SDL_Event *event) {
  // printf("Event: %s\n", buf);
  // printf("Ku Result: %d\n", strncmp(buf, "ku ", 3));
  // printf("Kd Result: %d\n", strncmp(buf, "kd ", 3));
  if (strncmp(buf, "ku ", 3) == 0) {
    event->type = SDL_KEYUP;
    int key = 0;
    for (; key < LENGTH(keyname) && strcmp(buf + 3, keyname[key]) != 0; ++key); 
    assert(key < LENGTH(keyname));
    event->key.keysym.sym = key;
  } else if (strncmp(buf, "kd ", 3) == 0) {
    event->type = SDL_KEYDOWN;
    int key = 0;
    for (; key < LENGTH(keyname) && strcmp(buf + 3, keyname[key]) != 0; ++key); 
    assert(key < LENGTH(keyname));
    event->key.keysym.sym = key;
  } else {
    assert(0);
  }
}


int SDL_PollEvent(SDL_Event *ev) {
  char buf[16];
  int len = NDL_PollEvent(buf, 16);
  if (len) {
    clean_line_break(buf);
    parse_event_buf(buf, ev);
    if (ev->type == SDL_KEYDOWN) {
      keys_status[ev->key.keysym.sym] = true;
    } else if (ev->type == SDL_KEYUP) {
      keys_status[ev->key.keysym.sym] = false;
    }
    return 1;
  } else {
    return 0;
  }
}

int SDL_WaitEvent(SDL_Event *event) {
  while (!SDL_PollEvent(event));
  return true;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  assert(0);
  return 0;
}

uint8_t* SDL_GetKeyState(int *numkeys) {
  assert(numkeys == NULL);
  return keys_status;
}
