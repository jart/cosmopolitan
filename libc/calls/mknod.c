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
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/s.h"
#include "libc/sysv/errfuns.h"

/**
 * Creates filesystem inode.
 *
 * @param mode is octal mode, e.g. 0600; needs to be or'd with one of:
 *     S_IFDIR: directory
 *     S_IFIFO: named pipe
 *     S_IFREG: regular file
 *    S_IFSOCK: named socket
 *     S_IFBLK: block device (root has authorization)
 *     S_IFCHR: character device (root has authorization)
 * @param dev it's complicated
 * @return 0 on success, or -1 w/ errno
 * @asyncsignalsafe
 */
int mknod(const char *path, uint32_t mode, uint64_t dev) {
  int e, rc;
  if (IsAsan() && !__asan_is_valid_str(path)) return efault();
  if (mode & S_IFREG) return creat(path, mode & ~S_IFREG);
  if (mode & S_IFDIR) return mkdir(path, mode & ~S_IFDIR);
  if (mode & S_IFIFO) return enosys();  // no named pipes!
  if (!IsWindows()) {
    // TODO(jart): Whys there code out there w/ S_xxx passed via dev?
    e = errno;
    rc = sys_mknod(path, mode, dev);
    if (rc == -1 && rc == ENOSYS) {
      errno = e;
      rc = sys_mknodat(AT_FDCWD, path, mode, dev);
    }
  } else {
    rc = enosys();
  }
  STRACE("mknod(%#s, %#o, %#lx) → %d% m", path, mode, dev, rc);
  return rc;
}
