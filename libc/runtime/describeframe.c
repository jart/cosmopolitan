/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/log/libfatal.internal.h"
#include "libc/macros.internal.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/runtime/runtime.h"

#define ADDR(x)     ((int64_t)((uint64_t)(x) << 32) >> 16)
#define UNSHADOW(x) ((int64_t)(MAX(0, (x)-0x7fff8000)) << 3)
#define FRAME(x)    ((int)((x) >> 16))

noasan const char *DescribeFrame(int x) {
  /* asan runtime depends on this function */
  char *p;
  static char buf[128];
  if (IsShadowFrame(x)) {
    p = buf;
    p = __stpcpy(p, " shadow of ");
    p = __fixcpy(p, UNSHADOW(ADDR(x)), 48);
    return buf;
    return " shadow ";
  } else if (IsAutoFrame(x)) {
    return " automap";
  } else if (IsFixedFrame(x)) {
    return " fixed  ";
  } else if (IsArenaFrame(x)) {
    return " arena  ";
  } else if (IsStaticStackFrame(x)) {
    return " stack  ";
  } else {
    return "";
  }
}
