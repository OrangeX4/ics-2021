#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

int main(int argc, char *argv[], char *envp[]);
extern char **environ;
void call_main(uintptr_t *args) {

  int argc = *(int *)args;
  char **argv = (char **)(args + 1);
  while (*args != NULL) ++args;
  char **envp = (char **)(args + 1);

  // char *empty[] =  { NULL };
  // int argc = 0;
  // char **argv = empty;
  // char **envp = empty;

  printf("args: %p\n", args);
  printf("argc: %d\n", argc);
  printf("argv[0]: %s\n", argv[0]);
  printf("argv[1]: %s\n", argv[1]);
  assert(argv[2] == NULL);
  printf("envp[0]: %s\n", envp[0]);
  assert(envp[1] == NULL);
  assert(0);

  environ = envp;
  exit(main(argc, argv, envp));
  assert(0);
}
