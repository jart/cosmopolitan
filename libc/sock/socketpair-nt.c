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
#include "libc/calls/state.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/nt/createfile.h"
#include "libc/nt/enum/accessmask.h"
#include "libc/nt/enum/creationdisposition.h"
#include "libc/nt/enum/fileflagandattributes.h"
#include "libc/nt/enum/filesharemode.h"
#include "libc/nt/ipc.h"
#include "libc/nt/runtime.h"
#include "libc/sock/internal.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/sock.h"
#include "libc/sysv/errfuns.h"
#ifdef __x86_64__

textwindows int sys_socketpair_nt(int family, int type, int proto, int sv[2]) {
  uint32_t mode;
  int64_t hpipe, h1;
  char16_t pipename[64];
  int rc, reader, writer, oflags;

  // Supports only AF_UNIX
  if (family != AF_UNIX) {
    return eafnosupport();
  }

  oflags = 0;
  if (type & SOCK_CLOEXEC) oflags |= O_CLOEXEC;
  type &= ~SOCK_CLOEXEC;

  if (type == SOCK_STREAM) {
    mode = kNtPipeTypeByte | kNtPipeReadmodeByte;
  } else if ((type == SOCK_DGRAM) || (type == SOCK_SEQPACKET)) {
    mode = kNtPipeTypeMessage | kNtPipeReadmodeMessage;
  } else {
    return eopnotsupp();
  }

  __create_pipe_name(pipename);
  __fds_lock();
  reader = __reservefd_unlocked(-1);
  writer = __reservefd_unlocked(-1);
  __fds_unlock();
  if (reader == -1 || writer == -1) {
    if (reader != -1) __releasefd(reader);
    if (writer != -1) __releasefd(writer);
    return -1;
  }
  if ((hpipe = CreateNamedPipe(
           pipename, kNtPipeAccessDuplex | kNtFileFlagOverlapped, mode, 1,
           65536, 65536, 0, &kNtIsInheritable)) == -1) {
    __releasefd(writer);
    __releasefd(reader);
    return -1;
  }

  h1 = CreateFile(pipename, kNtGenericWrite | kNtGenericRead,
                  kNtFileShareRead | kNtFileShareWrite | kNtFileShareDelete,
                  &kNtIsInheritable, kNtOpenExisting, kNtFileFlagOverlapped, 0);

  __fds_lock();

  if (h1 != -1) {

    g_fds.p[reader].kind = kFdFile;
    g_fds.p[reader].flags = O_RDWR | oflags;
    g_fds.p[reader].mode = 0140444;
    g_fds.p[reader].handle = hpipe;

    g_fds.p[writer].kind = kFdFile;
    g_fds.p[writer].flags = O_RDWR | oflags;
    g_fds.p[writer].mode = 0140222;
    g_fds.p[writer].handle = h1;

    sv[0] = reader;
    sv[1] = writer;

    rc = 0;
  } else {
    CloseHandle(hpipe);
    __releasefd(writer);
    __releasefd(reader);
    rc = -1;
  }

  __fds_unlock();

  return rc;
}

#endif /* __x86_64__ */
