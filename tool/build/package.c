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
#include "libc/calls/struct/iovec.h"
#include "libc/calls/struct/stat.h"
#include "libc/dce.h"
#include "libc/elf/def.h"
#include "libc/elf/elf.h"
#include "libc/elf/struct/rela.h"
#include "libc/elf/struct/shdr.h"
#include "libc/elf/struct/sym.h"
#include "libc/errno.h"
#include "libc/fmt/magnumstrs.internal.h"
#include "libc/intrin/bswap.h"
#include "libc/intrin/kprintf.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/mem/alg.h"
#include "libc/mem/arraylist.internal.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "third_party/getopt/getopt.internal.h"
#include "third_party/xed/x86.h"
#include "tool/build/lib/getargs.h"

__static_yoink("realloc");

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
#define PACKAGE_ABI   2

struct ObjectArrayParam {
  size_t len;
  size_t size;
  void *pp;
};

struct ObjectParam {
  size_t size;
  void *p;
  uint32_t *magic;
  int32_t *abi;
  struct ObjectArrayParam *arrays;
};

struct Packages {
  size_t i, n;
  struct Package {
    uint32_t magic;
    int32_t abi;
    uint32_t path;  // pkg->strings.p[path]
    int64_t fd;     // not persisted
    void *addr;     // not persisted
    ssize_t size;   // not persisted
    struct Strings {
      size_t i, n;
      char *p;  // persisted as pkg+RVA
    } strings;  // TODO(jart): interning?
    struct Objects {
      size_t i, n;
      struct Object {
        uint32_t path;           // pkg->strings.p[path]
        struct Elf64_Ehdr *elf;  // not persisted
        size_t size;             // not persisted
        char *strs;              // not persisted
        Elf64_Sym *syms;         // not persisted
        Elf64_Xword symcount;    // not persisted
        int section_offset;
        int section_count;
      } *p;
    } objects;
    struct Sections {
      size_t i, n;
      struct Section {
        int name;
        enum SectionKind {
          kUndef,
          kText,
          kPrivilegedText,
          kData,
          kBss,
          kOther,
        } kind;
      } *p;
    } sections;
    struct Symbols {
      size_t i, n;
      struct Symbol {
        uint32_t name;  // pkg->strings.p[name]
        enum SectionKind kind : 8;
        uint8_t bind_ : 4;
        uint8_t type : 4;
        uint16_t object;   // pkg->objects.p[object]
        uint16_t section;  // pkg->sections.p[section]
      } *p;                // persisted as pkg+RVA
    } symbols, undefs;     // TODO(jart): hash undefs?
  } **p;                   // persisted across multiple files
};

struct Relas {
  size_t i, n;
  struct Strings s;
  struct Rela {
    const char *symbol_name;
    const char *object_path;
  } *p;
} prtu;

static wontreturn void Die(const char *path, const char *reason) {
  tinyprint(2, path, ": ", reason, "\n", NULL);
  exit(1);
}

static wontreturn void SysExit(const char *path, const char *func) {
  const char *errstr;
  if (!(errstr = _strerrno(errno))) errstr = "EUNKNOWN";
  tinyprint(2, path, ": ", func, " failed with ", errstr, "\n", NULL);
  exit(1);
}

static int CompareSymbolName(const struct Symbol *a, const struct Symbol *b,
                             const char *tab) {
  return strcmp(tab + a->name, tab + b->name);
}

static void PrintSymbols(struct Package *pkg, struct Symbols *syms,
                         const char *name) {
  int i;
  kprintf("  - %s=%d\n", name, syms->i);
  for (i = 0; i < syms->i; ++i) {
    kprintf("    - id=%d\n", i);
    kprintf("      name=%d [%s]\n", syms->p[i].name,
            pkg->strings.p + syms->p[i].name);
    kprintf("      kind=%d\n", syms->p[i].kind);
    kprintf("      bind=%d\n", syms->p[i].bind_);
    kprintf("      type=%d\n", syms->p[i].type);
    kprintf("      object=%d [%s]\n", syms->p[i].object,
            pkg->strings.p + pkg->objects.p[syms->p[i].object].path);
    kprintf("      section=%d [%s]\n", syms->p[i].section,
            syms->p[i].section == SHN_ABS
                ? "SHN_ABS"
                : pkg->strings.p + pkg->sections.p[syms->p[i].section].name);
  }
}

