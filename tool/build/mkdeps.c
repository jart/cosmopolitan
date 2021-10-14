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
#include "libc/alg/arraylist.internal.h"
#include "libc/alg/arraylist2.internal.h"
#include "libc/alg/bisectcarleft.internal.h"
#include "libc/assert.h"
#include "libc/bits/bits.h"
#include "libc/bits/safemacros.internal.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/fmt.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/crc32.h"
#include "libc/runtime/ezmap.internal.h"
#include "libc/runtime/gc.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/append.internal.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/madv.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/x/x.h"
#include "third_party/getopt/getopt.h"
#include "tool/build/lib/getargs.h"

#define MAX_READ FRAMESIZE

/**
 * @fileoverview Make dependency generator.
 *
 * This generates Makefile code for source -> header dependencies.
 *
 * Includes look like this:
 *
 *   - #include "root/of/repository/foo.h"
 *   - .include "root/of/repository/foo.inc"
 *
 * They do not look like this:
 *
 *   -   #include "foo.h"
 *   - #  include "foo.h"
 *   - #include   "foo.h"
 *
 * Only the first 64kb of each source file is considered.
 */

_Alignas(16) const char kIncludePrefix[] = "include \"";

const char kSourceExts[][5] = {".s", ".S", ".c", ".cc", ".cpp"};

const char *const kIgnorePrefixes[] = {
#if 0
    "libc/sysv/consts/",   "libc/sysv/calls/",  "libc/nt/kernel32/",
    "libc/nt/KernelBase/", "libc/nt/advapi32/", "libc/nt/gdi32/",
    "libc/nt/ntdll/",      "libc/nt/user32/",   "libc/nt/shell32/",
#endif
};

struct Strings {
  size_t i, n;
  char *p;
};

struct Source {
  unsigned hash;
  unsigned name;
  unsigned id;
};

struct Edge {
  unsigned to;
  unsigned from;
};

struct Sources {
  size_t i, n;
  struct Source *p;
};

struct Sauce {
  unsigned name;
  unsigned id;
};

struct Edges {
  size_t i, n;
  struct Edge *p;
};

char *out;
char *bout;
unsigned counter;
uint32_t *visited;
struct Edges edges;
struct Sauce *sauces;
struct Strings strings;
struct Sources sources;
const char *buildroot;

unsigned Hash(const void *s, size_t l) {
  return max(1, crc32c(0, s, l));
}

unsigned FindFirstFromEdge(unsigned id) {
  unsigned m, l, r;
  l = 0;
  r = edges.i;
  while (l < r) {
    m = (l + r) >> 1;
    if (edges.p[m].from < id) {
      l = m + 1;
    } else {
      r = m;
    }
  }
  return l;
}

void Crunch(void) {
  size_t i, j;
  sauces = malloc(sizeof(*sauces) * sources.n);
  for (j = i = 0; i < sources.n; ++i) {
    if (sources.p[i].hash) {
      sauces[j].name = sources.p[i].name;
      sauces[j].id = sources.p[i].id;
      j++;
    }
  }
  free(sources.p);
  sources.p = 0;
  sources.i = j;
  longsort((const long *)sauces, sources.i);
  longsort((const long *)edges.p, edges.i);
}

void Rehash(void) {
  size_t i, j, step;
  struct Sources old;
  memcpy(&old, &sources, sizeof(sources));
  sources.n = sources.n ? sources.n << 1 : 16;
  sources.p = calloc(sources.n, sizeof(struct Source));
  for (i = 0; i < old.n; ++i) {
    if (!old.p[i].hash) continue;
    step = 0;
    do {
      j = (old.p[i].hash + step * (step + 1) / 2) & (sources.n - 1);
      step++;
    } while (sources.p[j].hash);
    sources.p[j] = old.p[i];
  }
  free(old.p);
}

unsigned GetSourceId(const char *name, size_t len) {
  size_t i, step;
  unsigned hash;
  i = 0;
  hash = Hash(name, len);
  if (sources.n) {
    step = 0;
    do {
      i = (hash + step * (step + 1) / 2) & (sources.n - 1);
      if (sources.p[i].hash == hash &&
          !timingsafe_bcmp(name, &strings.p[sources.p[i].name], len)) {
        return sources.p[i].id;
      }
      step++;
    } while (sources.p[i].hash);
  }
  if (++sources.i >= (sources.n >> 1)) {
    Rehash();
    step = 0;
    do {
      i = (hash + step * (step + 1) / 2) & (sources.n - 1);
      step++;
    } while (sources.p[i].hash);
  }
  sources.p[i].hash = hash;
  sources.p[i].name = CONCAT(&strings.p, &strings.i, &strings.n, name, len);
  strings.p[strings.i++] = '\0';
  return (sources.p[i].id = counter++);
}

bool ShouldSkipSource(const char *src) {
  unsigned j;
  for (j = 0; j < ARRAYLEN(kIgnorePrefixes); ++j) {
    if (startswith(src, kIgnorePrefixes[j])) {
      return true;
    }
  }
  return false;
}

wontreturn void OnMissingFile(const char *list, const char *src) {
  DCHECK_EQ(ENOENT, errno, "%s", src);
  /*
   * This code helps GNU Make automatically fix itself when we
   * delete a source file. It removes o/.../srcs.txt or
   * o/.../hdrs.txt and exits nonzero. Since we use hyphen
   * notation on mkdeps related rules, the build will
   * automatically restart itself.
   */
  if (list) {
    fprintf(stderr, "%s %s...\n", "Refreshing", list);
    unlink(list);
  }
  exit(1);
}

