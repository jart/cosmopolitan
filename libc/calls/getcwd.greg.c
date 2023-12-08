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
#include "libc/calls/struct/metastat.internal.h"
#include "libc/calls/syscall-nt.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/strace.internal.h"
#include "libc/limits.h"
#include "libc/nt/files.h"
#include "libc/stdio/sysparam.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/errfuns.h"

#define XNU_F_GETPATH  50
#define XNU_MAXPATHLEN 1024

static int sys_getcwd_xnu(char *res, size_t size) {
  int fd, len, rc = -1;
  union metastat st[2];
  char buf[XNU_MAXPATHLEN];
  if ((fd = __sys_openat_nc(AT_FDCWD, ".", O_RDONLY | O_DIRECTORY, 0)) != -1) {
    if (__sys_fstat(fd, &st[0]) != -1) {
      if (st[0].xnu.st_dev && st[0].xnu.st_ino) {
        if (__sys_fcntl(fd, XNU_F_GETPATH, (uintptr_t)buf) != -1) {
          if (__sys_fstatat(AT_FDCWD, buf, &st[1], 0) != -1) {
            if (st[0].xnu.st_dev == st[1].xnu.st_dev &&
                st[0].xnu.st_ino == st[1].xnu.st_ino) {
              if ((len = strlen(buf)) < size) {
                memcpy(res, buf, (rc = len + 1));
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
    sys_close(fd);
  }
  return rc;
}

static int sys_getcwd_metal(char *buf, size_t size) {
  if (size >= 5) {
    strcpy(buf, "/zip");
    return 5;
  } else {
    return erange();
  }
}

static inline int IsAlpha(int c) {
  return ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z');
}

static dontinline textwindows int sys_getcwd_nt(char *buf, size_t size) {

  // get current directory from the system
  char16_t p16[PATH_MAX];
  uint32_t n = GetCurrentDirectory(PATH_MAX, p16);
  if (!n) return eacces();             // system call failed
  if (n >= PATH_MAX) return erange();  // not enough room?!?

  // convert utf-16 to utf-8
  // we can't modify `buf` until we're certain of success
  char p8[PATH_MAX], *p = p8;
  n = tprecode16to8(p, PATH_MAX, p16).ax;
  if (n >= PATH_MAX) return erange();  // utf-8 explosion

  // turn \\?\c:\... into c:\...
  if (p[0] == '\\' &&   //
      p[1] == '\\' &&   //
      p[2] == '?' &&    //
      p[3] == '\\' &&   //
      IsAlpha(p[4]) &&  //
      p[5] == ':' &&    //
      p[6] == '\\') {
    p += 4;
    n -= 4;
  }

  // turn c:\... into \c\...
  if (IsAlpha(p[0]) &&  //
      p[1] == ':' &&    //
      p[2] == '\\') {
    p[1] = p[0];
    p[0] = '\\';
  }

  // we now know the final length
  // check if the user supplied a buffer large enough
  if (n >= size) {
    return erange();
  }

  // copy bytes converting backslashes to slash
  for (int i = 0; i < n; ++i) {
    int c = p[i];
    if (c == '\\') {
      c = '/';
    }
    buf[i] = c;
  }

  // return number of bytes including nul
  buf[n++] = 0;
  return n;
}

/**
 * Returns current working directory.
 *
 * Cosmo provides this function to address the shortcomings of getcwd().
 * First, this function doesn't link malloc(). Secondly, this offers the
 * Linux and NetBSD's getcwd() API across platforms since it's the best.
 *
 * @param buf receives utf-8 path and isn't modified on error
 * @param size is byte capacity of `buf`
 * @return bytes copied including nul on success, or -1 w/ errno
 * @raise EACCES if the current directory path couldn't be accessed
 * @raise ERANGE if `size` wasn't big enough for path and nul byte
 * @raise EFAULT if `buf` points to invalid memory
 */
int __getcwd(char *buf, size_t size) {
  int rc;
  if (IsLinux() || IsNetbsd()) {
    rc = sys_getcwd(buf, size);
  } else if (IsXnu()) {
    rc = sys_getcwd_xnu(buf, size);
  } else if (IsWindows()) {
    rc = sys_getcwd_nt(buf, size);
  } else if (IsFreebsd() || IsOpenbsd()) {
    if (sys_getcwd(buf, size) != -1) {
      rc = strlen(buf) + 1;
    } else if (SupportsFreebsd() && (errno == ENOMEM || errno == EINVAL)) {
      rc = erange();
    } else {
      rc = -1;
    }
  } else {
    rc = sys_getcwd_metal(buf, size);
  }
  STRACE("getcwd([%#hhs], %'zu) → %d% m", rc != -1 ? buf : "n/a", size, rc);
  return rc;
}
