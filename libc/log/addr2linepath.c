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
#include "libc/calls/calls.h"
#include "libc/limits.h"
#include "libc/log/log.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"

static const char *TryMonoRepoPath(const char *var, const char *path) {
  char buf[PATH_MAX];
  if (getenv(var)) return 0;
  if (!isexecutable(path)) return 0;
  if (*path != '/') {
    if (getcwd(buf, sizeof(buf)) <= 0) return 0;
    strlcat(buf, "/", sizeof(buf));
    strlcat(buf, path, sizeof(buf));
    path = buf;
  }
  setenv(var, path, false);
  return getenv(var);
}

const char *GetAddr2linePath(void) {
  const char *s = 0;
#ifdef __x86_64__
  s = TryMonoRepoPath("ADDR2LINE",
                      "o/third_party/gcc/bin/x86_64-linux-musl-addr2line");
#elif defined(__aarch64__)
  s = TryMonoRepoPath("ADDR2LINE",
                      "o/third_party/gcc/bin/aarch64-linux-musl-addr2line");
#endif
  if (!s) s = commandvenv("ADDR2LINE", "addr2line");
  return s;
}
