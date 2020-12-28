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
#include "libc/alg/arraylist2.internal.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/fmt/itoa.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"

#define LOOKINGAT(p, pe, s) LookingAt(p, pe, s, strlen(s))
#define APPENDSTR(s)        AppendData(s, strlen(s))

struct Output {
  size_t i, n;
  const char *p;
};

struct Visited {
  size_t i, n;
  const char **p;
};

static struct stat st;
static struct Output output;
static struct Visited visited;

static void Visit(const char *);

static bool HasVisited(const char *path) {
  int i;
  for (i = 0; i < visited.i; ++i) {
    if (strcmp(path, visited.p[i]) == 0) {
      return true;
    }
  }
  return false;
}

static void AppendData(const char *s, size_t n) {
  CONCAT(&output.p, &output.i, &output.n, s, n);
}

static void AppendInt(long x) {
  char ibuf[21];
  AppendData(ibuf, int64toarray_radix10(x, ibuf));
}

static bool LookingAt(const char *p, const char *pe, const char *s, size_t n) {
  return pe - p >= n && memcmp(p, s, n) == 0;
}

static void Process(const char *p, const char *pe, const char *path,
                    bool isheader) {
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
    AppendData(p, p2 - p);
  }
  if (noformat) {
    APPENDSTR("/* clang-format on */\n");
  }
}

static void Visit(const char *path) {
  int fd;
  char *map;
  bool isheader;
  if (!endswith(path, ".h") && !endswith(path, ".inc")) return;
  if (endswith(path, ".internal.h")) return;
  if (endswith(path, "/internal.h")) return;
  if (endswith(path, ".internal.inc")) return;
  if (endswith(path, "/internal.inc")) return;
  isheader = endswith(path, ".h");
  if (isheader && HasVisited(path)) return;
  APPENDSTR("\n\f\n/*!BEGIN ");
  APPENDSTR(path);
  APPENDSTR(" */\n\n");
  APPEND(&visited.p, &visited.i, &visited.n, &path);
  if ((fd = open(path, O_RDONLY)) == -1) {
    fprintf(stderr, "error: %s: failed to open\n", path);
    exit(1);
  }
  CHECK_NE(-1, fstat(fd, &st));
  if (st.st_size) {
    CHECK_NE(MAP_FAILED,
             (map = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0)));
    Process(map, map + st.st_size, path, isheader);
    LOGIFNEG1(munmap(map, st.st_size));
  }
  LOGIFNEG1(close(fd));
}

int main(int argc, char *argv[]) {
  int i;
  APPENDSTR("#ifndef COSMOPOLITAN_H_\n");
  APPENDSTR("#define COSMOPOLITAN_H_\n");
  /* APPENDSTR("#define IMAGE_BASE_VIRTUAL "); */
  /* AppendInt(IMAGE_BASE_VIRTUAL); */
  /* APPENDSTR("\n"); */
  /* APPENDSTR("#define IMAGE_BASE_PHYSICAL "); */
  /* AppendInt(IMAGE_BASE_PHYSICAL); */
  /* APPENDSTR("\n"); */
  for (i = 1; i < argc; ++i) {
    Visit(argv[i]);
  }
  APPENDSTR("\n");
  APPENDSTR("#endif /* COSMOPOLITAN_H_ */\n");
  CHECK_EQ(output.i, write(1, output.p, output.i));
  return 0;
}
