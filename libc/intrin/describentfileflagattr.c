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
#include "libc/nt/enum/fileflagandattributes.h"
#include "libc/runtime/runtime.h"

static const struct DescribeFlags kFileFlags[] = {
    {kNtFileAttributeReadonly, "AttributeReadonly"},                    //
    {kNtFileAttributeHidden, "AttributeHidden"},                        //
    {kNtFileAttributeSystem, "AttributeSystem"},                        //
    {kNtFileAttributeVolumelabel, "AttributeVolumelabel"},              //
    {kNtFileAttributeDirectory, "AttributeDirectory"},                  //
    {kNtFileAttributeArchive, "AttributeArchive"},                      //
    {kNtFileAttributeDevice, "AttributeDevice"},                        //
    {kNtFileAttributeNormal, "AttributeNormal"},                        //
    {kNtFileAttributeTemporary, "AttributeTemporary"},                  //
    {kNtFileAttributeSparseFile, "AttributeSparseFile"},                //
    {kNtFileAttributeReparsePoint, "AttributeReparsePoint"},            //
    {kNtFileAttributeCompressed, "AttributeCompressed"},                //
    {kNtFileAttributeOffline, "AttributeOffline"},                      //
    {kNtFileAttributeNotContentIndexed, "AttributeNotContentIndexed"},  //
    {kNtFileAttributeEncrypted, "AttributeEncrypted"},                  //
    {kNtFileFlagWriteThrough, "FlagWriteThrough"},                      //
    {kNtFileFlagOverlapped, "FlagOverlapped"},                          //
    {kNtFileFlagNoBuffering, "FlagNoBuffering"},                        //
    {kNtFileFlagRandomAccess, "FlagRandomAccess"},                      //
    {kNtFileFlagSequentialScan, "FlagSequentialScan"},                  //
    {kNtFileFlagDeleteOnClose, "FlagDeleteOnClose"},                    //
    {kNtFileFlagBackupSemantics, "FlagBackupSemantics"},                //
    {kNtFileFlagPosixSemantics, "FlagPosixSemantics"},                  //
    {kNtFileFlagOpenReparsePoint, "FlagOpenReparsePoint"},              //
    {kNtFileFlagOpenNoRecall, "FlagOpenNoRecall"},                      //
    {kNtFileFlagFirstPipeInstance, "FlagFirstPipeInstance"},            //
};

const char *(DescribeNtFileFlagAttr)(char buf[256], uint32_t x) {
  if (x == -1u) return "-1u";
  return DescribeFlags(buf, 256, kFileFlags, ARRAYLEN(kFileFlags), "kNtFile",
                       x);
}
