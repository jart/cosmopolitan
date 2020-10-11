#ifndef COSMOPOLITAN_LIBC_LINUX_OPEN_H_
#define COSMOPOLITAN_LIBC_LINUX_OPEN_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

forceinline long LinuxOpen(const char *path, long flags, long mode) {
  long rc;
  asm volatile("syscall"
               : "=a"(rc)
               : "0"(2), "D"(path), "S"(flags), "d"(mode)
               : "rcx", "r11", "memory");
  return rc;
}

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_LINUX_OPEN_H_ */
