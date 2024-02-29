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
#include "libc/atomic.h"
#include "libc/calls/calls.h"
#include "libc/cosmo.h"
#include "libc/errno.h"
#include "libc/limits.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"

#ifdef __x86_64__
#define ADDR2LINE "cosmocc/3.2/bin/x86_64-linux-musl-addr2line"
#elif defined(__aarch64__)
#define ADDR2LINE "cosmocc/3.2/bin/aarch64-linux-musl-addr2line"
#endif

static struct {
  atomic_uint once;
  char *res;
  char buf[PATH_MAX];
} g_addr2line;

void GetAddr2linePathInit(void) {
  int e = errno;
  const char *path;
  if (!(path = getenv("ADDR2LINE"))) {
    path = ADDR2LINE;
  }
  char *buf = g_addr2line.buf;
  if (isexecutable(path)) {
    if (*path != '/' && getcwd(buf, PATH_MAX)) {
      strlcat(buf, "/", PATH_MAX);
    }
    strlcat(buf, path, PATH_MAX);
  }
  if (*buf) {
    g_addr2line.res = buf;
  } else {
    g_addr2line.res = commandv("addr2line", buf, PATH_MAX);
  }
  errno = e;
}

const char *GetAddr2linePath(void) {
  cosmo_once(&g_addr2line.once, GetAddr2linePathInit);
  return g_addr2line.res;
}
