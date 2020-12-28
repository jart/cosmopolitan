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
#include "libc/bits/progn.internal.h"
#include "libc/bits/safemacros.h"
#include "libc/mem/mem.h"
#include "libc/runtime/interruptiblecall.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sig.h"

STATIC_YOINK("_init_onntconsoleevent");

static struct InterruptibleCall *g_interruptiblecall;

wontreturn static void interruptcall(int sig) {
  longjmp(g_interruptiblecall->jb, 1);
  unreachable;
}

/**
 * Calls function that may be cancelled by a signal.
 *
 * @param state is allocated and zero'd by the caller; state→sig and
 *     state→sa_new.sa_mask may be set; it may be re-used w/o
 *     reinitializing; it may be static or heap memory; it may be stack
 *     memory if re-entrant behavior isn't needed
 * @return the value returned by callback or -1 on interrupt; they may
 *     be differentiated using the state→returnval filed, which is only
 *     modified by this function when callbacks succeed
 */
intptr_t interruptiblecall(struct InterruptibleCall *icall,
                           intptr_t callback(intptr_t p1, intptr_t p2,
                                             intptr_t p3, intptr_t p4),
                           intptr_t p1, intptr_t p2, intptr_t p3, intptr_t p4) {
  intptr_t rc;
  if (!icall->sig) icall->sig = SIGINT;
  icall->sa_new.sa_handler = interruptcall;
  icall->sa_new.sa_flags |= SA_RESTART | SA_RESETHAND;
  if ((rc = (sigaction)(icall->sig, &icall->sa_new, &icall->sa_old)) != -1) {
    g_interruptiblecall = PROGN((icall->prev = g_interruptiblecall), icall);
    if (!setjmp(icall->jb)) {
      icall->returnval = rc = callback(p1, p2, p3, p4);
    } else {
      rc = -1;
    }
    asm volatile("" ::: "memory");
    struct InterruptibleCall *unwind;
    for (;;) {
      unwind = g_interruptiblecall;
      (sigaction)(unwind->sig, &unwind->sa_old, NULL);
      g_interruptiblecall = unwind->prev;
      if (unwind == icall) break;
      free_s(&unwind);
    }
    icall->prev = NULL;
  }
  return rc;
}
