/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
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
