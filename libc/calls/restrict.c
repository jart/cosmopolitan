/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2025 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/nt/enum/accessmode.h"
#include "libc/nt/enum/ace.h"
#include "libc/nt/enum/aclinfo.h"
#include "libc/nt/enum/inheritance.h"
#include "libc/nt/enum/localmem.h"
#include "libc/nt/enum/se.h"
#include "libc/nt/enum/securityinformation.h"
#include "libc/nt/errors.h"
#include "libc/nt/files.h"
#include "libc/nt/memory.h"
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/accessdeniedace.h"
#include "libc/nt/struct/acl.h"
#include "libc/nt/struct/aclsizeinfo.h"
#include "libc/nt/struct/explicitaccess.h"
#include "libc/nt/struct/securitydescriptor.h"
#include "libc/nt/struct/sid.h"
#include "libc/nt/struct/trustee.h"

/**
 * Applies restrictions to file.
 *
 * Handle must be opened with kNtReadControl and kNtWriteDac access.
 */
textwindows bool32 RestrictFileWin32(intptr_t hFile, uint32_t dwDenyMask) {
  bool32 bSuccess = false;
  uint32_t dwError = kNtErrorSuccess;

  void *pAce = NULL;
  struct NtSecurityDescriptor *pSD = NULL;
  struct NtAcl *pOldDacl = NULL;
  struct NtAcl *pNewDacl = NULL;
  struct NtSid *pEveryoneSid = NULL;
  struct NtAclSizeInformation aclSizeInfo;
  struct NtAccessDeniedAce *pDeniedAce = NULL;

  // Get the SID for "Everyone"
  struct NtSidIdentifierAuthority SIDAuthWorld = {kNtSecurityWorldSidAuthority};
  if (!AllocateAndInitializeSid(&SIDAuthWorld, 1, kNtSecurityWorldRid, 0, 0, 0,
                                0, 0, 0, 0, &pEveryoneSid)) {
    dwError = GetLastError();
    goto Cleanup;
  }

  // Need kNtReadControl to get SD, need kNtWriteDac to set SD later.
  // The handle hFile must have been opened with these rights.
  dwError = GetSecurityInfo(hFile, kNtSeFileObject, kNtDaclSecurityInformation,
                            NULL, NULL, &pOldDacl, NULL, &pSD);
  if (dwError != kNtErrorSuccess) {
    // If DACL is NULL, it grants full access implicitly. We need to
    // create one. However, GetSecurityInfo failing is usually a
    // dwDenyMask issue or invalid handle.
    goto Cleanup;
  }
  // A NULL DACL means unrestricted access. While we could build a new one,
  // it's safer to assume a DACL usually exists or fail if we can't get it.
  // If pOldDacl is NULL but pSD is not, it means unrestricted access.
  // Handling the NULL DACL case adds complexity; for now, assume it exists.
  if (pOldDacl == NULL) {
    SetLastError(kNtErrorInvalidAcl);  // Or another appropriate error
    goto Cleanup;                      // Or attempt to build a default DACL
  }

  if (dwDenyMask) {
    // Deny Write Access: Add an kNtAccessDeniedAce for Everyone
    struct NtExplicitAccess ea = {
        .grfAccessPermissions = dwDenyMask,
        .grfAccessMode = kNtDenyAccess,
        // Apply to the directory itself, not inherited by children.
        // kNtContainerInheritAce | kNtObjectInheritAce would affect
        // children. NO_INHERITANCE is closer to chmod on just the
        // directory.
        .grfInheritance = kNtNoInheritance,
        .Trustee.TrusteeForm = kNtTrusteeIsSid,
        .Trustee.TrusteeType = kNtTrusteeIsWellKnownGroup,
        .Trustee.pSid = pEveryoneSid,
    };

    // SetEntriesInAcl creates a *new* DACL by merging the old one with
    // the new entry. It correctly places DENY ACEs before ALLOW ACEs
    // for the same trustee.
    dwError = SetEntriesInAcl(1, &ea, pOldDacl, &pNewDacl);
    if (dwError != kNtErrorSuccess) {
      goto Cleanup;
    }
  } else {
    // Allow Write Access: Remove the specific ACCESS_DENIED_ACE. We
    // need to iterate through the existing DACL and build a new one
    // *without* the specific ACE we want to remove.
    if (!GetAclInformation(pOldDacl, &aclSizeInfo, sizeof(aclSizeInfo),
                           kNtAclSizeInformation)) {
      dwError = GetLastError();
      goto Cleanup;
    }

    // Allocate memory for the new DACL (worst case: same size as old)
    pNewDacl = (struct NtAcl *)LocalAlloc(kNtLptr, aclSizeInfo.AclBytesInUse);
    if (!pNewDacl) {
      dwError = kNtErrorOutofmemory;
      goto Cleanup;
    }

    if (!InitializeAcl(pNewDacl, aclSizeInfo.AclBytesInUse, kNtAclRevision)) {
      dwError = GetLastError();
      goto Cleanup;
    }

    // Iterate through the ACEs in the old DACL
    for (uint32_t i = 0; i < aclSizeInfo.AceCount; ++i) {
      if (!GetAce(pOldDacl, i, &pAce)) {
        dwError = GetLastError();
        goto Cleanup;
      }

      // Check if this is an kNtDenyAccess ACE
      if (((struct NtAceHeader *)pAce)->AceType == kNtAccessDeniedAceType) {
        pDeniedAce = (struct NtAccessDeniedAce *)pAce;
        // Check if the SID matches 'Everyone'
        if (EqualSid(pEveryoneSid, (struct NtSid *)&(pDeniedAce->SidStart))) {
          // XXX: this SHOULD be an ACE we added previously
          //      so we skip recreating it in the new dacl.
          continue;
        }
      }

      // If it's not the ACE to remove, add it to the new DACL
      if (!AddAce(pNewDacl, kNtAclRevision, 0xffffffffu, pAce,
                  ((struct NtAceHeader *)pAce)->AceSize)) {
        dwError = GetLastError();
        goto Cleanup;
      }
    }

    // If we iterated through and didn't find the specific ACE to
    // remove, there's nothing more to do for the 'allow' case. The
    // existing DACL is fine. However, we still need to apply pNewDacl
    // which is now a copy of pOldDacl minus the specific deny ACE (if
    // it was found).
  }

  // pNewDacl will be NULL if bApply was false and the specific ACE
  // wasn't found, OR if SetEntriesInAcl failed when bApply was true.
  // It will point to the modified DACL otherwise.
  if (pNewDacl) {
    dwError =
        SetSecurityInfo(hFile, kNtSeFileObject, kNtDaclSecurityInformation,
                        NULL, NULL, pNewDacl, NULL);
    if (dwError != kNtErrorSuccess) {
      goto Cleanup;
    }
    bSuccess = true;  // SetSecurityInfo succeeded
  } else if (!dwDenyMask) {
    // If bApply was false, and we didn't create a new DACL (because the
    // specific deny ACE wasn't found), then the operation is trivially
    // successful.
    bSuccess = true;
  }
  // If bApply was true and pNewDacl is NULL, SetEntriesInAcl failed,
  // handled by goto.

Cleanup:
  LocalFree(pSD);
  LocalFree(pNewDacl);
  if (pEveryoneSid)
    FreeSid(pEveryoneSid);
  if (!bSuccess && dwError != kNtErrorSuccess) {
    SetLastError(dwError);
  } else if (bSuccess) {
    SetLastError(kNtErrorSuccess);
  }
  return bSuccess;
}
