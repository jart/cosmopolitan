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
#include "libc/bits/weaken.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/sysdebug.internal.h"
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/log/log.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/errfuns.h"
#include "libc/zipos/zipos.internal.h"

/**
 * Opens file.
 *
 * @param dirfd is normally AT_FDCWD but if it's an open directory and
 *     file is a relative path, then file is opened relative to dirfd
 * @param file is a UTF-8 string, preferably relative w/ forward slashes
 * @param flags should be O_RDONLY, O_WRONLY, or O_RDWR, and can be or'd
 *     with O_CREAT, O_TRUNC, O_APPEND, O_EXCL, O_CLOEXEC, O_TMPFILE
 * @param mode is an octal user/group/other permission signifier, that's
 *     ignored if O_CREAT or O_TMPFILE weren't passed
 * @return number needing close(), or -1 w/ errno
 * @asyncsignalsafe
 * @vforksafe
 */
int openat(int dirfd, const char *file, int flags, ...) {
  int rc;
  va_list va;
  unsigned mode;
  struct ZiposUri zipname;
  va_start(va, flags);
  mode = va_arg(va, unsigned);
  va_end(va);
  if (file && (!IsAsan() || __asan_is_valid(file, 1))) {
    if (!__isfdkind(dirfd, kFdZip)) {
      if (weaken(__zipos_open) &&
          weaken(__zipos_parseuri)(file, &zipname) != -1) {
        if (!__vforked && dirfd == AT_FDCWD) {
          rc = weaken(__zipos_open)(&zipname, flags, mode);
        } else {
          rc = eopnotsupp(); /* TODO */
        }
      } else if (!IsWindows() && !IsMetal()) {
        rc = sys_openat(dirfd, file, flags, mode);
      } else if (IsMetal()) {
        rc = sys_openat_metal(dirfd, file, flags, mode);
      } else {
        rc = sys_open_nt(dirfd, file, flags, mode);
      }
    } else {
      rc = eopnotsupp(); /* TODO */
    }
  } else {
    rc = efault();
  }
  SYSDEBUG("openat(%d, %s, %d, %d) -> %d %s", (long)dirfd, file, flags,
           (flags & (O_CREAT | O_TMPFILE)) ? mode : 0, (long)rc,
           rc == -1 ? strerror(errno) : "");
  return rc;
}
