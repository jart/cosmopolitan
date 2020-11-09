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
#include "libc/nt/createfile.h"
#include "libc/nt/files.h"
#include "libc/nt/runtime.h"
#include "libc/sysv/consts/ok.h"

/**
 * Flushes all open file handles and, if possible, all disk drives.
 */
int sync$nt(void) {
  unsigned i;
  int64_t volume;
  uint32_t drives;
  char16_t path[] = u"\\\\.\\C:";
  for (i = 0; i < g_fds.n; ++i) {
    if (g_fds.p[i].kind == kFdFile) {
      FlushFileBuffers(g_fds.p[i].handle);
    }
  }
  for (drives = GetLogicalDrives(), i = 0; i <= 'Z' - 'A'; ++i) {
    if (!(drives & (1 << i))) continue;
    path[4] = 'A' + i;
    if (ntaccesscheck(path, R_OK | W_OK) != -1) {
      if ((volume = CreateFile(
               path, kNtFileReadAttributes,
               kNtFileShareRead | kNtFileShareWrite | kNtFileShareDelete, 0,
               kNtOpenExisting, 0, 0)) != -1) {
        FlushFileBuffers(volume);
        CloseHandle(volume);
      }
    }
  }
  return 0;
}
