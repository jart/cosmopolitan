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
#include "libc/bits/safemacros.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/errno.h"
#include "libc/fmt/fmt.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/macros.h"
#include "libc/nexgen32e/crc32.h"
#include "libc/runtime/ezmap.internal.h"
#include "libc/runtime/gc.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/madv.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/x/x.h"
#include "third_party/getopt/getopt.h"

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
  unsigned hash; /* 0 means empty w/ triangle probe */
  unsigned name; /* strings.p[name] w/ interning */
  unsigned id;   /* rehashing changes indexes */
};

struct Edge {
  unsigned from; /* sources.p[from.id] */
  unsigned to;   /* sources.p[to.id] */
};

struct Sources {
  size_t i, n;      /* phase 1: hashmap: popcnt(n)==1 if n */
  struct Source *p; /* phase 2: arraylist sorted by id */
};

struct Edges {
  size_t i, n;
  struct Edge *p;
};

char *out;
FILE *fout;
int *visited;
unsigned counter;
struct Edges edges;
struct Strings strings;
struct Sources sources;
const char *buildroot;

int CompareSourcesById(struct Source *a, struct Source *b) {
  return a->id > b->id ? 1 : a->id < b->id ? -1 : 0;
}

int CompareEdgesByFrom(struct Edge *a, struct Edge *b) {
  return a->from > b->from ? 1 : a->from < b->from ? -1 : 0;
}

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
  for (i = 0, j = 0; j < sources.n; ++j) {
    if (!sources.p[j].hash) continue;
    if (i != j) memcpy(&sources.p[i], &sources.p[j], sizeof(sources.p[j]));
    i++;
  }
  sources.i = i;
  qsort(sources.p, sources.i, sizeof(*sources.p), (void *)CompareSourcesById);
  qsort(edges.p, edges.i, sizeof(*edges.p), (void *)CompareEdgesByFrom);
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
    memcpy(&sources.p[j], &old.p[i], sizeof(old.p[i]));
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
          memcmp(name, &strings.p[sources.p[i].name], len) == 0) {
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
  fprintf(stderr, "%s %s...\n", "Refreshing", list);
  unlink(list);
  exit(1);
}

void LoadRelationships(int argc, char *argv[]) {
  int fd;
  ssize_t rc;
  bool skipme;
  FILE *finpaths;
  struct Edge edge;
  char *line, *buf;
  unsigned srcid, dependency;
  size_t i, linecap, inclen, size;
  const char *p, *pe, *src, *path, *pathend;
  line = NULL;
  linecap = 0;
  inclen = strlen(kIncludePrefix);
  buf = gc(xmemalign(PAGESIZE, PAGESIZE + MAX_READ + 16));
  buf += PAGESIZE;
  buf[-1] = '\n';
  for (i = optind; i < argc; ++i) {
    CHECK_NOTNULL((finpaths = fopen(argv[i], "r")));
    while (getline(&line, &linecap, finpaths) != -1) {
      src = chomp(line);
      if (ShouldSkipSource(src)) continue;
      srcid = GetSourceId(src, strlen(src));
      if ((fd = open(src, O_RDONLY)) == -1) OnMissingFile(argv[i], src);
      CHECK_NE(-1, (rc = read(fd, buf, MAX_READ)));
      close(fd);
      size = rc;
      memset(buf + size, 0, 16);
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
    CHECK_NE(-1, fclose(finpaths));
  }
  free(line);
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

void Dive(unsigned id) {
  int i;
  for (i = FindFirstFromEdge(id); i < edges.i && edges.p[i].from == id; ++i) {
    if (bts(visited, edges.p[i].to)) continue;
    fputs(" \\\n\t", fout);
    fputs(&strings.p[sources.p[edges.p[i].to].name], fout);
    Dive(edges.p[i].to);
  }
}

size_t GetFileSizeOrZero(const char *path) {
  struct stat st;
  st.st_size = 0;
  stat(path, &st);
  return st.st_size;
}

bool FilesHaveSameContent(const char *path1, const char *path2) {
  bool r;
  int c1, c2;
  size_t s1, s2;
  FILE *f1, *f2;
  s1 = GetFileSizeOrZero(path1);
  s2 = GetFileSizeOrZero(path2);
  if (s1 == s2) {
    r = true;
    if (s1) {
      CHECK_NOTNULL((f1 = fopen(path1, "r")));
      CHECK_NOTNULL((f2 = fopen(path2, "r")));
      for (;;) {
        c1 = getc(f1);
        c2 = getc(f2);
        if (c1 != c2) {
          r = false;
          break;
        }
        if (c1 == -1) {
          break;
        }
      }
      CHECK_NE(-1, fclose(f2));
      CHECK_NE(-1, fclose(f1));
    }
  } else {
    r = false;
  }
  return r;
}

int main(int argc, char *argv[]) {
  char *tp;
  bool needprefix;
  size_t i, bitmaplen;
  const char *path, *prefix;
  showcrashreports();
  out = "/dev/stdout";
  GetOpts(argc, argv);
  tp = !fileexists(out) || isregularfile(out) ? xasprintf("%s.tmp", out) : NULL;
  CHECK_NOTNULL((fout = fopen(tp ? tp : out, "w")));
  LoadRelationships(argc, argv);
  Crunch();
  bitmaplen = roundup((sources.i + 8) / 8, 4);
  visited = malloc(bitmaplen);
  for (i = 0; i < sources.i; ++i) {
    path = &strings.p[sources.p[i].name];
    if (!IsObjectSource(path)) continue;
    needprefix = !startswith(path, "o/");
    prefix = !needprefix ? "" : buildroot;
    fprintf(fout, "\n%s%s.o: \\\n\t%s", prefix, StripExt(path), path);
    memset(visited, 0, bitmaplen);
    bts(visited, i);
    Dive(i);
    fprintf(fout, "\n");
  }
  CHECK_NE(-1, fclose(fout));
  if (tp) {
    /* prevent gnu make from restarting unless necessary */
    if (!FilesHaveSameContent(tp, out)) {
      CHECK_NE(-1, rename(tp, out));
    } else {
      CHECK_NE(-1, unlink(tp));
    }
  }
  free_s(&strings.p);
  free_s(&sources.p);
  free_s(&edges.p);
  free_s(&visited);
  free_s(&tp);
  return 0;
}
