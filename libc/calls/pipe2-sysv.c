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
#include "libc/dce.h"
#include "libc/errno.h"

#define __NR_pipe2_linux 0x0125 /*RHEL5:CVE-2010-3301*/

int32_t pipe2$sysv(int pipefd[hasatleast 2], unsigned flags) {
  int rc, olderr;
  if (!flags) goto OldSkool;
  olderr = errno;
  rc = __pipe2$sysv(pipefd, flags);
  if ((rc == -1 && errno == ENOSYS) ||
      (SupportsLinux() && rc == __NR_pipe2_linux)) {
    errno = olderr;
  OldSkool:
    if ((rc = pipe$sysv(pipefd)) != -1) {
      fixupnewfd$sysv(pipefd[0], flags);
      fixupnewfd$sysv(pipefd[1], flags);
    }
  }
  return rc;
}