static void PrintObject(struct Package *pkg, struct Object *obj) {
  int i, o;
  kprintf("    path=%d [%s]\n", obj->path, pkg->strings.p + obj->path);
  kprintf("    sections=%d\n", obj->section_count);
  for (i = 0; i < obj->section_count; ++i) {
    o = obj->section_offset;
    kprintf("    - id=%d %p (%d+%d)\n", i, pkg->sections.p, o, i);
    kprintf("      name=%d [%s]\n", pkg->sections.p[o + i].name,
            pkg->strings.p + pkg->sections.p[o + i].name);
    kprintf("      kind=%d\n", pkg->sections.p[o + i].kind);
  }
}

static void PrintPackage(struct Package *pkg) {
  int i;
  kprintf("- %s\n", pkg->strings.p + pkg->path);
  kprintf("  objects=%d\n", pkg->objects.i);
  for (i = 0; i < pkg->objects.i; ++i) {
    kprintf("  - id=%d\n", i);
    PrintObject(pkg, pkg->objects.p + i);
  }
  PrintSymbols(pkg, &pkg->symbols, "symbols");
  PrintSymbols(pkg, &pkg->undefs, "undefs");
}

static void PrintPackages(struct Package *p, int n) {
  int i;
  for (i = 0; i < n; ++i) {
    PrintPackage(p + i);
  }
}

static struct Package *LoadPackage(const char *path) {
  int fd;
  ssize_t size;
  struct Package *pkg;
  if ((fd = open(path, O_RDONLY)) == -1) {
    SysExit(path, "open");
  }
  if ((size = lseek(fd, 0, SEEK_END)) == -1) {
    SysExit(path, "lseek");
  }
  if ((pkg = mmap(0, size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0)) ==
      MAP_FAILED) {
    SysExit(path, "mmap");
  }
  close(fd);
  if (pkg->magic != PACKAGE_MAGIC) {
    Die(path, "not a cosmo .pkg file");
  }
  if (pkg->abi < PACKAGE_ABI) {
    Die(path, "package has old abi try running make clean");
  }
  pkg->strings.p = (void *)((uintptr_t)pkg->strings.p + (uintptr_t)pkg);
  pkg->objects.p = (void *)((uintptr_t)pkg->objects.p + (uintptr_t)pkg);
  pkg->symbols.p = (void *)((uintptr_t)pkg->symbols.p + (uintptr_t)pkg);
  pkg->sections.p = (void *)((uintptr_t)pkg->sections.p + (uintptr_t)pkg);
  pkg->addr = pkg;
  pkg->size = size;
  if (mprotect(pkg, size, PROT_READ)) {
    SysExit(path, "mprotect");
  }
  return pkg;
}

static void AddDependency(struct Packages *deps, const char *path) {
  struct Package *pkg;
  pkg = LoadPackage(path);
  append(deps, &pkg);
}

static void WritePackage(struct Package *pkg) {
  int fd;
  size_t n;
  int64_t o;
  const char *path;
  pkg->magic = PACKAGE_MAGIC;
  pkg->abi = PACKAGE_ABI;
  path = pkg->strings.p + pkg->path;
  if ((fd = creat(path, 0644)) == -1) {
    SysExit(path, "creat");
  }
  o = sizeof(*pkg);
  // write objects
  n = pkg->objects.i * sizeof(*pkg->objects.p);
  if (pwrite(fd, pkg->objects.p, n, o) != n) {
    SysExit(path, "pwrite");
  }
  pkg->objects.p = (void *)o;
  o += n;
  // write symbols
  n = pkg->symbols.i * sizeof(*pkg->symbols.p);
  if (pwrite(fd, pkg->symbols.p, n, o) != n) {
    SysExit(path, "pwrite");
  }
  pkg->symbols.p = (void *)o;
  o += n;
  // write sections
  n = pkg->sections.i * sizeof(*pkg->sections.p);
  if (pwrite(fd, pkg->sections.p, n, o) != n) {
    SysExit(path, "pwrite");
  }
  pkg->sections.p = (void *)o;
  o += n;
  // write strings
  n = pkg->strings.i * sizeof(*pkg->strings.p);
  pwrite(fd, pkg->strings.p, n, o);
  pkg->strings.p = (void *)o;
  // write header
  if (pwrite(fd, pkg, sizeof(*pkg), 0) != sizeof(*pkg)) {
    SysExit(path, "pwrite");
  }
  // we're done
  if (close(fd) == -1) {
    SysExit(path, "close");
  }
}

