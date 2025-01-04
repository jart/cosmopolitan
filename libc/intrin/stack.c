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
#include "libc/dlopen/dlfcn.h"
#include "libc/errno.h"
#include "libc/intrin/describeflags.h"
#include "libc/intrin/dll.h"
#include "libc/intrin/maps.h"
#include "libc/intrin/rlimit.h"
#include "libc/intrin/strace.h"
#include "libc/intrin/weaken.h"
#include "libc/limits.h"
#include "libc/macros.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/internal.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread.h"

/**
 * @fileoverview cosmo stack memory manager
 */

#define MAP_GROWSDOWN_LINUX 0x00000100
#define MAP_ANONYMOUS_LINUX 0x00000020
#define MAP_NOREPLACE_LINUX 0x08000000
#define MAP_NORESERVE_LINUX 0x00004000

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
    .maxstacks = 3,
};

void cosmo_stack_lock(void) {
  _pthread_mutex_lock(&cosmo_stacks.lock);
}

void cosmo_stack_unlock(void) {
  _pthread_mutex_unlock(&cosmo_stacks.lock);
}

void cosmo_stack_wipe(void) {
  _pthread_mutex_wipe_np(&cosmo_stacks.lock);
}

// map_growsdown will not grow more than rlimit_stack
static size_t cosmo_stack_maxgrow(void) {
  return __rlimit_stack_get().rlim_cur & -__pagesize;
}

// allocates private anonymous fixed noreplace memory on linux
static void *flixmap(void *addr, size_t size, int prot, int flags) {
  flags |= MAP_PRIVATE | MAP_ANONYMOUS_LINUX | MAP_NOREPLACE_LINUX;
  void *res = __sys_mmap(addr, size, prot, flags, -1, 0, 0);
  if (res != MAP_FAILED) {
    if (res != addr) {
      sys_munmap(addr, size);
      errno = EEXIST;  // polyfill linux 4.17+ behavior
      res = 0;
    }
  } else {
    res = 0;
  }
  STRACE("mmap(%p, %'zu, %s, %s) → %p% m", addr, size, DescribeProtFlags(prot),
         DescribeMapFlags(flags), res);
  return res;
}

