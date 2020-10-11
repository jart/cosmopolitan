#ifndef COSMOPOLITAN_LIBC_LINUX_MUNMAP_H_
#define COSMOPOLITAN_LIBC_LINUX_MUNMAP_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

forceinline long LinuxMunmap(void *addr, size_t size) {
  long rc;
  asm volatile("syscall"
               : "=a"(rc)
               : "0"(0xb), "D"(addr), "S"(size)
               : "rcx", "r11", "memory");
  return rc;
}

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_LINUX_MUNMAP_H_ */
