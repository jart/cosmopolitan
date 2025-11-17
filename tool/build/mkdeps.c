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
#include "libc/cosmo.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/itoa.h"
#include "libc/fmt/libgen.h"
#include "libc/fmt/magnumstrs.internal.h"
#include "libc/limits.h"
#include "libc/macros.h"
#include "libc/mem/alg.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/crc32.h"
#include "libc/runtime/runtime.h"
#include "libc/serialize.h"
#include "libc/stdio/append.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/str/tab.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/s.h"
#include "third_party/getopt/getopt.internal.h"
#include "tool/build/lib/getargs.h"

#define VERSION                     \
  "cosmopolitan mkdeps v3.1\n"      \
  "copyright 2025 justine tunney\n" \
  "https://github.com/jart/cosmopolitan\n"

#define MANUAL                                                      \
  " -r o// -o OUTPUT INPUT...\n"                                    \
  "\n"                                                              \
  "DESCRIPTION\n"                                                   \
  "\n"                                                              \
  "  Generates header file dependencies for your makefile\n"        \
  "\n"                                                              \
  "  This tool computes the transitive closure of included paths\n" \
  "  for every source file in your repository. This program does\n" \
  "  it orders of a magnitude faster than `gcc -M` on each file.\n" \
  "\n"                                                              \
  "  Includes look like this:\n"                                    \
  "\n"                                                              \
  "    - #include <stdio.h>\n"                                      \
  "    - #include_next <stdio.h>\n"                                 \
  "    - #include \"samedir.h\"\n"                                  \
  "    - #include \"root/of/repository/foo.h\"\n"                   \
  "    - .include \"asm/x86_64/foo.s\"\n"                           \
  "\n"                                                              \
  "  Your generated make code looks like this:\n"                   \
  "\n"                                                              \
  "    o//package/foo.o: \\\n"                                      \
  "      package/foo.c \\\n"                                        \
  "      package/foo.h \\\n"                                        \
  "      package/bar.h \\\n"                                        \
  "      libc/isystem/stdio.h\n"                                    \
  "    o//package/bar.o: \\\n"                                      \
  "      package/bar.c \\\n"                                        \
  "      package/bar.h\n"                                           \
  "\n"                                                              \
  "FLAGS\n"                                                         \
  "\n"                                                              \
  "  -h              show usage\n"                                  \
  "  -o OUTPUT       set output path\n"                             \
  "  -g ROOT         set generated path [default: o/]\n"            \
  "  -r ROOT         set build output path, e.g. o/$(MODE)/\n"      \
  "  -S [LANG:]PATH  isystem include path [repeatable]\n"           \
  "  -s              relaxed hermetically sealed mode\n"            \
  "  -ss             zealous hermetically sealed mode\n"            \
  "\n"                                                              \
  "ARGUMENTS\n"                                                     \
  "\n"                                                              \
  "  OUTPUT     shall be makefile code\n"                           \
  "  INPUT      should be source or @args.txt\n"                    \
  "\n"

#define Read32(s) (s[3] << 24 | s[2] << 16 | s[1] << 8 | s[0])
#define EXT(s)    Read32(s "\0\0")

