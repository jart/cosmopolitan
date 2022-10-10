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
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/promises.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/stack.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"

static char stacklog[1024];

size_t GetStackUsage(char *s, size_t n) {
  // RHEL5 MAP_GROWSDOWN seems to only grow to 68kb :'(
  // So we count non-zero bytes down from the top
  // First clear 64 bytes is considered the end
  long *p;
  size_t got;
  p = (long *)(s + n);
  got = 0;
  for (;;) {
    p -= 8;
    if (p[0] | p[1] | p[2] | p[3] | p[4] | p[5] | p[6] | p[7]) {
      ++got;
    } else {
      break;
    }
  }
  return got * 8 * sizeof(long);
}

static textexit void LogStackUse(void) {
  bool quote;
  char *p, *q;
  int i, e, fd;
  size_t n, usage;
  if (!PLEDGED(STDIO) || !PLEDGED(WPATH) || !PLEDGED(CPATH)) return;
  usage = GetStackUsage((char *)GetStackAddr(), GetStackSize());
  e = errno;
  if ((fd = open(stacklog, O_APPEND | O_CREAT | O_WRONLY, 0644)) != -1) {
    p = FormatUint64(stacklog, usage);
    for (i = 0; i < __argc; ++i) {
      n = strlen(__argv[i]);
      if ((q = memchr(__argv[i], '\n', n))) n = q - __argv[i];
      if (p - stacklog + 1 + 1 + n + 1 + 1 < sizeof(stacklog)) {
        quote = !!memchr(__argv[i], ' ', n);
        *p++ = ' ';
        if (quote) *p++ = '\'';
        p = mempcpy(p, __argv[i], n);
        if (quote) *p++ = '\'';
      } else {
        break;
      }
    }
    *p++ = '\n';
    write(fd, stacklog, p - stacklog);
    close(fd);
  }
  errno = e;
}

static textstartup void LogStackUseInit(void) {
  if (IsTiny()) return;
  if (!PLEDGED(WPATH)) return;
  if (isdirectory("o/" MODE) &&
      getcwd(stacklog, sizeof(stacklog) - strlen("/o/" MODE "/stack.log"))) {
    strcat(stacklog, "/o/" MODE "/stack.log");
    atexit(LogStackUse);
  }
}

const void *const stack_usage_logging[] initarray = {
    LogStackUseInit,
};
