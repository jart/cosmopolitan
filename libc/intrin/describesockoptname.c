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
#include "libc/fmt/itoa.h"
#include "libc/fmt/magnumstrs.internal.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/sol.h"

/**
 * Describes setsockopt() optname arguments.
 */
const char *(DescribeSockOptname)(char buf[32], int l, int x) {
  char *p;
  const char *s;
  const struct MagnumStr *ms;
  p = buf;
  if (x) {
    if (l == SOL_SOCKET) {
      *p++ = 'S';
      *p++ = 'O';
      *p++ = '_';
      *p = 0;
      ms = kSockOptnames;
    } else if (l == SOL_TCP) {
      *p++ = 'T';
      *p++ = 'C';
      *p++ = 'P';
      *p++ = '_';
      ms = kTcpOptnames;
    } else if (l == SOL_IP) {
      *p++ = 'I';
      *p++ = 'P';
      *p++ = '_';
      *p = 0;
      ms = kIpOptnames;
    } else {
      ms = 0;
    }
  } else {
    ms = 0;
  }
  if (ms && (s = GetMagnumStr(ms, x))) {
    stpcpy(p, s);
  } else {
    FormatInt32(p, x);
  }
  return buf;
}
