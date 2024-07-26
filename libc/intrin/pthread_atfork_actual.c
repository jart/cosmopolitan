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
#include "libc/errno.h"
#include "libc/intrin/strace.h"
#include "libc/macros.internal.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread.h"

struct AtFork {
  struct AtFork *p[2];
  atfork_f f[3];
};

static struct AtForks {
  pthread_mutex_t lock;
  struct AtFork *list;
  struct AtFork pool[256];
  int allocated;
} _atforks = {
    PTHREAD_MUTEX_INITIALIZER,
};

static void _pthread_onfork(int i, const char *op) {
  struct AtFork *a;
  for (a = _atforks.list; a; a = a->p[!i]) {
    if (a->f[i]) {
      STRACE("pthread_atfork(%s, %t)", op, a->f[i]);
      a->f[i]();
    }
    _atforks.list = a;
  }
}

void _pthread_onfork_prepare(void) {
  pthread_mutex_lock(&_atforks.lock);
  _pthread_onfork(0, "prepare");
}

void _pthread_onfork_parent(void) {
  _pthread_onfork(1, "parent");
  pthread_mutex_unlock(&_atforks.lock);
}

void _pthread_onfork_child(void) {
  pthread_mutex_init(&_atforks.lock, 0);
  _pthread_onfork(2, "child");
}

int _pthread_atfork(atfork_f prepare, atfork_f parent, atfork_f child) {
  int rc;
  struct AtFork *a;
  pthread_mutex_lock(&_atforks.lock);
  if (_atforks.allocated < ARRAYLEN(_atforks.pool)) {
    a = &_atforks.pool[_atforks.allocated++];
    a->f[0] = prepare;
    a->f[1] = parent;
    a->f[2] = child;
    a->p[0] = 0;
    a->p[1] = _atforks.list;
    if (_atforks.list)
      _atforks.list->p[0] = a;
    _atforks.list = a;
    rc = 0;
  } else {
    rc = ENOMEM;
  }
  pthread_mutex_unlock(&_atforks.lock);
  return rc;
}
