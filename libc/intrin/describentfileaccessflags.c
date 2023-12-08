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
#include "libc/intrin/describeflags.internal.h"
#include "libc/macros.internal.h"
#include "libc/nt/enum/accessmask.h"
#include "libc/nt/enum/filesharemode.h"
// clang-format off

static const struct DescribeFlags kFileAccessflags[] = {
    {kNtFileAllAccess, "kNtFileAllAccess"},
    {kNtFileGenericRead|kNtFileGenericWrite|kNtFileGenericExecute,
     "kNtFileGenericRead|kNtFileGenericWrite|kNtFileGenericExecute"},
    {kNtFileGenericRead|kNtFileGenericWrite,
     "kNtFileGenericRead|kNtFileGenericWrite"},
    {kNtFileGenericRead|kNtFileGenericExecute,
     "kNtFileGenericRead|kNtFileGenericExecute"},
    {kNtFileGenericWrite|kNtFileGenericExecute,
     "kNtFileGenericWrite|kNtFileGenericExecute"},
    {kNtFileGenericRead, "kNtFileGenericRead"},
    {kNtFileGenericWrite, "kNtFileGenericWrite"},
    {kNtFileGenericExecute, "kNtFileGenericExecute"},
    {kNtGenericRead, "kNtGenericRead"},
    {kNtGenericWrite, "kNtGenericWrite"},
    {kNtGenericExecute, "kNtGenericExecute"},
    {kNtGenericAll, "kNtGenericAll"},
    {kNtDelete, "kNtDelete"},
    {kNtReadControl, "kNtReadControl"},
    {kNtWriteDac, "kNtWriteDac"},
    {kNtWriteOwner, "kNtWriteOwner"},
    {kNtSynchronize, "kNtSynchronize"},
    {kNtStandardRightsRequired, "kNtStandardRightsRequired"},
    {kNtAccessSystemSecurity, "kNtAccessSystemSecurity"},
    {kNtMaximumAllowed, "kNtMaximumAllowed"},
    {kNtFileReadData, "kNtFileReadData"},
    {kNtFileListDirectory, "kNtFileListDirectory"},
    {kNtFileWriteData, "kNtFileWriteData"},
    {kNtFileAddFile, "kNtFileAddFile"},
    {kNtFileAppendData, "kNtFileAppendData"},
    {kNtFileAddSubdirectory, "kNtFileAddSubdirectory"},
    {kNtFileCreatePipeInstance, "kNtFileCreatePipeInstance"},
    {kNtFileReadEa, "kNtFileReadEa"},
    {kNtFileWriteEa, "kNtFileWriteEa"},
    {kNtFileExecute, "kNtFileExecute"},
    {kNtFileTraverse, "kNtFileTraverse"},
    {kNtFileDeleteChild, "kNtFileDeleteChild"},
    {kNtFileReadAttributes, "kNtFileReadAttributes"},
    {kNtFileWriteAttributes, "kNtFileWriteAttributes"},
    {kNtTokenAssignPrimary, "kNtTokenAssignPrimary"},
    {kNtTokenDuplicate, "kNtTokenDuplicate"},
    {kNtTokenImpersonate, "kNtTokenImpersonate"},
    {kNtTokenQuery, "kNtTokenQuery"},
    {kNtTokenQuerySource, "kNtTokenQuerySource"},
    {kNtTokenAdjustPrivileges, "kNtTokenAdjustPrivileges"},
    {kNtTokenAdjustGroups, "kNtTokenAdjustGroups"},
    {kNtTokenAdjustDefault, "kNtTokenAdjustDefault"},
    {kNtTokenAdjustSessionid, "kNtTokenAdjustSessionid"},
};

const char *(DescribeNtFileAccessFlags)(char buf[512], uint32_t x) {
  return DescribeFlags(buf, 512, kFileAccessflags, ARRAYLEN(kFileAccessflags),
                       "", x);
}
