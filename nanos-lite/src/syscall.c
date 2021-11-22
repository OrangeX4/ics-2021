#include "syscall.h"

#include <common.h>

const char *syscall_names[] = {
    "SYS_exit",  "SYS_yield",  "SYS_open",   "SYS_read",   "SYS_write",
    "SYS_kill",  "SYS_getpid", "SYS_close",  "SYS_lseek",  "SYS_brk",
    "SYS_fstat", "SYS_time",   "SYS_signal", "SYS_execve", "SYS_fork",
    "SYS_link",  "SYS_unlink", "SYS_wait",   "SYS_times",  "SYS_gettimeofday"
};

void do_syscall(Context *c) {
    uintptr_t a[4];
    a[0] = c->GPR1;
    a[1] = c->GPR2;
    a[2] = c->GPR3;
    a[3] = c->GPR4;

    switch (a[0]) {
        case SYS_exit:
            printf("[strace] SYS_exit(%d)\n", a[1]);
            halt(0);
            break;
        case SYS_yield: {
            yield();
            c->GPRx = 0;
            break;
        }
        case SYS_write: {
            // int _write(int fd, void *buf, size_t count)
            if (a[1] == 1 || a[1] == 2) {
                for (size_t i = 0; i < a[3]; ++i) putch(((char *) a[2])[i]);
                c->GPRx = a[3];
            }
            break;
        }
        case SYS_brk: c->GPRx = 0; break;
        default:
            panic("Unhandled syscall ID = %d", a[0]);
    }

#ifdef ENABLE_STRACE
    printf("[strace] %s(%d, %d, %d) = %d\n", syscall_names[a[0]], a[1], a[2], a[3], c->GPRx);
#endif
}
