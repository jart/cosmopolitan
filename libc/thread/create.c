/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
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
#include "libc/bits/atomic.h"
#include "libc/calls/calls.h"
#include "libc/calls/strace.internal.h"
#include "libc/errno.h"
#include "libc/intrin/setjmp.internal.h"
#include "libc/macros.internal.h"
#include "libc/nexgen32e/threaded.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/clone.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"
#include "libc/thread/internal.h"
#include "libc/thread/thread.h"

STATIC_YOINK("_main_thread_ctor");

static cthread_t cthread_allocate(const cthread_attr_t *attr) {
  char *mem;
  size_t size;
  cthread_t td;
  size = ROUNDUP(
      attr->stacksize +
          ROUNDUP((uintptr_t)_tls_size + sizeof(struct cthread_descriptor_t),
                  PAGESIZE),
      FRAMESIZE);
  mem = mmap(0, size, PROT_READ | PROT_WRITE, MAP_STACK | MAP_ANONYMOUS, -1, 0);
  if (mem == MAP_FAILED) return 0;
  if (attr->guardsize > PAGESIZE) {
    mprotect(mem, attr->guardsize, PROT_NONE);
  }
  td = (cthread_t)(mem + size - sizeof(struct cthread_descriptor_t));
  td->self = td;
  td->self2 = td;
  td->err = errno;
  td->tid = -1;
  td->stack.bottom = mem;
  td->stack.top = mem + attr->stacksize;
  td->alloc.bottom = mem;
  td->alloc.top = mem + size;
  if (attr->mode & CTHREAD_CREATE_DETACHED) {
    td->state = cthread_detached;
  } else {
    td->state = cthread_started;
  }
  // Initialize TLS with content of .tdata section
  memmove((void *)((intptr_t)td - (intptr_t)_tls_size), _tdata_start,
          (intptr_t)_tdata_size);
  return td;
}

static int cthread_start(void *arg) {
  axdx_t rc;
  void *exitcode;
  cthread_t td = arg;
  if (!(rc = setlongerjmp(td->exiter)).ax) {
    exitcode = td->func(td->arg);
  } else {
    exitcode = (void *)rc.dx;
  }
  td->exitcode = exitcode;
  _pthread_key_destruct(td->key);
  if (atomic_load(&td->state) & cthread_detached) {
    // we're still using the stack
    // thus we can't munmap it yet
    // kick the can down the road!
    cthread_zombies_add(td);
  }
  atomic_fetch_add(&td->state, cthread_finished);
  return 0;
}

/**
 * Creates thread.
 *
 * @param ptd will receive pointer to new thread descriptor
 * @param attr contains special configuration if non-null
 * @param func is thread callback function
 * @param arg is argument supplied to `func`
 * @return 0 on success, or error number on failure
 * @threadsafe
 */
int cthread_create(cthread_t *ptd, const cthread_attr_t *attr,
                   void *(*func)(void *), void *arg) {
  int rc, tid;
  cthread_t td;
  cthread_attr_t default_attr;
  __threaded = true;
  cthread_zombies_reap();
  cthread_attr_init(&default_attr);
  if ((td = cthread_allocate(attr ? attr : &default_attr))) {
    td->func = func;
    td->arg = arg;
    cthread_attr_destroy(&default_attr);
    tid =
        clone(cthread_start, td->stack.bottom, td->stack.top - td->stack.bottom,
              CLONE_THREAD | CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND |
                  CLONE_SETTLS | CLONE_CHILD_SETTID | CLONE_CHILD_CLEARTID,
              td, 0, td, sizeof(struct cthread_descriptor_t), &td->tid);
    if (tid != -1) {
      *ptd = td;
      rc = 0;
    } else {
      rc = errno;
      munmap(td->alloc.bottom, td->alloc.top - td->alloc.bottom);
    }
  } else {
    rc = errno;
    tid = -1;
  }
  STRACE("cthread_create([%d], %p, %p, %p) → %s", tid, attr, func, arg,
         !rc ? "0" : strerrno(rc));
  return rc;
}
