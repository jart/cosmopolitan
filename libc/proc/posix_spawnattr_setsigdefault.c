/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/proc/posix_spawn.internal.h"

/**
 * Specifies which signals should be restored to `SIG_DFL`.
 *
 * This routine isn't necessary in most cases, since posix_spawn() by
 * default will try to avoid vfork() race conditions by tracking what
 * signals have a handler function and then resets them automatically
 * within the child process, before applying the child's signal mask.
 * This function may be used to ensure the `SIG_IGN` disposition will
 * not propagate across execve in cases where this process explicitly
 * set the signals to `SIG_IGN` earlier (since posix_spawn() will not
 * issue O(128) system calls just to be totally pedantic about that).
 *
 * You also need to pass `POSIX_SPAWN_SETSIGDEF` to
 * posix_spawnattr_setflags() for it to take effect.
 *
 * @return 0 on success, or errno on error
 */
int posix_spawnattr_setsigdefault(posix_spawnattr_t *attr,
                                  const sigset_t *sigdefault) {
  (*attr)->sigdefault = *sigdefault;
  return 0;
}
