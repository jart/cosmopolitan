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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/fmt.h"
#include "libc/intrin/bits.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/safemacros.internal.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/mem/alg.h"
#include "libc/mem/alloca.h"
#include "libc/mem/arraylist.internal.h"
#include "libc/mem/arraylist2.internal.h"
#include "libc/mem/bisectcarleft.internal.h"
#include "libc/mem/gc.internal.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/crc32.h"
#include "libc/runtime/ezmap.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/stack.h"
#include "libc/stdio/append.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/clone.h"
#include "libc/sysv/consts/madv.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/thread/spawn.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"
#include "libc/thread/wait0.internal.h"
#include "libc/time/time.h"
#include "libc/x/x.h"
#include "third_party/getopt/getopt.h"
#include "tool/build/lib/getargs.h"

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
 */

#define kIncludePrefix "include \""

#define THREADS 1       // _getcpucount()
#define LOCK    (void)  // if (__threaded) pthread_spin_lock
#define UNLOCK  (void)  // pthread_spin_unlock

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
char **bouts;
pthread_t *th;
unsigned counter;
struct GetArgs ga;
struct Edges edges;
struct Sauce *sauces;
struct Strings strings;
struct Sources sources;
const char *buildroot;
pthread_spinlock_t galock;
pthread_spinlock_t readlock;
pthread_spinlock_t writelock;
pthread_spinlock_t reportlock;

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
  _longsort((const long *)sauces, sources.i);
  _longsort((const long *)edges.p, edges.i);
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
          !memcmp(name, &strings.p[sources.p[i].name], len)) {
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
    if (_startswith(src, kIgnorePrefixes[j])) {
      return true;
    }
  }
  return false;
}

wontreturn void OnMissingFile(const char *list, const char *src) {
  kprintf("%s is missing\n", src);
  DCHECK_EQ(ENOENT, errno, "%s", src);
  /*
   * This code helps GNU Make automatically fix itself when we
   * delete a source file. It removes o/.../srcs.txt or
   * o/.../hdrs.txt and exits nonzero. Since we use hyphen
   * notation on mkdeps related rules, the build will
   * automatically restart itself.
   */
  if (list) {
    kprintf("%s %s...\n", "Refreshing", list);
    unlink(list);
  }
  exit(1);
}

void *LoadRelationshipsWorker(void *arg) {
  int fd;
  ssize_t rc;
  bool skipme;
  struct stat st;
  struct Edge edge;
  size_t i, n, size, inclen;
  unsigned srcid, dependency;
  char *buf, srcbuf[PATH_MAX];
  const char *p, *pe, *src, *path, *pathend;
  inclen = strlen(kIncludePrefix);
  for (;;) {
    LOCK(&galock);
    if ((src = getargs_next(&ga))) strcpy(srcbuf, src);
    UNLOCK(&galock);
    if (!src) break;
    src = srcbuf;
    if (ShouldSkipSource(src)) continue;
    n = strlen(src);
    LOCK(&readlock);
    srcid = GetSourceId(src, n);
    UNLOCK(&readlock);
    if ((fd = open(src, O_RDONLY)) == -1) {
      LOCK(&reportlock);
      OnMissingFile(ga.path, src);
    }
    CHECK_NE(-1, fstat(fd, &st));
    if ((size = st.st_size)) {
      CHECK_NE(MAP_FAILED, (buf = mmap(0, size, PROT_READ, MAP_SHARED, fd, 0)));
      for (p = buf + 1, pe = buf + size; p < pe; ++p) {
        if (!(p = memmem(p, pe - p, kIncludePrefix, inclen))) break;
        path = p + inclen;
        pathend = memchr(path, '"', pe - path);
        if (pathend &&                          //
            (p[-1] == '#' || p[-1] == '.') &&   //
            (p - buf == 1 || p[-2] == '\n')) {  //
          LOCK(&readlock);
          dependency = GetSourceId(path, pathend - path);
          UNLOCK(&readlock);
          edge.from = srcid;
          edge.to = dependency;
          LOCK(&writelock);
          append(&edges, &edge);
          UNLOCK(&writelock);
          p = pathend;
        }
      }
      munmap(buf, size);
    }
    close(fd);
  }
  return 0;
}

