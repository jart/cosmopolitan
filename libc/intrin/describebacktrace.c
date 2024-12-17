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
#include "libc/intrin/describebacktrace.h"
#include "libc/intrin/iscall.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/weaken.h"
#include "libc/nexgen32e/stackframe.h"

#define N 160

#define ABI privileged optimizesize

ABI static bool IsDangerous(const void *ptr) {
  if (_weaken(kisdangerous))
    return _weaken(kisdangerous)(ptr);
  return false;
}

ABI static char *FormatHex(char *p, unsigned long x) {
  int k = x ? (__builtin_clzl(x) ^ 63) + 1 : 1;
  k = (k + 3) & -4;
  while (k > 0)
    *p++ = "0123456789abcdef"[(x >> (k -= 4)) & 15];
  *p = '\0';
  return p;
}

ABI const char *_DescribeBacktrace(char buf[N], const struct StackFrame *fr) {
  char *p = buf;
  char *pe = p + N;
  bool gotsome = false;
  while (fr) {
    if (IsDangerous(fr)) {
      if (p + 1 + 1 + 1 < pe) {
        if (gotsome)
          *p++ = ' ';
        *p = '!';
        if (p + 16 + 1 < pe) {
          *p++ = ' ';
          p = FormatHex(p, (long)fr);
        }
      }
      break;
    }
    if (p + 16 + 1 < pe) {
      unsigned char *ip = (unsigned char *)fr->addr;
#ifdef __x86_64__
      // x86 advances the progrem counter before an instruction
      // begins executing. return addresses in backtraces shall
      // point to code after the call, which means addr2line is
      // going to print unrelated code unless we fixup the addr
      if (!IsDangerous(ip))
        ip -= __is_call(ip);
#endif
      if (gotsome)
        *p++ = ' ';
      else
        gotsome = true;
      p = FormatHex(p, (long)ip);
    } else {
      break;
    }
    fr = fr->next;
  }
  *p = '\0';
  return buf;
}
