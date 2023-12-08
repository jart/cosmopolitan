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
#include "libc/calls/struct/stat.h"
#include "libc/intrin/safemacros.internal.h"
#include "libc/macros.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/sig.h"
#include "libc/time/time.h"
#include "libc/x/xsigaction.h"

/**
 * @fileoverview tail -f with lower poll rate
 * @see busybox not having interval flag
 */

int fd;
bool exited;
struct stat st;
char buf[FRAMESIZE];

int WriteString(const char *s) {
  return write(1, s, strlen(s));
}

void HideCursor(void) {
  WriteString("\e[?25l");
}

void ShowCursor(void) {
  WriteString("\e[?25h");
}

void OnInt(void) {
  exited = true;
}

void OnExit(void) {
  ShowCursor();
}

int main(int argc, char *argv[]) {
  char *p;
  ssize_t n;
  size_t i, j;
  bool chopped;
  if (argc < 2) return 1;
  if ((fd = open(argv[1], O_RDONLY)) == -1) return 2;
  if (fstat(fd, &st) == -1) return 3;
  n = st.st_size - MIN(st.st_size, sizeof(buf));
  if ((n = pread(fd, buf, sizeof(buf), n)) == -1) return 4;
  for (p = buf + n, i = 0; i < 10; ++i) {
    p = firstnonnull(memrchr(buf, '\n', p - buf), buf);
  }
  chopped = false;
  if (buf + n - p) ++p;
  i = st.st_size - (buf + n - p);
  atexit(OnExit);
  HideCursor();
  xsigaction(SIGINT, OnInt, 0, 0, 0);
  xsigaction(SIGTERM, OnInt, 0, 0, 0);
  while (!exited) {
    if (fstat(fd, &st) == -1) return 5;
    if (i > st.st_size) i = 0;
    for (; i < st.st_size; i += n) {
      if ((n = pread(fd, buf, sizeof(buf), i)) == -1) return 6;
      j = n;
      while (j && (buf[j - 1] == '\n' || buf[j - 1] == '\r')) --j;
      if (j) {
        if (chopped) {
          WriteString("\r\n");
        }
        write(1, buf, j);
      }
      chopped = j != n;
    }
    usleep(10000);
  }
  close(fd);
  WriteString("\r\n");
  return 0;
}
