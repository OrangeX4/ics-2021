#include <nterm.h>
#include <stdarg.h>
#include <unistd.h>
#include <SDL.h>

char handle_key(SDL_Event *ev);

static void sh_printf(const char *format, ...) {
  static char buf[256] = {};
  va_list ap;
  va_start(ap, format);
  int len = vsnprintf(buf, 256, format, ap);
  va_end(ap);
  term->write(buf, len);
}

static void sh_banner() {
  sh_printf("Built-in Shell in NTerm (NJU Terminal)\n\n");
}

static void sh_prompt() {
  sh_printf("sh> ");
}

static void clean_line_break(char *buf) {
  for (; *buf != '\n' && *buf != '\0'; ++buf);
  if (*buf == '\n') *buf = '\0';
}

static bool isStringAllSpaces(const char *buf) {
  bool result = true;
  for (; *buf != '\n' && *buf != '\0'; ++buf) {
    if (*buf != ' ') result = false;
  }
  return result;
}

static void sh_handle_cmd(const char *cmd) {
  char *buf = (char *) malloc(strlen(cmd) + 1);
  strcpy(buf, cmd);
  clean_line_break(buf);
  if (!isStringAllSpaces(buf)) {
    execve(buf, NULL, (char *const *) getenv("PATH"));
  }
}

void builtin_sh_run() {
  sh_banner();
  sh_prompt();

  while (1) {
    SDL_Event ev;
    if (SDL_PollEvent(&ev)) {
      if (ev.type == SDL_KEYUP || ev.type == SDL_KEYDOWN) {
        const char *res = term->keypress(handle_key(&ev));
        if (res) {
          sh_handle_cmd(res);
          sh_prompt();
        }
      }
    }
    refresh_terminal();
  }
}
