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
#include "libc/calls/struct/stat.h"
#include "libc/elf/elf.h"
#include "libc/elf/struct/shdr.h"
#include "libc/elf/struct/sym.h"
#include "libc/intrin/bswap.h"
#include "libc/intrin/safemacros.internal.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/mem/alg.h"
#include "libc/mem/arraylist.internal.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "third_party/getopt/getopt.h"
#include "third_party/xed/x86.h"
#include "tool/build/lib/getargs.h"
#include "tool/build/lib/persist.h"

/**
 * @fileoverview Build Package Script.
 *
 * FIRST PURPOSE
 *
 * This script verifies the well-formedness of dependencies, e.g.
 *
 *     o/tool/build/package.com \
 *       -o o/libc/stubs/stubs.pkg \
 *       o/libc/stubs/{a,b,...}.o
 *
 *     o/tool/build/package.com \
 *       -o o/libc/nexgen32e/nexgen32e.pkg \
 *       -d o/libc/stubs/stubs.pkg \
 *       o/libc/nexgen32e/{a,b,...}.o
 *
 * We want the following:
 *
 *   1. FOO declares in FOO_DIRECTDEPS where its undefined symbols are.
 *   2. FOO_DIRECTDEPS is complete, so FOO ∪ FOO_DIRECTDEPS has no UNDEFs.
 *   3. FOO_DIRECTDEPS is non-transitive; thus this tool is incremental.
 *   4. Package relationships on a whole are acyclic.
 *
 * These rules help keep the structure of large codebases easy to
 * understand. More importantly, it allows us to further optimize
 * compiled objects very cheaply as the build progresses.
 */

#define PACKAGE_MAGIC bswap_32(0xBEEFBEEFu)
#define PACKAGE_ABI   1

struct Packages {
  size_t i, n;
  struct Package {
    uint32_t magic;
    int32_t abi;
    uint32_t path;  // pkg->strings.p[path]
    int64_t fd;     // not persisted
    void *addr;     // not persisted
    size_t size;    // not persisted
    struct Strings {
      size_t i, n;
      char *p;  // persisted as pkg+RVA
    } strings;  // TODO(jart): interning?
    struct Objects {
      size_t i, n;
      struct Object {
        uint32_t path;           // pkg->strings.p[path]
        unsigned mode;           // not persisted
        struct Elf64_Ehdr *elf;  // not persisted
        size_t size;             // not persisted
        char *strs;              // not persisted
        Elf64_Sym *syms;         // not persisted
        Elf64_Xword symcount;    // not persisted
        struct Sections {
          size_t i, n;
          struct Section {
            enum SectionKind {
              kUndef,
              kText,
              kData,
              kPiroRelo,
              kPiroData,
              kPiroBss,
              kBss,
            } kind;
          } * p;
        } sections;  // not persisted
      } * p;         // persisted as pkg+RVA
    } objects;
    struct Symbols {
      size_t i, n;
      struct Symbol {
        uint32_t name;  // pkg->strings.p[name]
        enum SectionKind kind : 8;
        uint8_t bind_ : 4;
        uint8_t type : 4;
        uint16_t object;  // pkg->objects.p[object]
      } * p;              // persisted as pkg+RVA
    } symbols, undefs;    // TODO(jart): hash undefs?
  } * *p;                 // persisted across multiple files
};

int CompareSymbolName(const struct Symbol *a, const struct Symbol *b,
                      const char *tab) {
  return strcmp(tab + a->name, tab + b->name);
}

