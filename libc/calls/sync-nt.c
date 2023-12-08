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
#include "libc/calls/struct/sigset.internal.h"
#include "libc/calls/syscall-nt.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/nt/createfile.h"
#include "libc/nt/enum/accessmask.h"
#include "libc/nt/enum/creationdisposition.h"
#include "libc/nt/enum/filesharemode.h"
#include "libc/nt/files.h"
#include "libc/nt/runtime.h"
#include "libc/sysv/consts/ok.h"

// Flushes all open file handles and, if possible, all disk drives.
textwindows int sys_sync_nt(void) {
  unsigned i;
  int64_t volume;
  uint32_t drives;
  char16_t path[] = u"\\\\.\\C:";
  for (i = 0; i < g_fds.n; ++i) {
    if (g_fds.p[i].kind == kFdFile) {
      FlushFileBuffers(g_fds.p[i].handle);
    }
  }
  for (drives = GetLogicalDrives(), i = 0; i <= 'Z' - 'A'; ++i) {
    if (!(drives & (1 << i))) continue;
    path[4] = 'A' + i;
    if (ntaccesscheck(path, R_OK | W_OK) != -1) {
      BLOCK_SIGNALS;
      if ((volume = CreateFile(
               path, kNtFileReadAttributes,
               kNtFileShareRead | kNtFileShareWrite | kNtFileShareDelete, 0,
               kNtOpenExisting, 0, 0)) != -1) {
        FlushFileBuffers(volume);
        CloseHandle(volume);
      }
      ALLOW_SIGNALS;
    }
  }
  return 0;
}
