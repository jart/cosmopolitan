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
#include "libc/calls/internal.h"
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
#include "libc/str/str.h"
#include "libc/sysv/consts/ok.h"

/**
 * Asks Microsoft if we're authorized to use a folder or file.
 *
 * Implementation Details: MSDN documentation imposes no limit on the
 * internal size of SECURITY_DESCRIPTOR, which we are responsible for
 * allocating. We've selected 1024 which shall hopefully be adequate.
 *
 * @param flags can have R_OK, W_OK, X_OK, etc.
 * @return 0 if authorized, or -1 w/ errno
 * @kudos Aaron Ballman for teaching this
 * @see libc/sysv/consts.sh
 */
textwindows int ntaccesscheck(const char16_t *pathname, uint32_t flags) {
  int rc;
  bool32 result;
  struct NtGenericMapping mapping;
  struct NtPrivilegeSet privileges;
  int64_t hToken, hImpersonatedToken;
  uint32_t secsize, granted, privsize;
  union NtSecurityDescriptorLol {
    struct NtSecurityDescriptor s;
    char b[1024];
  } security;
  granted = 0;
  result = false;
  secsize = sizeof(security);
  privsize = sizeof(privileges);
  memset(&privileges, 0, sizeof(privileges));
  mapping.GenericRead = kNtFileGenericRead;
  mapping.GenericWrite = kNtFileGenericWrite;
  mapping.GenericExecute = kNtFileGenericExecute;
  mapping.GenericAll = kNtFileAllAccess;
  MapGenericMask(&flags, &mapping);
  hImpersonatedToken = hToken = -1;
  if (GetFileSecurity(pathname,
                      kNtOwnerSecurityInformation |
                          kNtGroupSecurityInformation |
                          kNtDaclSecurityInformation,
                      &security.s, 0, &secsize) &&
      OpenProcessToken(GetCurrentProcess(),
                       kNtTokenImpersonate | kNtTokenQuery | kNtTokenDuplicate |
                           kNtStandardRightsRead,
                       &hToken) &&
      DuplicateToken(hToken, kNtSecurityImpersonation, &hImpersonatedToken) &&
      AccessCheck(&security.s, hImpersonatedToken, flags, &mapping, &privileges,
                  &privsize, &granted, &result) &&
      (result || flags == F_OK)) {
    rc = 0;
  } else {
    rc = __winerr();
  }
  close(hImpersonatedToken);
  close(hToken);
  return rc;
}
