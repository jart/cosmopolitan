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
#include "libc/alg/bisect.internal.h"
#include "libc/alg/bisectcarleft.internal.h"
#include "libc/assert.h"
#include "libc/bits/bswap.h"
#include "libc/bits/safemacros.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/elf/def.h"
#include "libc/elf/elf.h"
#include "libc/elf/struct/rela.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/macros.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/bsr.h"
#include "libc/nexgen32e/kompressor.h"
#include "libc/nt/enum/fileflagandattributes.h"
#include "libc/runtime/gc.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/sock.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/msync.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/time/time.h"
#include "libc/x/x.h"
#include "third_party/getopt/getopt.h"
#include "third_party/xed/x86.h"
#include "third_party/zlib/zlib.h"
#include "tool/build/lib/elfwriter.h"
#include "tool/build/lib/persist.h"

/**
 * @fileoverview Build Package Script.
 *
 * FIRST PURPOSE
 *
 * This script verifies the well-formedness of dependencies, e.g.
 *
 *   o/tool/build/package.com \
 *     -o o/libc/stubs/stubs.pkg \
 *     o/libc/stubs/{a,b,...}.o
 *
 *   o/tool/build/package.com \
 *     -o o/libc/nexgen32e/nexgen32e.pkg \
 *     -d o/libc/stubs/stubs.pkg \
 *     o/libc/nexgen32e/{a,b,...}.o
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
 *
 * SECOND PURPOSE
 *
 * We want all storage to be thread-local storage. So we change
 * RIP-relative instructions to be RBX-relative, only when they
 * reference sections in the binary mutable after initialization.
 *
 * This is basically what the Go language does to implement its fiber
 * multiprocessing model. We can have this in C by appropriating all the
 * work folks put into enriching GNU C with WIN32 and ASLR lool.
 *
 * THIRD PURPOSE
 *
 * Compress read-only data sections of particularly low entropy, using
 * the most appropriate directly-linked algorithm and then inject code
 * into _init() that calls it. If the data is extremely low energy, we
 * will inject code for merging page table entries too. The overcommit
 * here is limitless.
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
            struct Ops {
              size_t i, n;
              struct Op {
                int32_t offset;
                uint8_t length;
                uint8_t pos_disp;
                uint16_t __pad;
              } * p;
            } ops;
          } * p;
        } sections;  // not persisted
      } * p;         // persisted as pkg+RVA
    } objects;
    struct Symbols {
      size_t i, n;
      struct Symbol {
        uint32_t name;  // pkg->strings.p[name]
        enum SectionKind kind : 8;
        uint8_t bind : 4;
        uint8_t type : 4;
        uint16_t object;  // pkg->objects.p[object]
      } * p;              // persisted as pkg+RVA
    } symbols, undefs;    // TODO(jart): hash undefs?
  } * *p;                 // persisted across multiple files
};

int CompareSymbolName(const struct Symbol *a, const struct Symbol *b,
                      const char *strs[hasatleast 2]) {
  return strcmp(&strs[0][a->name], &strs[1][b->name]);
}

struct Package *LoadPackage(const char *path) {
  int fd;
  ssize_t i;
  struct stat st;
  struct Package *pkg;
  CHECK(fileexists(path), "%s: %s: %s\n", "error", path, "not found");
  CHECK_NE(-1, (fd = open(path, O_RDONLY)), "%s", path);
  CHECK_NE(-1, fstat(fd, &st));
  CHECK_NE(MAP_FAILED, (pkg = mmap(NULL, st.st_size, PROT_READ | PROT_WRITE,
                                   MAP_PRIVATE, fd, 0)));
  CHECK_NE(-1, close(fd));
  CHECK_EQ(PACKAGE_MAGIC, pkg->magic, "corrupt package: %`'s", path);
  pkg->strings.p = (char *)((intptr_t)pkg->strings.p + (intptr_t)pkg);
  pkg->objects.p = (struct Object *)((intptr_t)pkg->objects.p + (intptr_t)pkg);
  pkg->symbols.p = (struct Symbol *)((intptr_t)pkg->symbols.p + (intptr_t)pkg);
  CHECK(strcmp(path, &pkg->strings.p[pkg->path]) == 0,
        "corrupt package: %`'s pkg=%p strings=%p", path, pkg, pkg->strings.p);
  pkg->addr = pkg;
  pkg->size = st.st_size;
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
  pkg->path = -1;
  while ((opt = getopt(argc, argv, "vho:d:")) != -1) {
    switch (opt) {
      case 'v':
        g_loglevel = kLogDebug;
        break;
      case 'o':
        pkg->path = concat(&pkg->strings, optarg, strlen(optarg) + 1);
        break;
      case 'd':
        AddDependency(deps, optarg);
        break;
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
  for (i = optind; i < argc; ++i) {
    CHECK_NE(-1, (si = concat(&pkg->strings, argv[i], strlen(argv[i]) + 1)));
    CHECK_NE(-1, append(&pkg->objects, (&(struct Object){si})));
  }
}

void IndexSections(struct Object *obj) {
  size_t i;
  struct Op op;
  const char *name;
  const uint8_t *code;
  struct Section sect;
  const Elf64_Shdr *shdr;
  struct XedDecodedInst xedd;
  for (i = 0; i < obj->elf->e_shnum; ++i) {
    memset(&sect, 0, sizeof(sect));
    CHECK_NOTNULL((shdr = GetElfSectionHeaderAddress(obj->elf, obj->size, i)));
    if (shdr->sh_type != SHT_NULL) {
      CHECK_NOTNULL((name = GetElfSectionName(obj->elf, obj->size, shdr)));
      if (startswith(name, ".sort.")) name += 5;
      if ((strcmp(name, ".piro.relo") == 0 ||
           startswith(name, ".piro.relo.")) ||
          (strcmp(name, ".data.rel.ro") == 0 ||
           startswith(name, ".data.rel.ro."))) {
        sect.kind = kPiroRelo;
      } else if (strcmp(name, ".piro.data") == 0 ||
                 startswith(name, ".piro.data.")) {
        sect.kind = kPiroData;
      } else if (strcmp(name, ".piro.bss") == 0 ||
                 startswith(name, ".piro.bss.")) {
        sect.kind = kPiroBss;
      } else if (strcmp(name, ".data") == 0 || startswith(name, ".data.")) {
        sect.kind = kData;
      } else if (strcmp(name, ".bss") == 0 || startswith(name, ".bss.")) {
        sect.kind = kBss;
      } else {
        sect.kind = kText;
      }
    } else {
      sect.kind = kUndef; /* should always and only be section #0 */
    }
    if (shdr->sh_flags & SHF_EXECINSTR) {
      CHECK_NOTNULL((code = GetElfSectionAddress(obj->elf, obj->size, shdr)));
      for (op.offset = 0; op.offset < shdr->sh_size; op.offset += op.length) {
        if (xed_instruction_length_decode(
                xed_decoded_inst_zero_set_mode(&xedd, XED_MACHINE_MODE_LONG_64),
                &code[op.offset],
                min(shdr->sh_size - op.offset, XED_MAX_INSTRUCTION_BYTES)) ==
            XED_ERROR_NONE) {
          op.length = xedd.length;
          op.pos_disp = xedd.op.pos_disp;
        } else {
          op.length = 1;
          op.pos_disp = 0;
        }
        CHECK_NE(-1, append(&sect.ops, &op));
      }
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
    symbol.bind = ELF64_ST_BIND(obj->syms[i].st_info);
    symbol.type = ELF64_ST_TYPE(obj->syms[i].st_info);
    if (symbol.bind != STB_LOCAL &&
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
  CHECK_NE(MAP_FAILED, (obj->elf = mmap(NULL, (obj->size = st.st_size), prot,
                                        flags, fd, 0)));
  CHECK_NE(-1, close(fd));
  CHECK(IsElf64Binary(obj->elf, obj->size), "path=%`'s",
        &pkg->strings.p[obj->path]);
  CHECK_NOTNULL((obj->strs = GetElfStringTable(obj->elf, obj->size)));
  CHECK_NOTNULL(
      (obj->syms = GetElfSymbolTable(obj->elf, obj->size, &obj->symcount)));
  CHECK_NE(0, obj->symcount);
  IndexSections(obj);
}

void CloseObject(struct Object *obj) {
  if ((obj->mode & O_ACCMODE) != O_RDONLY) {
    CHECK_NE(-1, msync(obj->elf, obj->size, MS_ASYNC | MS_INVALIDATE));
  }
  CHECK_NE(-1, munmap(obj->elf, obj->size));
}

void LoadObjects(struct Package *pkg) {
  size_t i;
  struct Object *obj;
  for (i = 0; i < pkg->objects.i; ++i) {
    obj = &pkg->objects.p[i];
    OpenObject(pkg, obj, O_RDONLY, PROT_READ, MAP_SHARED);
    LoadSymbols(pkg, i);
    CloseObject(obj);
  }
  qsort_r(&pkg->symbols.p[0], pkg->symbols.i, sizeof(pkg->symbols.p[0]),
          (void *)CompareSymbolName,
          (const char *[2]){pkg->strings.p, pkg->strings.p});
}

bool FindSymbol(const char *name, struct Package *pkg,
                struct Packages *directdeps, struct Package **out_pkg,
                struct Symbol **out_sym) {
  size_t i;
  struct Package *dep;
  struct Symbol key, *sym;
  key.name = 0;
  if ((sym = bisect(&key, &pkg->symbols.p[0], pkg->symbols.i,
                    sizeof(pkg->symbols.p[0]), (void *)CompareSymbolName,
                    (const char *[2]){name, pkg->strings.p}))) {
    if (out_pkg) *out_pkg = pkg;
    if (out_sym) *out_sym = sym;
    return true;
  }
  for (i = 0; i < directdeps->i; ++i) {
    dep = directdeps->p[i];
    if ((sym = bisect(&key, &dep->symbols.p[0], dep->symbols.i,
                      sizeof(dep->symbols.p[0]), (void *)CompareSymbolName,
                      (const char *[2]){name, dep->strings.p}))) {
      if (out_pkg) *out_pkg = dep;
      if (out_sym) *out_sym = sym;
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
    if (undef->bind == STB_WEAK) continue;
    if (!FindSymbol(&pkg->strings.p[undef->name], pkg, deps, NULL, NULL)) {
      fprintf(stderr, "%s: %s (%s) %s %s\n", "error",
              &pkg->strings.p[undef->name],
              &pkg->strings.p[pkg->objects.p[undef->object].path],
              "not defined by direct deps of", &pkg->strings.p[pkg->path]);
      for (j = 0; j < deps->i; ++j) {
        dep = deps->p[j];
        fputc('\t', stderr);
        fputs(&dep->strings.p[dep->path], stderr);
        fputc('\n', stderr);
      }
      exit(1);
    }
  }
  free(pkg->undefs.p);
  memset(&pkg->undefs, 0, sizeof(pkg->undefs));
}

forceinline bool IsRipRelativeModrm(uint8_t modrm) {
  return (modrm & 0b11000111) == 0b00000101;
}

forceinline uint8_t ChangeRipToRbx(uint8_t modrm) {
  return (modrm & 0b00111000) | 0b10000011;
}

void OptimizeRelocations(struct Package *pkg, struct Packages *deps,
                         struct Object *obj) {
  Elf64_Half i;
  struct Op *op;
  Elf64_Rela *rela;
  struct Symbol *refsym;
  struct Package *refpkg;
  unsigned char *code, *p;
  Elf64_Shdr *shdr, *shdrcode;
  for (i = 0; i < obj->elf->e_shnum; ++i) {
    shdr = GetElfSectionHeaderAddress(obj->elf, obj->size, i);
    if (shdr->sh_type == SHT_RELA) {
      CHECK_EQ(sizeof(struct Elf64_Rela), shdr->sh_entsize);
      CHECK_NOTNULL((shdrcode = GetElfSectionHeaderAddress(obj->elf, obj->size,
                                                           shdr->sh_info)));
      if (!(shdrcode->sh_flags & SHF_EXECINSTR)) continue;
      CHECK_NOTNULL(
          (code = GetElfSectionAddress(obj->elf, obj->size, shdrcode)));
      for (rela = GetElfSectionAddress(obj->elf, obj->size, shdr);
           ((uintptr_t)rela + shdr->sh_entsize <=
            min((uintptr_t)obj->elf + obj->size,
                (uintptr_t)obj->elf + shdr->sh_offset + shdr->sh_size));
           ++rela) {
        CHECK_LT(ELF64_R_SYM(rela->r_info), obj->symcount);

#if 0
        /*
         * Change (%rip) to (%rbx) on program instructions that
         * reference memory, if and only if the memory location is a
         * global variable that's mutable after initialization. The
         * displacement is also updated to be relative to the image
         * base, rather than relative to the program counter.
         */
        if ((ELF64_R_TYPE(rela->r_info) == R_X86_64_PC32 ||
             ELF64_R_TYPE(rela->r_info) == R_X86_64_GOTPCREL) &&
            FindSymbol(
                GetElfString(obj->elf, obj->size, obj->strs,
                             obj->syms[ELF64_R_SYM(rela->r_info)].st_name),
                pkg, deps, &refpkg, &refsym) &&
            (refsym->kind == kData || refsym->kind == kBss) &&
            IsRipRelativeModrm(code[rela->r_offset - 1])) {
          op = &obj->sections.p[shdr->sh_info].ops.p[bisectcarleft(
              (const int32_t(*)[2])obj->sections.p[shdr->sh_info].ops.p,
              obj->sections.p[shdr->sh_info].ops.i, rela->r_offset)];
          CHECK_GT(op->length, 4);
          CHECK_GT(op->pos_disp, 0);
          rela->r_info = ELF64_R_INFO(ELF64_R_SYM(rela->r_info), R_X86_64_32S);
          rela->r_addend = -IMAGE_BASE_VIRTUAL + rela->r_addend +
                           (op->length - op->pos_disp);
          code[rela->r_offset - 1] = ChangeRipToRbx(code[rela->r_offset - 1]);
        }
#endif

        /*
         * GCC isn't capable of -mnop-mcount when using -fpie.
         * Let's fix that. It saves ~14 cycles per function call.
         * Then libc/runtime/ftrace.greg.c morphs it back at runtime.
         */
        if (ELF64_R_TYPE(rela->r_info) == R_X86_64_GOTPCRELX &&
            strcmp(GetElfString(obj->elf, obj->size, obj->strs,
                                obj->syms[ELF64_R_SYM(rela->r_info)].st_name),
                   "mcount") == 0) {
          rela->r_info = R_X86_64_NONE;
          p = &code[rela->r_offset - 2];
          p[0] = 0x66; /* nopw 0x00(%rax,%rax,1) */
          p[1] = 0x0f;
          p[2] = 0x1f;
          p[3] = 0x44;
          p[4] = 0x00;
          p[5] = 0x00;
        }

        /*
         * Let's just try to nop mcount calls in general due to the above.
         */
        if ((ELF64_R_TYPE(rela->r_info) == R_X86_64_PC32 ||
             ELF64_R_TYPE(rela->r_info) == R_X86_64_PLT32) &&
            strcmp(GetElfString(obj->elf, obj->size, obj->strs,
                                obj->syms[ELF64_R_SYM(rela->r_info)].st_name),
                   "mcount") == 0) {
          rela->r_info = R_X86_64_NONE;
          p = &code[rela->r_offset - 1];
          p[0] = 0x0f; /* nopl 0x00(%rax,%rax,1) */
          p[1] = 0x1f;
          p[2] = 0x44;
          p[3] = 0x00;
          p[4] = 0x00;
        }
      }
    }
  }
}

bool IsSymbolDirectlyReachable(struct Package *pkg, struct Packages *deps,
                               const char *symbol) {
  return FindSymbol(symbol, pkg, deps, NULL, NULL);
}

struct RlEncoder {
  size_t i, n;
  struct RlDecode *p;
};

ssize_t rlencode_extend(struct RlEncoder *rle, size_t n) {
  size_t n2;
  struct RlDecode *p2;
  n2 = rle->n;
  if (!n2) n2 = 512;
  while (n > n2) n2 += n2 >> 1;
  if (!(p2 = realloc(rle->p, n2 * sizeof(rle->p[0])))) return -1;
  rle->p = p2;
  rle->n = n2;
  return n2;
}

void rlencode_encode(struct RlEncoder *rle, const unsigned char *data,
                     size_t size) {
  size_t i, j;
  for (i = 0; i < size; i += j) {
    for (j = 1; j < 255 && i + j < size; ++j) {
      if (data[i] != data[i + j]) break;
    }
    rle->p[rle->i].repititions = j;
    rle->p[rle->i].byte = data[i];
    rle->i++;
  }
  rle->p[rle->i].repititions = 0;
  rle->p[rle->i].byte = 0;
  rle->i++;
}

ssize_t rlencode(struct RlEncoder *rle, const unsigned char *data,
                 size_t size) {
  if (size + 1 > rle->n && rlencode_extend(rle, size + 1) == -1) return -1;
  rlencode_encode(rle, data, size);
  assert(rle->i <= rle->n);
  return rle->i;
}

void CompressLowEntropyReadOnlyDataSections(struct Package *pkg,
                                            struct Packages *deps,
                                            struct Object *obj) {
  Elf64_Half i;
  const char *name;
  unsigned char *p;
  Elf64_Shdr *shdr;
  struct RlEncoder rle;
  bool haverldecode, isprofitable;
  memset(&rle, 0, sizeof(rle));
  haverldecode = IsSymbolDirectlyReachable(pkg, deps, "rldecode");
  for (i = 0; i < obj->elf->e_shnum; ++i) {
    if ((shdr = GetElfSectionHeaderAddress(obj->elf, obj->size, i)) &&
        shdr->sh_size >= 256 &&
        (shdr->sh_type == SHT_PROGBITS &&
         !(shdr->sh_flags &
           (SHF_WRITE | SHF_MERGE | SHF_STRINGS | SHF_COMPRESSED))) &&
        (p = GetElfSectionAddress(obj->elf, obj->size, shdr)) &&
        startswith((name = GetElfSectionName(obj->elf, obj->size, shdr)),
                   ".rodata") &&
        rlencode(&rle, p, shdr->sh_size) != -1) {
      isprofitable = rle.i * sizeof(rle.p[0]) <= shdr->sh_size / 2;
      LOGF("%s(%s): rlencode()%s on %s is%s profitable (%,zu → %,zu bytes)",
           &pkg->strings.p[pkg->path], &pkg->strings.p[obj->path],
           haverldecode ? "" : " [NOT LINKED]", name,
           isprofitable ? "" : " NOT", shdr->sh_size, rle.i * sizeof(rle.p[0]));
    }
  }
  free(rle.p);
}

void RewriteObjects(struct Package *pkg, struct Packages *deps) {
  size_t i;
  struct Object *obj;
#if 0
  struct ElfWriter *elf;
  elf = elfwriter_open(gc(xstrcat(&pkg->strings.p[pkg->path], ".o")), 0644);
  elfwriter_cargoculting(elf);
#endif
  for (i = 0; i < pkg->objects.i; ++i) {
    obj = &pkg->objects.p[i];
    OpenObject(pkg, obj, O_RDWR, PROT_READ | PROT_WRITE, MAP_SHARED);
    OptimizeRelocations(pkg, deps, obj);
#if 0
    CompressLowEntropyReadOnlyDataSections(pkg, deps, obj);
#endif
    CloseObject(obj);
  }
#if 0
  elfwriter_close(elf);
#endif
}

void Package(int argc, char *argv[], struct Package *pkg,
             struct Packages *deps) {
  size_t i, j;
  GetOpts(pkg, deps, argc, argv);
  LoadObjects(pkg);
  CheckStrictDeps(pkg, deps);
  RewriteObjects(pkg, deps);
  WritePackage(pkg);
  for (i = 0; i < deps->i; ++i) {
    CHECK_NE(-1, munmap(deps->p[i]->addr, deps->p[i]->size));
  }
  for (i = 0; i < pkg->objects.i; ++i) {
    for (j = 0; j < pkg->objects.p[i].sections.i; ++j) {
      free(pkg->objects.p[i].sections.p[j].ops.p);
    }
    free(pkg->objects.p[i].sections.p);
  }
  free_s(&pkg->strings.p);
  free_s(&pkg->objects.p);
  free_s(&pkg->symbols.p);
  free_s(&deps->p);
}

int main(int argc, char *argv[]) {
  struct Package pkg;
  struct Packages deps;
  showcrashreports();
  memset(&pkg, 0, sizeof(pkg));
  memset(&deps, 0, sizeof(deps));
  Package(argc, argv, &pkg, &deps);
  return 0;
}
