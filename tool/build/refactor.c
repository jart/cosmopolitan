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

#if 0
static const char kBefore[] = "\
│ Copyright 2019 Justine Alexandra Roberts Tunney                              │\n\
│                                                                              │\n\
│ Copying of this file is authorized only if (1) you are Justine Tunney, or    │\n\
│ (2) you make absolutely no changes to your copy.                             │\n\
│                                                                              │\n\
│ THE SOFTWARE IS PROVIDED \"AS IS\" AND THE AUTHOR DISCLAIMS ALL WARRANTIES     │\n\
│ WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF             │\n\
│ MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR      │\n\
│ ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES       │\n\
│ WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN        │\n\
│ ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF      │\n\
│ OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.               │\n\
";
const char kAfter[] = "\
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
#endif

static const char kBefore[] = "\
/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│";
const char kAfter[] = "\
/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│";

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
  char *path = gc(xmalloc(PATH_MAX));
  CHECK_NOTNULL(dir = opendir(firstnonnull(dpath, ".")));
  while ((ent = readdir(dir))) {
    if (startswith(ent->d_name, ".")) continue;
    if (strcmp(ent->d_name, "o") == 0) continue;
    snprintf(path, PATH_MAX, "%s%s%s", dpath ? dpath : "", dpath ? "/" : "",
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
