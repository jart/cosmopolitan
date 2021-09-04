#ifndef COSMOPOLITAN_LIBC_LINUX_EXECVE_H_
#define COSMOPOLITAN_LIBC_LINUX_EXECVE_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

forceinline long LinuxExecve(const char *program, char *const argv[],
                             char *const envp[]) {
  long rc;
  asm volatile("syscall"
               : "=a"(rc)
               : "0"(59), "D"(program), "S"(argv), "d"(envp)
               : "rcx", "r11", "memory");
  return rc;
}

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_LINUX_EXECVE_H_ */
