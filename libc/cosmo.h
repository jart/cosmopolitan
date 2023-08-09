#ifndef COSMOPOLITAN_LIBC_COSMO_H_
#define COSMOPOLITAN_LIBC_COSMO_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

errno_t cosmo_once(_Atomic(uint32_t) *, void (*)(void));
int systemvpe(const char *, char *const[], char *const[]);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_COSMO_H_ */
