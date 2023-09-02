/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#define ShouldUseMsabiAttribute() 1
#include "libc/dce.h"
#include "libc/fmt/fmt.h"
#include "libc/fmt/magnumstrs.internal.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/strace.internal.h"
#include "libc/macros.internal.h"
#include "libc/nt/enum/formatmessageflags.h"
#include "libc/nt/enum/lang.h"
#include "libc/nt/process.h"
#include "libc/str/str.h"
// clang-format off

#if defined(SYSDEBUG) && _NTTRACE
dontasan dontubsan privileged
#endif

/**
 * Converts errno value to string with explicit windows errno too.
 *
 * @param err is error number or zero if unknown
 * @return 0 on success, or error code
 */
int strerror_wr(int err, uint32_t winerr, char *buf, size_t size) {
  /* kprintf() weakly depends on this function */
  int c, n;
  char16_t winmsg[256];
  const char *sym, *msg;
  /* wanting = false; */
  if (!(sym = _strerrno(err))) {
    sym = "EUNKNOWN";
    /* wanting = true; */
  }
  if (!(msg = _strerdoc(err))) {
    msg = "No error information";
    /* wanting = true; */
  }
  if (IsTiny()) {
    if (!sym) sym = "EUNKNOWN";
    for (; (c = *sym++); --size)
      if (size > 1) *buf++ = c;
    if (size) *buf = 0;
  } else if (!IsWindows() /* || ((err == winerr || !winerr) && !wanting) */) {
    ksnprintf(buf, size, "%s/%d/%s", sym, err, msg);
  } else {
    if ((n = __imp_FormatMessageW(
             kNtFormatMessageFromSystem | kNtFormatMessageIgnoreInserts, 0,
             winerr, MAKELANGID(kNtLangNeutral, kNtSublangDefault), winmsg,
             ARRAYLEN(winmsg), 0))) {
      while ((n && winmsg[n - 1] <= ' ') || winmsg[n - 1] == '.') --n;
      ksnprintf(buf, size, "%s/%d/%s/%d/%.*hs", sym, err, msg, winerr, n,
                winmsg);
    } else {
      ksnprintf(buf, size, "%s/%d/%s/%d", sym, err, msg, winerr);
    }
  }
  return 0;
}