static wontreturn void PrintUsage(int fd, int exitcode) {
  tinyprint(fd, "\n\
NAME\n\
\n\
  Cosmopolitan Monorepo Packager\n\
\n\
SYNOPSIS\n\
\n\
  ",
            program_invocation_name, " [FLAGS] OBJECT...\n\
\n\
DESCRIPTION\n\
\n\
  This program verifies the well-formedness of symbolic references\n\
  and package dependencies in the cosmopolitan monolithic repository.\n\
  Validation happens incrementally and is granular to static libraries.\n\
  Each .a file should have its own .pkg file too, created by this tool.\n\
\n\
FLAGS\n\
\n\
  -h            show this help\n\
  -o PATH       package output path\n\
  -d PATH       package dependency path [repeatable]\n\
\n\
",
            NULL);
  exit(exitcode);
}

static void GetOpts(struct Package *pkg, struct Packages *deps, int argc,
                    char *argv[]) {
  long opt;
  const char *arg;
  struct GetArgs ga;
  pkg->path = -1;
  while ((opt = getopt(argc, argv, "ho:d:")) != -1) {
    switch (opt) {
      case 'o':
        pkg->path = concat(&pkg->strings, optarg, strlen(optarg) + 1);
        break;
      case 'd':
        AddDependency(deps, optarg);
        break;
      case 'h':
        PrintUsage(1, 0);
      default:
        PrintUsage(2, 1);
    }
  }
  if (pkg->path == -1) {
    tinyprint(2, "error: no packages passed to package.com\n", NULL);
    exit(1);
  }
  if (optind == argc) {
    tinyprint(
        2,
        "no objects passed to package.com; is your foo.mk $(FOO_OBJS) glob "
        "broken?\n",
        NULL);
    exit(1);
  }
  getargs_init(&ga, argv + optind);
  while ((arg = getargs_next(&ga))) {
    struct Object obj = {0};
    obj.path = concat(&pkg->strings, arg, strlen(arg) + 1);
    append(&pkg->objects, &obj);
  }
  getargs_destroy(&ga);
}

static void IndexSections(struct Package *pkg, struct Object *obj) {
  int i;
  const char *name;
  struct Section sect;
  const Elf64_Shdr *shdr;
  obj->section_offset = pkg->sections.i;
  for (i = 0; i < obj->elf->e_shnum; ++i) {
    bzero(&sect, sizeof(sect));
    if (!(shdr = GetElfSectionHeaderAddress(obj->elf, obj->size, i)) ||
        !(name = GetElfSectionName(obj->elf, obj->size, shdr))) {
      Die("error", "elf overflow");
    }
    if (shdr->sh_type == SHT_NULL) {
      sect.kind = kUndef;
    } else if (shdr->sh_type == SHT_NOBITS) {
      sect.kind = kBss;
    } else if (shdr->sh_type == SHT_PROGBITS &&
               !(shdr->sh_flags & SHF_EXECINSTR)) {
      sect.kind = kData;
    } else if (shdr->sh_type == SHT_PROGBITS &&
               (shdr->sh_flags & SHF_EXECINSTR)) {
      if (strcmp(name, ".privileged")) {
        sect.kind = kText;
      } else {
        sect.kind = kPrivilegedText;
      }
    } else {
      sect.kind = kOther;
    }
    sect.name = concat(&pkg->strings, name, strlen(name) + 1);
    append(&pkg->sections, &sect);
    ++obj->section_count;
  }
}

static enum SectionKind ClassifySection(struct Package *pkg, struct Object *obj,
                                        uint8_t type, Elf64_Section shndx) {
  if (shndx == SHN_ABS) return kOther;
  if (type == STT_COMMON) return kBss;
  return pkg->sections.p[obj->section_offset + shndx].kind;
}

