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
#include "libc/nt/privilege.h"
#include "libc/nt/struct/tokenprivileges.h"

/**
 * Sets NT permission thing, e.g.
 *
 *     int64_t htoken;
 *     if (OpenProcessToken(GetCurrentProcess(),
 *                          kNtTokenAdjustPrivileges | kNtTokenQuery,
 *                          &htoken)) {
 *       ntsetprivilege(htoken, u"SeManageVolumePrivilege",
 *                      kNtSePrivilegeEnabled);
 *       CloseHandle(htoken);
 *     }
 */
textwindows bool32 ntsetprivilege(int64_t token, const char16_t *name,
                                  uint32_t attrs) {
  struct NtTokenPrivileges tp;
  tp.PrivilegeCount = 1;
  tp.Privileges[0].Attributes = attrs;
  return LookupPrivilegeValue(NULL, name, &tp.Privileges[0].Luid) &&
         AdjustTokenPrivileges(token, false, &tp,
                               sizeof(struct NtTokenPrivileges), NULL, NULL);
}
