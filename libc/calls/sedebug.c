/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/intrin/strace.internal.h"
#include "libc/nt/enum/accessmask.h"
#include "libc/nt/enum/securityimpersonationlevel.h"
#include "libc/nt/errors.h"
#include "libc/nt/files.h"
#include "libc/nt/privilege.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/luid.h"
#include "libc/nt/struct/tokenprivileges.h"

static bool32 SetPrivilegeNt(int64_t hToken, const char16_t *lpwPrivilege,
                             bool32 bEnable) {
  struct NtLuid luid;
  uint32_t cbPrevious;
  struct NtTokenPrivileges tp, tpPrevious;
  cbPrevious = sizeof(struct NtTokenPrivileges);
  if (!LookupPrivilegeValue(0, lpwPrivilege, &luid)) {
    STRACE("LookupPrivilegeValue() failed");
    return false;
  }
  tp.PrivilegeCount = 1;
  tp.Privileges[0].Luid = luid;
  tp.Privileges[0].Attributes = 0;
  if (!AdjustTokenPrivileges(hToken, false, &tp, sizeof(tp), &tpPrevious,
                             &cbPrevious)) {
    STRACE("AdjustTokenPrivileges() failed");
    return false;
  }
  tpPrevious.PrivilegeCount = 1;
  tpPrevious.Privileges[0].Luid = luid;
  if (bEnable) {
    tpPrevious.Privileges[0].Attributes |= kNtSePrivilegeEnabled;
  } else {
    tpPrevious.Privileges[0].Attributes ^=
        kNtSePrivilegeEnabled & tpPrevious.Privileges[0].Attributes;
  }
  if (!AdjustTokenPrivileges(hToken, false, &tpPrevious, cbPrevious, 0, 0)) {
    STRACE("AdjustTokenPrivileges() failed");
    return false;
  }
  return true;
}

static int64_t GetCurrentProcessSecurityToken(void) {
  int64_t hToken;
  if (OpenProcessToken(GetCurrentProcess(),
                       kNtTokenAdjustPrivileges | kNtTokenQuery, &hToken)) {
    return hToken;
  } else if (GetLastError() == kNtErrorNoToken) {
    if (ImpersonateSelf(kNtSecurityImpersonation)) {
      if (OpenProcessToken(GetCurrentProcess(),
                           kNtTokenAdjustPrivileges | kNtTokenQuery, &hToken)) {
        return hToken;
      } else {
        STRACE("OpenProcessToken() failed");
      }
    } else {
      STRACE("ImpersonateSelf() failed");
    }
  } else {
    STRACE("OpenProcessToken() failed");
  }
  return 0;
}

bool32 ElevateSeDebugPrivilege(void) {
  int64_t hToken;
  if (!(hToken = GetCurrentProcessSecurityToken())) return false;
  SetPrivilegeNt(hToken, u"SeDebugPrivilege", true);
  RevertToSelf();
  CloseHandle(hToken);
  return true;
}