static void LoadSymbols(struct Package *pkg, uint32_t object) {
  Elf64_Xword i;
  const char *name;
  struct Object *obj;
  struct Symbol symbol;
  obj = &pkg->objects.p[object];
  symbol.object = object;
  for (i = 0; i < obj->symcount; ++i) {
    symbol.section = obj->section_offset + obj->syms[i].st_shndx;
    symbol.bind_ = ELF64_ST_BIND(obj->syms[i].st_info);
    symbol.type = ELF64_ST_TYPE(obj->syms[i].st_info);
    if (symbol.bind_ != STB_LOCAL &&
        (symbol.type == STT_OBJECT || symbol.type == STT_FUNC ||
         symbol.type == STT_COMMON || symbol.type == STT_NOTYPE ||
         symbol.type == STT_GNU_IFUNC)) {
      if (!(name = GetElfString(obj->elf, obj->size, obj->strs,
                                obj->syms[i].st_name))) {
        Die("error", "elf overflow");
      }
      if (strcmp(name, "_GLOBAL_OFFSET_TABLE_")) {
        symbol.kind =
            ClassifySection(pkg, obj, symbol.type, obj->syms[i].st_shndx);
        symbol.name = concat(&pkg->strings, name, strlen(name) + 1);
        append(symbol.kind != kUndef ? &pkg->symbols : &pkg->undefs, &symbol);
      }
    }
  }
}

static Elf64_Shdr *FindElfSection(Elf64_Ehdr *elf, size_t esize,
                                  const char *name) {
  long i;
  Elf64_Shdr *shdr;
  const char *secname;
  for (i = 0; i < elf->e_shnum; ++i) {
    if ((secname = GetElfSectionName(
             elf, esize, (shdr = GetElfSectionHeaderAddress(elf, esize, i)))) &&
        !strcmp(secname, name)) {
      return shdr;
    }
  }
  return 0;
}

static void LoadPriviligedRefsToUndefs(struct Package *pkg,
                                       struct Object *obj) {
  long x;
  struct Rela r;
  const char *s;
  Elf64_Shdr *shdr;
  Elf64_Rela *rela, *erela;
  if ((shdr = FindElfSection(obj->elf, obj->size, ".rela.privileged"))) {
    if (!(rela = GetElfSectionAddress(obj->elf, obj->size, shdr))) {
      Die("error", "elf overflow");
    }
    erela = rela + shdr->sh_size / sizeof(*rela);
    for (; rela < erela; ++rela) {
      if (!ELF64_R_TYPE(rela->r_info)) continue;
      if (!(x = ELF64_R_SYM(rela->r_info))) continue;
      if (x > obj->symcount) Die("error", "elf overflow");
      if (obj->syms[x].st_shndx) continue;  // symbol is defined
      if (ELF64_ST_BIND(obj->syms[x].st_info) != STB_WEAK &&
          ELF64_ST_BIND(obj->syms[x].st_info) != STB_GLOBAL) {
        tinyprint(2, "warning: undefined symbol not global\n", NULL);
        continue;
      }
      if (!(s = GetElfString(obj->elf, obj->size, obj->strs,
                             obj->syms[x].st_name))) {
        Die("error", "elf overflow");
      }
      r.symbol_name = strdup(s);
      r.object_path = strdup(pkg->strings.p + obj->path);
      append(&prtu, &r);
    }
  }
}

static void OpenObject(struct Package *pkg, struct Object *obj, int oid) {
  int fd;
  const char *path;
  path = pkg->strings.p + obj->path;
  if ((fd = open(path, O_RDONLY)) == -1) {
    SysExit(path, "open");
  }
  if ((obj->size = lseek(fd, 0, SEEK_END)) == -1) {
    SysExit(path, "lseek");
  }
  if ((obj->elf = mmap(0, obj->size, PROT_READ, MAP_SHARED, fd, 0)) ==
      MAP_FAILED) {
    SysExit(path, "mmap");
  }
  close(fd);
  if (!IsElf64Binary(obj->elf, obj->size)) {
    Die(path, "not an elf64 binary");
  }
  if (!(obj->strs = GetElfStringTable(obj->elf, obj->size, ".strtab"))) {
    Die(path, "missing elf string table");
  }
  if (!(obj->syms =
            GetElfSymbols(obj->elf, obj->size, SHT_SYMTAB, &obj->symcount))) {
    Die(path, "missing elf symbol table");
  }
  IndexSections(pkg, obj);
  LoadPriviligedRefsToUndefs(pkg, obj);
}

