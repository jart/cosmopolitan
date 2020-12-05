#ifndef COSMOPOLITAN_LIBC_LINUX_EXIT_H_
#define COSMOPOLITAN_LIBC_LINUX_EXIT_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

forceinline wontreturn long LinuxExit(long rc) {
  asm volatile("syscall"
               : /* no outputs */
               : "a"(0xE7), "D"(rc)
               : "memory");
  unreachable;
}

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_LINUX_EXIT_H_ */
