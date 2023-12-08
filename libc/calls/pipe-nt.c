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
#include "libc/calls/sig.internal.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/nt/createfile.h"
#include "libc/nt/enum/accessmask.h"
#include "libc/nt/enum/creationdisposition.h"
#include "libc/nt/enum/fileflagandattributes.h"
#include "libc/nt/enum/filesharemode.h"
#include "libc/nt/ipc.h"
#include "libc/nt/runtime.h"
#include "libc/sysv/consts/limits.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/errfuns.h"

static textwindows int sys_pipe_nt_impl(int pipefd[2], unsigned flags) {
  uint32_t mode;
  int64_t hin, hout;
  int reader, writer;
  char16_t pipename[64];
  __create_pipe_name(pipename);
  __fds_lock();
  if ((reader = __reservefd_unlocked(-1)) == -1) {
    __fds_unlock();
    return -1;
  }
  if ((writer = __reservefd_unlocked(-1)) == -1) {
    __releasefd(reader);
    __fds_unlock();
    return -1;
  }
  if (~flags & O_DIRECT) {
    mode = kNtPipeTypeByte | kNtPipeReadmodeByte;
  } else {
    mode = kNtPipeTypeMessage | kNtPipeReadmodeMessage;
  }
  __fds_unlock();
  hin = CreateNamedPipe(pipename, kNtPipeAccessInbound | kNtFileFlagOverlapped,
                        mode, 1, PIPE_BUF, PIPE_BUF, 0, &kNtIsInheritable);
  __fds_lock();
  if (hin != -1) {
    if ((hout = CreateFile(
             pipename, kNtGenericWrite,
             kNtFileShareRead | kNtFileShareWrite | kNtFileShareDelete,
             &kNtIsInheritable, kNtOpenExisting, kNtFileFlagOverlapped, 0)) !=
        -1) {
      g_fds.p[reader].kind = kFdFile;
      g_fds.p[reader].flags = O_RDONLY | flags;
      g_fds.p[reader].mode = 0010444;
      g_fds.p[reader].handle = hin;
      g_fds.p[writer].kind = kFdFile;
      g_fds.p[writer].flags = O_WRONLY | flags;
      g_fds.p[writer].mode = 0010222;
      g_fds.p[writer].handle = hout;
      pipefd[0] = reader;
      pipefd[1] = writer;
      __fds_unlock();
      return 0;
    } else {
      CloseHandle(hin);
    }
  }
  __releasefd(writer);
  __releasefd(reader);
  __fds_unlock();
  return -1;
}

textwindows int sys_pipe_nt(int pipefd[2], unsigned flags) {
  int rc;
  BLOCK_SIGNALS;
  rc = sys_pipe_nt_impl(pipefd, flags);
  ALLOW_SIGNALS;
  return rc;
}
