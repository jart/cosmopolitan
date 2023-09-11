#ifndef COSMOPOLITAN_LIBC_CALLS_EXECVE_SYSV_H_
#define COSMOPOLITAN_LIBC_CALLS_EXECVE_SYSV_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void __execve_lock(void);
void __execve_unlock(void);
bool IsApeLoadable(char[8]);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_EXECVE_SYSV_H_ */
