#ifndef COSMOPOLITAN_LIBC_LINUX_MUNMAP_H_
#define COSMOPOLITAN_LIBC_LINUX_MUNMAP_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

forceinline long LinuxMunmap(void *addr, size_t size) {
#ifdef __x86_64__
  long rc;
  asm volatile("syscall"
               : "=a"(rc)
               : "0"(0xb), "D"(addr), "S"(size)
               : "rcx", "r11", "memory");
  return rc;
#elif defined(__aarch64__)
  register long r0 asm("x0") = (long)addr;
  register long r1 asm("x1") = (long)size;
  register long res_x0 asm("x0");
  asm volatile("mov\tx8,%1\n\t"
               "svc\t0"
               : "=r"(res_x0)
               : "i"(215), "r"(r0), "r"(r1)
               : "x8", "memory");
  return res_x0;
#else
#error "unsupported architecture"
#endif
}

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_LINUX_MUNMAP_H_ */
