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
#include "libc/calls/struct/winsize.h"
#include "libc/calls/struct/winsize.internal.h"
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/kprintf.h"
#include "libc/limits.h"
#include "libc/macros.internal.h"

#define N 64

#define append(...) o += ksnprintf(buf + o, N - o, __VA_ARGS__)

const char *(DescribeWinsize)(char buf[N], int rc, const struct winsize *ws) {
  int o = 0;
  if (!ws) return "NULL";
  if (rc == -1) return "n/a";
  if ((!IsAsan() && kisdangerous(ws)) ||
      (IsAsan() && !__asan_is_valid(ws, sizeof(*ws)))) {
    ksnprintf(buf, N, "%p", ws);
    return buf;
  }

  append("{.ws_row=%d, .ws_col=%d", ws->ws_row, ws->ws_col);
  if (ws->ws_xpixel | ws->ws_ypixel) {
    append(", .ws_xpixel=%d, .ws_ypixel=%d", ws->ws_xpixel, ws->ws_ypixel);
  }
  append("}");

  return buf;
}