// maps stack on linux
static void *slackmap(size_t stacksize, size_t guardsize) {
  int olde = errno;
  struct Map *prev, *map;
  char *max = (char *)PTRDIFF_MAX;
  size_t need = guardsize + stacksize;
  __maps_lock();
  for (;;) {

    // look for empty space beneath higher mappings
    char *region = 0;
    for (map = __maps_floor(max); map; map = prev) {
      char *min = (char *)(intptr_t)__gransize;
      if ((prev = __maps_prev(map)))
        min = prev->addr + ROUNDUP(prev->size, __gransize);
      if (map->addr - min >= need) {
        region = map->addr - need;
        max = region - 1;
        break;
      }
    }
    if (!region)
      break;

    // track intended memory in rbtree
    if (!__maps_track(region, guardsize, PROT_NONE,
                      MAP_PRIVATE | MAP_ANONYMOUS_LINUX))
      break;
    if (!__maps_track(region + guardsize, stacksize, PROT_READ | PROT_WRITE,
                      MAP_PRIVATE | MAP_ANONYMOUS_LINUX)) {
      __maps_untrack(region, need);
      break;
    }
    __maps_unlock();

    // ask kernel to create guard region
    // taking special care to not clobber untracked mappings
    //
    // it's important that this call happen first, since it limits how
    // much memory map_growsdown will secretly consume. if there's
    // nothing beneath a map_growsdown mapping, then the kernel reserves
    // (and this isn't listed /proc/PID/maps so don't bother looking)
    // `rlimit_stack.rlim_cur & -__pagesize` bytes of memory including
    // this top-most page, and another 1mb of guard pages beneath that.
    // but by mapping our guard pages manually, we ensure the guard
    // region and the stack itself will be exactly as big as we want.
    //
    // you'd think we could mmap(0, pagesz, growsdown) to let the kernel
    // pick an address and then we could just upscale the user's stack
    // size request to whatever rlimit_stack is if it's bigger. but the
    // linux kernel will actually choose addresses between existing maps
    // where the hole is smaller than rlimit_stack.
    //
    // to use map_growsdown, we must use map_fixed. normally when we use
    // map_fixed, we reserve an entire kernel-assigned region beforehand
    // to ensure there isn't any overlap with existing mappings. however
    // since growsdown stops growing when it encounters another mapping,
    // you can't map it on top of a reservation mapping. so we must take
    // a leap of faith there aren't any mystery mappings twixt the guard
    // region and growsdown page below.
    char *guard_region =
        flixmap(region, guardsize, PROT_NONE, MAP_NORESERVE_LINUX);
    if (!guard_region) {
    RecoverFromMmapFailure:
      if (errno != EEXIST) {
        // mmap() probably raised enomem due to rlimit_as etc.
        __maps_untrack(region, need);
        return 0;
      } else {
        // we've encountered a mystery mapping. it's hard to imagine
        // this happening, since we don't use map_growsdown when
        // cosmo_dlopen() is linked in the binary. in that case, the
        // tracker we created covers at least some of the rogue map,
        // therefore this issue should fix itself if we keep going
        errno = olde;
        __maps_lock();
        ++max;
        continue;
      }
    }

    // ask kernel to create stack pages
    // taking special care to not clobber untracked mappings
    char *top_page = flixmap(region + need - __pagesize, __pagesize,
                             PROT_READ | PROT_WRITE, MAP_GROWSDOWN_LINUX);
    if (!top_page) {
      sys_munmap(region, guardsize);
      goto RecoverFromMmapFailure;
    }

    // return address to bottom of stack
    return region + guardsize;
  }
  __maps_unlock();
  errno = ENOMEM;
  return 0;
}

