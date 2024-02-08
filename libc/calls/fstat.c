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
#include "libc/calls/internal.h"
#include "libc/calls/struct/stat.internal.h"
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/runtime/zipos.internal.h"
#include "libc/sysv/errfuns.h"

/**
 * Returns information about file, via open()'d descriptor.
 *
 * On Windows, this implementation always sets `st_uid` and `st_gid` to
 * `getuid()` and `getgid()`. The `st_mode` field is generated based on
 * the current umask().
 *
 * @return 0 on success or -1 w/ errno
 * @raise EBADF if `fd` isn't a valid file descriptor
 * @raise EIO if an i/o error happens while reading from file system
 * @raise EOVERFLOW shouldn't be possible on 64-bit systems
 * @asyncsignalsafe
 */
int fstat(int fd, struct stat *st) {
  int rc;
  if (IsAsan() && !__asan_is_valid(st, sizeof(*st))) {
    rc = efault();
  } else if (__isfdkind(fd, kFdZip)) {
    rc = _weaken(__zipos_fstat)(
        (struct ZiposHandle *)(intptr_t)g_fds.p[fd].handle, st);
  } else if (IsLinux() || IsXnu() || IsFreebsd() || IsOpenbsd() || IsNetbsd()) {
    rc = sys_fstat(fd, st);
  } else if (IsMetal()) {
    rc = sys_fstat_metal(fd, st);
  } else if (IsWindows()) {
    rc = sys_fstat_nt(fd, st);
  } else {
    rc = enosys();
  }
  STRACE("fstat(%d, [%s]) → %d% m", fd, DescribeStat(rc, st), rc);
  return rc;
}

__weak_reference(fstat, fstat64);
