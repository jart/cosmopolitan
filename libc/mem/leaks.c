/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/mem/leaks.h"
#include "libc/cxxabi.h"
#include "libc/intrin/cxaatexit.h"
#include "libc/intrin/dll.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/weaken.h"
#include "libc/macros.h"
#include "libc/mem/mem.h"
#include "libc/nt/typedef/imagetlscallback.h"
#include "libc/runtime/runtime.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"

#define LEAK_CONTAINER(e) DLL_CONTAINER(struct Leak, elem, e)

struct Leak {
  void *alloc;
  struct Dll elem;
};

static int leak_count;
static struct Dll *leaks;
static struct Dll *freaks;
static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void __may_leak(void *alloc) {
  if (!alloc)
    return;
  _pthread_mutex_lock(&lock);
  if (dll_is_empty(freaks)) {
    int g = __gransize;
    struct Leak *p = _mapanon(g);
    int n = g / sizeof(struct Leak);
    for (int i = 0; i < n; ++i) {
      dll_init(&p[i].elem);
      dll_make_first(&freaks, &p[i].elem);
    }
  }
  struct Dll *e = dll_first(freaks);
  LEAK_CONTAINER(e)->alloc = alloc;
  dll_remove(&freaks, e);
  dll_make_first(&leaks, e);
  _pthread_mutex_unlock(&lock);
}

static void visitor(void *start, void *end, size_t used_bytes, void *arg) {
  if (!used_bytes)
    return;
  for (struct Dll *e = dll_first(leaks); e; e = dll_next(leaks, e))
    if (start == LEAK_CONTAINER(e)->alloc)
      return;
  kprintf("error: leaked %'zu byte allocation at %p\n", used_bytes, start);
  ++leak_count;
}

/**
 * Tests for memory leaks.
 */
void CheckForMemoryLeaks(void) {

  // validate usage of this api
  if (_weaken(_pthread_decimate))
    _weaken(_pthread_decimate)(kPosixThreadZombie);
  if (!pthread_orphan_np())
    kprintf("warning: called CheckForMemoryLeaks() from non-orphaned thread\n");

  // call dynamic global destructors
  __cxa_thread_finalize();
  __cxa_finalize(0);

  // check for leaks
  malloc_inspect_all(visitor, 0);
  if (leak_count) {
    kprintf("       you forgot to call free %'d time%s\n", leak_count,
            leak_count == 1 ? "" : "s");
    _exit(73);
  }
}

static bool IsHoldingLocks(struct CosmoTib *tib) {
  for (int i = 0; i < ARRAYLEN(tib->tib_locks); ++i)
    if (tib->tib_locks[i])
      return true;
  return false;
}

/**
 * Aborts if any locks are held by calling thread.
 */
void AssertNoLocksAreHeld(void) {
  struct CosmoTib *tib = __get_tls();
  if (IsHoldingLocks(tib)) {
    kprintf("error: the following locks are held by this thread:\n");
    for (int i = 0; i < ARRAYLEN(tib->tib_locks); ++i)
      if (tib->tib_locks[i])
        kprintf("\t- %t\n", tib->tib_locks[i]);
    _Exit(74);
  }
}
