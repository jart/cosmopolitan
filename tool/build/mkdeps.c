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
#include "libc/errno.h"
#include "libc/fmt/itoa.h"
#include "libc/fmt/libgen.h"
#include "libc/fmt/magnumstrs.internal.h"
#include "libc/intrin/kprintf.h"
#include "libc/limits.h"
#include "libc/macros.internal.h"
#include "libc/mem/alg.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/crc32.h"
#include "libc/runtime/runtime.h"
#include "libc/serialize.h"
#include "libc/stdio/append.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/str/tab.internal.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/s.h"
#include "third_party/getopt/getopt.internal.h"
#include "tool/build/lib/getargs.h"

#define VERSION                     \
  "cosmopolitan mkdeps v3.0\n"      \
  "copyright 2023 justine tunney\n" \
  "https://github.com/jart/cosmopolitan\n"

#define MANUAL                                                               \
  " -r o// -o OUTPUT INPUT...\n"                                             \
  "\n"                                                                       \
  "DESCRIPTION\n"                                                            \
  "\n"                                                                       \
  "  Generates header file dependencies for your makefile\n"                 \
  "\n"                                                                       \
  "  This tool computes the transitive closure of included paths\n"          \
  "  for every source file in your repository. This program does\n"          \
  "  it orders of a magnitude faster than `gcc -M` on each file.\n"          \
  "\n"                                                                       \
  "  Includes look like this:\n"                                             \
  "\n"                                                                       \
  "    - #include <stdio.h>\n"                                               \
  "    - #include \"samedir.h\"\n"                                           \
  "    - #include \"root/of/repository/foo.h\"\n"                            \
  "    - .include \"asm/x86_64/foo.s\"\n"                                    \
  "\n"                                                                       \
  "  Your generated make code looks like this:\n"                            \
  "\n"                                                                       \
  "    o//package/foo.o: \\\n"                                               \
  "      package/foo.c \\\n"                                                 \
  "      package/foo.h \\\n"                                                 \
  "      package/bar.h \\\n"                                                 \
  "      libc/isystem/stdio.h\n"                                             \
  "    o//package/bar.o: \\\n"                                               \
  "      package/bar.c \\\n"                                                 \
  "      package/bar.h\n"                                                    \
  "\n"                                                                       \
  "FLAGS\n"                                                                  \
  "\n"                                                                       \
  "  -h         show usage\n"                                                \
  "  -o OUTPUT  set output path\n"                                           \
  "  -g ROOT    set generated path [default: o/]\n"                          \
  "  -r ROOT    set build output path, e.g. o/$(MODE)/\n"                    \
  "  -S PATH    isystem include path [repeatable; default: libc/isystem/]\n" \
  "  -s         hermetically sealed mode [repeatable]\n"                     \
  "\n"                                                                       \
  "ARGUMENTS\n"                                                              \
  "\n"                                                                       \
  "  OUTPUT     shall be makefile code\n"                                    \
  "  INPUT      should be source or @args.txt\n"                             \
  "\n"

#define Read32(s) (s[3] << 24 | s[2] << 16 | s[1] << 8 | s[0])
#define EXT(s)    Read32(s "\0\0")

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

struct Paths {
  long n;
  const char *p[64];
};

static const uint32_t kSourceExts[] = {
    EXT("s"),    // assembly
    EXT("S"),    // assembly with c preprocessor
    EXT("c"),    // c
    EXT("cc"),   // c++
    EXT("cpp"),  // c++
    EXT("cu"),   // cuda
    EXT("m"),    // objective c
};

static char *names;
static int hermetic;
static unsigned counter;
static const char *prog;
static struct Edges edges;
static struct Sauce *sauces;
static struct Sources sources;
static struct Paths systempaths;
static const char *buildroot;
static const char *genroot;
static const char *outpath;

static inline bool IsBlank(int c) {
  return c == ' ' || c == '\t';
}

static inline bool IsGraph(wint_t c) {
  return 0x21 <= c && c <= 0x7E;
}

static wontreturn void Die(const char *reason) {
  tinyprint(2, prog, ": ", reason, "\n", NULL);
  exit(1);
}

static wontreturn void DieSys(const char *thing) {
  perror(thing);
  exit(1);
}

