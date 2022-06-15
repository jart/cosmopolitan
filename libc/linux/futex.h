#ifndef COSMOPOLITAN_LIBC_LINUX_FUTEX_H_
#define COSMOPOLITAN_LIBC_LINUX_FUTEX_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
#include "libc/calls/struct/timespec.h"

forceinline int LinuxFutexWait(void *addr, int expect,
                               struct timespec *timeout) {
  int ax;
  register void *r10 asm("r10") = timeout;
  asm volatile("syscall"
               : "=a"(ax)
               : "0"(202), "D"(addr), "S"(0), "d"(expect), "r"(r10)
               : "rcx", "r11", "memory");
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
