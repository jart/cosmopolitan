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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/syscall_support-sysv.internal.h"
#include "libc/dce.h"
#include "libc/nt/enum/processcreationflags.h"
#include "libc/paths.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/o.h"

/**
 * Runs process in background.
 *
 * On Unix this calls fork() and setsid(). On Windows this is
 * implemented using CreateProcess(kNtDetachedProcess).
 *
 * @return 0 on success, or -1 w/ errno
 */
int daemon(int nochdir, int noclose) {
  int fd;

  switch (_fork(kNtDetachedProcess)) {
    case -1:
      return -1;
    case 0:
      break;
    default:
      _Exit(0);
  }

  if (!IsWindows()) {
    if (setsid() == -1) {
      return -1;
    }
  }

  if (!nochdir) {
    unassert(!chdir("/"));
  }

  if (!noclose && (fd = open(_PATH_DEVNULL, O_RDWR)) != -1) {
    unassert(dup2(fd, 0) == 0);
    unassert(dup2(fd, 1) == 1);
    unassert(dup2(fd, 2) == 2);
    if (fd > 2) {
      unassert(!close(fd));
    }
  }

  return 0;
}
