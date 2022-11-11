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
#include "libc/calls/calls.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"

/**
 * Executes binary executable at file descriptor.
 *
 * This is only supported on Linux and FreeBSD. APE binaries currently
 * aren't supported.
 *
 * @param fd is opened executable and current file position is ignored
 * @return doesn't return on success, otherwise -1 w/ errno
 * @raise ENOEXEC if file at `fd` isn't an assimilated ELF executable
 * @raise ENOSYS on Windows, XNU, OpenBSD, NetBSD, and Metal
 */
int fexecve(int fd, char *const argv[], char *const envp[]) {
  int rc;
  size_t i;
  if (!argv || !envp ||
      (IsAsan() &&
       (!__asan_is_valid_strlist(argv) || !__asan_is_valid_strlist(envp)))) {
    rc = efault();
  } else {
    STRACE("fexecve(%d, %s, %s) → ...", fd, DescribeStringList(argv),
           DescribeStringList(envp));
    if (IsLinux()) {
      char path[14 + 12];
      FormatInt32(stpcpy(path, "/proc/self/fd/"), fd);
      rc = __sys_execve(path, argv, envp);
    } else if (IsFreebsd()) {
      rc = sys_fexecve(fd, argv, envp);
    } else {
      rc = enosys();
    }
  }
  STRACE("fexecve(%d) failed %d% m", fd, rc);
  return rc;
}
