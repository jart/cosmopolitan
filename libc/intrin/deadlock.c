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
#include "ape/sections.internal.h"
#include "libc/assert.h"
#include "libc/atomic.h"
#include "libc/cosmo.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/maps.h"
#include "libc/macros.h"
#include "libc/str/str.h"
#include "libc/thread/lock.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"

/**
 * @fileoverview deadlock detector for statically allocated locks
 *
 * This module helps you spot multi-threading bugs in your program.
 * High-level abstractions like mutexes are much easier to use than
 * atomics, but they still carry their own non-obvious dangers. For
 * example, nesting locks need to be nested in a consistent way and
 * normal mutexes can't be required recursively. Normally this will
 * cause your program to deadlock, i.e. hang indefinitely, but this
 * module can detect such conditions and return errors instead, and
 * better yet print helpful information when using `cosmocc -mdbg`.
 */

#define ABI privileged optimizesize

// building our visitor function using this optimizesize keyword shrinks
// the stack memory requirement from 7168 to 2048 bytes. totally amazing
// although please note this maximum isn't a hard limit. for normal mode
// builds your posix mandated mutex error checking will be less accurate
// but still helpful and reliable, although your cosmocc -mdbg will trap
// and report that you've run into the limit, so you can talk to justine
#define MAX_LOCKS 64

// cosmo's tib reserves space for 64 nested locks before things degrade.
// the cosmopolitan c runtime defines 16 locks, which are all registered
// with pthread_atfork(). it means you get to have 48 mutexes right now,
// and if you register all of them, then calling fork() will cause there
// to be 2080 edges in your lock graph. talk to justine if you need more
// because we're obviously going to need to find a way to make this grow
#define LOCK_EDGES_MAX 2080

// supported lock objects must define `void *_edges`
#define LOCK_EDGES_OFFSET 0
static_assert(offsetof(struct MapLock, edges) == LOCK_EDGES_OFFSET);
static_assert(offsetof(pthread_mutex_t, _edges) == LOCK_EDGES_OFFSET);

struct LockEdge {
  struct LockEdge *next;
  void *dest;
};

struct VisitedLock {
  struct VisitedLock *next;
  void *lock;
};

typedef _Atomic(struct LockEdge *) LockEdges;

static struct DeadlockDetector {
  atomic_size_t edges_allocated;
  struct LockEdge edges_memory[LOCK_EDGES_MAX];
} __deadlock;

forceinline struct CosmoTib *__deadlock_tls(void) {
  return __get_tls_privileged();
}

forceinline LockEdges *get_lock_edges(void *lock) {
  return (LockEdges *)((char *)lock + LOCK_EDGES_OFFSET);
}

forceinline struct LockEdge *load_lock_edges(LockEdges *edges) {
  return atomic_load_explicit(edges, memory_order_relaxed);
}

ABI static int is_static_memory(void *lock) {
  return _etext <= (unsigned char *)lock && (unsigned char *)lock < _end;
}

ABI static struct LockEdge *__deadlock_alloc(void) {
  size_t edges_allocated =
      atomic_load_explicit(&__deadlock.edges_allocated, memory_order_relaxed);
  for (;;) {
    if (edges_allocated == LOCK_EDGES_MAX) {
      if (IsModeDbg()) {
        kprintf("error: cosmo LOCK_EDGES_MAX needs to be increased\n");
        DebugBreak();
      }
      return 0;
    }
    if (atomic_compare_exchange_weak_explicit(
            &__deadlock.edges_allocated, &edges_allocated, edges_allocated + 1,
            memory_order_relaxed, memory_order_relaxed))
      return &__deadlock.edges_memory[edges_allocated];
  }
}

ABI static void __deadlock_add_edge(void *from, void *dest) {
  LockEdges *edges = get_lock_edges(from);
  for (struct LockEdge *e = load_lock_edges(edges); e; e = e->next)
    if (e->dest == dest)
      return;
  struct LockEdge *edge;
  if ((edge = __deadlock_alloc())) {
    edge->next = load_lock_edges(edges);
    edge->dest = dest;
    // we tolerate duplicate elements in the interest of performance.
    // once an element is inserted, it's never removed. that's why we
    // don't need need to worry about the aba problem. the cas itself
    // is very important since it ensures inserted edges aren't lost.
    for (;;)
      if (atomic_compare_exchange_weak_explicit(edges, &edge->next, edge,
                                                memory_order_relaxed,
                                                memory_order_relaxed))
        break;
  }
}