enum Language {
  LANG_NONE,
  LANG_ASM,
  LANG_C,
  LANG_CXX,
  LANG_OBJC,
  LANG_COUNT,
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

struct SystemPath {
  char *path;
  enum Language lang;
};

struct SystemPaths {
  long n;
  struct SystemPath *p;
};

// these are file extensions for source files that get turned into
// object code. everything that isn't listed here is considered a
// language agnostic header file.
static const struct SourceExtension {
  uint32_t ext;
  enum Language lang;
} kSourceExts[] = {
    {EXT("c"), LANG_C},      // c
    {EXT("s"), LANG_ASM},    // assembly
    {EXT("S"), LANG_ASM},    // assembly with c preprocessor
    {EXT("cc"), LANG_CXX},   // c++
    {EXT("c++"), LANG_CXX},  // c++
    {EXT("cpp"), LANG_CXX},  // c++
    {EXT("cu"), LANG_CXX},   // cuda
    {EXT("m"), LANG_OBJC},   // objective c
};

static char *names;
static int hermetic;
static unsigned counter;
static const char *prog;
static struct Edges edges[LANG_COUNT];
static struct Sauce *sauces;
static struct Sources sources;
static struct SystemPaths systempaths;
static const char *buildroot;
static const char *genroot;
static const char *outpath;

static inline bool IsBlank(int c) {
  return c == ' ' || c == '\t';
}

static inline bool IsGraph(wint_t c) {
  return 0x21 <= c && c <= 0x7E;
}

[[noreturn]] static void Die(const char *reason) {
  tinyprint(2, prog, ": ", reason, "\n", NULL);
  exit(1);
}

[[noreturn]] static void DieSys(const char *thing) {
  perror(thing);
  exit(1);
}

[[noreturn]] static void DiePathTooLong(const char *path) {
  errno = ENAMETOOLONG;
  DieSys(path);
}

[[noreturn]] static void DieOom(void) {
  Die("out of memory");
}

static unsigned Hash(const void *s, size_t l) {
  unsigned h;
  h = crc32c(0, s, l);
  if (!h)
    h = 1;
  return h;
}

static void *Malloc(size_t n) {
  void *p;
  if (!(p = malloc(n)))
    DieOom();
  return p;
}

static void *Calloc(size_t n, size_t z) {
  void *p;
  if (!(p = calloc(n, z)))
    DieOom();
  return p;
}

static void *Realloc(void *p, size_t n) {
  if (!(p = realloc(p, n)))
    DieOom();
  return p;
}

static void Appendw(char **b, uint64_t w) {
  if (appendw(b, w) == -1)
    DieOom();
}

static void Appends(char **b, const char *s) {
  if (appends(b, s) == -1)
    DieOom();
}

static void Appendd(char **b, const void *p, size_t n) {
  if (appendd(b, p, n) == -1)
    DieOom();
}

static unsigned FindFirstFromEdge(enum Language lang, unsigned id) {
  unsigned m, l, r;
  l = 0;
  r = edges[lang].i;
  while (l < r) {
    m = (l & r) + ((l ^ r) >> 1);  // floor((a+b)/2)
    if (edges[lang].p[m].from < id) {
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
  if (radix_sort_int64((long *)sauces, sources.i) == -1)
    DieOom();
  for (int lang = 0; lang < LANG_COUNT; ++lang)
    if (radix_sort_int64((long *)edges[lang].p, edges[lang].i) == -1)
      DieOom();
}

static void Rehash(void) {
  size_t i, j, step;
  struct Sources old;
  memcpy(&old, &sources, sizeof(sources));
  sources.n = sources.n ? sources.n << 1 : 16;
  sources.p = Calloc(sources.n, sizeof(struct Source));
  for (i = 0; i < old.n; ++i) {
    if (!old.p[i].hash)
      continue;
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
  if (!create)
    return -1;
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

static uint32_t GetFileExtension(const char *s) {
  uint32_t w;
  size_t i, n;
  n = s ? strlen(s) : 0;
  for (i = w = 0; n--;) {
    int c = s[n] & 255;
    if (!IsGraph(c))
      return 0;
    if (c == '.')
      break;
    if (++i > 4)
      return 0;
    w <<= 8;
    w |= c;
  }
  return w;
}

static enum Language LookupLanguage(uint32_t ext) {
  for (int i = 0; i < ARRAYLEN(kSourceExts); ++i)
    if (ext == kSourceExts[i].ext)
      return kSourceExts[i].lang;
  return LANG_NONE;
}

static enum Language GetLanguage(const char *name) {
  char buf[5] = {0};
  if (strlcpy(buf, name, 5) <= 4)
    return LookupLanguage(Read32(buf));
  return LANG_NONE;
}

static enum Language GetLanguageFromPath(const char *name) {
  uint32_t ext;
  if ((ext = GetFileExtension(name)))
    return LookupLanguage(ext);
  return LANG_NONE;
}

// `p` should point to substring "include"
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
                                   const char *p, bool is_assembly,
                                   bool *is_include_next) {
  const char *q = p;

  // scan backwards for hash character
  for (;;) {
    if (q == map)
      return 0;
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

  // tell include apart from include_next
  q = p + strlen("include");
  if (*q == ' ') {
    // this is #include
    q += 1;
    *is_include_next = false;
  } else if (q[0] == '_' &&  //
             q[1] == 'n' &&  //
             q[2] == 'e' &&  //
             q[3] == 'x' &&  //
             q[4] == 't' &&  //
             q[5] == ' ') {
    // this is #include_next
    q += 6;
    *is_include_next = true;
  } else {
    return 0;
  }

  // scan forward for path
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
  bool is_include_next;
  int srcid, dependency;
  enum Language slang, elang;
  static char srcdirbuf[PATH_MAX];
  const char *p, *pe, *src, *path, *pathend, *srcdir, *final;
  getargs_init(&ga, argv + optind);
  while ((src = getargs_next(&ga)))
    CreateSourceId(src);
  getargs_destroy(&ga);
  getargs_init(&ga, argv + optind);
  while ((src = getargs_next(&ga))) {
    slang = GetLanguageFromPath(src);
    is_assembly = endswith(src, ".s");
    srcid = GetSourceId(src);
    if (strlcpy(srcdirbuf, src, PATH_MAX) >= PATH_MAX)
      DiePathTooLong(src);
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
    if ((rc = lseek(fd, 0, SEEK_END)) == -1)
      DieSys(src);
    if ((size = rc)) {
      // repeatedly map to same fixed address so in order to weasel out
      // of incurring the additional overhead of all these munmap calls
      map = mmap(0, size, PROT_READ, MAP_SHARED, fd, 0);
      if (map == MAP_FAILED)
        DieSys(src);
      for (p = map, pe = map + size; p < pe; ++p) {
        if (!(p = memmem(p, pe - p, "include", 7)))
          break;
        if (!(path = FindIncludePath(map, size, p, is_assembly,  //
                                     &is_include_next)))
          continue;
        // copy the specified include path
        char right;
        if (path[-1] == '<') {
          if (!systempaths.n)
            continue;
          right = '>';
        } else {
          right = '"';
        }
        if (!(pathend = memchr(path, right, pe - path)))
          continue;
        if (pathend - path >= PATH_MAX) {
          tinyprint(2, src, ": uses really long include path\n", NULL);
          exit(1);
        }
        char juf[PATH_MAX];
        char incpath[PATH_MAX];
        *(char *)mempcpy(incpath, path, pathend - path) = 0;
        if (right == '>') {
          // handle angle bracket includes
          bool found = false;
          for (elang = 1; elang < LANG_COUNT; ++elang) {
            if (slang)
              if (elang != slang)
                continue;
            dependency = -1;
            for (long i = 0; i < systempaths.n; ++i) {
              if (systempaths.p[i].lang)
                if (systempaths.p[i].lang != elang)
                  continue;
              if (is_include_next)
                if (startswith(src, systempaths.p[i].path))
                  continue;
              if (!(final = __join_paths(juf, PATH_MAX, systempaths.p[i].path,
                                         incpath)))
                DiePathTooLong(incpath);
              if ((dependency = GetSourceId(final)) != -1)
                break;
            }
            if (dependency != -1) {
              AppendEdge(&edges[elang], dependency, srcid);
              p = pathend + 1;
              found = true;
            } else if (hermetic == 1) {
              // chances are the `#include <foo>` is in some #ifdef
              // that'll never actually be executed; thus we ignore
              // since landlock make unveil() shall catch it anyway
              found = true;
            }
          }
          if (!found) {
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
            if (!(final = __join_paths(juf, PATH_MAX, srcdir, final)))
              DiePathTooLong(incpath);
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
          if (slang) {
            AppendEdge(&edges[slang], dependency, srcid);
          } else {
            for (elang = 1; elang < LANG_COUNT; ++elang)
              AppendEdge(&edges[elang], dependency, srcid);
          }
          p = pathend + 1;
        }
      }
      if (munmap(map, size))
        DieSys(src);
    }
    if (close(fd))
      DieSys(src);
  }
  getargs_destroy(&ga);
}

[[noreturn]] static void ShowUsage(int rc, int fd) {
  tinyprint(fd, VERSION, "\nUSAGE\n\n  ", prog, MANUAL, NULL);
  exit(rc);
}

static void AddSystemPath(char *path) {
  enum Language lang = LANG_NONE;
  char *p = strchr(path, ':');
  if (p) {
    *p = 0;
    if (!(lang = GetLanguage(path)))
      Die("unrecognized language");
    path = p + 1;
  }
  if (!endswith(path, "/"))
    Die("system include path must end with slash");
  if (strlen(path) > PATH_MAX - 128)
    Die("system include path too long");
  struct stat st;
  if (stat(path, &st))
    DieSys(path);
  systempaths.p =
      Realloc(systempaths.p, (systempaths.n + 1) * sizeof(*systempaths.p));
  systempaths.p[systempaths.n].lang = lang;
  systempaths.p[systempaths.n].path = path;
  systempaths.n += 1;
}

static void GetOpts(int argc, char *argv[]) {
  int opt;
  while ((opt = getopt(argc, argv, "hnsgS:o:r:")) != -1) {
    switch (opt) {
      case 's':
        ++hermetic;
        break;
      case 'S':
        AddSystemPath(optarg);
        break;
      case 'o':
        if (outpath)
          Die("multiple output paths specified");
        outpath = optarg;
        break;
      case 'r':
        if (buildroot)
          Die("multiple build roots specified");
        buildroot = optarg;
        break;
      case 'g':
        if (genroot)
          Die("multiple generated roots specified");
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
  if (optind == argc)
    Die("missing input argument");
  if (!genroot)
    genroot = "o/";
  if (!endswith(genroot, "/"))
    Die("generated output path must end with slash");
  if (!buildroot)
    Die("need build output path");
  if (!endswith(buildroot, "/"))
    Die("build output path must end with slash");
  if (!startswith(buildroot, genroot))
    Die("build output path must start with generated output path");
  if (systempaths.n && !hermetic)
    Die("system path can only be specified in hermetic mode");
}

static const char *StripExt(char pathbuf[hasatleast PATH_MAX], const char *s) {
  static char *dot;
  if (strlcpy(pathbuf, s, PATH_MAX) >= PATH_MAX)
    DiePathTooLong(s);
  dot = strrchr(pathbuf, '.');
  if (dot)
    *dot = '\0';
  return pathbuf;
}

__funline bool Bts(uint32_t *p, size_t i) {
  uint32_t k;
  k = 1u << (i & 31);
  if (p[i >> 5] & k)
    return true;
  p[i >> 5] |= k;
  return false;
}

static void Dive(char **makefile, uint32_t *visited, unsigned id,
                 enum Language lang, long depth) {
  for (long i = FindFirstFromEdge(lang, id);
       i < edges[lang].i && edges[lang].p[i].from == id; ++i) {
    if (Bts(visited, edges[lang].p[i].to))
      continue;
    Appendw(makefile, READ64LE(" \\\n\t  \0"));
    for (long j = 0; j < depth; ++j)
      Appendw(makefile, READ32LE("  \0"));
    Appends(makefile, names + sauces[edges[lang].p[i].to].name);
    Dive(makefile, visited, edges[lang].p[i].to, lang, depth + 1);
  }
}

static char *Explore(void) {
  unsigned *visited;
  char *makefile = 0;
  char buf[PATH_MAX];
  enum Language lang;
  size_t visilen = (sources.i + sizeof(*visited) * CHAR_BIT - 1) /
                   (sizeof(*visited) * CHAR_BIT);
  Appends(&makefile, "# -*-makefile-*-\n");
  visited = Malloc(visilen * sizeof(*visited));
  for (size_t i = 0; i < sources.i; ++i) {
    const char *path = names + sauces[i].name;
    if (!(lang = GetLanguageFromPath(path)))
      continue;
    if (startswith(path, genroot))
      continue;
    Appendw(&makefile, '\n');
    Appends(&makefile, buildroot);
    Appends(&makefile, StripExt(buf, path));
    Appendw(&makefile, READ64LE(".o: \\\n\t"));
    Appends(&makefile, path);
    bzero(visited, visilen * sizeof(*visited));
    Bts(visited, i);
    Dive(&makefile, visited, i, lang, 0);
    Appendw(&makefile, '\n');
  }
  Appendw(&makefile, '\n');
  free(visited);
  return makefile;
}

int main(int argc, char *argv[]) {
#ifndef NDEBUG
  ShowCrashReports();
#endif
  prog = argv[0];
  if (!prog)
    prog = "mkdeps";
  GetOpts(argc, argv);

  // read all source files and build graph of #include statements
  LoadRelationships(argc, argv);
  Crunch();

  // generate makefile
  char *makefile = Explore();

  // write makefile to output
  int fd = STDOUT_FILENO;
  if (outpath)
    if ((fd = open(outpath, O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1)
      DieSys(outpath);
  ssize_t rc;
  size_t n = appendz(makefile).i;
  for (long i = 0; i < n; i += rc)
    if ((rc = write(fd, makefile + i, n - i)) == -1)
      DieSys(outpath);
  if (outpath)
    if (close(fd))
      DieSys(outpath);

#ifndef NDEBUG
  // clean up
  free(systempaths.p);
  free(makefile);
  for (int lang = 0; lang < LANG_COUNT; ++lang)
    free(edges[lang].p);
  free(sauces);
  free(names);
  CheckForMemoryLeaks();
#endif
}
