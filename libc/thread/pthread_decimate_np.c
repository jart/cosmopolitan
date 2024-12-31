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
#include "libc/cosmo.h"
#include "libc/intrin/stack.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread.h"

/**
 * Garbage collects POSIX threads runtime.
 *
 * This function frees unreferenced zombie threads and empties cache
 * memory associated with the Cosmopolitan POSIX threads runtime.
 *
 * Here's an example use case for this function. Let's say you want to
 * create a malloc() memory leak detector. If your program was running
 * threads earlier, then there might still be allocations lingering
 * around, that'll give you false positives. To fix this, what you would
 * do is call the following, right before running your leak detector:
 *
 *     while (!pthread_orphan_np())
 *       pthread_decimate_np();
 *
 * Which will wait until all threads have exited and their memory freed.
 *
 * @return 0 on success, or errno on error
 */
int pthread_decimate_np(void) {
  _pthread_decimate(kPosixThreadZombie);
  cosmo_stack_clear();
  return 0;
}
