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
#include "libc/alg/arraylist.h"
#include "libc/alg/arraylist2.h"
#include "libc/alg/bisectcarleft.h"
#include "libc/assert.h"
#include "libc/bits/bits.h"
#include "libc/bits/safemacros.h"
#include "libc/calls/calls.h"
#include "libc/errno.h"
#include "libc/fmt/fmt.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/macros.h"
#include "libc/nexgen32e/crc32.h"
#include "libc/runtime/ezmap.h"
#include "libc/runtime/gc.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/knuthmultiplicativehash.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/madv.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"
#include "libc/x/x.h"
#include "third_party/getopt/getopt.h"

/**
 * @fileoverview Make dependency generator.
 *
 * This program generates Makefile code saying which sources include
 * which headers, thus improving build invalidation.
 *
 * The same thing can be accomplished using GCC's -M flag. This tool is
 * much faster. It's designed to process over 9,000 sources to generate
 * 50k+ lines of make code in ~80ms using one core and a meg of ram.
 */

static const char kSourceExts[][5] = {".s", ".S", ".c", ".cc", ".cpp"};
static alignas(16) const char kIncludePrefix[] = "include \"";

static const char *const kModelessPackages[] = {
    "libc/nt/",
    "libc/stubs/",
    "libc/sysv/",
};

struct Strings {
  size_t i, n;
  char *p;
};

struct Source {
  uint32_t hash; /* 0 means empty w/ triangle probe */
  uint32_t name; /* strings.p[name] w/ interning */
  uint32_t id;   /* rehashing changes indexes */
};

struct Edge {
  int32_t from; /* sources.p[from.id] */
  int32_t to;   /* sources.p[to.id] */
};

struct Sources {
  size_t i, n;      /* phase 1: hashmap: popcnt(n)==1 if n */
  struct Source *p; /* phase 2: arraylist sorted by id */
};

struct Edges {
  size_t i, n;
  struct Edge *p;
};

int g_sourceid;
struct Strings strings;
struct Sources sources;
struct Edges edges;
const char *buildroot;
int *visited;
char *out;
FILE *fout;

static int CompareSourcesById(struct Source *a, struct Source *b) {
  return a->id > b->id ? 1 : a->id < b->id ? -1 : 0;
}

static int CompareEdgesByFrom(struct Edge *a, struct Edge *b) {
  return a->from > b->from ? 1 : a->from < b->from ? -1 : 0;
}

static uint32_t Hash(const void *s, size_t l) {
  return max(1, crc32c(0, s, l));
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

uint32_t GetSourceId(const char *name, size_t len) {
  size_t i, step;
  uint32_t hash;
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
  return (sources.p[i].id = g_sourceid++);
}

void LoadRelationships(int argc, char *argv[]) {
  struct MappedFile mf;
  const char *p, *pe;
  size_t i, linecap = 0;
  char *line = NULL;
  FILE *finpaths;
  for (i = optind; i < argc; ++i) {
    CHECK_NOTNULL((finpaths = fopen(argv[i], "r")));
    while (getline(&line, &linecap, finpaths) != -1) {
      if (mapfileread(chomp(line), &mf) == -1) {
        CHECK_EQ(ENOENT, errno, "%s", line);
        /*
         * This code helps GNU Make automatically fix itself when we
         * delete a source file. It removes o/.../srcs.txt or
         * o/.../hdrs.txt and exits nonzero. Since we use hyphen
         * notation on mkdeps related rules, the build will
         * automatically restart itself.
         */
        fprintf(stderr, "%s %s...\n", "Refreshing", argv[i]);
        unlink(argv[i]);
        exit(1);
      }
      if (mf.size) {
        if (mf.size > PAGESIZE) {
          madvise(mf.addr, mf.size, MADV_WILLNEED | MADV_SEQUENTIAL);
        }
        uint32_t sauce = GetSourceId(line, strlen(line));
        size_t inclen = strlen(kIncludePrefix);
        p = mf.addr;
        pe = p + mf.size;
        while ((p = strstr(p, kIncludePrefix))) {
          const char *path = p + inclen;
          const char *pathend = memchr(path, '"', pe - path);
          if (pathend && (intptr_t)p > (intptr_t)mf.addr &&
              (p[-1] == '#' || p[-1] == '.') &&
              (p - 1 == mf.addr || p[-2] == '\n')) {
            uint32_t dependency = GetSourceId(path, pathend - path);
            struct Edge edge;
            edge.from = sauce;
            edge.to = dependency;
            append(&edges, &edge);
          }
          p = path;
        }
      }
      CHECK_NE(-1, unmapfile(&mf));
    }
    CHECK_NE(-1, fclose(finpaths));
  }
  free_s(&line);
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
  for (size_t i = 0; i < ARRAYLEN(kSourceExts); ++i) {
    if (endswith(name, kSourceExts[i])) return true;
  }
  return false;
}

void Dive(uint32_t sauce) {
  for (uint32_t i = bisectcarleft((const int32_t(*)[2])edges.p, edges.i, sauce);
       edges.p[i].from == sauce; ++i) {
    int32_t dep = edges.p[i].to;
    if (bts(visited, dep)) continue;
    fputs(" \\\n\t", fout);
    fputs(&strings.p[sources.p[dep].name], fout);
    Dive(dep);
  }
}

bool IsModeless(const char *path) {
  size_t i;
  for (i = 0; i < ARRAYLEN(kModelessPackages); ++i) {
    if (startswith(path, kModelessPackages[i])) return true;
  }
  return false;
}

int main(int argc, char *argv[]) {
  out = "/dev/stdout";
  GetOpts(argc, argv);
  char *tmp =
      !fileexists(out) || isregularfile(out) ? xasprintf("%s.tmp", out) : NULL;
  CHECK_NOTNULL((fout = fopen(tmp ? tmp : out, "w")));
  LoadRelationships(argc, argv);
  Crunch();
  size_t bitmaplen = roundup((sources.i + 8) / 8, 4);
  visited = malloc(bitmaplen);
  for (size_t i = 0; i < sources.i; ++i) {
    const char *path = &strings.p[sources.p[i].name];
    if (!IsObjectSource(path)) continue;
    bool needprefix = !startswith(path, "o/");
    const char *prefix = !needprefix ? "" : IsModeless(path) ? "o/" : buildroot;
    fprintf(fout, "\n%s%s.o: \\\n\t%s", prefix, StripExt(path), path);
    memset(visited, 0, bitmaplen);
    bts(visited, i);
    Dive(i);
    fprintf(fout, "\n");
  }
  if (fclose(fout) == -1) perror(out), exit(1);
  if (tmp) {
    if (rename(tmp, out) == -1) perror(out), exit(1);
  }
  free_s(&strings.p);
  free_s(&sources.p);
  free_s(&edges.p);
  free_s(&visited);
  free_s(&tmp);
  return 0;
}
