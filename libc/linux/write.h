#ifndef COSMOPOLITAN_LIBC_LINUX_WRITE_H_
#define COSMOPOLITAN_LIBC_LINUX_WRITE_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

forceinline long LinuxWrite(long fd, const void *data, unsigned long size) {
  long rc;
  asm volatile("syscall"
               : "=a"(rc)
               : "0"(1), "D"(fd), "S"(data), "d"(size),
                 "m"(*(char(*)[size])data)
               : "rcx", "r11", "memory");
  return rc;
}

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_LINUX_WRITE_H_ */
