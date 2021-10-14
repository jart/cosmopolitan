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
#include "libc/fmt/itoa.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/stack.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"

static char stacklog[1024];

static size_t NullLength(const char *s) {
  typedef char xmm_t __attribute__((__vector_size__(16), __aligned__(16)));
  size_t n;
  xmm_t v, z = {0};
  unsigned m, k = (uintptr_t)s & 15;
  const xmm_t *p = (const xmm_t *)((uintptr_t)s & -16);
  m = (__builtin_ia32_pmovmskb128(*p == z) ^ 0xffff) >> k << k;
  while (!m) m = __builtin_ia32_pmovmskb128(*++p == z) ^ 0xffff;
  n = (const char *)p + __builtin_ctzl(m) - s;
  return n;
}

static textexit void LogStackUse(void) {
  int i, fd;
  bool quote;
  char *p, *q;
  size_t n, usage;
  const char *stack;
  fd = open(stacklog, O_APPEND | O_CREAT | O_WRONLY, 0644);
  stack = (char *)GetStackAddr(0);
  usage = GetStackSize() - (NullLength(stack + PAGESIZE) + PAGESIZE);
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

static textstartup void LogStackUseInit(void) {
  if (IsTiny()) return;
  if (isdirectory("o/" MODE) &&
      getcwd(stacklog, sizeof(stacklog) - strlen("/o/" MODE "/stack.log"))) {
    strcat(stacklog, "/o/" MODE "/stack.log");
    atexit(LogStackUse);
  }
}

const void *const stack_usage_logging[] initarray = {
    LogStackUseInit,
};
