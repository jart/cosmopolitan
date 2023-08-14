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
#include "libc/errno.h"
#include "libc/fmt/magnumstrs.internal.h"
#include "libc/intrin/bits.h"
#include "libc/macros.internal.h"
#include "libc/mem/alg.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/crc32.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/append.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "third_party/getopt/getopt.internal.h"
#include "tool/build/lib/getargs.h"

#define VERSION                     \
  "cosmopolitan mkdeps v2.0\n"      \
  "copyright 2023 justine tunney\n" \
  "https://github.com/jart/cosmopolitan\n"

#define MANUAL                                           \
  " -r o// -o OUTPUT INPUT...\n"                         \
  "\n"                                                   \
  "DESCRIPTION\n"                                        \
  "\n"                                                   \
  "  Generates makefile defining header dependencies.\n" \
  "\n"                                                   \
  "  Includes look like this:\n"                         \
  "\n"                                                   \
  "    - #include \"root/of/repository/foo.h\"\n"        \
  "    - .include \"root/of/repository/foo.inc\"\n"      \
  "\n"                                                   \
  "  They do NOT look like this:\n"                      \
  "\n"                                                   \
  "    - #include <stdio.h>\n"                           \
  "    -   #include \"foo.h\"\n"                         \
  "    - #  include \"foo.h\"\n"                         \
  "    - #include   \"foo.h\"\n"                         \
  "\n"                                                   \
  "  Your generated make code looks like this:\n"        \
  "\n"                                                   \
  "    o//package/foo.o: \\\n"                           \
  "      package/foo.c \\\n"                             \
  "      package/foo.h \\\n"                             \
  "      package/bar.h\n"                                \
  "    o//package/bar.o: \\\n"                           \
  "      package/bar.c \\\n"                             \
  "      package/bar.h\n"                                \
  "\n"                                                   \
  "FLAGS\n"                                              \
  "\n"                                                   \
  "  -h         show usage\n"                            \
  "  -o OUTPUT  set output path\n"                       \
  "  -r ROOT    set build output prefix, e.g. o//\n"     \
  "\n"                                                   \
  "ARGUMENTS\n"                                          \
  "\n"                                                   \
  "  OUTPUT     shall be makefile code\n"                \
  "  INPUT      should be source or @args.txt\n"         \
  "\n"

#define kIncludePrefix "include \""

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

static const char kSourceExts[][5] = {".s", ".S", ".c", ".cc", ".cpp"};

static char *names;
static unsigned counter;
static const char *prog;
static struct Edges edges;
static struct Sauce *sauces;
static struct Sources sources;
static const char *buildroot;
static const char *outpath;

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

static unsigned Hash(const void *s, size_t l) {
  unsigned h;
  h = crc32c(0, s, l);
  if (!h) h = 1;
  return h;
}

static void *Malloc(size_t n) {
  void *p;
  if (!(p = malloc(n))) DieOom();
  return p;
}

static void *Calloc(size_t n, size_t z) {
  void *p;
  if (!(p = calloc(n, z))) DieOom();
  return p;
}

static void *Realloc(void *p, size_t n) {
  if (!(p = realloc(p, n))) DieOom();
  return p;
}

static void Appendw(char **b, uint64_t w) {
  if (appendw(b, w) == -1) DieOom();
}

static void Appends(char **b, const char *s) {
  if (appends(b, s) == -1) DieOom();
}

static void Appendd(char **b, const void *p, size_t n) {
  if (appendd(b, p, n) == -1) DieOom();
}

static unsigned FindFirstFromEdge(unsigned id) {
  unsigned m, l, r;
  l = 0;
  r = edges.i;
  while (l < r) {
    m = (l & r) + ((l ^ r) >> 1);  // floor((a+b)/2)
    if (edges.p[m].from < id) {
      l = m + 1;
    } else {
      r = m;
    }
  }
  return l;
}

static void AppendEdge(struct Edges *edges, unsigned to, unsigned from) {
  if (edges->i + 1 > edges->n) {
    edges->n += 1;
    edges->n += edges->n >> 1;
    edges->p = Realloc(edges->p, edges->n * sizeof(*edges->p));
  }
  edges->p[edges->i++] = (struct Edge){to, from};
}

static void Crunch(void) {
  size_t i, j;
  sauces = Malloc(sizeof(*sauces) * sources.n);
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
  if (!radix_sort_int64((const long *)sauces, sources.i) ||
      !radix_sort_int64((const long *)edges.p, edges.i)) {
    DieOom();
  }
}

