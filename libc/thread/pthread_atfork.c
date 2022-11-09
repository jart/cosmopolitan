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
#include "libc/errno.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/kmalloc.h"
#include "libc/mem/mem.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/str/str.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"

static struct AtForks {
  pthread_spinlock_t lock;
  struct AtFork {
    struct AtFork *p[2];
    atfork_f f[3];
  } * list;
} _atforks;

static void _pthread_purge(void) {
  nsync_dll_element_ *e;
  while ((e = nsync_dll_first_(_pthread_list))) {
    _pthread_list = nsync_dll_remove_(_pthread_list, e);
    _pthread_free(e->container);
  }
}

static void _pthread_onfork(int i) {
  struct AtFork *a;
  struct PosixThread *pt;
  _unassert(0 <= i && i <= 2);
  if (!i) pthread_spin_lock(&_atforks.lock);
  for (a = _atforks.list; a; a = a->p[!i]) {
    if (a->f[i]) a->f[i]();
    _atforks.list = a;
  }
  if (i) pthread_spin_unlock(&_atforks.lock);
}

void _pthread_onfork_prepare(void) {
  _pthread_onfork(0);
  pthread_spin_lock(&_pthread_lock);
  __kmalloc_lock();
  __mmi_lock();
}

void _pthread_onfork_parent(void) {
  __mmi_unlock();
  __kmalloc_unlock();
  pthread_spin_unlock(&_pthread_lock);
  _pthread_onfork(1);
}

void _pthread_onfork_child(void) {
  struct CosmoTib *tib;
  struct PosixThread *pt;
  pthread_mutexattr_t attr;
  extern pthread_mutex_t __mmi_lock_obj;
  tib = __get_tls();
  pt = (struct PosixThread *)tib->tib_pthread;

  // let's choose to let the new process live.
  // even though it's unclear what to do with this kind of race.
  atomic_store_explicit(&pt->cancelled, false, memory_order_relaxed);

  // wipe core runtime locks
  pthread_mutexattr_init(&attr);
  pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
  pthread_mutex_init(&__mmi_lock_obj, &attr);
  pthread_spin_init(&_pthread_lock, 0);
  __kmalloc_unlock();

  // call user-supplied forked child callbacks
  _pthread_onfork(2);

  // delete other threads that existed before forking
  // this must come after onfork, since it calls free
  _pthread_list = nsync_dll_remove_(_pthread_list, &pt->list);
  _pthread_purge();
  _pthread_list = nsync_dll_make_first_in_list_(_pthread_list, &pt->list);
}

int _pthread_atfork(atfork_f prepare, atfork_f parent, atfork_f child) {
  int rc;
  struct AtFork *a;
  if (!(a = kmalloc(sizeof(struct AtFork)))) return ENOMEM;
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
