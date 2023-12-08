/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/log/libfatal.internal.h"
#include "libc/runtime/internal.h"
#include "libc/str/str.h"

textstartup bool __intercept_flag(int *argc, char *argv[], const char *flag) {
  /* asan isn't initialized yet at runlevel 300 */
  char *a;
  int i, j;
  bool found;
  found = false;
  for (j = i = 1; i <= *argc;) {
    a = argv[j++];
    if (a && !__strcmp(a, flag)) {
      found = true;
      --*argc;
    } else {
      /*
       * e.g. turns ----strace → --strace for execve.
       * todo: update this to allow ------strace etc.
       */
      if (a && a[0] == '-' && a[1] == '-' && !__strcmp(a + 2, flag)) {
        a = (char *)flag;
      }
      argv[i++] = a;
    }
  }
  return found;
}
