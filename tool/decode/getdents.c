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
#include "libc/calls/internal.h"
#include "libc/mem/mem.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"

int main(int argc, char *argv[]) {
  char *p;
  int i, j, rc, fd;
  char16_t glyphs[17] = {0};
  p = malloc(4096);
  fd = open(".", O_RDONLY | O_DIRECTORY | O_CLOEXEC, 0);
  if ((rc = getdents(fd, p, 4096)) != -1) {
    for (i = 0; i < ROUNDUP(rc, 16); ++i) {
      if (i % 16 == 0) printf("%08x  ", i);
      if (i < rc) {
        glyphs[i % 16] = kCp437[p[i] & 0xff];
        printf("%02x ", p[i] & 0xff);
      } else {
        glyphs[i % 16] = u'\0';
        printf("   ");
      }
      if (i % 8 == 7) printf(" ");
      if (i % 16 == 15) printf("%hs\n", glyphs);
    }
  }
  close(fd);
  free(p);
  return 0;
}
