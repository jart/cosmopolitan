#ifndef COSMOPOLITAN_LIBC_COSMO_H_
#define COSMOPOLITAN_LIBC_COSMO_H_
#include "libc/calls/struct/timespec.h"
COSMOPOLITAN_C_START_

#ifndef __cplusplus
#define _COSMO_ATOMIC(x) _Atomic(x)
#else
#define _COSMO_ATOMIC(x) x
#endif

typedef _COSMO_ATOMIC(int) cosmo_futex_t;
typedef _COSMO_ATOMIC(unsigned) cosmo_once_t;

errno_t cosmo_once(cosmo_once_t *, void (*)(void));
int cosmo_cpu_count(void) pureconst libcesque;
void cosmo_warmup_directory(const char *, int);
int systemvpe(const char *, char *const[], char *const[]) libcesque;
char *GetProgramExecutableName(void) libcesque;
void unleaf(void) libcesque;
bool32 IsLinuxModern(void) libcesque;
void AssertNoLocksAreHeld(void);
void CheckForMemoryLeaks(void);
double cosmo_entropy(const char *, size_t) libcesque;

int cosmo_demangle(char *, const char *, size_t) libcesque;
int cosmo_is_mangled(const char *) libcesque;

int cosmo_args(const char *, char ***) libcesque;
int LoadZipArgs(int *, char ***) libcesque;

int cosmo_futex_wake(cosmo_futex_t *, int, char);
int cosmo_futex_wait(cosmo_futex_t *, int, char, int, const struct timespec *);

libcesque void *cosmo_permalloc(size_t) attributeallocsize((1))
    returnsaligned((8)) returnspointerwithnoaliases;

errno_t cosmo_stack_alloc(size_t *, size_t *, void **) libcesque;
errno_t cosmo_stack_free(void *, size_t, size_t) libcesque;
void cosmo_stack_clear(void) libcesque;
void cosmo_stack_setmaxstacks(int) libcesque;
int cosmo_stack_getmaxstacks(void) libcesque;

int __deadlock_check(void *, int) libcesque;
int __deadlock_tracked(void *) libcesque;
void __deadlock_record(void *, int) libcesque;
void __deadlock_track(void *, int) libcesque;
void __deadlock_untrack(void *) libcesque;

void *cosmo_leak_track(void *);
void *cosmo_leak_untrack(void *);
void cosmo_leak_finalize(void);
int cosmo_leak_print(int (*)(void *));

extern void *(*__dlmalloc)(size_t);
extern void (*__dlfree)(void *);
extern void *(*__dlcalloc)(size_t, size_t);
extern void *(*__dlrealloc)(void *, size_t);
extern void *(*__dlrealloc_in_place)(void *, size_t);
extern void *(*__dlmemalign)(size_t, size_t);
extern void (*__dlmalloc_inspect_all)(void (*)(void *, void *, size_t, void *),
                                      void *);
extern struct mallinfo (*__dlmallinfo)(void);
extern int (*__dlmalloc_trim)(size_t);
extern size_t (*__dlmalloc_usable_size)(void *);

#define COSMO_SHARDS 32
extern unsigned long (*cosmo_shard)(void);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_COSMO_H_ */
