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

static bool is_string_all_spaces(const char *buf) {
  bool result = true;
  for (; *buf != '\n' && *buf != '\0'; ++buf) {
    if (*buf != ' ') result = false;
  }
  return result;
}

char** str_split(char* a_str, const char a_delim) {
  char** result = 0;
  size_t count = 0;
  char* tmp = a_str;
  char* last_comma = 0;
  char delim[2];
  delim[0] = a_delim;
  delim[1] = 0;

  /* Count how many elements will be extracted. */
  while (*tmp) {
    if (a_delim == *tmp) {
      count++;
      last_comma = tmp;
    }
    tmp++;
  }

  /* Add space for trailing token. */
  count += last_comma < (a_str + strlen(a_str) - 1);

  /* Add space for terminating null string so caller
      knows where the list of returned strings ends. */
  count++;

  result = (char**) malloc(sizeof(char*) * count);

  if (result) {
    size_t idx = 0;
    char* token = strtok(a_str, delim);

    while (token) {
      assert(idx < count);
      *(result + idx++) = strdup(token);
      token = strtok(0, delim);
    }
    *(result + idx) = 0;
  }

  return result;
}

static void sh_handle_cmd(const char *cmd) {
  char *const nterm_empty_argv[] = { NULL };
  char *buf = (char *) malloc(strlen(cmd) + 1);
  strcpy(buf, cmd);
  clean_line_break(buf);

  char **argv = str_split(buf, ' ');
  char *file = argv[0];

  if (!is_string_all_spaces(buf)) {
    // printf("getenv: %s\n", getenv("PATH"));
    // execve(buf, NULL, (char *const *) getenv("PATH"));
    execvp(file, argv);
  }

  free(buf);
  free(argv);
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
