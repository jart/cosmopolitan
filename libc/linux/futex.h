#ifndef COSMOPOLITAN_LIBC_LINUX_FUTEX_H_
#define COSMOPOLITAN_LIBC_LINUX_FUTEX_H_
#include "libc/calls/struct/timespec.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

forceinline int LinuxFutexWait(void *addr, int expect,
                               struct timespec *timeout) {
  int ax;
  asm volatile("mov\t%5,%%r10\n\t"
               "syscall"
               : "=a"(ax)
               : "0"(202), "D"(addr), "S"(0), "d"(expect), "g"(timeout)
               : "rcx", "r10", "r11", "memory");
  return ax;
}

forceinline int LinuxFutexWake(void *addr, int count) {
  int ax;
  asm volatile("syscall"
               : "=a"(ax)
               : "0"(202), "D"(addr), "S"(1), "d"(count)
               : "rcx", "r11", "memory");
  return ax;
}

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_LINUX_FUTEX_H_ */
