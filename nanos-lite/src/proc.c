#include <proc.h>

#define MAX_NR_PROC 4

void naive_uload(PCB *pcb, const char *filename);
uintptr_t pcb_uload(PCB *pcb, const char *filename);

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;

void switch_boot_pcb() {
  current = &pcb_boot;
}

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    Log("Hello World from Nanos-lite with string '%s' for the %dth time!", (uintptr_t)arg, j);
    j ++;
    yield();
  }
}

void context_kload(PCB* pcb, void (*entry)(void *), void *arg) {
  Area kstack = { (void *) pcb, (void *) pcb + sizeof(PCB) };
  pcb->cp = kcontext(kstack, entry, arg);
}

#define MAX_STRINGS_LEN 10000

// 返回字符串数组的元素个数
static int strings_len(char *const *strings) {
  int count = 0;
  for (; *strings != NULL; ++strings) ++count;
  return count;
}

// 返回字符串数组的对齐占用的空间大小
static int strings_size(char *const *strings, int align) {
  int size = 0;
  for (; *strings != NULL; ++strings) size += (strlen(*strings) / align + 1);
  return size;
}

// 复制字符串数组
static void strings_copy(char *const *strings, char **new_strings, char *buf, int align) {
  for (; *strings != NULL; ++strings, ++new_strings) {
    *new_strings = *strings;
    strcpy(buf, *strings);
    buf += (strlen(*strings) / align + 1) * align;
  }
  *new_strings = NULL;
}

void context_uload(PCB *pcb, const char *filename, char *const argv[], char *const envp[]) {
  Area kstack = { (void *) pcb, (void *) pcb + sizeof(PCB) };
  pcb->cp = ucontext(&pcb->as, kstack, (void *) pcb_uload(pcb, filename));
  Context *c = (Context*)kstack.end - 1;
  
  // 当前默认栈顶为 heap.end
  uintptr_t *ustack = (uintptr_t *)heap.end;

  // 向下生成字符串区等栈区所需数据, 还要注意对齐
  int argc = strings_len(argv);
  int envc = strings_len(envp);
  int arg_size = strings_size(argv, sizeof(uintptr_t));
  int env_size = strings_size(envp, sizeof(uintptr_t));


  // 栈顶自减
  ustack -= (env_size + arg_size + envc + 1 + argc + 1 + 1);

  // 各个地址
  int *argc_p = (int *) (ustack);
  char **argv_p = (char **) (ustack + 1);
  char **envp_p = (char **) (ustack + 1 + argc + 1);
  char *arg_str = (char *) (ustack + 1 + argc + 1 + envc + 1);
  char *env_str = (char *) (ustack + 1 + argc + 1 + envc + 1 + arg_size);
  
  // 写入数据
  *argc_p = argc;
  strings_copy(argv, argv_p, arg_str, sizeof(uintptr_t));
  strings_copy(envp, envp_p, env_str, sizeof(uintptr_t));

  c->GPRx = (uintptr_t)ustack;
}

void init_proc() {

  char *const empty[] =  { NULL };

  Log("Initializing processes...");

  // context_uload(&pcb[0], "/bin/hello");
  context_kload(&pcb[0], hello_fun, "&pcb[0]");
  // context_kload(&pcb[1], hello_fun, "&pcb[1]");
  // context_uload(&pcb[1], "/bin/pal");
  context_uload(&pcb[1], "/bin/hello", empty, empty);
  switch_boot_pcb();

  // load program here
  // naive_uload(NULL, ENTRY_PROGRAM);
  // naive_uload(NULL, "/bin/hello");
}

Context* schedule(Context *prev) {
  // save the context pointer
  current->cp = prev;

  // current = &pcb[0];
  current = (current == &pcb[0] ? &pcb[1] : &pcb[0]);

  // then return the new context
  return current->cp;
}
