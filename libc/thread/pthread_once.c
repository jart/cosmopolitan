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
#include "libc/thread/thread.h"
#include "third_party/nsync/once.h"

/**
 * Ensures initialization function is called exactly once, e.g.
 *
 *     static void *g_factory;
 *
 *     static void InitFactory(void) {
 *       g_factory = expensive();
 *     }
 *
 *     void *GetFactory(void) {
 *       static pthread_once_t once = PTHREAD_ONCE_INIT;
 *       pthread_once(&once, InitFactory);
 *       return g_factory;
 *     }
 *
 * If multiple threads try to initialize at the same time, then only a
 * single one will call `init` and the other threads will block until
 * the winner has returned from the `init` function.
 *
 * @return 0 on success, or errno on error
 */
errno_t pthread_once(pthread_once_t *once, void init(void)) {
  nsync_run_once((nsync_once *)once, init);
  return 0;
}
