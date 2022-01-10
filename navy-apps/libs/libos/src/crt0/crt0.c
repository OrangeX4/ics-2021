#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

int main(int argc, char *argv[], char *envp[]);
extern char **environ;
void call_main(uintptr_t *args) {
  printf("args: %p\n", args);
  char *empty[] =  { NULL };

  int argc = *(int *)args;
  assert(argc == 0);
  char **argv = (char **)(args + 1);
  assert(*argv == NULL);
  while (args != NULL) ++args;
  char **envp = (char **)(args + 1);
  assert(*envp == NULL);

  // int argc = 0;
  // char **argv = empty;
  // char **envp = empty;
  
  assert(0);

  environ = envp;
  exit(main(argc, argv, envp));
  assert(0);
}
