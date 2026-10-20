/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
│ Copyright 2026 Gavin Arthur Hayes                                            │
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
#include "libc/calls/syscall-nt.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"

static bool isAbsPath(const char *path) {
  return strlen(path) >= 2 && path[0] == '/' && path[1] != '/';
}

static bool isSameFile(const char *a, const char *b) {
  struct stat st_a, st_b;
  if (stat(a, &st_a) || stat(b, &st_b)) {
    return false;
  }
  return (st_a.st_dev == st_b.st_dev) && (st_a.st_ino == st_b.st_ino);
}

int main(int argc, char *argv[]) {
  if (argc >= 2 && !strcmp(argv[1], "Child")) {
    int rc;
    if (!IsWindows()) {
      rc = sys_chdir("/");
    } else {
      rc = sys_chdir_nt("/");
    }
    if (rc) {
      exit(122);
    }
    const char *path = GetProgramExecutableName();
    if (!path || !isAbsPath(path) || !isSameFile(argv[2], path)) {
      exit(123);
    }
    if (argc >= 4) {
      if (strcmp(argv[3], argv[0])) {
        exit(124);
      }
    }
    exit(0);
  }
  exit(128);
}
