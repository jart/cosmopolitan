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
#include "libc/calls/internal.h"
#include "libc/calls/struct/timeval.h"
#include "libc/conv/conv.h"
#include "libc/dce.h"
#include "libc/nt/createfile.h"
#include "libc/nt/enum/accessmask.h"
#include "libc/nt/files.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/filetime.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"
#include "libc/time/time.h"

static int utimes$nt(const char *path, const struct timeval times[2]) {
  int rc;
  int64_t fh;
  struct timeval tv;
  struct NtFileTime accessed;
  struct NtFileTime modified;
  uint16_t path16[PATH_MAX];
  if (mkntpath(path, path16) == -1) return -1;
  if (times) {
    accessed = timevaltofiletime(&times[0]);
    modified = timevaltofiletime(&times[1]);
  } else {
    gettimeofday(&tv, NULL);
    accessed = timevaltofiletime(&tv);
    modified = timevaltofiletime(&tv);
  }
  if ((fh = CreateFile(path16, kNtGenericWrite, kNtFileShareRead, NULL,
                       kNtOpenExisting, kNtFileAttributeNormal, 0)) != -1 &&
      SetFileTime(fh, NULL, &accessed, &modified)) {
    rc = 0;
  } else {
    rc = winerr();
  }
  CloseHandle(fh);
  return rc;
}

/**
 * Changes last accessed/modified times on file.
 *
 * @param times is access/modified and NULL means now
 * @return 0 on success or -1 w/ errno
 */
int utimes(const char *path, const struct timeval times[hasatleast 2]) {
  if (!IsWindows()) {
    return utimes$sysv(path, times);
  } else {
    return utimes$nt(path, times);
  }
}