ABI static bool __deadlock_visit(void *lock, struct VisitedLock *visited,
                                 int notrap, int depth) {
  if (++depth == MAX_LOCKS) {
    if (IsModeDbg()) {
      kprintf("error: too much recursion in deadlock detector\n");
      DebugBreak();
    }
    return false;
  }
  for (struct VisitedLock *v = visited; v; v = v->next) {
    if (v->lock == lock) {
      if (IsModeDbg() && !notrap) {
        // lock hierarchy violated!
        //
        // when you lock mutexes in a nested way, your locks must be
        // nested in the same order globally. otherwise deadlocks might
        // occur. for example, if you say in your first thread
        //
        //     pthread_mutex_lock(&x);
        //     pthread_mutex_lock(&y);
        //     pthread_mutex_unlock(&y);
        //     pthread_mutex_unlock(&x);
        //
        // then in your second thread you say
        //
        //     pthread_mutex_lock(&y);
        //     pthread_mutex_lock(&x);
        //     pthread_mutex_unlock(&x);
        //     pthread_mutex_unlock(&y);
        //
        // then a deadlock might happen, because {x→y, y→x} is cyclic!
        // they don't happen often, but this is the kind of thing that
        // matters if you want to build carrier grade production stuff
        kprintf("error: cycle detected in directed graph of nested locks\n");
        for (struct VisitedLock *v = visited; v; v = v->next)
          kprintf("\t- %t\n", v->lock);  // strongly connected component
        DebugBreak();
      }
      return true;
    }
  }
  LockEdges *edges = get_lock_edges(lock);
  struct VisitedLock visit = {visited, lock};
  for (struct LockEdge *e = load_lock_edges(edges); e; e = e->next)
    if (__deadlock_visit(e->dest, &visit, notrap, depth))
      return true;
  return false;
}

/**
 * Returns true if lock is already locked by calling thread.
 *
 * This function may return false negatives if we run out of TLS memory.
 * That suboptimal condition will be reported in debug mode.
 *
 * @return 1 if lock is certainly owned by calling thread, 0 if lock is
 *     certainly not owned by calling thread, and -1 if we're uncertain
 */
ABI int __deadlock_tracked(void *lock) {
  int full = 1;
  int owned = 0;
  struct CosmoTib *tib = __deadlock_tls();
  for (int i = 0; i < ARRAYLEN(tib->tib_locks); ++i) {
    full &= tib->tib_locks[i] != NULL;
    owned |= tib->tib_locks[i] == lock;
  }
  if (full)
    return -1;
  if (!owned && !is_static_memory(lock))
    return -1;
  return owned;
}

/**
 * Records that lock is held by thread.
 * @param notrap can prevent error printing and debug breaking
 * @asyncsignalsafe
 */
ABI void __deadlock_track(void *lock, int notrap) {
  if (!notrap && !is_static_memory(lock))
    return;
  struct CosmoTib *tib = __deadlock_tls();
  for (int i = 0; i < ARRAYLEN(tib->tib_locks); ++i) {
    if (!tib->tib_locks[i]) {
      tib->tib_locks[i] = lock;
      return;
    }
  }
  if (IsModeDbg()) {
    kprintf("error: cosmo tls max lock depth needs to be increased!\n");
    DebugBreak();
  }
}

/**
 * Records relationship for all held locks to `lock`.
 * @param notrap can prevent error printing and debug breaking
 * @asyncsignalsafe
 */
ABI void __deadlock_record(void *lock, int notrap) {
  if (!notrap && !is_static_memory(lock))
    return;
  struct CosmoTib *tib = __deadlock_tls();
  for (int i = 0; i < ARRAYLEN(tib->tib_locks); ++i)
    if (tib->tib_locks[i] && tib->tib_locks[i] != lock)
      __deadlock_add_edge(tib->tib_locks[i], lock);
}

/**
 * Returns EDEADLK if locking `lock` could cause a deadlock.
 * @param notrap can prevent error printing and debug breaking
 * @asyncsignalsafe
 */
ABI int __deadlock_check(void *lock, int notrap) {
  struct CosmoTib *tib = __deadlock_tls();
  for (int i = 0; i < ARRAYLEN(tib->tib_locks); ++i) {
    if (tib->tib_locks[i] == lock)
      return 0;
    if (tib->tib_locks[i]) {
      struct VisitedLock visit = {0, tib->tib_locks[i]};
      if (__deadlock_visit(lock, &visit, notrap, 0))
        return EDEADLK;
    }
  }
  return 0;
}

/**
 * Records that lock isn't held by thread.
 * @asyncsignalsafe
 */
ABI void __deadlock_untrack(void *lock) {
  struct CosmoTib *tib = __deadlock_tls();
  for (int i = 0; i < ARRAYLEN(tib->tib_locks); ++i)
    tib->tib_locks[i] = tib->tib_locks[i] != lock ? tib->tib_locks[i] : 0;
}
