#include <proc.h>

#define MAX_NR_PROC 4
#define PGSIZE 4096

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
    // Log("Hello World from Nanos-lite with string '%s' for the %dth time!", (uintptr_t)arg, j);
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
  assert(strings != NULL);
  for (; *strings != NULL; ++strings) ++count;
  return count;
}

// 返回字符串数组的对齐占用的空间大小
static int strings_size(char *const *strings, int align) {
  assert(strings != NULL);
  int size = 0;
  for (; *strings != NULL; ++strings) size += (strlen(*strings) / align + 1);
  return size;
}

// 复制字符串数组
static void strings_copy(char *const *strings, char **new_strings, char *buf, int align) {
  for (; *strings != NULL; ++strings, ++new_strings) {
    strcpy(buf, *strings);
    *new_strings = buf;
    buf += (strlen(*strings) / align + 1) * align;
  }
  *new_strings = NULL;
}

void context_uload(PCB *pcb, const char *filename, char *const argv[], char *const envp[]) {
  protect(&pcb->as);

  Area kstack = { (void *) pcb, (void *) pcb + sizeof(PCB) };
  Context *c = (Context*)kstack.end - 1;
  
  // 不断分新页, 进行映射
  // uintptr_t *ustack = (uintptr_t *) new_page(8) + 8 * (1 << 12);
  uintptr_t *ustack = (uintptr_t *)(new_page(8) + 8 * PGSIZE);
  void *p_ustack = (void *)ustack - 8 * PGSIZE;
  void *as_ustack = pcb->as.area.end - 8 * PGSIZE;
  for (int i = 0; i < 8; ++i, as_ustack += PGSIZE, p_ustack += PGSIZE) {
    map(&pcb->as, as_ustack, p_ustack, MMAP_READ | MMAP_WRITE);
  }

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
  // printf("*argc_p: %d\n", argc);
  *argc_p = argc;
  strings_copy(argv, argv_p, arg_str, sizeof(uintptr_t));
  strings_copy(envp, envp_p, env_str, sizeof(uintptr_t));

  pcb->cp = ucontext(&pcb->as, kstack, (void *) pcb_uload(pcb, filename));
  c->GPRx = (uintptr_t)ustack;
}

#define DEF_TEST_ARGV(P) char *const P ## _argv[] = { "/bin/" #P "-test", NULL };
#define DEF_ARGV(P) char *const P ## _argv[] = { "/bin/" #P, NULL };
#define REF_ARGV(P) P ## _argv

void init_proc() {

  char *const empty[] =  { NULL };
  char *const pal_argv[] =  { "other", "--skip", NULL };
  // char *const two[] =  { "one", "two", NULL };
  // DEF_TEST_ARGV(exec);
  // DEF_ARGV(menu);
  // DEF_ARGV(nterm);

  Log("Initializing processes...");

  // context_uload(&pcb[0], "/bin/dummy", empty, empty);
  // context_kload(&pcb[0], hello_fun, "&pcb[0]");
  // context_kload(&pcb[1], hello_fun, "&pcb[1]");
  context_uload(&pcb[0], "/bin/hello", pal_argv, empty);
  // context_uload(&pcb[0], "/bin/pal", pal_argv, empty);
  // context_uload(&pcb[1], "/bin/pal", pal_argv, empty);
  // context_uload(&pcb[1], "/bin/exec-test", REF_ARGV(exec), empty);
  // context_uload(&pcb[1], "/bin/menu", REF_ARGV(menu), empty);
  // context_uload(&pcb[1], "/bin/nterm", REF_ARGV(nterm), empty);
  // context_uload(&pcb[1], "/bin/pal", empty, empty);
  // context_uload(&pcb[1], "/bin/hello", one, empty);
  // context_uload(&pcb[1], "/bin/hello", empty, one);
  // context_uload(&pcb[1], "/bin/hello", two, one);
  switch_boot_pcb();

  // load program here
  // naive_uload(NULL, ENTRY_PROGRAM);
  // naive_uload(NULL, "/bin/hello");
}

Context* schedule(Context *prev) {
  // save the context pointer
  current->cp = prev;

  current = &pcb[0];
  // current = (current == &pcb[0] ? &pcb[1] : &pcb[0]);

  // then return the new context
  return current->cp;
}
