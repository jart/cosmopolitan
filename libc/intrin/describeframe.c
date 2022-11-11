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
#include "libc/dce.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/weaken.h"
#include "libc/macros.internal.h"
#include "libc/runtime/brk.internal.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/winargs.internal.h"

#ifdef DescribeFrame
#undef DescribeFrame
#endif

#define ADDR(x)     ((int64_t)((uint64_t)(x) << 32) >> 16)
#define UNSHADOW(x) ((int64_t)(MAX(0, (x)-0x7fff8000)) << 3)
#define FRAME(x)    ((int)((x) >> 16))

forceinline pureconst bool IsBrkFrame(int x) {
  unsigned char *p = (unsigned char *)((intptr_t)((uintptr_t)x << 32) >> 16);
  return _weaken(__brk) && p >= _end && p < _weaken(__brk)->p;
}

static const char *GetFrameName(int x) {
  if (!x) {
    return "null";
  } else if (IsShadowFrame(x)) {
    return "shadow";
  } else if (IsAutoFrame(x)) {
    return "automap";
  } else if (IsFixedFrame(x)) {
    return "fixed";
  } else if (IsArenaFrame(x)) {
    return "arena";
  } else if (IsStaticStackFrame(x)) {
    return "stack";
  } else if (IsBrkFrame(x)) {
    return "brk";
  } else if (IsGfdsFrame(x)) {
    return "g_fds";
  } else if (IsZiposFrame(x)) {
    return "zipos";
  } else if (IsKmallocFrame(x)) {
    return "kmalloc";
  } else if (IsMemtrackFrame(x)) {
    return "memtrack";
  } else if (IsOldStackFrame(x)) {
    return "oldstack";
  } else if (IsWindows() &&
             (((GetStaticStackAddr(0) + GetStackSize()) >> 16) <= x &&
              x <= ((GetStaticStackAddr(0) + GetStackSize() +
                     sizeof(struct WinArgs) - 1) >>
                    16))) {
    return "winargs";
  } else if ((int)((intptr_t)_base >> 16) <= x &&
             x <= (int)(((intptr_t)_end - 1) >> 16)) {
    return "image";
  } else {
    return "unknown";
  }
}

const char *DescribeFrame(char buf[32], int x) {
  char *p;
  if (IsShadowFrame(x)) {
    ksnprintf(buf, 32, "%s %s %.8x", GetFrameName(x),
              GetFrameName(FRAME(UNSHADOW(ADDR(x)))), FRAME(UNSHADOW(ADDR(x))));
    return buf;
  } else {
    return GetFrameName(x);
  }
}
