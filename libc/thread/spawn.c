/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/errno.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/runtime/clone.internal.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/stack.h"
#include "libc/stdalign.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/clone.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/spawn.h"
#include "libc/thread/tls.h"
#include "libc/thread/wait0.internal.h"

/**
 * @fileoverview Simple threading API
 *
 * This API is supported on all six operating systems. We have this
 * because the POSIX threads API is positively enormous. We currently
 * only implement a small subset of POSIX threads, e.g. mutexes. So
 * until we can implement all of POSIX threads, this API is great. If we
 * consider that the classic forking concurrency library consists of a
 * single function, it's a shame POSIX didn't define threads in the past
 * to just be this. Since create/join/atomics is really all we need.
 *
 * Your spawn library abstracts clone() which also works on all
 * platforms; however our implementation of clone() is significantly
 * complicated so we strongly recommend always favoring this API.
 */

#define _TLSZ ((intptr_t)_tls_size)
#define _TLDZ ((intptr_t)_tdata_size)
#define _TIBZ sizeof(struct CosmoTib)
#define _MEMZ ROUNDUP(_TLSZ + _TIBZ, alignof(struct CosmoTib))

struct spawner {
  int (*fun)(void *, int);
  void *arg;
};

static int Spawner(void *arg, int tid) {
  int rc;
  struct spawner *spawner = arg;
  rc = spawner->fun(spawner->arg, tid);
  _pthread_ungarbage();
  free(spawner);
  return 0;
}

/**
 * Spawns thread, e.g.
 *
 *     int worker(void *arg, int tid) {
 *       const char *s = arg;
 *       printf("%s\n", s);
 *       return 0;
 *     }
 *
 *     int main() {
 *       struct spawn th;
 *       _spawn(worker, "hi", &th);
 *       _join(&th);
 *     }
 *
 * @param fun is thread worker callback, which receives `arg` and `ctid`
 * @param arg shall be passed to `fun`
 * @param opt_out_thread needn't be initialiized and is always clobbered
 *     except when it isn't specified, in which case, the thread is kind
 *     of detached and will (currently) just leak the stack / tls memory
 * @return 0 on success, or -1 w/ errno
 */
int _spawn(int fun(void *, int), void *arg, struct spawn *opt_out_thread) {
  errno_t rc;
  struct spawn *th, ths;
  struct spawner *spawner;
  __require_tls();
  if (!fun) return einval();

  // we need to to clobber the output memory before calling clone, since
  // there's no guarantee clone() won't suspend the parent, and focus on
  // running the child instead; in that case child might want to read it
  if (opt_out_thread) {
    th = opt_out_thread;
  } else {
    th = &ths;
  }

  // allocate enough TLS memory for all the GNU Linuker (_tls_size)
  // organized _Thread_local data, as well as Cosmpolitan Libc (64)
  if (!(th->tls = _mktls(&th->tib))) {
    return -1;
  }

  // we must use _mapstack() to allocate the stack because OpenBSD has
  // very strict requirements for what's allowed to be used for stacks
  if (!(th->stk = _mapstack())) {
    free(th->tls);
    return -1;
  }

  spawner = malloc(sizeof(struct spawner));
  spawner->fun = fun;
  spawner->arg = arg;
  rc = clone(Spawner, th->stk, GetStackSize() - 16 /* openbsd:stackbound */,
             CLONE_VM | CLONE_THREAD | CLONE_FS | CLONE_FILES | CLONE_SIGHAND |
                 CLONE_SETTLS | CLONE_PARENT_SETTID | CLONE_CHILD_SETTID |
                 CLONE_CHILD_CLEARTID,
             spawner, &th->ptid, th->tib, &th->tib->tib_tid);
  if (rc) {
    errno = rc;
    _freestack(th->stk);
    free(th->tls);
    return -1;
  }

  return 0;
}

/**
 * Waits for thread created by _spawn() to terminate.
 *
 * This will free your thread's stack and tls memory too.
 */
int _join(struct spawn *th) {
  int rc;
  if (th->tib) {
    // wait for ctid to become zero
    _npassert(!_wait0(&th->tib->tib_tid, 0));
    // free thread memory
    free(th->tls);
    rc = munmap(th->stk, GetStackSize());
    rc = 0;
  } else {
    rc = 0;
  }
  bzero(th, sizeof(*th));
  return rc;
}
