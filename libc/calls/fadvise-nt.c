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
#include "libc/calls/internal.h"
#include "libc/nt/createfile.h"
#include "libc/nt/enum/fileflagandattributes.h"
#include "libc/nt/enum/filesharemode.h"
#include "libc/nt/enum/status.h"
#include "libc/nt/files.h"
#include "libc/nt/nt/file.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/fileaccessinformation.h"
#include "libc/nt/struct/filebasicinformation.h"
#include "libc/nt/struct/iostatusblock.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/madv.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/errfuns.h"

textwindows int sys_fadvise_nt(int fd, uint64_t offset, uint64_t len,
                               int advice) {
  int64_t h1, h2;
  int flags, mode;
  uint32_t perm, share, attr;
  if (!__isfdkind(fd, kFdFile)) return ebadf();
  h1 = g_fds.p[fd].handle;
  mode = g_fds.p[fd].mode;
  flags = g_fds.p[fd].flags;
  flags &= ~(O_SEQUENTIAL | O_RANDOM);
  switch (advice) {
    case MADV_NORMAL:
      break;
    case MADV_RANDOM:
      flags |= O_RANDOM;
      break;
    case MADV_SEQUENTIAL:
      flags |= O_SEQUENTIAL;
      break;
    default:
      return einval();
  }
  if (GetNtOpenFlags(flags, mode, &perm, &share, 0, &attr) == -1) return -1;
  if ((h2 = ReOpenFile(h1, perm, share, attr)) != -1) {
    if (h2 != h1) {
      CloseHandle(h1);
      g_fds.p[fd].handle = h2;
    }
    g_fds.p[fd].flags = flags;
    return 0;
  } else {
    return __winerr();
  }
}