static void CloseObject(struct Object *obj) {
  if (munmap(obj->elf, obj->size)) notpossible;
}

static void LoadObjects(struct Package *pkg) {
  size_t i;
  struct Object *obj;
  for (i = 0; i < pkg->objects.i; ++i) {
    obj = pkg->objects.p + i;
    OpenObject(pkg, obj, i);
    LoadSymbols(pkg, i);
    CloseObject(obj);
  }
  qsort_r(pkg->symbols.p, pkg->symbols.i, sizeof(*pkg->symbols.p),
          (void *)CompareSymbolName, pkg->strings.p);
}

static struct Symbol *BisectSymbol(struct Package *pkg, const char *name) {
  int c;
  long m, l, r;
  l = 0;
  r = pkg->symbols.i - 1;
  while (l <= r) {
    m = (l & r) + ((l ^ r) >> 1);  // floor((a+b)/2)
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

static bool FindSymbol(const char *name, struct Package *pkg,
                       struct Packages *directdeps, struct Package **out_pkg,
                       struct Symbol **out_sym) {
  size_t i;
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

static void CheckStrictDeps(struct Package *pkg, struct Packages *deps) {
  size_t i, j;
  struct Package *dep;
  struct Symbol *undef;
  for (i = 0; i < pkg->undefs.i; ++i) {
    undef = &pkg->undefs.p[i];
    if (undef->bind_ == STB_WEAK) continue;
    if (!FindSymbol(pkg->strings.p + undef->name, pkg, deps, NULL, NULL)) {
      tinyprint(2, pkg->strings.p + pkg->path, ": undefined symbol '",
                pkg->strings.p + undef->name, "' (",
                pkg->strings.p + pkg->objects.p[undef->object].path,
                ") not defined by direct dependencies:\n", NULL);
      for (j = 0; j < deps->i; ++j) {
        dep = deps->p[j];
        tinyprint(2, "\t", dep->strings.p + dep->path, "\n", NULL);
      }
      exit(1);
    }
  }
  free(pkg->undefs.p);
  bzero(&pkg->undefs, sizeof(pkg->undefs));
}

static void CheckYourPrivilege(struct Package *pkg, struct Packages *deps) {
  int i, f;
  const char *name;
  struct Symbol *sym;
  struct Package *dep;
  for (f = i = 0; i < prtu.i; ++i) {
    name = prtu.p[i].symbol_name;
    if (FindSymbol(name, pkg, deps, &dep, &sym) &&
        dep->sections.p[sym->section].kind == kText) {
      tinyprint(2, prtu.p[i].object_path,
                ": privileged code referenced unprivileged symbol '", name,
                "' in section '",
                dep->strings.p + dep->sections.p[sym->section].name, "'\n",
                NULL);
      ++f;
    }
  }
  if (f) exit(1);
}

static bool IsSymbolDirectlyReachable(struct Package *pkg,
                                      struct Packages *deps,
                                      const char *symbol) {
  return FindSymbol(symbol, pkg, deps, 0, 0);
}

static void Package(int argc, char *argv[], struct Package *pkg,
                    struct Packages *deps) {
  size_t i;
  GetOpts(pkg, deps, argc, argv);
  LoadObjects(pkg);
  CheckStrictDeps(pkg, deps);
  CheckYourPrivilege(pkg, deps);
  WritePackage(pkg);
  for (i = 0; i < deps->i; ++i) {
    if (munmap(deps->p[i]->addr, deps->p[i]->size)) notpossible;
  }
}

int main(int argc, char *argv[]) {
  struct Package pkg;
  struct Packages deps;
  if (argc == 2 && !strcmp(argv[1], "-n")) {
    exit(0);
  }
#ifdef MODE_DBG
  ShowCrashReports();
#endif
  bzero(&pkg, sizeof(pkg));
  bzero(&deps, sizeof(deps));
  Package(argc, argv, &pkg, &deps);
  return 0;
}
