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
#include "ape/relocations.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/errno.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/kprintf.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/mem/arraylist2.internal.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/append.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "tool/build/lib/getargs.h"
#include "tool/build/lib/interner.h"

#define LOOKINGAT(p, pe, s) LookingAt(p, pe, s, strlen(s))

struct Visited {
  size_t i, n;
  const char **p;
};

char *output;
struct Interner *visited;

void Visit(const char *);

size_t GetFdSize(int fd) {
  struct stat st;
  CHECK_NE(-1, fstat(fd, &st));
  return st.st_size;
}

bool LookingAt(const char *p, const char *pe, const char *s, size_t n) {
  return pe - p >= n && memcmp(p, s, n) == 0;
}

void Process(const char *p, const char *pe, const char *path, bool isheader) {
  int level;
  bool noformat;
  const char *p2, *dq, *name;
  for (noformat = level = 0; p < pe; p = p2) {
    p2 = memchr(p, '\n', pe - p);
    p2 = p2 ? p2 + 1 : pe;
    if (LOOKINGAT(p, pe, "#if")) {
      if (isheader && !level++) continue;
    }
    if (LOOKINGAT(p, pe, "#endif")) {
      if (isheader && !--level) continue;
    }
    if (LOOKINGAT(p, pe, "/* clang-format off */")) {
      noformat = true;
    } else if (LOOKINGAT(p, pe, "/* clang-format on */")) {
      noformat = false;
    }
    if (LOOKINGAT(p, pe, "#include \"")) {
      name = p + strlen("#include \"");
      dq = memchr(name, '"', pe - name);
      if (dq) {
        Visit(strndup(name, dq - name));
        continue;
      }
    }
    appendd(&output, p, p2 - p);
  }
  if (noformat) {
    appends(&output, "/* clang-format on */\n");
  }
  kprintf("finished%n");
}

void Visit(const char *path) {
  int fd;
  char *map;
  size_t size;
  bool isheader;
  if (!_endswith(path, ".h") && !_endswith(path, ".inc")) return;
  if (_endswith(path, ".internal.h")) return;
  if (_endswith(path, "/internal.h")) return;
  if (_endswith(path, ".internal.inc")) return;
  if (_endswith(path, "/internal.inc")) return;
  if (_startswith(path, "libc/isystem/")) return;
  isheader = _endswith(path, ".h");
  if (isheader && isinterned(visited, path)) return;
  appends(&output, "\n\f\n/*!BEGIN ");
  appends(&output, path);
  appends(&output, " */\n\n");
  intern(visited, path);
  if ((fd = open(path, O_RDONLY)) == -1) {
    fprintf(stderr, "error: %s: failed to open\n", path);
    exit(1);
  }
  if ((size = GetFdSize(fd))) {
    kprintf("size 1 = %'zu%n", size);
    CHECK_NE(MAP_FAILED,
             (map = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0)));
    Process(map, map + size, path, isheader);
    kprintf("size = %'zu%n", size);
    CHECK_EQ(0, munmap(map, size), "p=%p z=%'zu path=%s", map, size, path);
  }
  CHECK_EQ(0, close(fd));
}

ssize_t WriteAll(int fd, const char *p, size_t n) {
  ssize_t rc;
  size_t i, got;
  for (i = 0; i < n;) {
    rc = write(fd, p + i, n - i);
    if (rc != -1) {
      got = rc;
      i += got;
    } else if (errno != EINTR) {
      return -1;
    }
  }
  return i;
}

int main(int argc, char *argv[]) {
  const char *src;
  struct GetArgs ga;
  ShowCrashReports();
  visited = newinterner();
  appends(&output, "#ifndef COSMOPOLITAN_H_\n");
  appends(&output, "#define COSMOPOLITAN_H_\n");
  /* appends(&output, "#define IMAGE_BASE_VIRTUAL "); */
  /* appendf(&output, "%p", IMAGE_BASE_VIRTUAL); */
  /* appends(&output, "\n"); */
  /* appends(&output, "#define IMAGE_BASE_PHYSICAL "); */
  /* appendf(&output, "%p", IMAGE_BASE_PHYSICAL); */
  /* appends(&output, "\n"); */
  getargs_init(&ga, argv + 1);
  while ((src = getargs_next(&ga))) {
    Visit(src);
  }
  getargs_destroy(&ga);
  appends(&output, "\n");
  appends(&output, "#endif /* COSMOPOLITAN_H_ */\n");
  CHECK_NE(-1, WriteAll(1, output, appendz(output).i));
  freeinterner(visited);
  return 0;
}
