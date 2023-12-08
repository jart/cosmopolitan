/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/atomic.h"
#include "libc/calls/state.internal.h"
#include "libc/cosmo.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/dll.h"
#include "libc/intrin/leaky.internal.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/proc/proc.internal.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"

struct AtFork {
  struct AtFork *p[2];
  atfork_f f[3];
};

static struct AtForks {
  pthread_spinlock_t lock;
  struct AtFork *list;
  struct AtFork pool[8];
  atomic_int allocated;
} _atforks;

extern pthread_spinlock_t _pthread_lock_obj;

static void _pthread_onfork(int i) {
  struct AtFork *a;
  unassert(0 <= i && i <= 2);
  if (!i) pthread_spin_lock(&_atforks.lock);
  for (a = _atforks.list; a; a = a->p[!i]) {
    if (a->f[i]) a->f[i]();
    _atforks.list = a;
  }
  if (i) pthread_spin_unlock(&_atforks.lock);
}

void _pthread_onfork_prepare(void) {
  _pthread_onfork(0);
  _pthread_lock();
  __fds_lock();
  __mmi_lock();
}

void _pthread_onfork_parent(void) {
  __mmi_unlock();
  __fds_unlock();
  _pthread_unlock();
  _pthread_onfork(1);
}

void _pthread_onfork_child(void) {
  pthread_mutexattr_t attr;
  pthread_mutexattr_init(&attr);
  pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
  extern pthread_mutex_t __mmi_lock_obj;
  pthread_mutex_init(&__mmi_lock_obj, &attr);
  pthread_mutex_init(&__fds_lock_obj, &attr);
  pthread_mutexattr_destroy(&attr);
  (void)pthread_spin_init(&_pthread_lock_obj, 0);
  _pthread_onfork(2);
}

static struct AtFork *_pthread_atfork_alloc(void) {
  int i, n = ARRAYLEN(_atforks.pool);
  if (atomic_load_explicit(&_atforks.allocated, memory_order_relaxed) < n &&
      (i = atomic_fetch_add(&_atforks.allocated, 1)) < n) {
    return _atforks.pool + i;
  } else {
    return malloc(sizeof(struct AtFork));
  }
}

int _pthread_atfork(atfork_f prepare, atfork_f parent, atfork_f child) {
  int rc;
  struct AtFork *a;
  if (!(a = _pthread_atfork_alloc())) return ENOMEM;
  a->f[0] = prepare;
  a->f[1] = parent;
  a->f[2] = child;
  pthread_spin_lock(&_atforks.lock);
  a->p[0] = 0;
  a->p[1] = _atforks.list;
  if (_atforks.list) _atforks.list->p[0] = a;
  _atforks.list = a;
  pthread_spin_unlock(&_atforks.lock);
  rc = 0;
  return rc;
}

IGNORE_LEAKS(_pthread_atfork)
