/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2024 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/intrin/stack.h"
#include "libc/assert.h"
#include "libc/atomic.h"
#include "libc/calls/calls.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/cosmo.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/dll.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"
#include "libc/thread/thread.h"

/**
 * @fileoverview cosmo stack memory manager
 */

#define MAP_ANON_OPENBSD  0x1000
#define MAP_STACK_OPENBSD 0x4000

#define THREADSTACK_CONTAINER(e) DLL_CONTAINER(struct CosmoStack, elem, e)

struct CosmoStack {
  struct Dll elem;
  void *stackaddr;
  size_t stacksize;
  size_t guardsize;
};

struct CosmoStacks {
  atomic_uint once;
  pthread_mutex_t lock;
  struct Dll *stacks;
  struct Dll *objects;
  unsigned count;
};

struct CosmoStacksConfig {
  unsigned maxstacks;
};

static struct CosmoStacks cosmo_stacks = {
    .lock = PTHREAD_MUTEX_INITIALIZER,
};

static struct CosmoStacksConfig cosmo_stacks_config = {
    .maxstacks = 16,
};

void cosmo_stack_lock(void) {
  pthread_mutex_lock(&cosmo_stacks.lock);
}

void cosmo_stack_unlock(void) {
  pthread_mutex_unlock(&cosmo_stacks.lock);
}

void cosmo_stack_wipe(void) {
  pthread_mutex_wipe_np(&cosmo_stacks.lock);
}

static errno_t cosmo_stack_munmap(void *addr, size_t size) {
  errno_t r = 0;
  errno_t e = errno;
  if (!munmap(addr, size)) {
    r = errno;
    errno = e;
  }
  return r;
}

