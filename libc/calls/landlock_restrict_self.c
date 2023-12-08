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
#include "libc/calls/landlock.h"
#include "libc/intrin/strace.internal.h"

int sys_landlock_restrict_self(int, uint32_t);

/**
 * Enforces Landlock ruleset on calling thread.
 *
 * @error EOPNOTSUPP if Landlock supported but disabled at boot time
 * @error EINVAL if flags isn't zero
 * @error EBADF if `fd` isn't file descriptor for the current thread
 * @error EBADFD if `fd` is not a ruleset file descriptor
 * @error EPERM if `fd` has no read access to underlying ruleset, or
 *     current thread is not running with no_new_privs, or it doesn’t
 *     have CAP_SYS_ADMIN in its namespace
 * @error E2BIG if the maximum number of stacked rulesets is
 *     reached for current thread
 */
int landlock_restrict_self(int fd, uint32_t flags) {
  int rc;
  rc = sys_landlock_restrict_self(fd, flags);
  KERNTRACE("landlock_create_ruleset(%d, %#x) → %d% m", fd, flags, rc);
  return rc;
}
