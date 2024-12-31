/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/dce.h"
#include "libc/intrin/atomic.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread.h"

//
// - tib_ptid: always guaranteed to be non-zero in thread itself. on
//             some platforms (e.g. xnu) the parent thread and other
//             threads may need to wait for this value to be set. this
//             is generally the value you want to read to get the tid.
//
// - tib_ctid: starts off as -1. once thread starts, it's set to the
//             thread's tid before calling the thread callback. when
//             thread is done executing, this is set to zero, and then
//             this address is futex woken, in case the parent thread or
//             any other thread is waiting on its completion. when a
//             thread wants to read its own tid, it shouldn't use this,
//             because the thread might need to do things after clearing
//             its own tib_ctid (see pthread_exit() for static thread).
//
int _pthread_tid(struct PosixThread *pt) {
  int tid = 0;
  while (pt && !(tid = atomic_load_explicit(&pt->tib->tib_ptid,
                                            memory_order_acquire)))
    pthread_yield_np();
  return tid;
}