void LoadRelationships(int argc, char *argv[]) {
  int fd;
  char *buf;
  ssize_t rc;
  bool skipme;
  struct Edge edge;
  struct GetArgs ga;
  unsigned srcid, dependency;
  size_t i, inclen, size;
  const char *p, *pe, *src, *path, *pathend;
  inclen = strlen(kIncludePrefix);
  buf = gc(xmemalign(PAGESIZE, PAGESIZE + MAX_READ + 16));
  buf += PAGESIZE;
  buf[-1] = '\n';
  getargs_init(&ga, argv + optind);
  while ((src = getargs_next(&ga))) {
    if (ShouldSkipSource(src)) continue;
    srcid = GetSourceId(src, strlen(src));
    if ((fd = open(src, O_RDONLY)) == -1) OnMissingFile(ga.path, src);
    CHECK_NE(-1, (rc = read(fd, buf, MAX_READ)));
    close(fd);
    size = rc;
    bzero(buf + size, 16);
    for (p = buf, pe = p + size; p < pe; ++p) {
      p = strstr(p, kIncludePrefix);
      if (!p) break;
      path = p + inclen;
      pathend = memchr(path, '"', pe - path);
      if (pathend && (p[-1] == '#' || p[-1] == '.') && p[-2] == '\n') {
        dependency = GetSourceId(path, pathend - path);
        edge.from = srcid;
        edge.to = dependency;
        append(&edges, &edge);
        p = pathend;
      }
    }
  }
  getargs_destroy(&ga);
}

void GetOpts(int argc, char *argv[]) {
  int opt;
  while ((opt = getopt(argc, argv, "ho:r:")) != -1) {
    switch (opt) {
      case 'o':
        out = optarg;
        break;
      case 'r':
        buildroot = optarg;
        break;
      default:
        fprintf(stderr, "%s: %s [-r %s] [-o %s] [%s...]\n", "Usage", argv[0],
                "BUILDROOT", "OUTPUT", "PATHSFILE");
        exit(1);
    }
  }
  if (isempty(out)) fprintf(stderr, "need -o FILE"), exit(1);
  if (isempty(buildroot)) fprintf(stderr, "need -r o/$(MODE)"), exit(1);
}

const char *StripExt(const char *s) {
  static bool once;
  static size_t i, n;
  static char *p, *dot;
  if (!once) {
    once = true;
    __cxa_atexit(free_s, &p, NULL);
  }
  i = 0;
  CONCAT(&p, &i, &n, s, strlen(s) + 1);
  dot = strrchr(p, '.');
  if (dot) *dot = '\0';
  return p;
}

bool IsObjectSource(const char *name) {
  int i;
  for (i = 0; i < ARRAYLEN(kSourceExts); ++i) {
    if (endswith(name, kSourceExts[i])) return true;
  }
  return false;
}

forceinline bool Bts(uint32_t *p, size_t i) {
  bool r;
  uint32_t k;
  k = 1u << (i & 31);
  if (p[i >> 5] & k) return true;
  p[i >> 5] |= k;
  return false;
}

void Dive(unsigned id) {
  int i;
  for (i = FindFirstFromEdge(id); i < edges.i && edges.p[i].from == id; ++i) {
    if (Bts(visited, edges.p[i].to)) continue;
    appendw(&bout, READ32LE(" \\\n\t"));
    appends(&bout, strings.p + sauces[edges.p[i].to].name);
    Dive(edges.p[i].to);
  }
}

size_t GetFileSizeOrZero(const char *path) {
  struct stat st;
  st.st_size = 0;
  stat(path, &st);
  return st.st_size;
}

/* prevents gnu make from restarting unless necessary */
bool HasSameContent(void) {
  bool r;
  int fd;
  char *m;
  size_t s;
  s = GetFileSizeOrZero(out);
  if (s == appendz(bout).i) {
    if (s) {
      CHECK_NE(-1, (fd = open(out, O_RDONLY)));
      CHECK_NE(MAP_FAILED, (m = mmap(0, s, PROT_READ, MAP_SHARED, fd, 0)));
      r = !bcmp(bout, m, s);
      munmap(m, s);
      close(fd);
    } else {
      r = true;
    }
  } else {
    r = false;
  }
  return r;
}

int main(int argc, char *argv[]) {
  int fd;
  const char *path;
  size_t i, visilen;
  if (argc == 2 && !strcmp(argv[1], "-n")) exit(0);
  out = "/dev/stdout";
  GetOpts(argc, argv);
  LoadRelationships(argc, argv);
  Crunch();
  visilen = (sources.i + sizeof(*visited) * CHAR_BIT - 1) /
            (sizeof(*visited) * CHAR_BIT);
  visited = malloc(visilen * sizeof(*visited));
  for (i = 0; i < sources.i; ++i) {
    path = strings.p + sauces[i].name;
    if (!IsObjectSource(path)) continue;
    appendw(&bout, '\n');
    if (!startswith(path, "o/")) {
      appends(&bout, buildroot);
    }
    appends(&bout, StripExt(path));
    appendw(&bout, READ64LE(".o: \\\n\t"));
    appends(&bout, path);
    bzero(visited, visilen * sizeof(*visited));
    Bts(visited, i);
    Dive(i);
    appendw(&bout, '\n');
  }
  /* if (!fileexists(out) || !HasSameContent()) { */
  CHECK_NE(-1, (fd = open(out, O_CREAT | O_WRONLY, 0644)));
  CHECK_NE(-1, ftruncate(fd, appendz(bout).i));
  CHECK_NE(-1, xwrite(fd, bout, appendz(bout).i));
  CHECK_NE(-1, close(fd));
  /* } */
  free(strings.p);
  free(edges.p);
  free(visited);
  free(sauces);
  free(bout);
  return 0;
}
