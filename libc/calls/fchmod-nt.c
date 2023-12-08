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
#include "libc/calls/struct/fd.internal.h"
#include "libc/calls/syscall-nt.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/nt/enum/fileflagandattributes.h"
#include "libc/nt/enum/fileinfobyhandleclass.h"
#include "libc/nt/files.h"
#include "libc/nt/struct/filebasicinfo.h"
#include "libc/sysv/errfuns.h"

textwindows int sys_fchmod_nt(int fd, uint32_t mode) {

  // validate file descriptor
  if (fd + 0u >= g_fds.n) return ebadf();
  if (g_fds.p[fd].kind == kFdEmpty) return ebadf();

  // get current information
  struct NtFileBasicInfo fbi;
  if (!GetFileInformationByHandleEx(g_fds.p[fd].handle, kNtFileBasicInfo, &fbi,
                                    sizeof(fbi))) {
    return __winerr();
  }

  // change attributes
  if (mode & 0222) {
    fbi.FileAttributes &= ~kNtFileAttributeReadonly;
  } else {
    fbi.FileAttributes |= kNtFileAttributeReadonly;
  }

  // set new attributes
  if (!SetFileInformationByHandle(g_fds.p[fd].handle, kNtFileBasicInfo, &fbi,
                                  sizeof(fbi))) {
    return __winerr();
  }

  // all good
  return 0;
}
