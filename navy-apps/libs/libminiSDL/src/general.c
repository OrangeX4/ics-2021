#include <NDL.h>
#include <assert.h>
#include <stdarg.h>

int SDL_Init(uint32_t flags) {
  return NDL_Init(flags);
}

void SDL_Quit() {
  NDL_Quit();
}

char *SDL_GetError() {
  assert(0);
  return "Navy does not support SDL_GetError()";
}

int SDL_SetError(const char* fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  printf("Error: ");

  printf(fmt, ap);

  printf("\n");

  va_end(ap);

  assert(0);
  return -1;
}

int SDL_ShowCursor(int toggle) {
  assert(0);
  return 0;
}

void SDL_WM_SetCaption(const char *title, const char *icon) {
}
