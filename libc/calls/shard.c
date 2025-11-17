// Copyright 2025 Justine Alexandra Roberts Tunney
//
// Permission to use, copy, modify, and/or distribute this software for
// any purpose with or without fee is hereby granted, provided that the
// above copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
// WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
// AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
// DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
// PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
// TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.

#include "ape/sections.internal.h"
#include "libc/calls/struct/rseq.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/cosmo.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/bsr.h"
#include "libc/intrin/likely.h"
#include "libc/intrin/weaken.h"
#include "libc/limits.h"
#include "libc/nexgen32e/rdtscp.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/nt/struct/processornumber.h"
#include "libc/nt/synchronization.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/runtime/syslib.internal.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"

static_assert(!(COSMO_SHARDS & (COSMO_SHARDS - 1)),
              "COSMO_SHARDS must be two-power");

__msabi extern typeof(GetCurrentProcessorNumberEx)
    *const __imp_GetCurrentProcessorNumberEx;

static inline unsigned cosmo_shard_murmur3(unsigned h) {
  /* It's important that we hash the thread id. For example, on Windows,
     the lowest two bits of the tid will always be zero. On Linux thread
     ids are usually sequential, like many other platforms. Simpler hash
     functions like Knuth's help with Windows but do bad when sequential */
  h ^= h >> 16;
  h *= 0x85ebca6b;
  h ^= h >> 13;
  h *= 0xc2b2ae35;
  h ^= h >> 16;
  return h;
}

static inline unsigned long cosmo_shard_modulus(unsigned idx) {
  idx &= COSMO_SHARDS - 1;
  return idx;
}

static unsigned long cosmo_shard_zero(void) {
  return 0;
}

static unsigned long cosmo_shard_tid(void) {
  int tid = atomic_load_explicit(&__get_tls()->tib_ptid, memory_order_relaxed);
  return cosmo_shard_modulus(cosmo_shard_murmur3(tid));
}

/* Linux 4.18+ (c. 2018) */
static unsigned long cosmo_shard_linux(void) {
  /* it's desirable to index shards using the id of the cpu on which
     this thread is currently running. Otherwise we will fall back to
     using the thread id which has more conflicts */
  return cosmo_shard_modulus(__get_rseq()->cpu_id);
}

static unsigned long cosmo_shard_silicon(void) {
  size_t cpu;
  __syslib->__pthread_cpu_number_np(&cpu);
  return cosmo_shard_modulus(cpu);
}

textwindows static unsigned long cosmo_shard_windows(void) {
  struct NtProcessorNumber pn;
  __imp_GetCurrentProcessorNumberEx(&pn);
  return cosmo_shard_modulus(64 * pn.Group + pn.Number);
}

#ifdef __x86_64__
static unsigned long cosmo_shard_rdtscp(void) {
  /* Only the Linux, FreeBSD, and Windows kernels can be counted upon to
     populate the TSC_AUX register with the current thread number. */
  unsigned tsc_aux;
  rdtscp(&tsc_aux);
  return cosmo_shard_modulus(TSC_AUX_CORE(tsc_aux));
}
#endif

static unsigned long cosmo_shard_pick(unsigned long impl(void)) {
  cosmo_shard = impl;
  return impl();
}

static int pthread_cpu_number_np(void) {
  size_t cpu = -1;
  if (__syslib->__version >= 9)
    __syslib->__pthread_cpu_number_np(&cpu);
  return cpu;
}

static unsigned long cosmo_shard_init(void) {
  if (!_weaken(pthread_create))
    return cosmo_shard_pick(cosmo_shard_zero);
  if (IsLinux())
    if (__get_rseq()->cpu_id >= 0)
      return cosmo_shard_pick(cosmo_shard_linux);
  if (IsXnuSilicon())
    if (pthread_cpu_number_np() >= 0)
      return cosmo_shard_pick(cosmo_shard_silicon);
  if (IsWindows())
    return cosmo_shard_pick(cosmo_shard_windows);
#ifdef __x86_64__
  if ((IsLinux() || IsFreebsd()) && X86_HAVE(RDTSCP))
    return cosmo_shard_pick(cosmo_shard_rdtscp);
#endif
  return cosmo_shard_pick(cosmo_shard_tid);
}

/**
 * Returns number on the interval `[0,COSMO_SHARDS)`.
 *
 * This function makes functions like malloc() scalable. We do that by
 * creating a fixed number of heaps indexed by sched_getcpu() modulo the
 * shard count. That's how this works in a nutshell except this function
 * solves additional issues.
 *
 * For starters sched_getcpu() isn't available on all platforms. So this
 * implementation will fall back to using the current thread id.
 *
 * This function is also designed to avoid sharding as much as possible.
 * Single threaded programs will always receive an index of zero so that
 * functions like malloc() don't need to create 32 separate heaps. With
 * multithreaded programs, we also consider the active thread count. You
 * won't need to initialize all 32 heaps unless you have 17+ threads.
 *
 * This function doesn't let you avoid locks entirely. Each of your
 * shards should have its own mutex, because two threads might get the
 * same index from this function at once. However you're not going to
 * care because uncontended mutexes cost next to nothing.
 */
__data_rarechange unsigned long (*cosmo_shard)(void) = cosmo_shard_init;
