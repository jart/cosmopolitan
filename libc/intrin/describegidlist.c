/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/groups.internal.h"
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/popcnt.h"
#include "libc/macros.internal.h"
#include "libc/str/str.h"

#define N 128

const char *(DescribeGidList)(char buf[N], int rc, int size,
                              const uint32_t list[]) {
  if ((rc == -1) || (size < 0)) return "n/a";
  if (!size) return "{}";
  if (!list) return "NULL";
  if ((!IsAsan() && kisdangerous(list)) ||
      (IsAsan() && !__asan_is_valid(list, size * sizeof(list[0])))) {
    ksnprintf(buf, N, "%p", list);
    return buf;
  }
  int i = 0, n = N;
  i += ksnprintf(buf + i, MAX(0, n - i), "{");
  unsigned c;
  for (c = 0; c < size && MAX(0, n - i) > 0; c++) {
    i += ksnprintf(buf + i, MAX(0, n - i), "%u, ", list[c]);
  }
  if (c == size) {
    if (buf[i - 1] == ' ') i--;
    if (buf[i - 1] == ',') i--;
    i += ksnprintf(buf + i, MAX(0, n - i), "}");
  }
  return buf;
}
