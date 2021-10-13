#ifndef COSMOPOLITAN_LIBC_LINUX_CLONE_H_
#define COSMOPOLITAN_LIBC_LINUX_CLONE_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

forceinline long LinuxClone(unsigned long flags, void* stack, int* parent_tid, int* child_tid, void* tls) {
  long rc;
  register int* child_tid_ asm("r10") = child_tid;
  register void* tls_ asm("r8") = tls;
  asm volatile("syscall"
               : "=a"(rc)
               : "0"(56), "D"(flags), "S"(stack), "d"(parent_tid), "r"(child_tid_), "r"(tls_)
               : "rcx", "r11", "memory");
  return rc;
}

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_LINUX_MMAP_H_ */
