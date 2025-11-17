/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2025 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/struct/sigset.h"
#include "libc/intrin/atomic.h"
#include "libc/macros.h"
#include "libc/nt/events.h"
#include "libc/nt/runtime.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/tls.h"

// returns non-inheritable manual reset event in unknown state
intptr_t CreateEventTls(void) {
  intptr_t h;
  struct CosmoTib *tib = __get_tls_win32();
  for (int i = 0; i < ARRAYLEN(tib->tib_events); ++i) {
    if (tib->tib_events[i]) {
      h = tib->tib_events[i];
      tib->tib_events[i] = 0;
      return h;
    }
  }
  if (!(h = CreateEvent(0, 1, 0, 0)))
    return 0;
  return h;
}

void CloseEventTls(intptr_t h) {
  if (h) {
    struct CosmoTib *tib = __get_tls_win32();
    for (int i = 0; i < ARRAYLEN(tib->tib_events); ++i) {
      if (!tib->tib_events[i]) {
        tib->tib_events[i] = h;
        return;
      }
    }
    CloseHandle(h);
  }
}

intptr_t __interruptible_start(sigset_t waitmask) {
  intptr_t h;
  if (!(h = CreateEventTls()))
    return 0;
  ResetEvent(h);
  struct PosixThread *pt = _pthread_self();
  pt->pt_event = h;
  pt->pt_blkmask = waitmask;
  atomic_store(&pt->pt_blocker, PT_BLOCKER_EVENT);
  return h;
}

void __interruptible_end(void) {
  struct PosixThread *pt = _pthread_self();
  // synchronize with sig.c to ensure no one else is using the
  // handle before we put it back on the free list
  for (;;)
    if (atomic_exchange(&pt->pt_blocker, 0))
      break;
  CloseEventTls(pt->pt_event);
}
