#ifndef COSMOPOLITAN_LIBC_CALLS_HEFTY_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_HEFTY_INTERNAL_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int faccessat$nt(int, const char *, int, uint32_t) hidden;
int execve$nt(const char *, char *const[], char *const[]) hidden;
int spawnve$nt(unsigned, int[3], const char *, char *const[],
               char *const[]) hidden;
int spawnve$sysv(unsigned, int[3], const char *, char *const[],
                 char *const[]) hidden;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_HEFTY_INTERNAL_H_ */
