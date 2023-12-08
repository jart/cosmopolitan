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
#include "libc/calls/internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/nt/enum/fileflagandattributes.h"
#include "libc/nt/enum/filetype.h"
#include "libc/nt/files.h"
#include "libc/nt/struct/byhandlefileinformation.h"
#include "libc/sysv/errfuns.h"
#ifdef __x86_64__

textwindows int sys_fdatasync_nt(int fd, bool fake) {
  struct NtByHandleFileInformation wst;
  if (!__isfdopen(fd)) return ebadf();
  if (!__isfdkind(fd, kFdFile)) return einval();
  if (GetFileType(g_fds.p[fd].handle) != kNtFileTypeDisk) return einval();
  if (!GetFileInformationByHandle(g_fds.p[fd].handle, &wst)) return __winerr();
  if (wst.dwFileAttributes & kNtFileAttributeDirectory) {
    // Flushing a directory handle is possible, but it needs
    // kNtGenericWrite access, and MSDN doesn't document it.
    return 0;
  }
  if (fake) return 0;
  if (_check_signal(false) == -1) return -1;
  return FlushFileBuffers(g_fds.p[fd].handle) ? 0 : __winerr();
}

#endif /* __x86_64__ */
