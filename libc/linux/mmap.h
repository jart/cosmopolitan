#ifndef COSMOPOLITAN_LIBC_LINUX_MMAP_H_
#define COSMOPOLITAN_LIBC_LINUX_MMAP_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

forceinline long LinuxMmap(void *addr, size_t size, long prot, long flags,
                           long fd, long off) {
  long rc;
  register long r10 asm("r10") = flags;
  register long r8 asm("r8") = fd;
  register long r9 asm("r9") = off;
  asm volatile("syscall"
               : "=a"(rc)
               : "0"(9), "D"(addr), "S"(size), "d"(prot), "r"(r10), "r"(r8),
                 "r"(r9)
               : "rcx", "r11", "memory");
  return rc;
}

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_LINUX_MMAP_H_ */
