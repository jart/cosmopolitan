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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/conv/conv.h"
#include "libc/conv/isslash.internal.h"
#include "libc/conv/itoa.h"
#include "libc/dce.h"
#include "libc/nt/createfile.h"
#include "libc/nt/enum/accessmask.h"
#include "libc/nt/enum/creationdisposition.h"
#include "libc/nt/enum/fileflagandattributes.h"
#include "libc/nt/enum/filesharemode.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/o.h"

static struct OpenAnon { int count; } g_openanon;

static void openanon$genpath(const char *name, struct OpenAnon *state,
                             char pathbuf[hasatleast PATH_MAX]) {
  char c;
  size_t i;
  char *p, *pe;
  p = stpcpy(pathbuf, kTmpPath);
  pe = pathbuf + PATH_MAX - 8 - 10 - 1 - 10 - 1;
  if (!name) name = "openanon";
  for (i = 0; p < pe; ++i) {
    if (!(c = name[i])) break;
    if (isslash(c)) c = '_';
    *p++ = c;
  }
  *p++ = '.';
  p += uint64toarray_radix10(getpid(), p);
  *p++ = '.';
  p += uint64toarray_radix10(++state->count, p);
  *p = '\0';
  assert(p < pe);
}

static int openanon$impl(const char *name, unsigned flags,
                         struct OpenAnon *state,
                         char pathbuf[hasatleast PATH_MAX]) {
  int fd;
  openanon$genpath(name, state, pathbuf);
  flags |= O_RDWR | O_CREAT | O_EXCL | O_TRUNC;
  if (!IsWindows()) {
    if ((fd = openat$sysv(AT_FDCWD, pathbuf, flags, 0600)) != -1) {
      unlink(pathbuf);
    }
    return fd;
  } else {
    if ((fd = __getemptyfd()) != -1 &&
        (g_fds.p[fd].handle = CreateFileA(
             pathbuf, kNtGenericRead | kNtGenericWrite, kNtFileShareExclusive,
             &kNtIsInheritable, kNtCreateAlways,
             (kNtFileAttributeNotContentIndexed | kNtFileAttributeNormal |
              kNtFileAttributeTemporary | kNtFileFlagDeleteOnClose),
             0)) != -1) {
      g_fds.p[fd].kind = kFdFile;
      g_fds.p[fd].flags = flags;
      return fd;
    } else {
      return __winerr();
    }
  }
}

/**
 * Creates anonymous file.
 *
 * @param name is purely informative
 * @param flags can have O_CLOEXEC
 * @return fd of file with no name, needing close(), or -1 w/ errno
 * @see memfd_create() if disk-paranoid
 * @see mkostempsm() for named files
 */
int openanon(char *name, unsigned flags) {
  char pathbuf[PATH_MAX];
  return openanon$impl(name, flags, &g_openanon, pathbuf);
}
