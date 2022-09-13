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
#include "libc/mem/gc.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/spawn.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"

/**
 * Terminates current POSIX thread.
 *
 * If this function is called from the main thread, or a thread created
 * with clone() or _spawn(), then this function is the same as _Exit1()
 * in which case `rc` is coerced to a `uint8_t` exit status, which will
 * only be reported to the parent process on Linux, FreeBSD and Windows
 *
 * @param rc is reported later to pthread_join()
 * @threadsafe
 * @noreturn
 */
wontreturn void pthread_exit(void *rc) {
  struct PosixThread *pt;
  if ((pt = (struct PosixThread *)__get_tls()->tib_pthread)) {
    pt->rc = rc;
    _gclongjmp(pt->exiter, 1);
  } else {
    _Exit1((int)(intptr_t)rc);
  }
}
