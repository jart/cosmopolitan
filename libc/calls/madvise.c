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
#include "libc/calls/calls.h"
#include "libc/calls/syscall-nt.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/sysv/errfuns.h"

/**
 * Drops hints to O/S about intended access patterns of mmap()'d memory.
 *
 * @param advice can be MADV_WILLNEED, MADV_SEQUENTIAL, MADV_FREE, etc.
 * @return 0 on success, or -1 w/ errno
 * @raise EINVAL if `advice` isn't valid or supported by system
 * @raise EINVAL on Linux if addr/length isn't page size aligned with
 *     respect to `getauxval(AT_PAGESZ)`
 * @raise ENOMEM on Linux if addr/length overlaps unmapped regions
 * @see libc/sysv/consts.sh
 * @see fadvise()
 */
int madvise(void *addr, size_t length, int advice) {
  int rc;
  if (IsAsan() && !__asan_is_valid(addr, length)) {
    rc = enomem();
  } else if (!IsWindows()) {
    rc = sys_madvise(addr, length, advice);
  } else {
    rc = sys_madvise_nt(addr, length, advice);
  }
  STRACE("madvise(%p, %'zu, %d) → %d% m", addr, length, advice, rc);
  return rc;
}
