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