struct Package *LoadPackage(const char *path) {
  int fd;
  ssize_t i;
  struct stat st;
  struct Package *pkg;
  CHECK(fileexists(path), "%s: %s: %s\n", "error", path, "not found");
  CHECK_NE(-1, (fd = open(path, O_RDONLY)), "%s", path);
  CHECK_NE(-1, fstat(fd, &st));
  CHECK_NE(MAP_FAILED,
           (pkg = mmap(NULL, st.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE,
                       fd, 0)),
           "path=%s", path);
  CHECK_NE(-1, close(fd));
  CHECK_EQ(PACKAGE_MAGIC, pkg->magic, "corrupt package: %`'s", path);
  pkg->strings.p = (char *)((intptr_t)pkg->strings.p + (intptr_t)pkg);
  pkg->objects.p = (struct Object *)((intptr_t)pkg->objects.p + (intptr_t)pkg);
  pkg->symbols.p = (struct Symbol *)((intptr_t)pkg->symbols.p + (intptr_t)pkg);
  pkg->addr = pkg;
  pkg->size = st.st_size;
  CHECK_NE(-1, mprotect(pkg, st.st_size, PROT_READ));
  return pkg;
}

void AddDependency(struct Packages *deps, const char *path) {
  struct Package *pkg;
  pkg = LoadPackage(path);
  CHECK_NE(-1, append(deps, &pkg));
}

void WritePackage(struct Package *pkg) {
  CHECK_NE(0, PACKAGE_MAGIC);
  pkg->magic = PACKAGE_MAGIC;
  pkg->abi = PACKAGE_ABI;
  DEBUGF("%s has %,ld objects, %,ld symbols, and a %,ld byte string table",
         &pkg->strings.p[pkg->path], pkg->objects.i, pkg->symbols.i,
         pkg->strings.i);
  PersistObject(
      &pkg->strings.p[pkg->path], 64,
      &(struct ObjectParam){
          sizeof(struct Package),
          pkg,
          &pkg->magic,
          &pkg->abi,
          (struct ObjectArrayParam[]){
              {pkg->strings.i, sizeof(pkg->strings.p[0]), &pkg->strings.p},
              {pkg->objects.i, sizeof(pkg->objects.p[0]), &pkg->objects.p},
              {pkg->symbols.i, sizeof(pkg->symbols.p[0]), &pkg->symbols.p},
              {0},
          },
      });
}

void GetOpts(struct Package *pkg, struct Packages *deps, int argc,
             char *argv[]) {
  long i, si, opt;
  const char *arg;
  struct GetArgs ga;
  pkg->path = -1;
  while ((opt = getopt(argc, argv, "vho:d:")) != -1) {
    switch (opt) {
      case 'v':
        __log_level = kLogDebug;
        break;
      case 'o':
        pkg->path = concat(&pkg->strings, optarg, strlen(optarg) + 1);
        break;
      case 'd':
        AddDependency(deps, optarg);
        break;
      case 'h':
        exit(0);
      default:
        fprintf(stderr, "%s: %s [%s %s] [%s %s] %s\n", "Usage",
                program_invocation_name, "-o", "OUTPACKAGE", "-d", "DEPPACKAGE",
                "OBJECT...");
        exit(1);
    }
  }
  CHECK_NE(-1, pkg->path, "no packages passed to package.com");
  CHECK_LT(optind, argc,
           "no objects passed to package.com; "
           "is your foo.mk $(FOO_OBJS) glob broken?");
  getargs_init(&ga, argv + optind);
  while ((arg = getargs_next(&ga))) {
    CHECK_NE(-1, (si = concat(&pkg->strings, arg, strlen(arg) + 1)));
    CHECK_NE(-1, append(&pkg->objects, (&(struct Object){si})));
  }
  getargs_destroy(&ga);
}

