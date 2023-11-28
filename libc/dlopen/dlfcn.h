#ifndef COSMOPOLITAN_LIBC_DLFCN_H_
#define COSMOPOLITAN_LIBC_DLFCN_H_

#define RTLD_LOCAL  0
#define RTLD_LAZY   1
#define RTLD_NOW    2
#define RTLD_GLOBAL 256

COSMOPOLITAN_C_START_

#define RTLD_NEXT    ((void *)-1)
#define RTLD_DEFAULT ((void *)0)

char *dlerror(void);
void *dlopen(const char *, int);
void *dlsym(void *, const char *);
int dlclose(void *);
int dl_iterate_phdr(int (*)(void *, size_t, void *), void *);

#ifdef _COSMO_SOURCE
char *cosmo_dlerror(void);
void *cosmo_dlopen(const char *, int);
void *cosmo_dlsym(void *, const char *);
int cosmo_dlclose(void *);
int cosmo_dl_iterate_phdr(int (*)(void *, size_t, void *), void *);
#endif

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_DLFCN_H_ */
