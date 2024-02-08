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
#include "libc/calls/syscall-nt.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/log/backtrace.internal.h"
#include "libc/runtime/zipos.internal.h"
#include "libc/sysv/errfuns.h"

/**
 * Changes current position of file descriptor, e.g.
 *
 *     int fd = open("hello.bin", O_RDONLY);
 *     lseek(fd, 100, SEEK_SET);  // set position to 100th byte
 *     read(fd, buf, 8);          // read bytes 100 through 107
 *
 * This function may be used to inspect the current position:
 *
 *     int64_t pos = lseek(fd, 0, SEEK_CUR);
 *
 * You may seek past the end of file. If a write happens afterwards
 * then the gap leading up to it will be filled with zeroes. Please
 * note that lseek() by itself will not extend the physical medium.
 *
 * If dup() is used then the current position will be shared across
 * multiple file descriptors. If you seek in one it will implicitly
 * seek the other too.
 *
 * The current position of a file descriptor is shared between both
 * processes and threads. For example, if an fd is inherited across
 * fork(), and both the child and parent want to read from it, then
 * changes made by one are observable to the other.
 *
 * The pread() and pwrite() functions obfuscate the need for having
 * global shared file position state. Consider using them, since it
 * helps avoid the gotchas of this interface described above.
 *
 * This function is supported by all OSes within our support vector
 * and our unit tests demonstrate the behaviors described above are
 * consistent across platforms.
 *
 * @param fd is a number returned by open()
 * @param offset is 0-indexed byte count w.r.t. `whence`
 * @param whence can be one of:
 *     - `SEEK_SET`: Sets the file position to `offset` [default]
 *     - `SEEK_CUR`: Sets the file position to `position + offset`
 *     - `SEEK_END`: Sets the file position to `filesize + offset`
 * @return new position relative to beginning, or -1 w/ errno
 * @raise ESPIPE if `fd` is a pipe, socket, or fifo
 * @raise EBADF if `fd` isn't an open file descriptor
 * @raise EINVAL if resulting offset would be negative
 * @raise EINVAL if `whence` isn't valid
 * @asyncsignalsafe
 * @vforksafe
 */
int64_t lseek(int fd, int64_t offset, int whence) {
  int64_t rc;
  if (fd < g_fds.n && g_fds.p[fd].kind == kFdZip) {
    rc = _weaken(__zipos_seek)(
        (struct ZiposHandle *)(intptr_t)g_fds.p[fd].handle, offset, whence);
  } else if (IsLinux() || IsXnu() || IsFreebsd() || IsOpenbsd()) {
    rc = sys_lseek(fd, offset, whence, 0);
  } else if (IsNetbsd()) {
    rc = sys_lseek(fd, offset, offset, whence);
  } else if (IsWindows()) {
    rc = sys_lseek_nt(fd, offset, whence);
  } else {
    rc = enosys();
  }
  STRACE("lseek(%d, %'ld, %s) → %'ld% m", fd, offset, DescribeWhence(whence),
         rc);
  return rc;
}

__weak_reference(lseek, lseek64);
