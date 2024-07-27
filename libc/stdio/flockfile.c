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
#include "libc/assert.h"
#include "libc/stdio/fflush.internal.h"
#include "libc/stdio/internal.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/thread/thread.h"

/**
 * Acquires reentrant lock on stdio object, blocking if needed.
 */
void flockfile(FILE *f) {
  unassert(f != NULL);
  pthread_mutex_lock(&f->lock);
}

void(__fflush_lock)(void) {
  pthread_mutex_lock(&__fflush_lock_obj);
}

void(__fflush_unlock)(void) {
  pthread_mutex_unlock(&__fflush_lock_obj);
}

static void __stdio_fork_prepare(void) {
  FILE *f;
  __fflush_lock();
  for (int i = 0; i < __fflush.handles.i; ++i)
    if ((f = __fflush.handles.p[i]))
      pthread_mutex_lock(&f->lock);
}

static void __stdio_fork_parent(void) {
  FILE *f;
  for (int i = __fflush.handles.i; i--;)
    if ((f = __fflush.handles.p[i]))
      pthread_mutex_unlock(&f->lock);
  __fflush_unlock();
}

static void __stdio_fork_child(void) {
  FILE *f;
  for (int i = __fflush.handles.i; i--;)
    if ((f = __fflush.handles.p[i]))
      f->lock = (pthread_mutex_t)PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
  pthread_mutex_init(&__fflush_lock_obj, 0);
}

__attribute__((__constructor__(60))) static textstartup void stdioinit(void) {
  pthread_atfork(__stdio_fork_prepare, __stdio_fork_parent, __stdio_fork_child);
}
