/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/rusage.h"
#include "libc/errno.h"
#include "libc/stdio/internal.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/errfuns.h"

/**
 * Closes stream created by popen().
 *
 * This function may be interrupted or cancelled, however it won't
 * actually return until the child process has terminated. Thus we
 * always release the resource, and errors are purely advisory.
 *
 * @return termination status of subprocess, or -1 w/ ECHILD
 * @raise ECANCELED if thread was cancelled in masked mode
 * @raise ECHILD if child pid didn't exist
 * @raise EINTR if signal was delivered
 * @cancelationpoint
 */
int pclose(FILE *f) {
  int e, rc, ws, pid;
  bool iscancelled, wasinterrupted;
  pid = f->pid;
  fclose(f);
  if (!pid) return 0;
  iscancelled = false;
  wasinterrupted = false;
  for (e = errno;;) {
    if (wait4(pid, &ws, 0, 0) != -1) {
      rc = ws;
      break;
    } else if (errno == ECANCELED) {
      iscancelled = true;
      errno = e;
    } else if (errno == EINTR) {
      wasinterrupted = true;
      errno = e;
    } else {
      rc = echild();
      break;
    }
  }
  if (iscancelled) {
    return ecanceled();
  } else if (wasinterrupted) {
    return eintr();
  } else {
    return rc;
  }
}