void LoadRelationships(int argc, char *argv[]) {
  int i;
  getargs_init(&ga, argv + optind);
  if (THREADS == 1) {
    LoadRelationshipsWorker((void *)(intptr_t)0);
  } else {
    for (i = 0; i < THREADS; ++i) {
      if (pthread_create(th + i, 0, LoadRelationshipsWorker,
                         (void *)(intptr_t)i)) {
        LOCK(&reportlock);
        kprintf("error: _spawn(%d) failed %m\n", i);
        exit(1);
      }
    }
    for (i = 0; i < THREADS; ++i) {
      pthread_join(th[i], 0);
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
        kprintf("%s: %s [-r %s] [-o %s] [%s...]\n", "Usage", argv[0],
                "BUILDROOT", "OUTPUT", "PATHSFILE");
        exit(1);
    }
  }
  if (isempty(out)) kprintf("need -o FILE"), exit(1);
  if (isempty(buildroot)) kprintf("need -r o/$(MODE)"), exit(1);
}

const char *StripExt(char pathbuf[PATH_MAX], const char *s) {
  static char *dot;
  strcpy(pathbuf, s);
  dot = strrchr(pathbuf, '.');
  if (dot) *dot = '\0';
  return pathbuf;
}

bool IsObjectSource(const char *name) {
  int i;
  for (i = 0; i < ARRAYLEN(kSourceExts); ++i) {
    if (_endswith(name, kSourceExts[i])) return true;
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

size_t GetFileSizeOrZero(const char *path) {
  struct stat st;
  st.st_size = 0;
  stat(path, &st);
  return st.st_size;
}

void Dive(char **bout, uint32_t *visited, unsigned id) {
  int i;
  for (i = FindFirstFromEdge(id); i < edges.i && edges.p[i].from == id; ++i) {
    if (Bts(visited, edges.p[i].to)) continue;
    appendw(bout, READ32LE(" \\\n\t"));
    appends(bout, strings.p + sauces[edges.p[i].to].name);
    Dive(bout, visited, edges.p[i].to);
  }
}

void *Diver(void *arg) {
  char *bout = 0;
  const char *path;
  uint32_t *visited;
  size_t i, visilen;
  char pathbuf[PATH_MAX];
  int x = (intptr_t)arg;
  visilen = (sources.i + sizeof(*visited) * CHAR_BIT - 1) /
            (sizeof(*visited) * CHAR_BIT);
  visited = malloc(visilen * sizeof(*visited));
  for (i = x; i < sources.i; i += THREADS) {
    path = strings.p + sauces[i].name;
    if (!IsObjectSource(path)) continue;
    appendw(&bout, '\n');
    if (!_startswith(path, "o/")) {
      appends(&bout, buildroot);
    }
    appends(&bout, StripExt(pathbuf, path));
    appendw(&bout, READ64LE(".o: \\\n\t"));
    appends(&bout, path);
    bzero(visited, visilen * sizeof(*visited));
    Bts(visited, i);
    Dive(&bout, visited, i);
    appendw(&bout, '\n');
  }
  free(visited);
  appendw(&bout, '\n');
  bouts[x] = bout;
  return 0;
}

void Explore(void) {
  int i;
  if (THREADS == 1) {
    Diver((void *)(intptr_t)0);
  } else {
    for (i = 0; i < THREADS; ++i) {
      if (pthread_create(th + i, 0, Diver, (void *)(intptr_t)i)) {
        LOCK(&reportlock);
        kprintf("error: _spawn(%d) failed %m\n", i);
        exit(1);
      }
    }
    for (i = 0; i < THREADS; ++i) {
      pthread_join(th[i], 0);
    }
  }
}

int main(int argc, char *argv[]) {
  int i, fd;
  ShowCrashReports();
  if (argc == 2 && !strcmp(argv[1], "-n")) exit(0);
  GetOpts(argc, argv);
  th = calloc(THREADS, sizeof(*th));
  bouts = calloc(THREADS, sizeof(*bouts));
  LoadRelationships(argc, argv);
  Crunch();
  Explore();
  CHECK_NE(-1, (fd = open(out, O_WRONLY | O_CREAT | O_TRUNC, 0644)),
           "open(%#s)", out);
  for (i = 0; i < THREADS; ++i) {
    CHECK_NE(-1, xwrite(fd, bouts[i], appendz(bouts[i]).i));
  }
  CHECK_NE(-1, close(fd));
  for (i = 0; i < THREADS; ++i) {
    free(bouts[i]);
  }
  free(strings.p);
  free(edges.p);
  free(sauces);
  free(bouts);
  free(th);
  return 0;
}
