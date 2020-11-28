/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/nt/enum/accessmask.h"
#include "libc/nt/enum/fileflagandattributes.h"
#include "libc/nt/enum/filesharemode.h"
#include "libc/nt/files.h"
#include "libc/nt/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/fd.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/o.h"

/**
 * Overwrites existing stream.
 *
 * This function can be used in two ways. The first is sort of a
 * mutating assignment. The second behavior, if pathname is NULL, is
 * just changing the mode of an already open file descriptor.
 *
 * @param pathname is the file to open or NULL
 * @param mode is the mode string flags, see fopenflags()
 * @param stream is the existing allocated stream memory, which is
 *     flushed and closed if already open
 * @return stream object if successful, or NULL w/ errno
 */
FILE *freopen(const char *pathname, const char *mode, FILE *stream) {
  int fd;
  unsigned flags;
  flags = fopenflags(mode);
  fflush(stream);
  if (pathname) {
    /* open new stream, overwriting existing alloc */
    if ((fd = open(pathname, flags, 0666)) != -1) {
      if (!IsWindows()) {
        dup3(fd, stream->fd, (flags & O_CLOEXEC));
        close(fd);
      } else {
        g_fds.p[stream->fd].handle = g_fds.p[fd].handle;
        g_fds.p[fd].kind = kFdEmpty;
      }
      stream->iomode = flags;
      return stream;
    } else {
      return NULL;
    }
  } else {
    /* change mode of open file */
    if (!IsWindows()) {
      if (flags & O_CLOEXEC) {
        if (fcntl$sysv(stream->fd, F_SETFD, FD_CLOEXEC) == -1) return NULL;
        flags &= ~O_CLOEXEC;
      }
      if (flags) {
        if (fcntl$sysv(stream->fd, F_SETFL, flags) == -1) return NULL;
      }
      return stream;
    } else {
      if (ReOpenFile(
              stream->fd,
              (flags & O_RDWR) == O_RDWR ? kNtGenericWrite : kNtGenericRead,
              (flags & O_EXCL) == O_EXCL
                  ? kNtFileShareExclusive
                  : kNtFileShareRead | kNtFileShareWrite | kNtFileShareDelete,
              kNtFileAttributeNormal)) {
        return stream;
      } else {
        __winerr();
        return NULL;
      }
    }
  }
}
