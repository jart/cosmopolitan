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
#include "libc/macros.h"
#include "libc/str/str.h"

static const char kSig[4] = "SIG";
static const char kUnknown[8] = "UNKNOWN";

_Alignas(char) static const char kStrSignals[][8] = {
    "EXIT",   "HUP",  "INT",    "QUIT", "ILL",   "TRAP", "ABRT", "BUS",
    "FPE",    "KILL", "USR1",   "SEGV", "USR2",  "PIPE", "ALRM", "TERM",
    "STKFLT", "CHLD", "CONT",   "STOP", "TSTP",  "TTIN", "TTOU", "URG",
    "XCPU",   "XFSZ", "VTALRM", "PROF", "WINCH", "IO",   "PWR",  "SYS",
};

static char g_strsignal[4 + 8];

/**
 * Returns name associated with signal code.
 * @see sigaction()
 */
char *strsignal(int sig) {
  if (0 <= sig && (unsigned)sig < ARRAYLEN(kStrSignals)) {
    memcpy(g_strsignal, kSig, 4);
    memcpy(&g_strsignal[3], kStrSignals[sig], 8);
  } else {
    memcpy(g_strsignal, &kUnknown, 8);
  }
  return g_strsignal;
}
