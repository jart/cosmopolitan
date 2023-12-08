/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/fmt/magnumstrs.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/sig.h"

/**
 * Returns string describing signal code.
 *
 * This returns `"0"` for 0 which is the empty value. Symbolic names
 * should be available for signals 1 through 32. If the system supports
 * real-time signals, they're returned as `SIGRTMIN+%d`.
 *
 * @param sig is signal number which should be in range 1 through 128
 * @param buf may be used to store output having at least 15 bytes
 * @return pointer to .rodata string, or to `buf` after mutating
 * @see sigaction()
 * @asyncsignalsafe
 */
privileged const char *strsignal_r(int sig, char buf[21]) {
  char *p;
  const char *s;
  if (!sig) {
    return "0";
  }
  if ((s = GetMagnumStr(kSignalNames, sig))) {
    return s;
  }
  if (SIGRTMIN <= sig && sig <= SIGRTMAX) {
    sig -= SIGRTMIN;
    buf[0] = 'S';
    buf[1] = 'I';
    buf[2] = 'G';
    buf[3] = 'R';
    buf[4] = 'T';
    buf[5] = 'M';
    buf[6] = 'I';
    buf[7] = 'N';
    buf[8] = '+';
    p = buf + 9;
  } else {
    p = buf;
  }
  if (sig >= 100) *p++ = '0' + (unsigned char)sig / 100 % 10;
  if (sig >= 10) *p++ = '0' + (unsigned char)sig / 10 % 10;
  *p++ = '0' + (unsigned char)sig % 10;
  *p = 0;
  return buf;
}
