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
#include "libc/bits/bits.h"
#include "libc/calls/calls.h"
#include "libc/calls/sigbits.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/dce.h"
#include "libc/log/log.h"
#include "libc/macros.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sig.h"

/**
 * @fileoverview Default handlers for normal termination signals.
 * @note This gets linked when __cxa_atexit() is linked.
 * @see also libc/oncrash.c
 */

struct ucontext;
struct siginfo;

static const int sigs[] = {
    SIGHUP, /* hangup aka ctrl_close_event */
    SIGINT, /* ctrl+c aka ^C aka ETX aka \003 aka ♥ */
    SIGTERM /* kill (default signal) */
};

textexit static void __onkill(int sig, struct siginfo *si,
                              struct ucontext *context) {
  /* https://www.tldp.org/LDP/abs/html/exitcodes.html */
  exit(128 + sig);
  unreachable;
}

/**
 * Installs default handlers for friendly kill signals.
 * @see showcrashreports()
 */
void callexitontermination(sigset_t *opt_out_exitsigs) {
  struct sigaction sa;
  if (opt_out_exitsigs) sigemptyset(opt_out_exitsigs);
  memset(&sa, 0, sizeof(sa));
  sa.sa_sigaction = __onkill;
  sa.sa_flags = SA_RESETHAND;
  for (unsigned i = 0; i < ARRAYLEN(sigs); ++i) {
    if (!sigs[i]) continue;
    if (opt_out_exitsigs) sigaddset(opt_out_exitsigs, sigs[i]);
    sigaction(sigs[i], &sa, NULL);
  }
}
