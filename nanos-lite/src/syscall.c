#include "syscall.h"

#include <common.h>
#include <fs.h>
#include <my_time.h>
#include <proc.h>

char *getenv(const char *__name);
void naive_uload(PCB *pcb, const char *filename);
void context_uload(PCB *pcb, const char *filename, char *const argv[], char *const envp[]);
void switch_boot_pcb();
extern PCB *current;

// static char program_buf[64];

const char *syscall_names[] = {
    "SYS_exit",  "SYS_yield",  "SYS_open",   "SYS_read",   "SYS_write",
    "SYS_kill",  "SYS_getpid", "SYS_close",  "SYS_lseek",  "SYS_brk",
    "SYS_fstat", "SYS_time",   "SYS_signal", "SYS_execve", "SYS_fork",
    "SYS_link",  "SYS_unlink", "SYS_wait",   "SYS_times",  "SYS_gettimeofday"};

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;

  switch (a[0]) {
    case SYS_exit: {
#ifdef ENABLE_STRACE
      printf("[strace] %s(%d)\n", syscall_names[a[0]], a[1]);
#endif
      // 恢复终端
      char *empty[] = { NULL };
      context_uload(current, ENTRY_PROGRAM, empty, empty);
      switch_boot_pcb();
      yield();
      // halt(0);
      // printf("exit()\n");
      // assert(0);
      // naive_uload(NULL, ENTRY_PROGRAM);
      // yield();
      break;
    }
    case SYS_yield: {
      yield();
      c->GPRx = 0;
#ifdef ENABLE_STRACE
      printf("[strace] %s() = %d\n", syscall_names[a[0]], c->GPRx);
#endif
      break;
    }
    case SYS_open: {
      // int _open(const char *path, int flags, mode_t mode)
      c->GPRx = fs_open((const char *)a[1], a[2], a[3]);
#ifdef ENABLE_STRACE
      printf("[strace] %s(*path = \"%s\", flags = %d, mode = %d) = %d\n",
             syscall_names[a[0]], (const char *)a[1], a[2], a[3], c->GPRx);
#endif
      break;
    }
    case SYS_read: {
      // int _read(int fd, void *buf, size_t count)
      c->GPRx = fs_read(a[1], (void *)a[2], a[3]);
#ifdef ENABLE_STRACE
      printf("[strace] %s(\"%s\", *buf = 0x%p, len = %d) = %d\n",
             syscall_names[a[0]], fs_getname(a[1]), a[2], a[3], c->GPRx);
#endif
      break;
    }
    case SYS_write: {
      // int _write(int fd, void *buf, size_t count)
      c->GPRx = fs_write(a[1], (void *)a[2], a[3]);
#ifdef ENABLE_STRACE
      printf("[strace] %s(\"%s\", *buf = 0x%p, len = %d) = %d\n",
             syscall_names[a[0]], fs_getname(a[1]), a[2], a[3], c->GPRx);
#endif
      break;
    }
    case SYS_close: {
      c->GPRx = fs_close(a[1]);
#ifdef ENABLE_STRACE
      printf("[strace] %s(\"%s\") = %d\n", syscall_names[a[0]],
             fs_getname(a[1]), c->GPRx);
#endif
      break;
    }
    case SYS_lseek: {
      // off_t _lseek(int fd, off_t offset, int whence)
      c->GPRx = fs_lseek(a[1], a[2], a[3]);
#ifdef ENABLE_STRACE
      printf("[strace] %s(\"%s\", offset = %d, whence = %s) = %d\n",
             syscall_names[a[0]], fs_getname(a[1]), a[2],
             (a[3] == SEEK_SET)
                 ? "SEEK_SET"
                 : ((a[3] == SEEK_CUR) ? "SEEK_CUR" : "SEEK_END"),
             c->GPRx);
#endif
      break;
    }
    case SYS_brk: {
      c->GPRx = 0;
#ifdef ENABLE_STRACE
      printf("[strace] %s(increment = %d) = %d\n", syscall_names[a[0]], a[1],
             c->GPRx);
#endif
      break;
    }
    case SYS_execve: {
#ifdef ENABLE_STRACE
      printf("[strace] %s(file = %s, argv = %s, envp = %s)\n", syscall_names[a[0]], (char *) a[1], *(char **) a[2], *(char **) a[3]);
#endif
        if (fs_open((char *) a[1], 0, 0) < 0) {
          c->GPRx = -2;
          break;
        }
        context_uload(current, (char *) a[1], (char **) a[2], (char **) a[3]);
        switch_boot_pcb();
        yield();
        c->GPRx = 0;
        // int len = strlen((char *) a[3]);
        // strcpy(program_buf, (char *) a[3]);
        // *(program_buf + len) = '/';
        // strcpy(program_buf + len + 1, (char *) a[1]);
        // printf("Program buf: [%s]\n", program_buf);
        // naive_uload(NULL, program_buf);
        // context_uload(current, program_buf, (char **) a[2], (char **) a[3]);
        // switch_boot_pcb();
        // yield();
      break;
    }
    case SYS_gettimeofday: {
      // int _gettimeofday(struct timeval *tv, struct timezone *tz)
      c->GPRx = my_gettimeofday((struct timeval *)a[1], (struct timezone *)a[2]);
#ifdef ENABLE_STRACE
      if (a[2]) {
        printf("[strace] %s(timeval = {%d, %d}, timezone = {%d, %d}) = %d\n",
               syscall_names[a[0]], ((struct timeval *)a[1])->tv_sec,
               ((struct timeval *)a[1])->tv_usec,
               ((struct timezone *)a[2])->tz_dsttime,
               ((struct timezone *)a[2])->tz_minuteswest, c->GPRx);
      } else {
        printf("[strace] %s(timeval = {%d, %d}, timezone = NULL) = %d\n",
               syscall_names[a[0]], ((struct timeval *)a[1])->tv_sec,
               ((struct timeval *)a[1])->tv_usec, c->GPRx);
      }
#endif
      break;
    }
    default:
      panic("Unhandled syscall ID = %d", a[0]);
  }
}
