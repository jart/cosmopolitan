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
#include "libc/alg/alg.h"
#include "libc/assert.h"
#include "libc/bits/safemacros.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/dirent.h"
#include "libc/calls/struct/stat.h"
#include "libc/fmt/fmt.h"
#include "libc/log/check.h"
#include "libc/runtime/gc.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/dt.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/x/x.h"

/**
 * @fileoverview Pretty fast substring refactor tool.
 */

static const char kBefore[] = "\
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │\n\
│                                                                              │\n\
│ This program is free software; you can redistribute it and/or modify         │\n\
│ it under the terms of the GNU General Public License as published by         │\n\
│ the Free Software Foundation; version 2 of the License.                      │\n\
│                                                                              │\n\
│ This program is distributed in the hope that it will be useful, but          │\n\
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │\n\
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │\n\
│ General Public License for more details.                                     │\n\
│                                                                              │\n\
│ You should have received a copy of the GNU General Public License            │\n\
│ along with this program; if not, write to the Free Software                  │\n\
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │\n\
│ 02110-1301 USA                                                               │\n\
";
const char kAfter[] = "\
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │\n\
│                                                                              │\n\
│ Permission to use, copy, modify, and/or distribute this software for         │\n\
│ any purpose with or without fee is hereby granted, provided that the         │\n\
│ above copyright notice and this permission notice appear in all copies.      │\n\
│                                                                              │\n\
│ THE SOFTWARE IS PROVIDED \"AS IS\" AND THE AUTHOR DISCLAIMS ALL                │\n\
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │\n\
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │\n\
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │\n\
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │\n\
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │\n\
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │\n\
│ PERFORMANCE OF THIS SOFTWARE.                                                │\n\
";

#if 0
static const char kBefore[] = "\
/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│";
const char kAfter[] = "\
/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│";
#endif

void RefactorFile(const char *path) {
  int fd;
  struct stat st;
  size_t len, partlen, len1, len2;
  char *mem, *spot = NULL, *part1, *part2;
  CHECK_NE(-1, (fd = open(path, O_RDONLY)));
  CHECK_NE(-1, fstat(fd, &st));
  len2 = 0;
  if ((len = st.st_size)) {
    CHECK_NE(MAP_FAILED,
             (mem = mmap(NULL, len, PROT_READ, MAP_PRIVATE, fd, 0)));
    partlen = sizeof(kBefore) - 1;
    if ((spot = memmem(mem, len, kBefore, partlen))) {
      part1 = gc(xmalloc((len1 = spot - mem)));
      part2 = gc(xmalloc((len2 = len - partlen - (spot - mem))));
      memcpy(part1, mem, len1);
      memcpy(part2, spot + partlen, len2);
    }
    CHECK_NE(-1, munmap(mem, len));
  }
  CHECK_NE(-1, close(fd));
  if (spot) {
    fprintf(stderr, "found! %s\n", path);
    CHECK_NE(-1, (fd = open(path, O_RDWR | O_TRUNC)));
    CHECK_EQ(len1, write(fd, part1, len1));
    CHECK_EQ(sizeof(kAfter) - 1, write(fd, kAfter, sizeof(kAfter) - 1));
    CHECK_EQ(len2, write(fd, part2, len2));
    CHECK_NE(-1, close(fd));
  }
}

void RefactorDir(const char *dpath) {
  DIR *dir;
  struct dirent *ent;
  char *path = gc(xmalloc(4096));
  CHECK_NOTNULL(dir = opendir(firstnonnull(dpath, ".")));
  while ((ent = readdir(dir))) {
    if (startswith(ent->d_name, ".")) continue;
    if (strcmp(ent->d_name, "o") == 0) continue;
    snprintf(path, 4096, "%s%s%s", dpath ? dpath : "", dpath ? "/" : "",
             ent->d_name);
    if (isdirectory(path)) {
      RefactorDir(path);
    } else if (isregularfile(path)) {
      RefactorFile(path);
    }
  }
  CHECK_NE(-1, closedir(dir));
}

int main(int argc, char *argv[]) {
  RefactorDir(NULL);
  return 0;
}