static void Rehash(void) {
  size_t i, j, step;
  struct Sources old;
  memcpy(&old, &sources, sizeof(sources));
  sources.n = sources.n ? sources.n << 1 : 16;
  sources.p = Calloc(sources.n, sizeof(struct Source));
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

static unsigned GetSourceId(const char *name, size_t len) {
  unsigned hash;
  size_t i, step;
  i = 0;
  hash = Hash(name, len);
  if (sources.n) {
    step = 0;
    do {
      i = (hash + step * (step + 1) / 2) & (sources.n - 1);
      if (sources.p[i].hash == hash &&
          !memcmp(name, names + sources.p[i].name, len)) {
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
  sources.p[i].name = appendz(names).i;
  Appendd(&names, name, len);
  Appendw(&names, 0);
  return (sources.p[i].id = counter++);
}

static void LoadRelationships(int argc, char *argv[]) {
  int fd;
  char *map;
  ssize_t rc;
  struct GetArgs ga;
  size_t i, n, size, inclen;
  unsigned srcid, dependency;
  const char *p, *pe, *src, *path, *pathend;
  getargs_init(&ga, argv + optind);
  inclen = strlen(kIncludePrefix);
  while ((src = getargs_next(&ga))) {
    n = strlen(src);
    srcid = GetSourceId(src, n);
    if ((fd = open(src, O_RDONLY)) == -1) {
      if (errno == ENOENT && ga.path) {
        // This code helps GNU Make automatically fix itself when we
        // delete a source file. It removes o/.../srcs.txt or
        // o/.../hdrs.txt and exits nonzero. Since we use hyphen
        // notation on mkdeps related rules, the build will
        // automatically restart itself.
        tinyprint(2, prog, ": deleting ", ga.path, " to refresh build...\n",
                  NULL);
      }
      DieSys(src);
    }
    if ((rc = lseek(fd, 0, SEEK_END)) == -1) {
      DieSys(src);
    }
    if ((size = rc)) {
      map = mmap(0, size, PROT_READ, MAP_SHARED, fd, 0);
      if (map == MAP_FAILED) {
        DieSys(src);
      }
      for (p = map + 1, pe = map + size; p < pe; ++p) {
        if (!(p = memmem(p, pe - p, kIncludePrefix, inclen))) break;
        path = p + inclen;
        pathend = memchr(path, '"', pe - path);
        if (pathend &&                          //
            (p[-1] == '#' || p[-1] == '.') &&   //
            (p - map == 1 || p[-2] == '\n')) {  //
          dependency = GetSourceId(path, pathend - path);
          AppendEdge(&edges, dependency, srcid);
          p = pathend;
        }
      }
      if (munmap(map, size)) {
        DieSys(src);
      }
    }
    if (close(fd)) {
      DieSys(src);
    }
  }
  getargs_destroy(&ga);
}

static wontreturn void ShowUsage(int rc, int fd) {
  tinyprint(fd, VERSION, "\nUSAGE\n\n  ", prog, MANUAL, NULL);
  exit(rc);
}

static void GetOpts(int argc, char *argv[]) {
  int opt;
  while ((opt = getopt(argc, argv, "hno:r:")) != -1) {
    switch (opt) {
      case 'o':
        outpath = optarg;
        break;
      case 'r':
        buildroot = optarg;
        break;
      case 'n':
        exit(0);
      case 'h':
        ShowUsage(0, 1);
      default:
        ShowUsage(1, 2);
    }
  }
  if (!outpath) {
    Die("need output path");
  }
  if (!buildroot) {
    Die("need build output prefix");
  }
  if (optind == argc) {
    Die("missing input argument");
  }
}

static const char *StripExt(char pathbuf[PATH_MAX + 1], const char *s) {
  static char *dot;
  strlcpy(pathbuf, s, PATH_MAX + 1);
  dot = strrchr(pathbuf, '.');
  if (dot) *dot = '\0';
  return pathbuf;
}

static bool IsObjectSource(const char *name) {
  int i;
  for (i = 0; i < ARRAYLEN(kSourceExts); ++i) {
    if (endswith(name, kSourceExts[i])) return true;
  }
  return false;
}

__funline bool Bts(uint32_t *p, size_t i) {
  bool r;
  uint32_t k;
  k = 1u << (i & 31);
  if (p[i >> 5] & k) return true;
  p[i >> 5] |= k;
  return false;
}

static void Dive(char **makefile, uint32_t *visited, unsigned id) {
  int i;
  for (i = FindFirstFromEdge(id); i < edges.i && edges.p[i].from == id; ++i) {
    if (Bts(visited, edges.p[i].to)) continue;
    Appendw(makefile, READ32LE(" \\\n\t"));
    Appends(makefile, names + sauces[edges.p[i].to].name);
    Dive(makefile, visited, edges.p[i].to);
  }
}

static char *Explore(void) {
  const char *path;
  unsigned *visited;
  size_t i, visilen;
  char *makefile = 0;
  char buf[PATH_MAX + 1];
  visilen = (sources.i + sizeof(*visited) * CHAR_BIT - 1) /
            (sizeof(*visited) * CHAR_BIT);
  visited = Malloc(visilen * sizeof(*visited));
  for (i = 0; i < sources.i; ++i) {
    path = names + sauces[i].name;
    if (!IsObjectSource(path)) continue;
    Appendw(&makefile, '\n');
    if (!startswith(path, "o/")) {
      Appends(&makefile, buildroot);
    }
    Appends(&makefile, StripExt(buf, path));
    Appendw(&makefile, READ64LE(".o: \\\n\t"));
    Appends(&makefile, path);
    bzero(visited, visilen * sizeof(*visited));
    Bts(visited, i);
    Dive(&makefile, visited, i);
    Appendw(&makefile, '\n');
  }
  Appendw(&makefile, '\n');
  free(visited);
  return makefile;
}

int main(int argc, char *argv[]) {
  int fd;
  ssize_t rc;
  size_t i, n;
  char *makefile;
#ifndef NDEBUG
  ShowCrashReports();
#endif
  prog = argv[0];
  if (!prog) prog = "mkdeps";
  GetOpts(argc, argv);
  LoadRelationships(argc, argv);
  Crunch();
  makefile = Explore();
  if ((fd = open(outpath, O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1) {
    DieSys(outpath);
  }
  n = appendz(makefile).i;
  for (i = 0; i < n; i += (size_t)rc) {
    if ((rc = write(fd, makefile + i, n - i)) == -1) {
      DieSys(outpath);
    }
  }
  if (close(fd)) {
    DieSys(outpath);
  }
  free(makefile);
  free(edges.p);
  free(sauces);
  free(names);
  return 0;
}
