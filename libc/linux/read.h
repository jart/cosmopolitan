#ifndef COSMOPOLITAN_LIBC_LINUX_READ_H_
#define COSMOPOLITAN_LIBC_LINUX_READ_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

forceinline long LinuxRead(long fd, void *data, unsigned long size) {
  long rc;
  asm volatile("syscall"
               : "=a"(rc), "=m"(*(char(*)[size])data)
               : "0"(0), "D"(fd), "S"(data), "d"(size)
               : "rcx", "r11", "memory");
  return rc;
}

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_LINUX_READ_H_ */
