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
#include "libc/calls/struct/stat.h"
#include "libc/dce.h"
#include "libc/fmt/fmt.h"
#include "libc/log/check.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/o.h"
#include "tool/build/lib/psk.h"

/**
 * Returns preshared key for runit testing infrastructure.
 */
void *GetRunitPsk(void) {
  int fd;
  struct stat st;
  const char *a, *b;
  char *r, p[PATH_MAX + 1];
  if ((a = getenv("HOME"))) {
    b = "";
  } else if (IsWindows()) {
    a = getenv("HOMEDRIVE");
    b = getenv("HOMEPATH");
    if (!a || !b) {
      a = "C:";
      b = "";
    }
  } else {
    fprintf(stderr, "need $HOME\n");
    exit(1);
  }
  snprintf(p, sizeof(p), "%s%s/.runit.psk", a, b);
  if (stat(p, &st) == -1 || st.st_size != 32) {
    fprintf(stderr, "need o//examples/getrandom.com -bn32 >~/.runit.psk\n");
    exit(1);
  }
  CHECK_NOTNULL((r = malloc(32)));
  CHECK_NE(-1, (fd = open(p, O_RDONLY)));
  CHECK_EQ(32, read(fd, r, 32));
  CHECK_NE(-1, close(fd));
  return r;
}
