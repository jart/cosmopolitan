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
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/dce.h"
#include "libc/sysv/errfuns.h"

/**
 * Duplicates file descriptor/handle.
 *
 * On Windows, we can't guarantee the desired file descriptor is used.
 * We can however remap the standard handles (non-atomically) if their
 * symbolic names are used.
 *
 * @param oldfd isn't closed afterwards
 * @param newfd if already assigned, is silently closed beforehand;
 *     unless it's equal to oldfd, in which case dup2() is a no-op
 * @flags can have O_CLOEXEC
 * @see dup(), dup2()
 * @syscall
 */
int dup3(int oldfd, int newfd, int flags) {
  if (oldfd == newfd) return einval();
  if (!IsWindows()) {
    return dup3$sysv(oldfd, newfd, flags);
  } else {
    return dup$nt(oldfd, newfd, flags);
  }
}
