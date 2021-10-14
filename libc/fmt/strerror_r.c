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
#include "libc/bits/safemacros.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/fmt.h"
#include "libc/fmt/itoa.h"
#include "libc/log/libfatal.internal.h"
#include "libc/macros.internal.h"
#include "libc/nexgen32e/bsr.h"
#include "libc/nt/enum/formatmessageflags.h"
#include "libc/nt/enum/lang.h"
#include "libc/nt/memory.h"
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"
#include "libc/str/str.h"
#include "libc/str/tpenc.h"

#if !IsTiny()
STATIC_YOINK("__dos2errno");
#endif

struct Error {
  int x;
  int s;
};

extern const struct Error kErrorNames[];
extern const struct Error kErrorNamesLong[];

noasan static inline const char *GetErrorName(long x) {
  int i;
  if (x) {
    for (i = 0; kErrorNames[i].x; ++i) {
      if (x == *(const long *)((uintptr_t)kErrorNames + kErrorNames[i].x)) {
        return (const char *)((uintptr_t)kErrorNames + kErrorNames[i].s);
      }
    }
  }
  return "EUNKNOWN";
}

noasan static inline const char *GetErrorNameLong(long x) {
  int i;
  if (x) {
    for (i = 0; kErrorNamesLong[i].x; ++i) {
      if (x ==
          *(const long *)((uintptr_t)kErrorNamesLong + kErrorNamesLong[i].x)) {
        return (const char *)((uintptr_t)kErrorNamesLong +
                              kErrorNamesLong[i].s);
      }
    }
  }
  return "EUNKNOWN[No error information]";
}

/**
 * Converts errno value to string.
 * @return 0 on success, or error code
 */
noasan int strerror_r(int err, char *buf, size_t size) {
  uint64_t w;
  int c, i, n;
  char *p, *e;
  const char *s;
  char16_t *ws = 0;
  p = buf;
  e = p + size;
  err &= 0xFFFF;
  s = IsTiny() ? GetErrorName(err) : GetErrorNameLong(err);
  while ((c = *s++)) {
    if (p + 1 + 1 <= e) *p++ = c;
  }
  if (!IsTiny()) {
    if (p + 1 + 5 + 1 + 1 <= e) {
      *p++ = '[';
      p = __intcpy(p, err);
      *p++ = ']';
    }
    if (IsWindows()) {
      err = GetLastError() & 0xffff;
      if ((n = FormatMessage(
               kNtFormatMessageAllocateBuffer | kNtFormatMessageFromSystem |
                   kNtFormatMessageIgnoreInserts,
               0, err, MAKELANGID(kNtLangNeutral, kNtSublangDefault),
               (char16_t *)&ws, 0, 0))) {
        while (n && ws[n - 1] <= L' ' || ws[n - 1] == L'.') --n;
        if (p + 1 + 1 <= e) *p++ = '[';
        for (i = 0; i < n; ++i) {
          w = tpenc(ws[i] & 0xffff);
          if (p + (bsrll(w) >> 3) + 1 + 1 <= e) {
            do *p++ = w;
            while ((w >>= 8));
          }
        }
        if (p + 1 + 1 <= e) *p++ = ']';
        LocalFree(ws);
      }
      if (p + 1 + 5 + 1 + 1 <= e) {
        *p++ = '[';
        p = __intcpy(p, err);
        *p++ = ']';
      }
    }
  }
  if (p + 1 <= e) *p = 0;
  return 0;
}
