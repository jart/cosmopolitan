/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/internal.h"
#include "libc/calls/syscall-nt.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/intrin/fds.h"
#include "libc/nt/enum/accessmask.h"
#include "libc/nt/enum/fileflagandattributes.h"
#include "libc/nt/enum/fileinfobyhandleclass.h"
#include "libc/nt/files.h"
#include "libc/nt/struct/filebasicinfo.h"
#include "libc/sysv/errfuns.h"
#include "libc/sysv/pib.h"

textwindows bool32 sys_fchmod_nt_handle(intptr_t handle, uint32_t mode) {

  // get current information
  struct NtFileBasicInfo fbi;
  if (!GetFileInformationByHandleEx(handle, kNtFileBasicInfo, &fbi,
                                    sizeof(fbi)))
    return false;

  // change attributes
  if (mode & 0200) {
    fbi.FileAttributes &= ~kNtFileAttributeReadonly;
  } else {
    fbi.FileAttributes |= kNtFileAttributeReadonly;
  }

  // set new attributes
  if (!SetFileInformationByHandle(handle, kNtFileBasicInfo, &fbi, sizeof(fbi)))
    return false;

  // on unix saying `chmod -w dir` prevents writing files in that dir.
  // however on windows, setting kNtFileAttributeReadonly on a directory
  // doesn't prevent us from creating files within that directory. so we
  // need to do something special to make the expected behavior happen.
  unsigned dwDenyMask = 0;
  if (fbi.FileAttributes & kNtFileAttributeDirectory)
    if (fbi.FileAttributes & kNtFileAttributeReadonly)
      dwDenyMask |=
          kNtFileAddFile | kNtFileAddSubdirectory | kNtFileDeleteChild;
  if (~mode & 0400)
    dwDenyMask |= kNtFileReadData;
  if (!RestrictFileWin32(handle, 0))
    return false;
  if (dwDenyMask)
    if (!RestrictFileWin32(handle, dwDenyMask))
      return false;

  // all good
  return true;
}

textwindows int sys_fchmod_nt(int fd, uint32_t mode) {
  if (!__isfdkind(fd, kFdFile))
    return ebadf();
  if (!sys_fchmod_nt_handle(__get_pib()->fds.p[fd].handle, mode))
    return __winerr();
  return 0;
}
