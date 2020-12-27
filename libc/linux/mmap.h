#ifndef COSMOPOLITAN_LIBC_LINUX_MMAP_H_
#define COSMOPOLITAN_LIBC_LINUX_MMAP_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

forceinline long LinuxMmap(void *addr, size_t size, long prot, long flags,
                           long fd, long off) {
  long rc;
  asm volatile("mov\t%5,%%r10\n\t"
               "mov\t%6,%%r8\n\t"
               "mov\t%7,%%r9\n\t"
               "syscall"
               : "=a"(rc)
               : "0"(9), "D"(addr), "S"(size), "d"(prot), "g"(flags), "g"(fd),
                 "g"(off)
               : "rcx", "r8", "r9", "r10", "r11", "memory");
  return rc;
}

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_LINUX_MMAP_H_ */
