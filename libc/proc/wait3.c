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
#include "libc/calls/struct/rusage.h"
#include "libc/sysv/errfuns.h"

/**
 * Waits for status to change on any child process.
 *
 * @param opt_out_wstatus optionally returns status code, and *wstatus
 *     may be inspected using WEEXITSTATUS(), etc.
 * @param options can have WNOHANG, WUNTRACED, WCONTINUED, etc.
 * @param opt_out_rusage optionally returns accounting data
 * @return process id of terminated child or -1 w/ errno
 * @cancelationpoint
 * @asyncsignalsafe
 * @restartable
 */
int wait3(int *opt_out_wstatus, int options, struct rusage *opt_out_rusage) {
  return wait4(-1, opt_out_wstatus, options, opt_out_rusage);
}