void IndexSections(struct Object *obj) {
  size_t i;
  const char *name;
  const uint8_t *code;
  struct Section sect;
  const Elf64_Shdr *shdr;
  struct XedDecodedInst xedd;
  for (i = 0; i < obj->elf->e_shnum; ++i) {
    bzero(&sect, sizeof(sect));
    CHECK_NOTNULL((shdr = GetElfSectionHeaderAddress(obj->elf, obj->size, i)));
    if (shdr->sh_type != SHT_NULL) {
      CHECK_NOTNULL((name = GetElfSectionName(obj->elf, obj->size, shdr)));
      if (_startswith(name, ".sort.")) name += 5;
      if ((strcmp(name, ".piro.relo") == 0 ||
           _startswith(name, ".piro.relo.")) ||
          (strcmp(name, ".data.rel.ro") == 0 ||
           _startswith(name, ".data.rel.ro."))) {
        sect.kind = kPiroRelo;
      } else if (strcmp(name, ".piro.data") == 0 ||
                 _startswith(name, ".piro.data.")) {
        sect.kind = kPiroData;
      } else if (strcmp(name, ".piro.bss") == 0 ||
                 _startswith(name, ".piro.bss.")) {
        sect.kind = kPiroBss;
      } else if (strcmp(name, ".data") == 0 || _startswith(name, ".data.")) {
        sect.kind = kData;
      } else if (strcmp(name, ".bss") == 0 || _startswith(name, ".bss.")) {
        sect.kind = kBss;
      } else {
        sect.kind = kText;
      }
    } else {
      sect.kind = kUndef; /* should always and only be section #0 */
    }
    CHECK_NE(-1, append(&obj->sections, &sect));
  }
}

enum SectionKind ClassifySection(struct Object *obj, uint8_t type,
                                 Elf64_Section shndx) {
  if (type == STT_COMMON) return kBss;
  if (!obj->sections.i) return kText;
  return obj->sections.p[min(max(0, shndx), obj->sections.i - 1)].kind;
}

void LoadSymbols(struct Package *pkg, uint32_t object) {
  Elf64_Xword i;
  const char *name;
  struct Object *obj;
  struct Symbol symbol;
  obj = &pkg->objects.p[object];
  symbol.object = object;
  for (i = 0; i < obj->symcount; ++i) {
    symbol.bind_ = ELF64_ST_BIND(obj->syms[i].st_info);
    symbol.type = ELF64_ST_TYPE(obj->syms[i].st_info);
    if (symbol.bind_ != STB_LOCAL &&
        (symbol.type == STT_OBJECT || symbol.type == STT_FUNC ||
         symbol.type == STT_COMMON || symbol.type == STT_NOTYPE)) {
      name = GetElfString(obj->elf, obj->size, obj->strs, obj->syms[i].st_name);
      DEBUGF("%s", name);
      if (strcmp(name, "_GLOBAL_OFFSET_TABLE_") != 0) {
        symbol.kind = ClassifySection(obj, symbol.type, obj->syms[i].st_shndx);
        CHECK_NE(-1,
                 (symbol.name = concat(&pkg->strings, name, strlen(name) + 1)));
        CHECK_NE(-1,
                 append(symbol.kind != kUndef ? &pkg->symbols : &pkg->undefs,
                        &symbol));
      }
    }
  }
}

void OpenObject(struct Package *pkg, struct Object *obj, int mode, int prot,
                int flags) {
  int fd;
  struct stat st;
  CHECK_NE(-1, (fd = open(&pkg->strings.p[obj->path], (obj->mode = mode))),
           "path=%`'s", &pkg->strings.p[obj->path]);
  CHECK_NE(-1, fstat(fd, &st));
  CHECK_NE(
      MAP_FAILED,
      (obj->elf = mmap(NULL, (obj->size = st.st_size), prot, flags, fd, 0)),
      "path=%`'s", &pkg->strings.p[obj->path]);
  CHECK_NE(-1, close(fd));
  CHECK(IsElf64Binary(obj->elf, obj->size), "path=%`'s",
        &pkg->strings.p[obj->path]);
  CHECK_NOTNULL((obj->strs = GetElfStringTable(obj->elf, obj->size)), "on %s",
                &pkg->strings.p[obj->path]);
  CHECK_NOTNULL(
      (obj->syms = GetElfSymbolTable(obj->elf, obj->size, &obj->symcount)));
  CHECK_NE(0, obj->symcount);
  IndexSections(obj);
}

void CloseObject(struct Object *obj) {
  CHECK_NE(-1, munmap(obj->elf, obj->size));
}

void LoadObjects(struct Package *pkg) {
  size_t i;
  struct Object *obj;
  for (i = 0; i < pkg->objects.i; ++i) {
    obj = pkg->objects.p + i;
    OpenObject(pkg, obj, O_RDONLY, PROT_READ, MAP_SHARED);
    LoadSymbols(pkg, i);
    CloseObject(obj);
  }
  qsort_r(pkg->symbols.p, pkg->symbols.i, sizeof(*pkg->symbols.p),
          (void *)CompareSymbolName, pkg->strings.p);
}

