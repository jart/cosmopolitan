#ifndef COSMOPOLITAN_LIBC_COSMO_H_
#define COSMOPOLITAN_LIBC_COSMO_H_
COSMOPOLITAN_C_START_

errno_t cosmo_once(_Atomic(uint32_t) *, void (*)(void));
int systemvpe(const char *, char *const[], char *const[]) libcesque;
char *GetProgramExecutableName(void);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_COSMO_H_ */
