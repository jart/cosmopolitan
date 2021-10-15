/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/bits/weaken.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/sysdebug.internal.h"
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/sysv/errfuns.h"
#include "libc/zipos/zipos.internal.h"

/**
 * Changes permissions on file, e.g.:
 *
 *     CHECK_NE(-1, fchmodat(AT_FDCWD, "foo/bar.txt", 0644));
 *     CHECK_NE(-1, fchmodat(AT_FDCWD, "o/default/program.com", 0755));
 *     CHECK_NE(-1, fchmodat(AT_FDCWD, "privatefolder/", 0700));
 *
 * @param path must exist
 * @param mode contains octal flags (base 8)
 * @param flags can have `AT_SYMLINK_NOFOLLOW`
 * @errors ENOENT, ENOTDIR, ENOSYS
 * @asyncsignalsafe
 * @see fchmod()
 */
int fchmodat(int dirfd, const char *path, uint32_t mode, int flags) {
  int rc;
  if (IsAsan() && !__asan_is_valid(path, 1)) return efault();
  if (weaken(__zipos_notat) && weaken(__zipos_notat)(dirfd, path) == -1) {
    rc = -1; /* TODO(jart): implement me */
  } else if (!IsWindows()) {
    rc = sys_fchmodat(dirfd, path, mode, flags);
  } else {
    rc = sys_fchmodat_nt(dirfd, path, mode, flags);
  }
  SYSDEBUG("fchmodat(%d, %s, %o, %d) -> %d %s", (long)dirfd, path, mode, flags,
           rc != -1 ? "" : strerror(errno));
  return rc;
}