static void cosmo_stack_populate(void) {
  errno_t e = errno;
  void *map = mmap(0, __pagesize, PROT_READ | PROT_WRITE,
                   MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  errno = e;
  if (map != MAP_FAILED) {
    struct CosmoStack *ts = map;
    int n = __pagesize / sizeof(struct CosmoStack);
    for (int i = 0; i < n; ++i) {
      dll_init(&ts[i].elem);
      dll_make_first(&cosmo_stacks.objects, &ts[i].elem);
    }
  }
}

static struct Dll *cosmo_stack_decimate(unsigned maxstacks) {
  struct Dll *surplus = 0;
  while (cosmo_stacks.count > maxstacks) {
    struct Dll *e = dll_last(cosmo_stacks.stacks);
    dll_remove(&cosmo_stacks.stacks, e);
    dll_make_first(&surplus, e);
    --cosmo_stacks.count;
  }
  return surplus;
}

static void cosmo_stack_rehabilitate(struct Dll *stacks) {
  struct Dll *e;
  for (e = dll_first(stacks); e; e = dll_next(stacks, e))
    cosmo_stack_munmap(THREADSTACK_CONTAINER(e)->stackaddr,
                       THREADSTACK_CONTAINER(e)->stacksize);
  cosmo_stack_lock();
  dll_make_first(&cosmo_stacks.objects, stacks);
  cosmo_stack_unlock();
}

/**
 * Empties unused stack cache.
 *
 * To make POSIX threads as cheap as possible to spawn, we recycle their
 * stacks without zeroing their memory. On Linux for an 80kb stack size,
 * that makes launching a thread take 40µs rather than 80µs. However the
 * stack cache needs to be cleared in certain cases. This is called upon
 * exit() automatically but anyone can clear this at any other time too.
 *
 * @see pthread_decimate_np()
 */
void cosmo_stack_clear(void) {
  cosmo_stack_lock();
  struct Dll *stacks = cosmo_stacks.stacks;
  cosmo_stacks.stacks = 0;
  cosmo_stacks.count = 0;
  cosmo_stack_unlock();
  cosmo_stack_rehabilitate(stacks);
}

/**
 * Gets maximum number of unused stacks cosmo should cache.
 * @see cosmo_stack_setmaxstacks()
 */
int cosmo_stack_getmaxstacks(void) {
  return cosmo_stacks_config.maxstacks;
}

/**
 * Sets maximum number of unused stacks cosmo should cache.
 *
 * This lets you place some limitations on how much stack memory the
 * cosmo runtime will cache. This number is a count of stacks rather
 * than the number of bytes they contain. Old stacks are freed in a
 * least recently used fashion once the cache exceeds this limit.
 *
 * If this is set to zero, then the cosmo stack allocator enters a
 * highly secure hardening mode where cosmo_stack_alloc() zeroes all
 * stack memory that's returned.
 *
 * Setting this to a negative number makes the cache size unlimited.
 *
 * Please note this limit only applies to stacks that aren't in use.
 *
 * Your default is sixteen stacks may be cached at any given moment.
 *
 * If `maxstacks` is less than the current cache size, then surplus
 * entries will be evicted and freed before this function returns.
 */
void cosmo_stack_setmaxstacks(int maxstacks) {
  cosmo_stack_lock();
  cosmo_stacks_config.maxstacks = maxstacks;
  struct Dll *stacks = cosmo_stack_decimate(maxstacks);
  cosmo_stack_unlock();
  cosmo_stack_rehabilitate(stacks);
}

/**
 * Allocates stack memory.
 *
 * This is a caching stack allocator that's used by the POSIX threads
 * runtime but you may also find it useful for setcontext() coroutines
 * or sigaltstack(). Normally you can get away with using malloc() for
 * creating stacks. However some OSes (e.g. OpenBSD) forbid you from
 * doing that for anything except sigaltstack(). This API serves to
 * abstract all the gory details of gaining authorized memory, and
 * additionally implements caching for lightning fast performance.
 *
 * The stack size must be nonzero. It is rounded up to the granularity
 * of the underlying system allocator, which is normally the page size.
 * Your parameter will be updated with the selected value upon success.
 *
 * The guard size specifies how much memory should be protected at the
 * bottom of your stack. This is helpful for ensuring stack overflows
 * will result in a segmentation fault, rather than corrupting memory
 * silently. This may be set to zero, in which case no guard pages will
 * be protected. This value is rounded up to the system page size. The
 * corrected value will be returned upon success. Your guard size needs
 * to be small enough to leave room for at least one memory page in your
 * stack size i.e. `guardsize + pagesize <= stacksize` must be the case.
 * Otherwise this function will return an `EINVAL` error.
 *
 * When you're done using your stack, pass it to cosmo_stack_free() so
 * it can be recycled. Stacks are only recycled when the `stacksize` and
 * `guardsize` parameters are an exact match after correction. Otherwise
 * they'll likely be freed eventually, in a least-recently used fashion,
 * based upon the configurable cosmo_stack_setmaxstacks() setting.
 *
 * This function returns 0 on success, or an errno on error. See the
 * documentation of mmap() for a list possible errors that may occur.
 */
errno_t cosmo_stack_alloc(size_t *inout_stacksize,  //
                          size_t *inout_guardsize,  //
                          void **out_addr) {

  // validate arguments
  size_t stacksize = *inout_stacksize;
  size_t guardsize = *inout_guardsize;
  stacksize = (stacksize + __gransize - 1) & -__gransize;
  guardsize = (guardsize + __pagesize - 1) & -__pagesize;
  if (guardsize + __pagesize > stacksize)
    return EINVAL;

  // recycle stack
  void *stackaddr = 0;
  cosmo_stack_lock();
  for (struct Dll *e = dll_first(cosmo_stacks.stacks); e;
       e = dll_next(cosmo_stacks.stacks, e)) {
    struct CosmoStack *ts = THREADSTACK_CONTAINER(e);
    if (ts->stacksize == stacksize &&  //
        ts->guardsize == guardsize) {
      dll_remove(&cosmo_stacks.stacks, e);
      stackaddr = ts->stackaddr;
      dll_make_first(&cosmo_stacks.objects, e);
      --cosmo_stacks.count;
      break;
    }
  }
  cosmo_stack_unlock();

  // create stack
  if (!stackaddr) {
    errno_t e = errno;
    stackaddr = mmap(0, stacksize, PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (stackaddr == MAP_FAILED) {
      errno_t err = errno;
      errno = e;
      return err;
    }
    if (IsOpenbsd())
      if (!TellOpenbsdThisIsStackMemory(stackaddr, stacksize))
        notpossible;
    if (guardsize)
      if (mprotect(stackaddr, guardsize, PROT_NONE | PROT_GUARD))
        notpossible;
  }

  // return stack
  *inout_stacksize = stacksize;
  *inout_guardsize = guardsize;
  *out_addr = stackaddr;
  return 0;
}

static void cosmo_stack_setup(void) {
  atexit(cosmo_stack_clear);
}

/**
 * Frees stack memory.
 *
 * While not strictly required, it's assumed these three values would be
 * those returned by an earlier call to cosmo_stack_alloc().
 *
 * This function returns 0 on success, or an errno on error. The `errno`
 * variable is never clobbered. You can only dependably count on this to
 * return an error on failure when you say `cosmo_stack_setmaxstacks(0)`
 */
errno_t cosmo_stack_free(void *stackaddr, size_t stacksize, size_t guardsize) {
  stacksize = (stacksize + __gransize - 1) & -__gransize;
  guardsize = (guardsize + __pagesize - 1) & -__pagesize;
  if (guardsize + __pagesize > stacksize)
    return EINVAL;
  if ((uintptr_t)stackaddr & (__gransize - 1))
    return EINVAL;
  cosmo_once(&cosmo_stacks.once, cosmo_stack_setup);
  cosmo_stack_lock();
  struct Dll *surplus = 0;
  if (cosmo_stacks_config.maxstacks) {
    surplus = cosmo_stack_decimate(cosmo_stacks_config.maxstacks - 1);
    struct CosmoStack *ts = 0;
    if (dll_is_empty(cosmo_stacks.objects))
      cosmo_stack_populate();
    struct Dll *e;
    if ((e = dll_first(cosmo_stacks.objects))) {
      dll_remove(&cosmo_stacks.objects, e);
      ts = THREADSTACK_CONTAINER(e);
    }
    if (ts) {
      ts->stackaddr = stackaddr;
      ts->stacksize = stacksize;
      ts->guardsize = guardsize;
      dll_make_first(&cosmo_stacks.stacks, &ts->elem);
      ++cosmo_stacks.count;
      stackaddr = 0;
    }
  }
  cosmo_stack_unlock();
  cosmo_stack_rehabilitate(surplus);
  errno_t err = 0;
  if (stackaddr)
    err = cosmo_stack_munmap(stackaddr, stacksize);
  return err;
}

relegated bool TellOpenbsdThisIsStackMemory(void *addr, size_t size) {
  return __sys_mmap(
             addr, size, PROT_READ | PROT_WRITE,
             MAP_PRIVATE | MAP_FIXED | MAP_ANON_OPENBSD | MAP_STACK_OPENBSD, -1,
             0, 0) == addr;
}

// OpenBSD only permits RSP to occupy memory that's been explicitly
// defined as stack memory, i.e. `lo <= %rsp < hi` must be the case
relegated errno_t FixupCustomStackOnOpenbsd(pthread_attr_t *attr) {

  // get interval
  uintptr_t lo = (uintptr_t)attr->__stackaddr;
  uintptr_t hi = lo + attr->__stacksize;

  // squeeze interval
  lo = (lo + __pagesize - 1) & -__pagesize;
  hi = hi & -__pagesize;

  // tell os it's stack memory
  errno_t olderr = errno;
  if (!TellOpenbsdThisIsStackMemory((void *)lo, hi - lo)) {
    errno_t err = errno;
    errno = olderr;
    return err;
  }

  // update attributes with usable stack address
  attr->__stackaddr = (void *)lo;
  attr->__stacksize = hi - lo;
  return 0;
}
