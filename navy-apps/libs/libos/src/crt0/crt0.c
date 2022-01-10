#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

int main(int argc, char *argv[], char *envp[]);
extern char **environ;
void call_main(uintptr_t *args) {
  printf("args: %p\n", args);
  char *empty[] =  { NULL };

  int argc = *(int *)args;
  char **argv = (char **)(args + 1);
  while (*args != NULL) ++args;
  char **envp = (char **)(args + 1);

  // int argc = 0;
  // char **argv = empty;
  // char **envp = empty;
  
  assert(argc == 0);
  assert(*argv == NULL);
  assert(*envp == NULL);
  assert(0);

  environ = envp;
  exit(main(argc, argv, envp));
  assert(0);
}
