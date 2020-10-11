#ifndef COSMOPOLITAN_LIBC_LINUX_CLOSE_H_
#define COSMOPOLITAN_LIBC_LINUX_CLOSE_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

forceinline long LinuxClose(long fd) {
  long rc;
  asm volatile("syscall" : "=a"(rc) : "0"(3), "D"(fd) : "rcx", "r11", "memory");
  return rc;
}

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_LINUX_CLOSE_H_ */
