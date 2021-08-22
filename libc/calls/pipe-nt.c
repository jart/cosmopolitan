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
#include "libc/nt/enum/accessmask.h"
#include "libc/nt/enum/creationdisposition.h"
#include "libc/nt/ipc.h"
#include "libc/nt/runtime.h"
#include "libc/sysv/errfuns.h"

textwindows int sys_pipe_nt(int pipefd[2], unsigned flags) {
  int64_t hin, hout;
  int reader, writer;
  char16_t pipename[64];
  if (!pipefd) return efault();
  CreatePipeName(pipename);
  if ((reader = __reservefd()) == -1) return -1;
  if ((writer = __reservefd()) == -1) {
    __releasefd(reader);
    return -1;
  }
  if ((hin = CreateNamedPipe(pipename, kNtPipeAccessInbound,
                             kNtPipeWait | kNtPipeReadmodeByte, 1, 65536, 65536,
                             0, &kNtIsInheritable)) != -1) {
    if ((hout = CreateFile(pipename, kNtGenericWrite, 0, &kNtIsInheritable,
                           kNtOpenExisting, 0, 0)) != -1) {
      g_fds.p[reader].kind = kFdFile;
      g_fds.p[reader].flags = flags;
      g_fds.p[reader].handle = hin;
      g_fds.p[writer].kind = kFdFile;
      g_fds.p[writer].flags = flags;
      g_fds.p[writer].handle = hout;
      pipefd[0] = reader;
      pipefd[1] = writer;
      return 0;
    } else {
      __winerr();
      CloseHandle(hin);
    }
  } else {
    __winerr();
  }
  __releasefd(writer);
  __releasefd(reader);
  return -1;
}
