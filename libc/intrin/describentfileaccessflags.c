/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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

#ifdef DescribeNtFileAccessFlags
#undef DescribeNtFileAccessFlags
#endif

static const struct DescribeFlags kFileAccessflags[] = {
    {kNtFileAllAccess, "FileAllAccess"},                    // order matters
    {kNtFileGenericRead, "FileGenericRead"},                // order matters
    {kNtFileGenericWrite, "FileGenericWrite"},              // order matters
    {kNtFileGenericExecute, "FileGenericExecute"},          // order matters
    {kNtGenericRead, "GenericRead"},                        //
    {kNtGenericWrite, "GenericWrite"},                      //
    {kNtGenericExecute, "GenericExecute"},                  //
    {kNtGenericAll, "GenericAll"},                          //
    {kNtDelete, "Delete"},                                  //
    {kNtReadControl, "ReadControl"},                        //
    {kNtWriteDac, "WriteDac"},                              //
    {kNtWriteOwner, "WriteOwner"},                          //
    {kNtSynchronize, "Synchronize"},                        //
    {kNtStandardRightsRequired, "StandardRightsRequired"},  //
    {kNtAccessSystemSecurity, "AccessSystemSecurity"},      //
    {kNtMaximumAllowed, "MaximumAllowed"},                  //
    {kNtFileReadData, "FileReadData"},                      //
    {kNtFileListDirectory, "FileListDirectory"},            //
    {kNtFileWriteData, "FileWriteData"},                    //
    {kNtFileAddFile, "FileAddFile"},                        //
    {kNtFileAppendData, "FileAppendData"},                  //
    {kNtFileAddSubdirectory, "FileAddSubdirectory"},        //
    {kNtFileCreatePipeInstance, "FileCreatePipeInstance"},  //
    {kNtFileReadEa, "FileReadEa"},                          //
    {kNtFileWriteEa, "FileWriteEa"},                        //
    {kNtFileExecute, "FileExecute"},                        //
    {kNtFileTraverse, "FileTraverse"},                      //
    {kNtFileDeleteChild, "FileDeleteChild"},                //
    {kNtFileReadAttributes, "FileReadAttributes"},          //
    {kNtFileWriteAttributes, "FileWriteAttributes"},        //
    {kNtTokenAssignPrimary, "TokenAssignPrimary"},          //
    {kNtTokenDuplicate, "TokenDuplicate"},                  //
    {kNtTokenImpersonate, "TokenImpersonate"},              //
    {kNtTokenQuery, "TokenQuery"},                          //
    {kNtTokenQuerySource, "TokenQuerySource"},              //
    {kNtTokenAdjustPrivileges, "TokenAdjustPrivileges"},    //
    {kNtTokenAdjustGroups, "TokenAdjustGroups"},            //
    {kNtTokenAdjustDefault, "TokenAdjustDefault"},          //
    {kNtTokenAdjustSessionid, "TokenAdjustSessionid"},      //
};

const char *DescribeNtFileAccessFlags(char buf[512], uint32_t x) {
  return DescribeFlags(buf, 512, kFileAccessflags, ARRAYLEN(kFileAccessflags),
                       "kNt", x);
}
