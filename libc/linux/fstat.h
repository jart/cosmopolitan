#ifndef COSMOPOLITAN_LIBC_LINUX_FSTAT_H_
#define COSMOPOLITAN_LIBC_LINUX_FSTAT_H_
#include "libc/calls/struct/stat.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

forceinline long LinuxFstat(long fd, struct stat *st) {
  long rc;
  asm volatile("syscall"
               : "=a"(rc)
               : "0"(5), "D"(fd), "S"(st)
               : "rcx", "r11", "memory");
  return rc;
}

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_LINUX_FSTAT_H_ */
