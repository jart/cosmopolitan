#ifndef COSMOPOLITAN_LIBC_COSMO_H_
#define COSMOPOLITAN_LIBC_COSMO_H_
COSMOPOLITAN_C_START_

errno_t cosmo_once(_Atomic(uint32_t) *, void (*)(void));
int systemvpe(const char *, char *const[], char *const[]) libcesque;
char *GetProgramExecutableName(void);
void unleaf(void);
int __demangle(char *, const char *, size_t);
int __is_mangled(const char *);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_COSMO_H_ */
