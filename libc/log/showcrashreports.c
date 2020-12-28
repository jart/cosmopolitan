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
#include "libc/calls/typedef/sigaction_f.h"
#include "libc/dce.h"
#include "libc/log/internal.h"
#include "libc/log/log.h"
#include "libc/log/ubsan.internal.h"
#include "libc/macros.h"
#include "libc/nt/signals.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sig.h"

STATIC_YOINK("__die");
STATIC_YOINK("__ubsan_abort");

extern const unsigned char __oncrash_thunks[7][11];

/**
 * Installs crash signal handlers.
 *
 * Normally, only functions calling die() will print backtraces. This
 * function may be called at program startup to install handlers that
 * will display similar information, for most types of crashes.
 *
 *   - Backtraces
 *   - CPU state printout
 *   - Automatic debugger attachment
 *
 * Another trick this function enables, is you can press CTRL+\ to open
 * the debugger GUI at any point while the program is running. It can be
 * useful, for example, if a program is caught in an infinite loop.
 *
 * @see callexitontermination()
 */
void showcrashreports(void) {
  size_t i;
  struct sigaction sa;
  /* <SYNC-LIST>: oncrashthunks.S */
  kCrashSigs[0] = SIGQUIT; /* ctrl+\ aka ctrl+break */
  kCrashSigs[1] = SIGFPE;  /* 1 / 0 */
  kCrashSigs[2] = SIGILL;  /* illegal instruction */
  kCrashSigs[3] = SIGSEGV; /* bad memory access */
  kCrashSigs[4] = SIGTRAP; /* bad system call */
  kCrashSigs[5] = SIGABRT; /* abort() called */
  kCrashSigs[6] = SIGBUS;  /* misaligned, noncanonical ptr, etc. */
  /* </SYNC-LIST>: oncrashthunks.S */
  memset(&sa, 0, sizeof(sa));
  sa.sa_flags = SA_RESETHAND;
  sigfillset(&sa.sa_mask);
  for (i = 0; i < ARRAYLEN(kCrashSigs); ++i) {
    sigdelset(&sa.sa_mask, kCrashSigs[i]);
  }
  for (i = 0; i < ARRAYLEN(kCrashSigs); ++i) {
    if (kCrashSigs[i]) {
      sa.sa_sigaction = (sigaction_f)__oncrash_thunks[i];
      sigaction(kCrashSigs[i], &sa, &g_oldcrashacts[i]);
    }
  }
}