static wontreturn void DiePathTooLong(const char *path) {
  errno = ENAMETOOLONG;
  DieSys(path);
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
  if (radix_sort_int64((long *)sauces, sources.i) == -1 ||
      radix_sort_int64((long *)edges.p, edges.i) == -1) {
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

static int HashSource(const char *name, size_t len, bool create) {
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
  if (!create) return -1;
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

static int CreateSourceId(const char *name) {
  return HashSource(name, strlen(name), true);
}

static int GetSourceId(const char *name) {
  return HashSource(name, strlen(name), false);
}

// `p` should point to substring "include "
// `map` and `mapsize` define legal memory range
// returns pointer to path, or null if `p` isn't an include
//
// syntax like the following is supported:
//
//   - `#include "foo.h"`
//   - `#include <foo.h>`
//   - `# include <foo.h>`
//   - `#include  <foo.h>`
//   - ` #include  <foo.h>`
//   - `.include "foo.h"` (for .s files only)
//
// known issues:
//
//   - we can't tell if it's inside a /* comment */
//   - whitespace like vertical tab isn't supported
//
static const char *FindIncludePath(const char *map, size_t mapsize,
                                   const char *p, bool is_assembly) {
  const char *q = p;

  // scan backwards for hash character
  for (;;) {
    if (q == map) {
      return 0;
    }
    if (IsBlank(q[-1])) {
      --q;
      continue;
    }
    if (q[-1] == '#' && !is_assembly) {
      --q;
      break;
    } else if (q[-1] == '.' && is_assembly) {
      --q;
      break;
    } else {
      return 0;
    }
  }

  // scan backwards for newline character
  if (!is_assembly) {
    for (;;) {
      if (q == map) {
        break;
      }
      if (IsBlank(q[-1])) {
        --q;
        continue;
      }
      if (q[-1] == '\n') {
        break;
      } else {
        return 0;
      }
    }
  }

  // scan forward for path
  q = p + strlen("include ");
  for (;;) {
    if (q >= map + mapsize) {
      break;
    }
    if (IsBlank(*q)) {
      ++q;
      continue;
    }
    if (*q == '"' || (*q == '<' && !is_assembly)) {
      ++q;
      break;
    } else {
      return 0;
    }
  }

  // return pointer to path
  return q;
}

static void LoadRelationships(int argc, char *argv[]) {
  int fd;
  char *map;
  ssize_t rc;
  size_t size;
  bool is_assembly;
  struct GetArgs ga;
  int srcid, dependency;
  static char srcdirbuf[PATH_MAX];
  const char *p, *pe, *src, *path, *pathend, *srcdir, *final;
  getargs_init(&ga, argv + optind);
  while ((src = getargs_next(&ga))) {
    CreateSourceId(src);
  }
  getargs_destroy(&ga);
  getargs_init(&ga, argv + optind);
  while ((src = getargs_next(&ga))) {
    is_assembly = endswith(src, ".s");
    srcid = GetSourceId(src);
    if (strlcpy(srcdirbuf, src, PATH_MAX) >= PATH_MAX) {
      DiePathTooLong(src);
    }
    srcdir = dirname(srcdirbuf);
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
      // repeatedly map to same fixed address so in order to weasel out
      // of incurring the additional overhead of all these munmap calls
      map = mmap((void *)0x311987030000, size, PROT_READ,
                 MAP_SHARED | MAP_FIXED, fd, 0);
      if (map == MAP_FAILED) {
        DieSys(src);
      }
      for (p = map, pe = map + size; p < pe; ++p) {
        if (!(p = memmem(p, pe - p, "include ", 8))) break;
        if (!(path = FindIncludePath(map, size, p, is_assembly))) continue;
        // copy the specified include path
        char right;
        if (path[-1] == '<') {
          if (!systempaths.n) continue;
          right = '>';
        } else {
          right = '"';
        }
        if (!(pathend = memchr(path, right, pe - path))) continue;
        if (pathend - path >= PATH_MAX) {
          tinyprint(2, src, ": uses really long include path\n", NULL);
          exit(1);
        }
        char juf[PATH_MAX];
        char incpath[PATH_MAX];
        *(char *)mempcpy(incpath, path, pathend - path) = 0;
        if (right == '>') {
          // handle angle bracket includes
          dependency = -1;
          for (long i = 0; i < systempaths.n; ++i) {
            if (!(final =
                      __join_paths(juf, PATH_MAX, systempaths.p[i], incpath))) {
              DiePathTooLong(incpath);
            }
            if ((dependency = GetSourceId(final)) != -1) {
              break;
            }
          }
          if (dependency != -1) {
            AppendEdge(&edges, dependency, srcid);
            p = pathend + 1;
          } else {
            if (hermetic == 1) {
              // chances are the `#include <foo>` is in some #ifdef
              // that'll never actually be executed; thus we ignore
              // since landlock make unveil() shall catch it anyway
              continue;
            }
            tinyprint(2, incpath,
                      ": system header not specified by the HDRS/SRCS/INCS "
                      "make variables defined by the hermetic mono repo\n",
                      NULL);
            exit(1);
          }
        } else {
          // handle double quote includes
          // let foo/bar.c say `#include "foo/hdr.h"`
          dependency = GetSourceId((final = incpath));
          // let foo/bar.c say `#include "hdr.h"`
          if (dependency == -1 && !strchr(final, '/')) {
            if (!(final = __join_paths(juf, PATH_MAX, srcdir, final))) {
              DiePathTooLong(incpath);
            }
            dependency = GetSourceId(final);
          }
          if (dependency == -1) {
            if (startswith(final, genroot)) {
              dependency = CreateSourceId(src);
            } else {
              tinyprint(2, incpath,
                        ": path not specified by HDRS/SRCS/INCS make variables "
                        "(it was included by ",
                        src, ")\n", NULL);
              exit(1);
            }
          }
          AppendEdge(&edges, dependency, srcid);
          p = pathend + 1;
        }
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

static void AddPath(struct Paths *paths, const char *path) {
  if (paths->n == ARRAYLEN(paths->p)) {
    Die("too many path arguments");
  }
  paths->p[paths->n++] = path;
}

static void GetOpts(int argc, char *argv[]) {
  int opt;
  while ((opt = getopt(argc, argv, "hnsgS:o:r:")) != -1) {
    switch (opt) {
      case 's':
        ++hermetic;
        break;
      case 'S':
        AddPath(&systempaths, optarg);
        break;
      case 'o':
        if (outpath) {
          Die("multiple output paths specified");
        }
        outpath = optarg;
        break;
      case 'r':
        if (buildroot) {
          Die("multiple build roots specified");
        }
        buildroot = optarg;
        break;
      case 'g':
        if (genroot) {
          Die("multiple generated roots specified");
        }
        genroot = optarg;
        break;
      case 'n':
        exit(0);
      case 'h':
        ShowUsage(0, 1);
      default:
        ShowUsage(1, 2);
    }
  }
  if (optind == argc) {
    Die("missing input argument");
  }
  if (!genroot) {
    genroot = "o/";
  }
  if (!endswith(genroot, "/")) {
    Die("generated output path must end with slash");
  }
  if (!buildroot) {
    Die("need build output path");
  }
  if (!endswith(buildroot, "/")) {
    Die("build output path must end with slash");
  }
  if (!startswith(buildroot, genroot)) {
    Die("build output path must start with generated output path");
  }
  if (!systempaths.n && hermetic) {
    AddPath(&systempaths, "third_party/libcxx/include/");
    AddPath(&systempaths, "libc/isystem/");
  }
  if (systempaths.n && !hermetic) {
    Die("system path can only be specified in hermetic mode");
  }
  long j = 0;
  for (long i = 0; i < systempaths.n; ++i) {
    size_t n;
    struct stat st;
    const char *path = systempaths.p[i];
    if (!stat(path, &st)) {
      systempaths.p[j++] = path;
      if (!S_ISDIR(st.st_mode)) {
        errno = ENOTDIR;
        DieSys(path);
      }
    }
    if ((n = strlen(path)) >= PATH_MAX) {
      DiePathTooLong(path);
    }
    if (!n || path[n - 1] != '/') {
      Die("system path must end with slash");
    }
  }
  systempaths.n = j;
}

static const char *StripExt(char pathbuf[hasatleast PATH_MAX], const char *s) {
  static char *dot;
  if (strlcpy(pathbuf, s, PATH_MAX) >= PATH_MAX) {
    DiePathTooLong(s);
  }
  dot = strrchr(pathbuf, '.');
  if (dot) *dot = '\0';
  return pathbuf;
}

static uint32_t GetFileExtension(const char *s) {
  uint32_t w;
  size_t i, n;
  n = s ? strlen(s) : 0;
  for (i = w = 0; n--;) {
    wint_t c = s[n];
    if (!IsGraph(c)) return 0;
    if (c == '.') break;
    if (++i > 4) return 0;
    w <<= 8;
    w |= kToLower[c];
  }
  return w;
}

static bool IsObjectSource(const char *name) {
  int i;
  uint32_t ext;
  if ((ext = GetFileExtension(name))) {
    for (i = 0; i < ARRAYLEN(kSourceExts); ++i) {
      if (ext == kSourceExts[i]) {
        return true;
      }
    }
  }
  return false;
}

__funline bool Bts(uint32_t *p, size_t i) {
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
  char buf[PATH_MAX];
  visilen = (sources.i + sizeof(*visited) * CHAR_BIT - 1) /
            (sizeof(*visited) * CHAR_BIT);
  visited = Malloc(visilen * sizeof(*visited));
  for (i = 0; i < sources.i; ++i) {
    path = names + sauces[i].name;
    if (!IsObjectSource(path)) continue;
    if (startswith(path, genroot)) continue;
    Appendw(&makefile, '\n');
    Appends(&makefile, buildroot);
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
  int fd = 1;
  ssize_t rc;
  size_t i, n;
  char *makefile;
#ifdef MODE_DBG
  ShowCrashReports();
#endif
  prog = argv[0];
  if (!prog) prog = "mkdeps";
  GetOpts(argc, argv);
  LoadRelationships(argc, argv);
  Crunch();
  makefile = Explore();
  if (outpath &&
      (fd = open(outpath, O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1) {
    DieSys(outpath);
  }
  n = appendz(makefile).i;
  for (i = 0; i < n; i += (size_t)rc) {
    if ((rc = write(fd, makefile + i, n - i)) == -1) {
      DieSys(outpath);
    }
  }
  if (outpath && close(fd)) {
    DieSys(outpath);
  }
  free(makefile);
  free(edges.p);
  free(sauces);
  free(names);
  return 0;
}
