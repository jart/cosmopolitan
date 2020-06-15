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
#include "libc/macros.h"
#include "libc/str/str.h"

static const char kSig[4] = "SIG";
static const char kUnknown[8] = "UNKNOWN";
alignas(1) static const char kStrSignals[][8] = {
    "EXIT",   "HUP",  "INT",    "QUIT", "ILL",   "TRAP", "ABRT", "BUS",
    "FPE",    "KILL", "USR1",   "SEGV", "USR2",  "PIPE", "ALRM", "TERM",
    "STKFLT", "CHLD", "CONT",   "STOP", "TSTP",  "TTIN", "TTOU", "URG",
    "XCPU",   "XFSZ", "VTALRM", "PROF", "WINCH", "IO",   "PWR",  "SYS"};

static char g_strsignal[4 + 8];

char *strsignal(int sig) {
  if (0 <= sig && (unsigned)sig < ARRAYLEN(kStrSignals)) {
    memcpy(g_strsignal, kSig, 4);
    memcpy(&g_strsignal[3], kStrSignals[sig], 8);
  } else {
    memcpy(g_strsignal, &kUnknown, 8);
  }
  return g_strsignal;
}
