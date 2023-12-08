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
#include "libc/calls/struct/rlimit.h"
#include "libc/errno.h"
#include "libc/macros.internal.h"
#include "libc/proc/posix_spawn.h"
#include "libc/proc/posix_spawn.internal.h"
#include "libc/stdio/sysparam.h"
#include "libc/sysv/consts/rlim.h"

/**
 * Gets resource limit for spawned process.
 *
 * @return 0 on success, or errno on error
 * @raise EINVAL if `resource` is invalid or unsupported by host
 * @raise ENOENT if `resource` is absent
 */
int posix_spawnattr_getrlimit(const posix_spawnattr_t *attr, int resource,
                              struct rlimit *rlim) {
  if (0 <= resource && resource < MIN(RLIM_NLIMITS, ARRAYLEN((*attr)->rlim))) {
    if (((*attr)->rlimset & (1u << resource))) {
      *rlim = (*attr)->rlim[resource];
      return 0;
    } else {
      return ENOENT;
    }
  } else {
    return EINVAL;
  }
}
