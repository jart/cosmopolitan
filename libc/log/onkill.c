/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
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
