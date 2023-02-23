/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/fmt.h"
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/mem/mem.h"
#include "libc/nt/enum/accessmask.h"
#include "libc/nt/enum/securityimpersonationlevel.h"
#include "libc/nt/enum/securityinformation.h"
#include "libc/nt/errors.h"
#include "libc/nt/files.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/genericmapping.h"
#include "libc/nt/struct/privilegeset.h"
#include "libc/nt/struct/securitydescriptor.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/ok.h"
#include "libc/sysv/errfuns.h"

/**
 * Asks Microsoft if we're authorized to use a folder or file.
 *
 * Implementation Details: MSDN documentation imposes no limit on the
 * internal size of SECURITY_DESCRIPTOR, which we are responsible for
 * allocating. We've selected 1024 which shall hopefully be adequate.
 *
 * @param flags can have R_OK, W_OK, X_OK, etc.
 * @return 0 if authorized, or -1 w/ errno
 * @see https://blog.aaronballman.com/2011/08/how-to-check-access-rights/
 * @see libc/sysv/consts.sh
 */
textwindows int ntaccesscheck(const char16_t *pathname, uint32_t flags) {
  int rc, e;
  void *freeme;
  bool32 result;
  struct NtSecurityDescriptor *s;
  struct NtGenericMapping mapping;
  struct NtPrivilegeSet privileges;
  int64_t hToken, hImpersonatedToken;
  uint32_t secsize, granted, privsize;
  intptr_t buffer[1024 / sizeof(intptr_t)];
  freeme = 0;
  granted = 0;
  result = false;
  s = (void *)buffer;
  secsize = sizeof(buffer);
  privsize = sizeof(privileges);
  bzero(&privileges, sizeof(privileges));
  mapping.GenericRead = kNtFileGenericRead;
  mapping.GenericWrite = kNtFileGenericWrite;
  mapping.GenericExecute = kNtFileGenericExecute;
  mapping.GenericAll = kNtFileAllAccess;
  MapGenericMask(&flags, &mapping);
  hImpersonatedToken = hToken = -1;
TryAgain:
  if (GetFileSecurity(pathname,
                      kNtOwnerSecurityInformation |
                          kNtGroupSecurityInformation |
                          kNtDaclSecurityInformation,
                      s, secsize, &secsize)) {
    if (OpenProcessToken(GetCurrentProcess(),
                         kNtTokenImpersonate | kNtTokenQuery |
                             kNtTokenDuplicate | kNtStandardRightsRead,
                         &hToken)) {
      if (DuplicateToken(hToken, kNtSecurityImpersonation,
                         &hImpersonatedToken)) {
        if (AccessCheck(s, hImpersonatedToken, flags, &mapping, &privileges,
                        &privsize, &granted, &result)) {
          if (result || flags == F_OK) {
            rc = 0;
          } else {
            STRACE("ntaccesscheck finale failed %d %d", result, flags);
            rc = eacces();
          }
        } else {
          rc = __winerr();
          STRACE("%s(%#hs) failed: %m", "AccessCheck", pathname);
        }
      } else {
        rc = __winerr();
        STRACE("%s(%#hs) failed: %m", "DuplicateToken", pathname);
      }
    } else {
      rc = __winerr();
      STRACE("%s(%#hs) failed: %m", "OpenProcessToken", pathname);
    }
  } else {
    e = GetLastError();
    if (!IsTiny() && e == kNtErrorInsufficientBuffer) {
      if (!freeme && _weaken(malloc) && (freeme = _weaken(malloc)(secsize))) {
        s = freeme;
        goto TryAgain;
      } else {
        rc = enomem();
        STRACE("%s(%#hs) failed: %m", "GetFileSecurity", pathname);
      }
    } else {
      errno = e;
      STRACE("%s(%#hs) failed: %m", "GetFileSecurity", pathname);
      rc = -1;
    }
  }
  if (freeme && _weaken(free)) _weaken(free)(freeme);
  if (hImpersonatedToken != -1) CloseHandle(hImpersonatedToken);
  if (hToken != -1) CloseHandle(hToken);
  return rc;
}
