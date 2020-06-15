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
#include "libc/mem/mem.h"
#include "libc/nt/enum/accessmask.h"
#include "libc/nt/enum/securityinformation.h"
#include "libc/nt/errors.h"
#include "libc/nt/files.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/genericmapping.h"
#include "libc/nt/struct/privilegeset.h"
#include "libc/nt/struct/securitydescriptor.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/calls/internal.h"
#include "libc/calls/calls.h"
#include "libc/sysv/consts/ok.h"

/**
 * Checks if current process has access to folder or file.
 *
 * @param flags can have R_OK, W_OK, X_OK, etc.
 * @return 0 if authorized, or -1 w/ errno
 * @kudos Aaron Ballman for teaching how to do this
 * @see libc/sysv/consts.sh
 */
textwindows int ntaccesscheck(const char16_t *pathname, uint32_t flags) {
  int rc;
  bool32 result;
  int64_t hToken, hImpersonatedToken;
  uint32_t secsize, granted, privsize;
  struct NtPrivilegeSet privileges;
  struct NtGenericMapping mapping;
  struct NtSecurityDescriptor security;
  struct NtSecurityDescriptor *psecurity;
  const uint32_t request = kNtOwnerSecurityInformation |
                           kNtGroupSecurityInformation |
                           kNtDaclSecurityInformation;
  granted = 0;
  result = false;
  psecurity = &security;
  secsize = sizeof(security);
  privsize = sizeof(privileges);
  memset(&privileges, 0, sizeof(privileges));
  mapping.GenericRead = kNtFileGenericRead;
  mapping.GenericWrite = kNtFileGenericWrite;
  mapping.GenericExecute = kNtFileGenericExecute;
  mapping.GenericAll = kNtFileAllAccess;
  MapGenericMask(&flags, &mapping);
  hImpersonatedToken = hToken = -1;
  if ((GetFileSecurity(pathname, request, psecurity, 0, &secsize) ||
       (GetLastError() == kNtErrorInsufficientBuffer &&
        (psecurity = malloc(secsize)) &&
        GetFileSecurity(pathname, request, psecurity, secsize, &secsize))) &&
      OpenProcessToken(GetCurrentProcess(),
                       kNtTokenImpersonate | kNtTokenQuery | kNtTokenDuplicate |
                           kNtStandardRightsRead,
                       &hToken) &&
      DuplicateToken(hToken, kNtSecurityImpersonation, &hImpersonatedToken) &&
      AccessCheck(psecurity, hImpersonatedToken, flags, &mapping, &privileges,
                  &privsize, &granted, &result) &&
      (result || flags == F_OK)) {
    rc = 0;
  } else {
    rc = winerr();
  }
  free_s(&psecurity);
  close(hImpersonatedToken);
  close(hToken);
  return rc;
}
