#ifndef COSMOPOLITAN_LIBC_COSMO_H_
#define COSMOPOLITAN_LIBC_COSMO_H_
COSMOPOLITAN_C_START_

errno_t cosmo_once(_Atomic(unsigned) *, void (*)(void)) libcesque;
int systemvpe(const char *, char *const[], char *const[]) libcesque;
char *GetProgramExecutableName(void) libcesque;
void unleaf(void) libcesque;
int __demangle(char *, const char *, size_t) libcesque;
int __is_mangled(const char *) libcesque;

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_COSMO_H_ */
