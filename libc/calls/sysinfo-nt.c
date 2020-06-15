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
#include "libc/calls/struct/sysinfo.h"
#include "libc/nt/accounting.h"
#include "libc/nt/struct/memorystatusex.h"
#include "libc/nt/struct/systeminfo.h"
#include "libc/nt/systeminfo.h"

textwindows int sysinfo$nt(struct sysinfo *info) {
  struct NtMemoryStatusEx memstat;
  memstat.dwLength = sizeof(struct NtMemoryStatusEx);
  if (GlobalMemoryStatusEx(&memstat)) {
    info->totalram = memstat.ullTotalPhys;
    info->freeram = memstat.ullAvailPhys;
    info->procs = g_ntsysteminfo.dwNumberOfProcessors;
    info->mem_unit = 1;
    return 0;
  } else {
    return winerr();
  }
}
