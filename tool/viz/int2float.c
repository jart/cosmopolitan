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
#include "libc/fmt/conv.h"
#include "libc/limits.h"
#include "libc/macros.internal.h"
#include "libc/mem/gc.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/x/x.h"
#include "libc/x/xasprintf.h"

float b32;
double b64;
long double b80;
uint32_t u32;
uint64_t u64;
int128_t x;

void int2float(const char *s) {
  x = strtoi128(s, NULL, 0);
  if ((0 <= x && x <= UINT32_MAX) && !_startswith(s, "-") &&
      (!_endswith(s, "l") && !_endswith(s, "L"))) {
    u32 = x;
    memcpy(&b32, &u32, 4);
    s = _gc(xdtoa(b32));
    if (!strchr(s, '.')) s = _gc(xasprintf("%s.", s));
    s = _gc(xasprintf("%sf", s));
    puts(s);
  } else if ((0 <= x && x <= UINT64_MAX) && !_startswith(s, "-")) {
    u64 = x;
    memcpy(&b64, &u64, 8);
    s = _gc(xdtoa(b64));
    if (!strchr(s, '.')) s = _gc(xasprintf("%s.", s));
    puts(s);
  } else if ((INT32_MIN <= x && x <= 0) &&
             (!_endswith(s, "l") && !_endswith(s, "L"))) {
    u32 = ABS(x);
    memcpy(&b32, &u32, 4);
    b32 = -b32;
    s = _gc(xdtoa(b32));
    if (!strchr(s, '.')) s = _gc(xasprintf("%s.", s));
    s = _gc(xasprintf("%sf", s));
    puts(s);
  } else if (INT64_MIN <= x && x <= 0) {
    u64 = ABS(x);
    memcpy(&b64, &u64, 8);
    b64 = -b64;
    s = _gc(xdtoa(b64));
    if (!strchr(s, '.')) s = _gc(xasprintf("%s.", s));
    puts(s);
  } else {
    memcpy(&b80, &x, 16);
    s = _gc(xdtoa(b80));
    if (!strchr(s, '.')) s = _gc(xasprintf("%s.", s));
    s = _gc(xasprintf("%sL", s));
    puts(s);
  }
}

int main(int argc, char *argv[]) {
  int i;
  for (i = 1; i < argc; ++i) {
    int2float(argv[i]);
  }
  return 0;
}