static errno_t cosmo_stack_munmap(char *stackaddr, size_t stacksize,
                                  size_t guardsize) {
  errno_t r = 0;
  errno_t e = errno;
  if (!munmap(stackaddr - guardsize,  //
              guardsize + stacksize)) {
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
                       THREADSTACK_CONTAINER(e)->stacksize,
                       THREADSTACK_CONTAINER(e)->guardsize);
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
 * Your default is three stacks may be cached at any given moment.
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
 * The stack size must be nonzero. It specifies the minimum amount of
 * stack space that will be available for use. The provided value is
 * rounded up to the system page size. It may be increased further for
 * various reasons. Your stack size parameter will be updated with the
 * chosen value upon success.
 *
 * The guard size specifies the minimum amount of memory that should be
 * protected beneath your stack. This helps ensure stack overflows cause
 * a segfault rather than corrupting memory silently. This may be set to
 * zero in which case no guard pages will be made. This value is rounded
 * up to the system page size. The corrected value will be returned upon
 * success. Your guard size needs to be small enough to leave room for
 * at least one memory page in your stack size i.e. `guardsize +
 * pagesize <= stacksize` must be the case. Otherwise this function will
 * return an `EINVAL` error.
 *
 * When you're done using your stack, pass it to cosmo_stack_free() so
 * it can be recycled. Stacks are only recycled when the `stacksize` and
 * `guardsize` parameters match the constraints described above. Stacks
 * that don't end up getting reused will be freed eventually, in a least
 * recently used way based upon your cosmo_stack_setmaxstacks() setting.
 *
 * This function returns 0 on success, or an errno on error. See the
 * documentation of mmap() for a list possible errors that may occur.
 */
errno_t cosmo_stack_alloc(size_t *inout_stacksize,  //
                          size_t *inout_guardsize,  //
                          void **out_stackaddr) {

  // validate arguments
  size_t stacksize = *inout_stacksize;
  size_t guardsize = *inout_guardsize;
  stacksize = (stacksize + __pagesize - 1) & -__pagesize;
  guardsize = (guardsize + __pagesize - 1) & -__pagesize;
  if (!stacksize)
    return EINVAL;

  // recycle stack
  void *stackaddr = 0;
  cosmo_stack_lock();
  for (struct Dll *e = dll_first(cosmo_stacks.stacks); e;
       e = dll_next(cosmo_stacks.stacks, e)) {
    struct CosmoStack *ts = THREADSTACK_CONTAINER(e);
    if (ts->stacksize == stacksize &&  //
        ts->guardsize == guardsize) {
      stackaddr = ts->stackaddr;
      stacksize = ts->stacksize;
      guardsize = ts->guardsize;
      dll_remove(&cosmo_stacks.stacks, e);
      dll_make_first(&cosmo_stacks.objects, e);
      --cosmo_stacks.count;
      break;
    }
  }
  cosmo_stack_unlock();

  // create stack
  if (!stackaddr) {
    errno_t olde = errno;
    if (!IsTiny() && IsLinux() && guardsize && !_weaken(cosmo_dlopen) &&
        stacksize <= cosmo_stack_maxgrow() && !IsQemuUser()) {
      // this special linux-only stack allocator significantly reduces
      // the consumption of virtual memory.
      if (!(stackaddr = slackmap(stacksize, guardsize))) {
        errno_t err = errno;
        errno = olde;
        return err;
      }
    } else {
      char *map = mmap(0, guardsize + stacksize, PROT_READ | PROT_WRITE,
                       MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
      if (map == MAP_FAILED) {
        errno_t err = errno;
        errno = olde;
        return err;
      }
      stackaddr = map + guardsize;
      if (IsOpenbsd())
        if (!TellOpenbsdThisIsStackMemory(stackaddr, stacksize))
          notpossible;
      if (guardsize) {
        if (mprotect(map, guardsize, PROT_NONE | PROT_GUARD)) {
          errno_t err = errno;
          munmap(map, guardsize + stacksize);
          errno = olde;
          return err;
        }
      }
    }
  }

  // return stack
  *inout_stacksize = stacksize;
  *inout_guardsize = guardsize;
  *out_stackaddr = stackaddr;
  return 0;
}

static void cosmo_stack_setup(void) {
  atexit(cosmo_stack_clear);
}

/**
 * Frees stack memory.
 *
 * While not strictly required, it's assumed the three parameters are
 * those returned by an earlier call to cosmo_stack_alloc(). If they
 * aren't page aligned and rounded, this function will return EINVAL.
 *
 * This function returns 0 on success, or an errno on error. The `errno`
 * variable is never clobbered. You can only dependably count on this to
 * return an error on failure when you say `cosmo_stack_setmaxstacks(0)`
 */
errno_t cosmo_stack_free(void *stackaddr, size_t stacksize, size_t guardsize) {
  if (!stacksize)
    return EINVAL;
  if (stacksize & (__pagesize - 1))
    return EINVAL;
  if (guardsize & (__pagesize - 1))
    return EINVAL;
  if ((uintptr_t)stackaddr & (__pagesize - 1))
    return EINVAL;
  cosmo_stack_lock();
  struct Dll *surplus = 0;
  if (cosmo_stacks_config.maxstacks) {
    cosmo_once(&cosmo_stacks.once, cosmo_stack_setup);
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
    err = cosmo_stack_munmap(stackaddr, stacksize, guardsize);
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
relegated bool FixupCustomStackOnOpenbsd(pthread_attr_t *attr) {

  // get interval
  uintptr_t lo = (uintptr_t)attr->__stackaddr;
  uintptr_t hi = lo + attr->__stacksize;

  // squeeze interval
  lo = (lo + __pagesize - 1) & -__pagesize;
  hi = hi & -__pagesize;

  // tell os it's stack memory
  if (!TellOpenbsdThisIsStackMemory((void *)lo, hi - lo))
    return false;

  // update attributes with usable stack address
  attr->__stackaddr = (void *)lo;
  attr->__stacksize = hi - lo;
  return true;
}
