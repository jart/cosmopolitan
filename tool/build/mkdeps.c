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
#include "libc/intrin/kprintf.h"
#include "libc/intrin/spinlock.h"
#include "libc/intrin/wait0.internal.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/mem/alloca.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/crc32.h"
#include "libc/nexgen32e/threaded.h"
#include "libc/runtime/ezmap.internal.h"
#include "libc/runtime/gc.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/stack.h"
#include "libc/runtime/sysconf.h"
#include "libc/stdio/append.internal.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/clone.h"
#include "libc/sysv/consts/madv.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/time/time.h"
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
char **tls;
int threads;
char **bouts;
char **stack;
unsigned counter;
struct GetArgs ga;
struct Edges edges;
struct Sauce *sauces;
struct Strings strings;
struct Sources sources;
const char *buildroot;
_Alignas(64) char galock;
_Alignas(64) char readlock;
_Alignas(64) char writelock;
_Alignas(64) char reportlock;

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
    kprintf("%s %s...\n", "Refreshing", list);
    unlink(list);
  }
  exit(1);
}

int LoadRelationshipsWorker(void *arg) {
  int fd;
  ssize_t rc;
  bool skipme;
  struct Edge edge;
  char *buf, *freeme;
  char srcbuf[PATH_MAX];
  size_t i, n, inclen, size;
  unsigned srcid, dependency;
  const char *p, *pe, *src, *path, *pathend;
  inclen = strlen(kIncludePrefix);
  freeme = buf = memalign(PAGESIZE, PAGESIZE + MAX_READ + 16);
  buf += PAGESIZE;
  buf[-1] = '\n';
  for (;;) {
    _spinlock(&galock);
    if ((src = getargs_next(&ga))) strcpy(srcbuf, src);
    _spunlock(&galock);
    if (!src) break;
    src = srcbuf;
    if (ShouldSkipSource(src)) continue;
    n = strlen(src);
    _spinlock(&readlock);
    srcid = GetSourceId(src, n);
    _spunlock(&readlock);
    if ((fd = open(src, O_RDONLY)) == -1) {
      _spinlock(&reportlock);
      OnMissingFile(ga.path, src);
    }
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
        _spinlock(&readlock);
        dependency = GetSourceId(path, pathend - path);
        _spunlock(&readlock);
        edge.from = srcid;
        edge.to = dependency;
        _spinlock(&writelock);
        append(&edges, &edge);
        _spunlock(&writelock);
        p = pathend;
      }
    }
  }
  free(freeme);
  return 0;
}

void LoadRelationships(int argc, char *argv[]) {
  int i;
  getargs_init(&ga, argv + optind);
  for (i = 0; i < threads; ++i) {
    if (clone(LoadRelationshipsWorker, stack[i], GetStackSize(),
              CLONE_THREAD | CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND |
                  CLONE_SETTLS | CLONE_CHILD_SETTID | CLONE_CHILD_CLEARTID,
              (void *)(intptr_t)i, 0, __initialize_tls(tls[i]), 64,
              (int *)(tls[i] + 0x38)) == -1) {
      _spinlock(&reportlock);
      kprintf("error: clone(%d) failed %m\n", i);
      exit(1);
    }
  }
  for (i = 0; i < threads; ++i) {
    _wait0((int *)(tls[i] + 0x38));
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
    if (endswith(name, kSourceExts[i])) return true;
  }
  if (strstr(name, "/libcxx/")) {
    return true;
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

int Diver(void *arg) {
  char *bout = 0;
  const char *path;
  uint32_t *visited;
  size_t i, visilen;
  char pathbuf[PATH_MAX];
  int tid = (intptr_t)arg;
  visilen = (sources.i + sizeof(*visited) * CHAR_BIT - 1) /
            (sizeof(*visited) * CHAR_BIT);
  visited = malloc(visilen * sizeof(*visited));
  for (i = tid; i < sources.i; i += threads) {
    path = strings.p + sauces[i].name;
    if (!IsObjectSource(path)) continue;
    appendw(&bout, '\n');
    if (!startswith(path, "o/")) {
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
  bouts[tid] = bout;
  return 0;
}

void Explore(void) {
  int i;
  for (i = 0; i < threads; ++i) {
    if (clone(Diver, stack[i], GetStackSize(),
              CLONE_THREAD | CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND |
                  CLONE_SETTLS | CLONE_CHILD_SETTID | CLONE_CHILD_CLEARTID,
              (void *)(intptr_t)i, 0, __initialize_tls(tls[i]), 64,
              (int *)(tls[i] + 0x38)) == -1) {
      _spinlock(&reportlock);
      kprintf("error: clone(%d) failed %m\n", i);
      exit(1);
    }
  }
  for (i = 0; i < threads; ++i) {
    _wait0((int *)(tls[i] + 0x38));
  }
}

int main(int argc, char *argv[]) {
  int i, fd;
  char path[PATH_MAX];
  if (argc == 2 && !strcmp(argv[1], "-n")) exit(0);
  GetOpts(argc, argv);
  threads = GetCpuCount();
  tls = calloc(threads, sizeof(*tls));
  stack = calloc(threads, sizeof(*stack));
  bouts = calloc(threads, sizeof(*bouts));
  for (i = 0; i < threads; ++i) {
    if (!(tls[i] = malloc(64)) ||
        (stack[i] = mmap(0, GetStackSize(), PROT_READ | PROT_WRITE,
                         MAP_STACK | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED) {
      kprintf("error: mmap(%d) failed %m\n", i);
      exit(1);
    }
  }
  LoadRelationships(argc, argv);
  Crunch();
  Explore();
  ksnprintf(path, sizeof(path), "%s.%d", out, getpid());
  CHECK_NE(-1, (fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644)),
           "open(%#s)", path);
  for (i = 0; i < threads; ++i) {
    CHECK_NE(-1, xwrite(fd, bouts[i], appendz(bouts[i]).i));
  }
  CHECK_NE(-1, close(fd));
  CHECK_NE(-1, rename(path, out));
  for (i = 0; i < threads; ++i) {
    munmap(stack[i], GetStackSize());
    free(bouts[i]);
    free(tls[i]);
  }
  free(strings.p);
  free(edges.p);
  free(sauces);
  free(stack);
  free(bouts);
  free(tls);
  return 0;
}
