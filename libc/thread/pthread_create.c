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
#include "libc/errno.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/pthread.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/gettls.h"
#include "libc/runtime/runtime.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread.h"

static int PosixThread(void *arg, int tid) {
  struct PosixThread *pt = arg;
  enum PosixThreadStatus status;
  pt->tid = tid;
  if (!setjmp(pt->exiter)) {
    ((cthread_t)__get_tls())->pthread = pt;
    pt->rc = pt->start_routine(pt->arg);
  }
  if (atomic_load_explicit(&pt->status, memory_order_relaxed) ==
      kPosixThreadDetached) {
    atomic_store_explicit(&pt->status, kPosixThreadZombie,
                          memory_order_relaxed);
  } else {
    atomic_store_explicit(&pt->status, kPosixThreadTerminated,
                          memory_order_relaxed);
  }
  return 0;
}

/**
 * Creates thread.
 *
 * @return 0 on success, or errno on error
 */
int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                   void *(*start_routine)(void *), void *arg) {
  int e, rc;
  struct PosixThread *pt;
  e = errno;
  pthread_zombies_decimate();
  if ((pt = calloc(1, sizeof(struct PosixThread)))) {
    pt->start_routine = start_routine;
    pt->arg = arg;
    if (!_spawn(PosixThread, pt, &pt->spawn)) {
      *thread = pt;
      rc = 0;
    } else {
      free(pt);
      rc = errno;
    }
  } else {
    rc = errno;
  }
  errno = e;
  return rc;
}
