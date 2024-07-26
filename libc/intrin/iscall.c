/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2024 Justine Alexandra Roberts Tunney                              │
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
#include "libc/intrin/iscall.h"

// returns true if `p` is preceded by x86 call instruction
// this is actually impossible to do but we'll do our best
privileged dontinstrument int __is_call(const unsigned char *p) {
  if (p[-5] == 0xe8)
    return 5;  // call Jvds
  if (p[-2] == 0xff && (p[-1] & 070) == 020)
    return 2;  // call %reg
  if (p[-4] == 0xff && (p[-3] & 070) == 020)
    return 4;  // call disp8(%reg,%reg)
  if (p[-3] == 0xff && (p[-2] & 070) == 020)
    return 3;  // call disp8(%reg)
  if (p[-7] == 0xff && (p[-6] & 070) == 020)
    return 7;  // call disp32(%reg,%reg)
  if (p[-6] == 0xff && (p[-5] & 070) == 020)
    return 6;  // call disp32(%reg)
  return 0;
}
