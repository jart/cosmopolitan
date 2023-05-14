#ifndef COSMOPOLITAN_LIBC_LINUX_MMAP_H_
#define COSMOPOLITAN_LIBC_LINUX_MMAP_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

forceinline long LinuxMmap(void *addr, size_t size, long prot, long flags,
                           long fd, long off) {
#ifdef __x86_64__
  long rc;
  register long flags_ asm("r10") = flags;
  register long fd_ asm("r8") = fd;
  register long off_ asm("r9") = off;
  asm volatile("syscall"
               : "=a"(rc)
               : "0"(9), "D"(addr), "S"(size), "d"(prot), "r"(flags_), "r"(fd_),
                 "r"(off_)
               : "rcx", "r11", "memory");
  return rc;
#elif defined(__aarch64__)
  register long r0 asm("x0") = (long)addr;
  register long r1 asm("x1") = (long)size;
  register long r2 asm("x2") = (long)prot;
  register long r3 asm("x3") = (long)flags;
  register long r4 asm("x4") = (long)fd;
  register long r5 asm("x5") = (long)off;
  register long res_x0 asm("x0");
  asm volatile("mov\tx8,%1\n\t"
               "svc\t0"
               : "=r"(res_x0)
               : "i"(222), "r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5)
               : "x8", "memory");
  return res_x0;
#else
#error "unsupported architecture"
#endif
}

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_LINUX_MMAP_H_ */
