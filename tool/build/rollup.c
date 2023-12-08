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
const char *prog;
struct Interner *visited;

void Visit(const char *);

static wontreturn void Die(const char *reason) {
  tinyprint(2, prog, ": ", reason, "\n", NULL);
  exit(1);
}

static wontreturn void DieSys(const char *thing) {
  perror(thing);
  exit(1);
}

static wontreturn void DieOom(void) {
  Die("out of memory");
}

static void Appends(char **b, const char *s) {
  if (appends(b, s) == -1) DieOom();
}

static void Appendd(char **b, const void *p, size_t n) {
  if (appendd(b, p, n) == -1) DieOom();
}

size_t GetFdSize(int fd) {
  struct stat st;
  if (fstat(fd, &st)) {
    DieSys("fstat");
  }
  return st.st_size;
}

bool LookingAt(const char *p, const char *pe, const char *s, size_t n) {
  return pe - p >= n && memcmp(p, s, n) == 0;
}

void Process(const char *p, const char *pe, const char *path, bool isheader) {
  int level;
  bool noformat;
  const char *p2, *dq, *name;
  for (noformat = false, level = 0; p < pe; p = p2) {
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
    Appendd(&output, p, p2 - p);
  }
  if (noformat) {
    Appends(&output, "/* clang-format on */\n");
  }
}

void Visit(const char *path) {
  int fd;
  char *map;
  size_t size;
  bool isheader;
  if (!endswith(path, ".h") && !endswith(path, ".inc")) return;
  if (endswith(path, ".internal.h")) return;
  if (endswith(path, "/internal.h")) return;
  if (endswith(path, ".internal.inc")) return;
  if (endswith(path, "/internal.inc")) return;
  if (startswith(path, "libc/isystem/")) return;
  isheader = endswith(path, ".h");
  if (isheader && isinterned(visited, path)) return;
  Appends(&output, "\n\f\n/*!BEGIN ");
  Appends(&output, path);
  Appends(&output, " */\n\n");
  intern(visited, path);
  if ((fd = open(path, O_RDONLY)) == -1) DieSys(path);
  if ((size = GetFdSize(fd))) {
    map = mmap(0, size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (map == MAP_FAILED) DieSys(path);
    Process(map, map + size, path, isheader);
    if (munmap(map, size)) DieSys(path);
  }
  if (close(fd)) DieSys(path);
}

int main(int argc, char *argv[]) {
  size_t bytes;
  const char *src;
  struct GetArgs ga;
  prog = argv[0];
  if (!prog) prog = "rollup";
  visited = newinterner();
  Appends(&output, "#ifndef COSMOPOLITAN_H_\n");
  Appends(&output, "#define COSMOPOLITAN_H_\n");
  getargs_init(&ga, argv + 1);
  while ((src = getargs_next(&ga))) {
    Visit(src);
  }
  getargs_destroy(&ga);
  Appends(&output, "\n");
  Appends(&output, "#endif /* COSMOPOLITAN_H_ */\n");
  bytes = appendz(output).i;
  if (write(1, output, bytes) != bytes) {
    DieSys(prog);
  }
  freeinterner(visited);
  return 0;
}
