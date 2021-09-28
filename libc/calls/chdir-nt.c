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
#include "libc/macros.internal.h"
#include "libc/nt/errors.h"
#include "libc/nt/files.h"
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"
#include "libc/nt/synchronization.h"
#include "libc/sysv/errfuns.h"

textwindows int sys_chdir_nt(const char *path) {
  uint32_t n;
  int e, ms, len;
  char16_t path16[PATH_MAX], var[4];
  if ((len = __mkntpath(path, path16)) == -1) return -1;
  if (len && path16[len - 1] != u'\\') {
    if (len + 2 > PATH_MAX) return enametoolong();
    path16[len + 0] = u'\\';
    path16[len + 1] = u'\0';
  }
  /*
   * chdir() seems flaky on windows 7
   * in a similar way to rmdir() sigh
   */
  for (ms = 1;; ms *= 2) {
    if (SetCurrentDirectory(path16)) {
      /*
       * Now we need to set a magic environment variable.
       */
      if ((n = GetCurrentDirectory(ARRAYLEN(path16), path16))) {
        if (n < ARRAYLEN(path16)) {
          if (!((path16[0] == '/' && path16[1] == '/') ||
                (path16[0] == '\\' && path16[1] == '\\'))) {
            var[0] = '=';
            var[1] = path16[0];
            var[2] = ':';
            var[3] = 0;
            if (!SetEnvironmentVariable(var, path16)) {
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
        continue;
      } else {
        break;
      }
    }
  }
  errno = e;
  return -1;
}
