/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/struct/stat.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/errfuns.h"

#define XNU_F_GETPATH  50
#define XNU_MAXPATHLEN 1024

char *sys_getcwd_xnu(char *res, size_t size) {
  int fd;
  struct stat st[2];
  char buf[XNU_MAXPATHLEN], *ret = NULL;
  if ((fd = sys_openat(AT_FDCWD, ".", O_RDONLY | O_DIRECTORY, 0)) != -1) {
    if (sys_fstat(fd, &st[0]) != -1) {
      if (st[0].st_dev && st[0].st_ino) {
        if (__sys_fcntl(fd, XNU_F_GETPATH, (uintptr_t)buf) != -1) {
          if (sys_fstatat(AT_FDCWD, buf, &st[1], 0) != -1) {
            if (st[0].st_dev == st[1].st_dev && st[0].st_ino == st[1].st_ino) {
              if (memccpy(res, buf, '\0', size)) {
                ret = res;
              } else {
                erange();
              }
            } else {
              einval();
            }
          }
        }
      } else {
        einval();
      }
    }
    close(fd);
  }
  return ret;
}
