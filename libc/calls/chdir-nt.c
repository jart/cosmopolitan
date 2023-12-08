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
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/errno.h"
#include "libc/macros.internal.h"
#include "libc/nt/errors.h"
#include "libc/nt/files.h"
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"
#include "libc/nt/synchronization.h"
#include "libc/sysv/errfuns.h"

textwindows int sys_chdir_nt_impl(char16_t path[hasatleast PATH_MAX],
                                  uint32_t len) {
  uint32_t n;
  int e, ms, err;
  char16_t var[4];

  if (len && path[len - 1] != u'\\') {
    if (len + 2 > PATH_MAX) return enametoolong();
    path[len + 0] = u'\\';
    path[len + 1] = u'\0';
  }

  /*
   * chdir() seems flaky on windows 7
   * in a similar way to rmdir() sigh
   */
  for (err = errno, ms = 1;; ms *= 2) {
    if (SetCurrentDirectory(path)) {
      /*
       * Now we need to set a magic environment variable.
       */
      if ((n = GetCurrentDirectory(PATH_MAX, path))) {
        if (n < PATH_MAX) {
          if (!((path[0] == '/' && path[1] == '/') ||
                (path[0] == '\\' && path[1] == '\\'))) {
            var[0] = '=';
            var[1] = path[0];
            var[2] = ':';
            var[3] = 0;
            if (!SetEnvironmentVariable(var, path)) {
              return __winerr();
            }
          }
          return 0;
        } else {
          return enametoolong();
        }
      } else {
        return __winerr();
      }
    } else {
      e = GetLastError();
      if (ms <= 512 &&
          (e == kNtErrorFileNotFound || e == kNtErrorAccessDenied)) {
        Sleep(ms);
        errno = err;
        continue;
      } else {
        break;
      }
    }
  }
  return __fix_enotdir(-1, path);
}

textwindows int sys_chdir_nt(const char *path) {
  int len;
  char16_t path16[PATH_MAX];
  if ((len = __mkntpath(path, path16)) == -1) return -1;
  if (!len) return enoent();
  return sys_chdir_nt_impl(path16, len);
}
