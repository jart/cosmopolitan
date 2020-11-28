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
#include "libc/nt/enum/fileflagandattributes.h"
#include "libc/nt/enum/filesharemode.h"
#include "libc/nt/enum/status.h"
#include "libc/nt/files.h"
#include "libc/nt/nt/file.h"
#include "libc/nt/ntdll.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/fileaccessinformation.h"
#include "libc/nt/struct/filebasicinformation.h"
#include "libc/nt/struct/iostatusblock.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/errfuns.h"

textwindows int fadvise$nt(int fd, uint64_t offset, uint64_t len, int advice) {
  int64_t h2;
  NtStatus status;
  uint32_t sharemode;
  struct NtIoStatusBlock iostatus;
  struct NtFileBasicInformation basicinfo;
  struct NtFileAccessInformation accessinfo;
  if (!__isfdkind(fd, kFdFile)) return ebadf();
  sharemode = /* xxx: no clue how to query this */
      kNtFileShareRead | kNtFileShareWrite | kNtFileShareDelete;
  /* TODO(jart): can we do it in one call w/ NtQueryObject? */
  if (!NtError(status = NtQueryInformationFile(g_fds.p[fd].handle, &iostatus,
                                               &basicinfo, sizeof(basicinfo),
                                               kNtFileBasicInformation)) &&
      !NtError(status = NtQueryInformationFile(g_fds.p[fd].handle, &iostatus,
                                               &accessinfo, sizeof(accessinfo),
                                               kNtFileAccessInformation))) {
    if ((h2 = ReOpenFile(g_fds.p[fd].handle, accessinfo.AccessFlags, sharemode,
                         advice | basicinfo.FileAttributes)) != -1) {
      if (h2 != g_fds.p[fd].handle) {
        CloseHandle(g_fds.p[fd].handle);
        g_fds.p[fd].handle = h2;
      }
      return 0;
    }
    return __winerr();
  } else if (status == kNtStatusDllNotFound) {
    return enosys();
  } else {
    return ntreturn(status);
  }
}