struct Symbol *BisectSymbol(struct Package *pkg, const char *name) {
  int c;
  long m, l, r;
  l = 0;
  r = pkg->symbols.i - 1;
  while (l <= r) {
    m = (l + r) >> 1;
    c = strcmp(pkg->strings.p + pkg->symbols.p[m].name, name);
    if (c < 0) {
      l = m + 1;
    } else if (c > 0) {
      r = m - 1;
    } else {
      return pkg->symbols.p + m;
    }
  }
  return NULL;
}

bool FindSymbol(const char *name, struct Package *pkg,
                struct Packages *directdeps, struct Package **out_pkg,
                struct Symbol **out_sym) {
  size_t i, j;
  struct Symbol *sym;
  if ((sym = BisectSymbol(pkg, name))) {
    if (out_sym) *out_sym = sym;
    if (out_pkg) *out_pkg = pkg;
    return true;
  }
  for (i = 0; i < directdeps->i; ++i) {
    if ((sym = BisectSymbol(directdeps->p[i], name))) {
      if (out_sym) *out_sym = sym;
      if (out_pkg) *out_pkg = directdeps->p[i];
      return true;
    }
  }
  return false;
}

void CheckStrictDeps(struct Package *pkg, struct Packages *deps) {
  size_t i, j;
  struct Package *dep;
  struct Symbol *undef;
  for (i = 0; i < pkg->undefs.i; ++i) {
    undef = &pkg->undefs.p[i];
    if (undef->bind_ == STB_WEAK) continue;
    if (!FindSymbol(pkg->strings.p + undef->name, pkg, deps, NULL, NULL)) {
      fprintf(stderr, "%s: %`'s (%s) %s %s\n", "error",
              pkg->strings.p + undef->name,
              pkg->strings.p + pkg->objects.p[undef->object].path,
              "not defined by direct deps of", pkg->strings.p + pkg->path);
      for (j = 0; j < deps->i; ++j) {
        dep = deps->p[j];
        fputc('\t', stderr);
        fputs(dep->strings.p + dep->path, stderr);
        fputc('\n', stderr);
      }
      exit(1);
    }
  }
  free(pkg->undefs.p);
  bzero(&pkg->undefs, sizeof(pkg->undefs));
}

forceinline bool IsRipRelativeModrm(uint8_t modrm) {
  return (modrm & 0b11000111) == 0b00000101;
}

forceinline uint8_t ChangeRipToRbx(uint8_t modrm) {
  return (modrm & 0b00111000) | 0b10000011;
}

bool IsSymbolDirectlyReachable(struct Package *pkg, struct Packages *deps,
                               const char *symbol) {
  return FindSymbol(symbol, pkg, deps, NULL, NULL);
}

void Package(int argc, char *argv[], struct Package *pkg,
             struct Packages *deps) {
  size_t i, j;
  GetOpts(pkg, deps, argc, argv);
  LoadObjects(pkg);
  CheckStrictDeps(pkg, deps);
  WritePackage(pkg);
  for (i = 0; i < deps->i; ++i) {
    CHECK_NE(-1, munmap(deps->p[i]->addr, deps->p[i]->size));
  }
  for (i = 0; i < pkg->objects.i; ++i) {
    free(pkg->objects.p[i].sections.p);
  }
  free(pkg->strings.p);
  free(pkg->objects.p);
  free(pkg->symbols.p);
  free(deps->p);
}

int main(int argc, char *argv[]) {
  struct Package pkg;
  struct Packages deps;
  if (argc == 2 && !strcmp(argv[1], "-n")) exit(0);
  if (IsModeDbg()) ShowCrashReports();
  bzero(&pkg, sizeof(pkg));
  bzero(&deps, sizeof(deps));
  Package(argc, argv, &pkg, &deps);
  return 0;
}
