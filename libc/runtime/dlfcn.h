#ifndef COSMOPOLITAN_LIBC_RUNTIME_DLFCN_H_
#define COSMOPOLITAN_LIBC_RUNTIME_DLFCN_H_

#define RTLD_LOCAL  0
#define RTLD_LAZY   1
#define RTLD_NOW    2
#define RTLD_GLOBAL 256

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define RTLD_NEXT    ((void *)-1)
#define RTLD_DEFAULT ((void *)0)

char *dlerror(void);
void *dlopen(const char *, int);
void *dlsym(void *, const char *);
int dlclose(void *);
int dl_iterate_phdr(int (*)(void *, size_t, void *), void *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_RUNTIME_DLFCN_H_ */
