#ifndef COSMOPOLITAN_LIBC_DLFCN_H_
#define COSMOPOLITAN_LIBC_DLFCN_H_

#define RTLD_LOCAL  0
#define RTLD_LAZY   1
#define RTLD_NOW    2
#define RTLD_GLOBAL 256

COSMOPOLITAN_C_START_

#define RTLD_NEXT    ((void *)-1)
#define RTLD_DEFAULT ((void *)0)

char *dlerror(void) libcesque;
void *dlopen(const char *, int) libcesque;
void *dlsym(void *, const char *) libcesque;
int dlclose(void *) libcesque;

#ifdef _COSMO_SOURCE
char *cosmo_dlerror(void) libcesque;
void *cosmo_dlopen(const char *, int) libcesque;
void *cosmo_dlsym(void *, const char *) libcesque;
void *cosmo_dltramp(void *) libcesque;
int cosmo_dlclose(void *) libcesque;
#endif

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_DLFCN_H_ */
