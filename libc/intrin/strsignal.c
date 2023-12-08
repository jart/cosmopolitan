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
#include "libc/str/str.h"

static char g_strsignal[21];

/**
 * Returns string describing signal code.
 *
 * This returns `"0"` for 0 which is the empty value. Symbolic names
 * should be available for signals 1 through 32. If the system supports
 * real-time signals, they're returned as `SIGRTMIN+%d`. For all other
 * 32-bit signed integer, a plain integer representation is returned.
 *
 * This function is thread safe when `sig` is a known signal magnum.
 * Otherwise a pointer to static memory is returned which is unsafe.
 *
 * @param sig is signal number which should be in range 1 through 128
 * @return string which is valid code describing signal
 * @see strsignal_r()
 * @see sigaction()
 * @threadunsafe
 */
char *strsignal(int sig) {
  return (char *)strsignal_r(sig, g_strsignal);
}
