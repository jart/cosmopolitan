/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/dce.h"
#include "libc/log/log.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/o.h"

static int __klog_fd;
extern long __klog_handle;

/**
 * Redirects kprintf(), `--strace`, etc. output to file.
 *
 * @param path is filename to append to; if null is specified then
 *     this file logging facility will be disabled; when the empty
 *     string is specified, then the default path shall be used
 * @return 0 on success, or -1 w/ errno
 */
int LogKprintfToFile(const char *path) {
  int fd, dd;
  if (!path) {
    if (__klog_fd) {
      __klog_handle = 0;
      close(__klog_fd);
      __klog_fd = 0;
    }
    return 0;
  }
  if (!*path) path = "/tmp/kprintf.log";
  fd = open(path, O_WRONLY | O_APPEND | O_CREAT, 0644);
  if (fd == -1) return -1;
  dd = fcntl(fd, F_DUPFD_CLOEXEC, 100);
  close(fd);
  if (dd == -1) return -1;
  if (__klog_fd) close(__klog_fd);
  __klog_fd = dd;
  __klog_handle = IsWindows() ? g_fds.p[dd].handle : dd;
  return 0;
}
