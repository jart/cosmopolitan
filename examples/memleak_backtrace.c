#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include <cosmo.h>
#include <malloc.h>
#include <pthread.h>
#include <stdlib.h>

// cosmo bulitin memory leak detector demo
//
// you can use cosmo's memory leak detector to track backtraces for your
// memory allocations and then print error messages using addr2line. you
// generally want to have your app override malloc with your own malloc.
// you can use libc's builtin dlmalloc implementation like this as well.
// using this leak detector makes malloc() go 10x slower and allocations
// will each use 200 bytes of additional memory. if you don't care about
// backtraces, then you can create a leak detector for free by using the
// CheckForMemoryLeaks() function at the end of main().
//
//     $ make -j o//examples/memleak_backtrace
//     $ o//examples/memleak_backtrace
//     let's demo the builtin memory leak detector
//     error: memory leak detected 0x778dd0979fc0
//      42175f4 in malloc at /home/jart/cosmo/examples/memleak_backtrace.c:34
//      42049b7 in main at /home/jart/cosmo/examples/memleak_backtrace.c:67
//      4204db4 in cosmo at /home/jart/cosmo/libc/runtime/cosmo.S:103
//      42045d9 in _start at /home/jart/cosmo/libc/crt/crt.S:128
//     error: memory leak detected 0x778dd0979fe0
//      42175f4 in malloc at /home/jart/cosmo/examples/memleak_backtrace.c:34
//      42049aa in main at /home/jart/cosmo/examples/memleak_backtrace.c:66
//      4204db4 in cosmo at /home/jart/cosmo/libc/runtime/cosmo.S:103
//      42045d9 in _start at /home/jart/cosmo/libc/crt/crt.S:128
//

// wrap cosmopolitan's memory allocator to track allocations
static pthread_mutex_t mu = PTHREAD_MUTEX_INITIALIZER;
void *malloc(size_t n) {
  pthread_mutex_lock(&mu);
  void *r = cosmo_leak_track(__dlmalloc(n));
  pthread_mutex_unlock(&mu);
  return r;
}
void *calloc(size_t n, size_t z) {
  pthread_mutex_lock(&mu);
  void *r = cosmo_leak_track(__dlcalloc(n, z));
  pthread_mutex_unlock(&mu);
  return r;
}
void *realloc(void *p, size_t n) {
  pthread_mutex_lock(&mu);
  void *t = cosmo_leak_track(__dlrealloc(p, n));
  if (t && t != p)
    cosmo_leak_untrack(p);
  pthread_mutex_unlock(&mu);
  return t;
}
void *memalign(size_t a, size_t n) {
  pthread_mutex_lock(&mu);
  void *r = cosmo_leak_track(__dlmemalign(a, n));
  pthread_mutex_unlock(&mu);
  return r;
}
void free(void *p) {
  pthread_mutex_lock(&mu);
  __dlfree(cosmo_leak_untrack(p));
  pthread_mutex_unlock(&mu);
}

// the c standard says this function can't be optimized away
void *identity(void *arg) {
  return arg;
}
void *(*pIdentity)(void *) = identity;

int main(void) {
  kprintf("let's demo the builtin memory leak detector\n");

  // leak two allocations, one small, one large
  pIdentity(malloc(1));
  pIdentity(malloc(257));

  // wait for any detached posix threads to exit
  while (!pthread_orphan_np())
    pthread_decimate_np();

  // this runs atexit functions and thread/process finalizers
  cosmo_leak_finalize();

  // print information about memory leaks with backtraces
  if (cosmo_leak_print(0) > 0)
    _Exit(73);
}
