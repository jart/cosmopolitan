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
#include "libc/calls/internal.h"
#include "libc/macros.internal.h"
#include "libc/nt/files.h"
#include "libc/str/str.h"
#include "libc/str/tpenc.h"
#include "libc/str/utf16.h"
#include "libc/sysv/errfuns.h"

textwindows char *sys_getcwd_nt(char *buf, size_t size) {
  uint64_t w;
  wint_t x, y;
  uint32_t n, i, j;
  char16_t name16[PATH_MAX + 1];
  if ((n = GetCurrentDirectory(ARRAYLEN(name16), name16))) {
    if (n <= PATH_MAX) {
      tprecode16to8(buf, size, name16);
      for (j = i = 0; i < n;) {
        x = name16[i++] & 0xffff;
        if (!IsUcs2(x)) {
          if (i < n) {
            y = name16[i++] & 0xffff;
            x = MergeUtf16(x, y);
          } else {
            x = 0xfffd;
          }
        }
        if (x < 0200) {
          if (x == '\\') {
            x = '/';
          }
          w = x;
        } else {
          w = tpenc(x);
        }
        do {
          if (j < size) {
            buf[j++] = w;
          }
          w >>= 8;
        } while (w);
      }
      if (j < size) {
        buf[j] = 0;
        return buf;
      }
    }
    erange();
    return NULL;
  } else {
    __winerr();
    return NULL;
  }
}
