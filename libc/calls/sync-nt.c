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
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/calls/syscall-nt.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/nt/createfile.h"
#include "libc/nt/enum/accessmask.h"
#include "libc/nt/enum/creationdisposition.h"
#include "libc/nt/enum/filesharemode.h"
#include "libc/nt/enum/securityimpersonationlevel.h"
#include "libc/nt/enum/securityinformation.h"
#include "libc/nt/files.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/genericmapping.h"
#include "libc/nt/struct/privilegeset.h"
#include "libc/nt/struct/securitydescriptor.h"
#include "libc/sysv/consts/ok.h"
#include "libc/sysv/pib.h"

// https://blog.aaronballman.com/2011/08/how-to-check-access-rights/
textwindows static bool32 MyAccessCheck(const char16_t *pathname,
                                        uint32_t accessmask) {

  intptr_t buffer[1024 / sizeof(intptr_t)];
  uint32_t secsize = sizeof(buffer);
  struct NtSecurityDescriptor *s = (struct NtSecurityDescriptor *)buffer;
  if (!GetFileSecurity(pathname,
                       kNtOwnerSecurityInformation |
                           kNtGroupSecurityInformation |
                           kNtDaclSecurityInformation,
                       s, secsize, &secsize))
    return false;

  intptr_t hToken;
  if (!OpenProcessToken(GetCurrentProcess(),
                        kNtTokenImpersonate | kNtTokenQuery |
                            kNtTokenDuplicate | kNtStandardRightsRead,
                        &hToken))
    return false;

  intptr_t hImpersonatedToken;
  if (!DuplicateToken(hToken, kNtSecurityImpersonation, &hImpersonatedToken)) {
    CloseHandle(hToken);
    return false;
  }

  uint32_t granted = 0;
  bool32 result = false;
  struct NtPrivilegeSet privileges = {0};
  uint32_t privsize = sizeof(privileges);
  struct NtGenericMapping mapping = {
      .GenericRead = kNtFileGenericRead,
      .GenericWrite = kNtFileGenericWrite,
      .GenericExecute = kNtFileGenericExecute,
      .GenericAll = kNtFileAllAccess,
  };
  MapGenericMask(&accessmask, &mapping);
  if (!AccessCheck(s, hImpersonatedToken, accessmask, &mapping, &privileges,
                   &privsize, &granted, &result)) {
    CloseHandle(hImpersonatedToken);
    CloseHandle(hToken);
    return false;
  }

  CloseHandle(hImpersonatedToken);
  CloseHandle(hToken);
  return result;
}

// Flushes all open file handles and, if possible, all disk drives.
textwindows int sys_sync_nt(void) {

  // flush handles
  __fds_lock();
  for (int i = 0; i < __get_pib()->fds.n; ++i)
    if (__get_pib()->fds.p[i].kind == kFdFile)
      FlushFileBuffers(__get_pib()->fds.p[i].handle);
  __fds_unlock();

  // flush drives
  char16_t path[] = u"\\\\.\\C:";
  uint32_t drives = GetLogicalDrives();
  for (int i = 0; i <= 'Z' - 'A'; ++i) {
    if (!(drives & (1 << i)))
      continue;
    path[4] = 'A' + i;
    if (MyAccessCheck(path, kNtGenericRead | kNtGenericWrite)) {
      intptr_t volume;
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
