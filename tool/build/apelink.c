/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "ape/ape.h"
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/dce.h"
#include "libc/dos.internal.h"
#include "libc/elf/def.h"
#include "libc/elf/elf.h"
#include "libc/elf/scalar.h"
#include "libc/elf/struct/ehdr.h"
#include "libc/elf/struct/phdr.h"
#include "libc/fmt/conv.h"
#include "libc/fmt/itoa.h"
#include "libc/limits.h"
#include "libc/macho.internal.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/nt/pedef.internal.h"
#include "libc/nt/struct/imageimportbyname.internal.h"
#include "libc/nt/struct/imageimportdescriptor.internal.h"
#include "libc/nt/struct/imagentheaders.internal.h"
#include "libc/nt/struct/imagesectionheader.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/symbols.internal.h"
#include "libc/serialize.h"
#include "libc/stdalign.internal.h"
#include "libc/stdckdint.h"
#include "libc/stdio/stdio.h"
#include "libc/str/blake2.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/zip.internal.h"
#include "third_party/getopt/getopt.internal.h"
#include "third_party/zlib/zlib.h"
#include "tool/build/lib/lib.h"

#define VERSION                     \
  "apelink v0.1\n"                  \
  "copyright 2023 justine tunney\n" \
  "https://github.com/jart/cosmopolitan\n"

#define MANUAL                                                 \
  " -o OUTPUT INPUT...\n"                                      \
  "\n"                                                         \
  "DESCRIPTION\n"                                              \
  "\n"                                                         \
  "  actually portable executable linker\n"                    \
  "\n"                                                         \
  "  this program may be used to turn elf executables into\n"  \
  "  ape executables. it's useful for creating fat binaries\n" \
  "  that run on multiple architectures.\n"                    \
  "\n"                                                         \
  "FLAGS\n"                                                    \
  "\n"                                                         \
  "  -h         show usage\n"                                  \
  "\n"                                                         \
  "  -v         show version\n"                                \
  "\n"                                                         \
  "  -o OUTPUT  set output path\n"                             \
  "\n"                                                         \
  "  -s         never embed symbol table\n"                    \
  "\n"                                                         \
  "  -l PATH    bundle ape loader executable [repeatable]\n"   \
  "             if no ape loaders are specified then your\n"   \
  "             executable will self-modify its header on\n"   \
  "             the first run, to use the platform format\n"   \
  "\n"                                                         \
  "  -M PATH    bundle ape loader source code file for m1\n"   \
  "             processors running the xnu kernel so that\n"   \
  "             it can be compiled on the fly by xcode\n"      \
  "\n"                                                         \
  "  -V BITS    set OS support vector\n"                       \
  "\n"                                                         \
  "             the default value is -1 which sets the bits\n" \
  "             for all supported operating systems to true\n" \
  "             of which the following are defined:\n"         \
  "\n"                                                         \
  "               -  1: linux\n"                               \
  "               -  2: metal\n"                               \
  "               -  4: windows\n"                             \
  "               -  8: xnu\n"                                 \
  "               - 16: openbsd\n"                             \
  "               - 32: freebsd\n"                             \
  "               - 64: netbsd\n"                              \
  "\n"                                                         \
  "             for example, `-s 0b1110001` may be used to\n"  \
  "             produce ELF binaries that only support the\n"  \
  "             truly open unix systems. in this case when\n"  \
  "             a single input executable is supplied, the\n"  \
  "             output file will end up being portable elf\n"  \
  "\n"                                                         \
  "             when the default of -1 is supplied, certain\n" \
  "             operating systems can have support detected\n" \
  "             by examining your symbols and sections. for\n" \
  "             example, xnu is detected by looking for the\n" \
  "             _apple() entrypoint which is defined on elf\n" \
  "             platforms as being e_entry. if only the XNU\n" \
  "             platform is supported then e_entry is used.\n" \
  "\n"                                                         \
  "             in addition to accepting numbers, you could\n" \
  "             also pass strings in a variety of intuitive\n" \
  "             supported representations. for example, bsd\n" \
  "             will enable freebsd+netbsd+openbsd and that\n" \
  "             string too is a legal input. the -s flag is\n" \
  "             also repeatable, e.g. `-s nt -s xnu` to use\n" \
  "             the union of the two.\n"                       \
  "\n"                                                         \
  "             since the support vector controls the file\n"  \
  "             format that's outputted too you can supply\n"  \
  "             the keywords `pe`, `elf`, & 'macho' to use\n"  \
  "             a support vector targetting whichever OSes\n"  \
  "             use it as a native format.\n"                  \
  "\n"                                                         \
  "  -g         generate a debugging friendly ape binary\n"    \
  "\n"                                                         \
  "  -G         don't $PATH lookup systemwide ape loader\n"    \
  "\n"                                                         \
  "  -S CODE    add custom code to start of shell script\n"    \
  "             for example you can use `-S 'set -ex' to\n"    \
  "             troubleshoot any issues with your script\n"    \
  "\n"                                                         \
  "  -B         force bypassing of any binfmt_misc loader\n"   \
  "             by using alternative 'APEDBG=' file magic\n"   \
  "\n"                                                         \
  "ARGUMENTS\n"                                                \
  "\n"                                                         \
  "  OUTPUT     is your ape executable\n"                      \
  "  INPUT      specifies multiple ELF builds of the same\n"   \
  "             program, for different architectures that\n"   \
  "             shall be merged into a single output file\n"   \
  "\n"

#define ALIGN(p, a) (char *)ROUNDUP((uintptr_t)(p), (a))

enum Strategy {
  kElf,
  kApe,
  kMacho,
  kPe,
};

struct MachoLoadThread64 {
  struct MachoLoadThreadCommand thread;
  uint64_t regs[21];
};

struct OffsetRelocs {
  int n;
  uint64_t *p[64];
};

struct Input {
  union {
    char *map;
    Elf64_Ehdr *elf;
    unsigned char *umap;
  };
  size_t size;
  const char *path;
  Elf64_Off machoff;
  Elf64_Off minload;
  Elf64_Off maxload;
  char *printf_phoff;
  char *printf_phnum;
  char *ddarg_macho_skip;
  char *ddarg_macho_count;
  int pe_offset;
  int pe_e_lfanew;
  int pe_SizeOfHeaders;
  int size_of_pe_headers;
  bool we_are_generating_pe;
  bool we_must_skew_pe_vaspace;
  struct NtImageNtHeaders *pe;
  struct OffsetRelocs offsetrelocs;
  struct MachoLoadSegment *first_macho_load;
};

struct Inputs {
  int n;
  struct Input p[16];
};

struct Loader {
  int os;
  int machine;
  bool used;
  void *map;
  size_t size;
  const char *path;
  int macho_offset;
  int macho_length;
  char *ddarg_skip1;
  char *ddarg_size1;
  char *ddarg_skip2;
  char *ddarg_size2;
};

struct Loaders {
  int n;
  struct Loader p[16];
};

struct Asset {
  unsigned char *cfile;
  unsigned char *lfile;
};

struct Assets {
  int n;
  struct Asset *p;
  size_t total_centraldir_bytes;
  size_t total_local_file_bytes;
};

static int outfd;
static int hashes;
static const char *prog;
static bool want_stripped;
static int support_vector;
static int macholoadcount;
static const char *outpath;
static struct Assets assets;
static struct Inputs inputs;
static char ape_heredoc[15];
static enum Strategy strategy;
static struct Loaders loaders;
static const char *custom_sh_code;
static bool force_bypass_binfmt_misc;
static bool generate_debuggable_binary;
static bool dont_path_lookup_ape_loader;
_Alignas(4096) static char prologue[1048576];
static uint8_t hashpool[BLAKE2B256_DIGEST_LENGTH];
static const char *macos_silicon_loader_source_path;
static const char *macos_silicon_loader_source_text;
static char *macos_silicon_loader_source_ddarg_skip;
static char *macos_silicon_loader_source_ddarg_size;

static Elf64_Off noteoff;
static Elf64_Xword notesize;

static char *r_off32_e_lfanew;

static wontreturn void Die(const char *thing, const char *reason) {
  tinyprint(2, thing, ": ", reason, "\n", NULL);
  exit(1);
}

static wontreturn void DieSys(const char *thing) {
  perror(thing);
  exit(1);
}

static wontreturn void ShowUsage(int rc, int fd) {
  tinyprint(fd, "USAGE\n\n  ", prog, MANUAL, NULL);
  exit(rc);
}

static wontreturn void DieOom(void) {
  Die("apelink", "out of memory");
}

static void *Malloc(size_t n) {
  void *p;
  if (!(p = malloc(n))) DieOom();
  return p;
}

static void *Realloc(void *p, size_t n) {
  if (!(p = realloc(p, n))) DieOom();
  return p;
}

// By convention, Cosmopolitan's GetSymbolTable() function will inspect
// its own executable image to see if it's also a zip archive, in which
// case it tries to load the symbol table from an architecture-specific
// filename below, and falls back to the ".symtab" when it isn't found.
// the file format for these symbol files is unique to cosmopiltan libc
static const char *ConvertElfMachineToSymtabName(Elf64_Ehdr *e) {
  switch (e->e_machine) {
    case EM_NEXGEN32E:
      return ".symtab.amd64";
    case EM_AARCH64:
      return ".symtab.arm64";
    case EM_PPC64:
      return ".symtab.ppc64";
    case EM_S390:
      return ".symtab.s390x";
    case EM_RISCV:
      return ".symtab.riscv";
    default:
      Die(prog, "unsupported architecture");
  }
}

static const char *DescribePhdrType(Elf64_Word p_type) {
  static char buf[12];
  switch (p_type) {
    case PT_LOAD:
      return "LOAD";
    case PT_TLS:
      return "TLS";
    case PT_PHDR:
      return "PHDR";
    case PT_NOTE:
      return "NOTE";
    case PT_INTERP:
      return "INTERP";
    case PT_DYNAMIC:
      return "DYNAMIC";
    case PT_GNU_STACK:
      return "GNU_STACK";
    default:
      FormatInt32(buf, p_type);
      return buf;
  }
}

static bool IsBinary(const char *p, size_t n) {
  size_t i;
  for (i = 0; i < n; ++i) {
    if ((p[i] & 255) < '\t') {
      return true;
    }
  }
  return false;
}

static void BlendHashes(uint8_t out[static BLAKE2B256_DIGEST_LENGTH],
                        uint8_t inp[static BLAKE2B256_DIGEST_LENGTH]) {
  int i;
  for (i = 0; i < BLAKE2B256_DIGEST_LENGTH; ++i) {
    out[i] ^= inp[i];
  }
}

static void HashInput(const void *data, size_t size) {
  uint8_t digest[BLAKE2B256_DIGEST_LENGTH];
  uint32_t hash = crc32_z(hashes, data, size);  // 30 GB/s
  BLAKE2B256(&hash, sizeof(hash), digest);      // .6 GB/s
  BlendHashes(hashpool, digest);
  ++hashes;
}

static void HashInputString(const char *str) {
  HashInput(str, strlen(str));
}

static char *LoadSourceCode(const char *path) {
  int fd;
  size_t i;
  char *text;
  ssize_t rc, size;
  if ((fd = open(path, O_RDONLY)) == -1) DieSys(path);
  if ((size = lseek(fd, 0, SEEK_END)) == -1) DieSys(path);
  text = Malloc(size + 1);
  text[size] = 0;
  for (i = 0; i < size; i += rc) {
    if ((rc = pread(fd, text, size - i, i)) <= 0) {
      DieSys(path);
    }
    if (IsBinary(text, rc)) {
      Die(path, "source code contains binary data");
    }
  }
  if (close(fd)) DieSys(path);
  HashInput(text, size);
  return text;
}

static void Pwrite(const void *data, size_t size, uint64_t offset) {
  ssize_t rc;
  const char *p, *e;
  for (p = data, e = p + size; p < e; p += (size_t)rc, offset += (size_t)rc) {
    if ((rc = pwrite(outfd, p, e - p, offset)) == -1) {
      DieSys(outpath);
    }
  }
}

static void LogElfPhdrs(FILE *f, Elf64_Phdr *p, size_t n) {
  size_t i;
  fprintf(f, "Type           "
             "Offset   "
             "VirtAddr           "
             "PhysAddr           "
             "FileSiz  "
             "MemSiz   "
             "Flg Align\n");
  for (i = 0; i < n; ++i) {
    fprintf(f,
            "%-14s 0x%06lx 0x%016lx 0x%016lx 0x%06lx 0x%06lx %c%c%c 0x%04lx\n",
            DescribePhdrType(p[i].p_type), p[i].p_offset, p[i].p_vaddr,
            p[i].p_paddr, p[i].p_filesz, p[i].p_memsz,
            p[i].p_flags & PF_R ? 'R' : ' ', p[i].p_flags & PF_W ? 'W' : ' ',
            p[i].p_flags & PF_X ? 'E' : ' ', p[i].p_align);
  }
}

static void LogPeSections(FILE *f, struct NtImageSectionHeader *p, size_t n) {
  size_t i;
  fprintf(f, "Name           "
             "Offset   "
             "RelativeVirtAddr   "
             "PhysAddr           "
             "FileSiz  "
             "MemSiz   "
             "Flg\n");
  for (i = 0; i < n; ++i) {
    fprintf(f, "%-14.8s 0x%06lx 0x%016lx 0x%016lx 0x%06lx 0x%06lx %c%c%c\n",
            p[i].Name, p[i].PointerToRawData, p[i].VirtualAddress,
            p[i].VirtualAddress, p[i].SizeOfRawData, p[i].Misc.VirtualSize,
            p[i].Characteristics & kNtPeSectionMemRead ? 'R' : ' ',
            p[i].Characteristics & kNtPeSectionMemWrite ? 'W' : ' ',
            p[i].Characteristics & kNtPeSectionMemExecute ? 'E' : ' ');
  }
}

static void ValidateElfImage(Elf64_Ehdr *e, Elf64_Off esize,  //
                             const char *epath, bool isloader) {

  // validate elf header
  if (e->e_type != ET_EXEC && e->e_type != ET_DYN)
    Die(epath, "elf binary isn't an executable");
  if (!e->e_phnum)
    Die(epath, "elf executable needs at least one program header");
  if (e->e_phnum > 65534)
    Die(epath, "elf with more than 65534 phdrs not supported");
  if (e->e_phentsize != sizeof(Elf64_Phdr))
    Die(epath, "elf e_phentsize isn't sizeof(Elf64_Phdr)");
  if (e->e_phoff > esize)
    Die(epath, "elf program header offset points past image eof");
  if (e->e_phoff & 7)
    Die(epath, "elf e_phoff must be aligned on an 8-byte boundary");
  if (e->e_phoff + e->e_phoff + e->e_phnum * sizeof(Elf64_Phdr) > esize)
    Die(epath, "elf program header array overlaps image eof");

  // determine microprocessor page size requirement
  //
  // even though operating systems (windows) and c libraries (cosmo)
  // sometimes impose a larger page size requirement than the actual
  // microprocessor itself, the cpu page size is still the only page
  // size that actually matters when it comes to executable loading.
  unsigned long pagesz;
  if (e->e_machine == EM_AARCH64) {
    pagesz = 16384;  // apple m1 (xnu, linux)
  } else {           //
    pagesz = 4096;   // x86-64, raspberry pi
  }

  // remove empty segment loads
  // empty loads should be inconsequential; linux ignores them
  // however they tend to trip up many systems such as openbsd
  int i, j;
  Elf64_Phdr *p = (Elf64_Phdr *)((char *)e + e->e_phoff);

  for (i = 0; i < e->e_phnum;) {
    if (p[i].p_type == PT_LOAD && !p[i].p_memsz) {
      if (i + 1 < e->e_phnum) {
        memmove(p + i, p + i + 1, (e->e_phnum - (i + 1)) * sizeof(*p));
      }
      --e->e_phnum;
    } else {
      ++i;
    }
  }

  // oracle says loadable segment entries in the program header table
  // appear in ascending order, sorted on the p_vaddr member.
  int found_load = 0;
  Elf64_Addr last_vaddr = 0;
  for (i = 0; i < e->e_phnum; ++i) {
    if (p[i].p_type != PT_LOAD) continue;
    if (found_load && p[i].p_vaddr <= last_vaddr) {
      Die(epath, "ELF PT_LOAD segments must be ordered by p_vaddr");
    }
    last_vaddr = p[i].p_vaddr;
    found_load = 1;
  }
  if (!found_load) {
    Die(epath, "ELF must have at least one PT_LOAD segment");
  }

  // merge adjacent loads that are contiguous with equal protection
  for (i = 0; i + 1 < e->e_phnum;) {
    if (p[i].p_type == PT_LOAD && p[i + 1].p_type == PT_LOAD &&
        ((p[i].p_flags & (PF_R | PF_W | PF_X)) ==
         (p[i + 1].p_flags & (PF_R | PF_W | PF_X))) &&
        ((p[i].p_offset + p[i].p_filesz + (pagesz - 1)) & -pagesz) -
                (p[i + 1].p_offset & -pagesz) <=
            pagesz &&
        ((p[i].p_vaddr + p[i].p_memsz + (pagesz - 1)) & -pagesz) -
                (p[i + 1].p_vaddr & -pagesz) <=
            pagesz) {
      p[i].p_memsz = (p[i + 1].p_vaddr + p[i + 1].p_memsz) - p[i].p_vaddr;
      p[i].p_filesz = (p[i + 1].p_offset + p[i + 1].p_filesz) - p[i].p_offset;
      if (i + 2 < e->e_phnum) {
        memmove(p + i + 1, p + i + 2, (e->e_phnum - (i + 2)) * sizeof(*p));
      }
      --e->e_phnum;
    } else {
      ++i;
    }
  }

  // validate program headers
  bool found_entry = false;
  for (i = 0; i < e->e_phnum; ++i) {
    if (!isloader && p[i].p_type == PT_INTERP) {
      Die(epath, "ELF has PT_INTERP which isn't supported");
    }
    if (!isloader && p[i].p_type == PT_DYNAMIC) {
      Die(epath, "ELF has PT_DYNAMIC which isn't supported");
    }
    if (p[i].p_type != PT_LOAD) {
      continue;
    }
    if (!p[i].p_memsz) {
      Die(epath, "ELF PT_LOAD p_memsz was zero");
    }
    if (p[i].p_offset > esize) {
      Die(epath, "ELF PT_LOAD p_offset points past EOF");
    }
    if (p[i].p_filesz > p[i].p_memsz) {
      Die(epath, "ELF PT_LOAD p_filesz exceeds p_memsz");
    }
    if (p[i].p_align > 1 && (p[i].p_align & (p[i].p_align - 1))) {
      Die(epath, "ELF PT_LOAD p_align must be two power");
    }
    if (p[i].p_vaddr + p[i].p_memsz < p[i].p_vaddr ||
        p[i].p_vaddr + p[i].p_memsz + (pagesz - 1) < p[i].p_vaddr) {
      Die(epath, "ELF PT_LOAD p_vaddr + p_memsz overflow");
    }
    if (p[i].p_offset + p[i].p_filesz < p[i].p_offset ||
        p[i].p_offset + p[i].p_filesz + (pagesz - 1) < p[i].p_offset) {
      Die(epath, "ELF PT_LOAD p_offset + p_filesz overflow");
    }
    if (p[i].p_align > 1 && ((p[i].p_vaddr & (p[i].p_align - 1)) !=
                             (p[i].p_offset & (p[i].p_align - 1)))) {
      Die(epath, "ELF p_vaddr incongruent w/ p_offset modulo p_align");
    }
    if ((p[i].p_vaddr & (pagesz - 1)) != (p[i].p_offset & (pagesz - 1))) {
      Die(epath, "ELF p_vaddr incongruent w/ p_offset modulo AT_PAGESZ");
    }
    if (p[i].p_offset + p[i].p_filesz > esize) {
      Die(epath, "ELF PT_LOAD p_offset and p_filesz overlaps image EOF");
    }
    Elf64_Off a = p[i].p_vaddr & -pagesz;
    Elf64_Off b = (p[i].p_vaddr + p[i].p_memsz + (pagesz - 1)) & -pagesz;
    for (j = i + 1; j < e->e_phnum; ++j) {
      if (p[j].p_type != PT_LOAD) continue;
      Elf64_Off c = p[j].p_vaddr & -pagesz;
      Elf64_Off d = (p[j].p_vaddr + p[j].p_memsz + (pagesz - 1)) & -pagesz;
      if (MAX(a, c) < MIN(b, d)) {
        Die(epath,
            "ELF PT_LOAD phdrs %d and %d overlap each others virtual memory");
      }
    }
    if (e->e_machine == EM_AARCH64 &&
        p[i].p_vaddr + p[i].p_memsz > 0x0001000000000000) {
      Die(epath, "this is an ARM ELF program but it loads to virtual"
                 " memory addresses outside the legal range [0,2^48)");
    }
    if (e->e_machine == EM_NEXGEN32E &&
        !(-140737488355328 <= (Elf64_Sxword)p[i].p_vaddr &&
          (Elf64_Sxword)(p[i].p_vaddr + p[i].p_memsz) <= 140737488355328)) {
      Die(epath, "this is an x86-64 ELF program, but it loads to virtual"
                 " memory addresses outside the legal range [-2^47,2^47)");
    }
    if (e->e_type == ET_EXEC &&           //
        (support_vector & _HOSTMETAL) &&  //
        (Elf64_Sxword)p[i].p_vaddr < 0x200000) {
      Die(epath, "ELF program header loads to address less than 2mb even"
                 " though you have userspace OSes in your support vector");
    }
    if (!isloader &&                   //
        e->e_type == ET_EXEC &&        //
        !(p[i].p_vaddr >> 32) &&       //
        e->e_machine == EM_AARCH64 &&  //
        (support_vector & _HOSTXNU)) {
      Die(epath, "ELF program header loads to address less than 4gb even"
                 " though you have XNU on AARCH64 in your support vector"
                 " which forbids 32-bit pointers without any compromises");
    }
    if ((p[i].p_flags & (PF_W | PF_X)) == (PF_W | PF_X)) {
      if (support_vector & _HOSTOPENBSD) {
        Die(epath, "found RWX ELF program segment, but you have OpenBSD in"
                   " your support vector, which totally forbids RWX memory");
      }
      if (e->e_machine == EM_AARCH64 && (support_vector & _HOSTXNU)) {
        Die(epath, "found RWX ELF program segment on an AARCH64 executable"
                   " with XNU support, which completely forbids RWX memory");
      }
    }
    if ((p[i].p_flags & PF_X) &&  //
        p[i].p_vaddr <= e->e_entry &&
        e->e_entry < p[i].p_vaddr + p[i].p_memsz) {
      found_entry = 1;
    }
  }
  if (!found_entry) {
    Die(epath, "ELF entrypoint not found in PT_LOAD with PF_X");
  }
}

static bool IsSymbolWorthyOfSymtab(Elf64_Sym *sym) {
  return sym->st_size > 0 && (ELF64_ST_TYPE(sym->st_info) == STT_FUNC ||
                              ELF64_ST_TYPE(sym->st_info) == STT_OBJECT);
}

static void AppendZipAsset(unsigned char *lfile, unsigned char *cfile) {
  if (assets.n == 65534) Die(outpath, "fat binary has >65534 zip assets");
  assets.p = Realloc(assets.p, (assets.n + 1) * sizeof(*assets.p));
  assets.p[assets.n].cfile = cfile;
  assets.p[assets.n].lfile = lfile;
  assets.total_local_file_bytes += ZIP_LFILE_SIZE(lfile);
  assets.total_centraldir_bytes += ZIP_CFILE_HDRSIZE(cfile);
  ++assets.n;
}

static void *Compress(const void *data, size_t size, size_t *out_size, int wb) {
  void *res;
  z_stream zs;
  zs.zfree = 0;
  zs.zalloc = 0;
  zs.opaque = 0;
  unassert(deflateInit2(&zs, Z_BEST_COMPRESSION, Z_DEFLATED, wb, DEF_MEM_LEVEL,
                        Z_DEFAULT_STRATEGY) == Z_OK);
  zs.next_in = data;
  zs.avail_in = size;
  zs.avail_out = compressBound(size);
  zs.next_out = res = Malloc(zs.avail_out);
  unassert(deflate(&zs, Z_FINISH) == Z_STREAM_END);
  unassert(deflateEnd(&zs) == Z_OK);
  *out_size = zs.total_out;
  return res;
}

static void *Deflate(const void *data, size_t size, size_t *out_size) {
  return Compress(data, size, out_size, -MAX_WBITS);
}

static void *Gzip(const void *data, size_t size, size_t *out_size) {
  return Compress(data, size, out_size, MAX_WBITS + 16);
}

static void LoadSymbols(Elf64_Ehdr *e, Elf64_Off size, const char *path) {
  const char *name = ConvertElfMachineToSymtabName(e);
  size_t name_size = strlen(name);
  struct SymbolTable *st = OpenSymbolTable(path);
  if (!st) Die(path, "could not load elf symbol table");
  size_t data_size;
  void *data = Deflate(st, st->size, &data_size);
  uint32_t crc = crc32_z(0, st, st->size);
  size_t cfile_size = kZipCfileHdrMinSize + name_size;
  unsigned char *cfile = Malloc(cfile_size);
  bzero(cfile, cfile_size);
  WRITE32LE(cfile, kZipCfileHdrMagic);
  WRITE16LE(cfile + kZipCfileOffsetVersionMadeBy,
            kZipOsUnix << 8 | kZipCosmopolitanVersion);
  WRITE16LE(cfile + kZipCfileOffsetVersionNeeded, kZipEra2001);
  WRITE16LE(cfile + kZipCfileOffsetGeneralflag, kZipGflagUtf8);
  WRITE16LE(cfile + kZipCfileOffsetCompressionmethod, kZipCompressionDeflate);
  WRITE16LE(cfile + kZipCfileOffsetLastmodifieddate, DOS_DATE(2023, 7, 29));
  WRITE16LE(cfile + kZipCfileOffsetLastmodifiedtime, DOS_TIME(0, 0, 0));
  WRITE32LE(cfile + kZipCfileOffsetCompressedsize, data_size);
  WRITE32LE(cfile + kZipCfileOffsetUncompressedsize, st->size);
  WRITE32LE(cfile + kZipCfileOffsetCrc32, crc);
  WRITE16LE(cfile + kZipCfileOffsetNamesize, name_size);
  memcpy(cfile + kZipCfileHdrMinSize, name, name_size);
  unassert(ZIP_CFILE_HDRSIZE(cfile) == cfile_size);
  size_t lfile_size = kZipLfileHdrMinSize + name_size + data_size;
  unsigned char *lfile = Malloc(lfile_size);
  bzero(lfile, lfile_size);
  WRITE32LE(lfile, kZipLfileHdrMagic);
  WRITE16LE(lfile + kZipLfileOffsetVersionNeeded, kZipEra2001);
  WRITE16LE(lfile + kZipLfileOffsetGeneralflag, kZipGflagUtf8);
  WRITE16LE(lfile + kZipLfileOffsetCompressionmethod, kZipCompressionDeflate);
  WRITE16LE(lfile + kZipLfileOffsetLastmodifieddate, DOS_DATE(2023, 7, 29));
  WRITE16LE(lfile + kZipLfileOffsetLastmodifiedtime, DOS_TIME(0, 0, 0));
  WRITE32LE(lfile + kZipLfileOffsetCompressedsize, data_size);
  WRITE32LE(lfile + kZipLfileOffsetUncompressedsize, st->size);
  WRITE32LE(lfile + kZipLfileOffsetCrc32, crc);
  WRITE16LE(lfile + kZipLfileOffsetNamesize, name_size);
  memcpy(lfile + kZipLfileHdrMinSize, name, name_size);
  memcpy(lfile + kZipLfileHdrMinSize + name_size, data, data_size);
  unassert(ZIP_LFILE_SIZE(lfile) == lfile_size);
  free(data);
  AppendZipAsset(lfile, cfile);
}

// resolves portable executable relative virtual address
//
// this is a trivial process when an executable has been loaded properly
// i.e. a separate mmap() call was made for each individual section; but
// we've only mapped the executable file itself into memory; thus, we'll
// need to remap a virtual address into a file offset to get the pointer
//
// returns pointer to image data, or null on error
static void *GetPeRva(char *map, struct NtImageNtHeaders *pe, uint32_t rva) {
  struct NtImageSectionHeader *sections =
      (struct NtImageSectionHeader *)((char *)&pe->OptionalHeader +
                                      pe->FileHeader.SizeOfOptionalHeader);
  for (int i = 0; i < pe->FileHeader.NumberOfSections; ++i) {
    if (sections[i].VirtualAddress <= rva &&
        rva < sections[i].VirtualAddress + sections[i].Misc.VirtualSize) {
      return map + sections[i].PointerToRawData +
             (rva - sections[i].VirtualAddress);
    }
  }
  return 0;
}

static int ValidatePeImage(char *img, size_t imgsize,    //
                           struct NtImageNtHeaders *pe,  //
                           const char *path) {

  int pagesz = 4096;

  // sanity check pe header
  if (pe->Signature != ('P' | 'E' << 8))
    Die(path, "PE Signature must be 0x00004550");
  if (!(pe->FileHeader.Characteristics & kNtPeFileExecutableImage))
    Die(path, "PE Characteristics must have executable bit set");
  if (pe->FileHeader.Characteristics & kNtPeFileDll)
    Die(path, "PE Characteristics can't have DLL bit set");
  if (pe->FileHeader.NumberOfSections < 1)
    Die(path, "PE NumberOfSections >= 1 must be the case");
  if (pe->OptionalHeader.Magic != kNtPe64bit)
    Die(path, "PE OptionalHeader Magic must be 0x020b");
  if (pe->OptionalHeader.FileAlignment < 512)
    Die(path, "PE FileAlignment must be at least 512");
  if (pe->OptionalHeader.FileAlignment > 65536)
    Die(path, "PE FileAlignment can't exceed 65536");
  if (pe->OptionalHeader.FileAlignment & (pe->OptionalHeader.FileAlignment - 1))
    Die(path, "PE FileAlignment must be a two power");
  if (pe->OptionalHeader.SectionAlignment &
      (pe->OptionalHeader.SectionAlignment - 1))
    Die(path, "PE SectionAlignment must be a two power");
  if (pe->OptionalHeader.SectionAlignment < pe->OptionalHeader.FileAlignment)
    Die(path, "PE SectionAlignment >= FileAlignment must be the case");
  if (pe->OptionalHeader.SectionAlignment < pagesz &&
      pe->OptionalHeader.SectionAlignment != pe->OptionalHeader.FileAlignment)
    Die(path, "PE SectionAlignment must equal FileAlignment if it's less than "
              "the microprocessor architecture's page size");
  if (pe->OptionalHeader.ImageBase & 65535)
    Die(path, "PE ImageBase must be multiple of 65536");
  if (pe->OptionalHeader.ImageBase > INT_MAX &&
      !(pe->FileHeader.Characteristics & kNtImageFileLargeAddressAware))
    Die(path, "PE FileHeader.Characteristics needs "
              "IMAGE_FILE_LARGE_ADDRESS_AWARE if ImageBase > INT_MAX");
  if (!(support_vector & _HOSTMETAL) &&
      pe->OptionalHeader.Subsystem == kNtImageSubsystemEfiApplication)
    Die(path, "PE Subsystem is EFI but Metal wasn't in support_vector");
  if (!(support_vector & _HOSTWINDOWS) &&
      (pe->OptionalHeader.Subsystem == kNtImageSubsystemWindowsCui ||
       pe->OptionalHeader.Subsystem == kNtImageSubsystemWindowsGui))
    Die(path, "PE Subsystem is Windows but Windows wasn't in support_vector");

  // fixup pe header
  int len;
  if (ckd_mul(&len, pe->OptionalHeader.NumberOfRvaAndSizes, 8) ||
      ckd_add(&len, len, sizeof(struct NtImageOptionalHeader)) ||
      pe->FileHeader.SizeOfOptionalHeader < len)
    Die(path, "PE SizeOfOptionalHeader too small");
  if (len > imgsize || (char *)&pe->OptionalHeader + len > img + imgsize)
    Die(path, "PE OptionalHeader overflows image");

  // perform even more pe validation
  if (pe->OptionalHeader.SizeOfImage &
      (pe->OptionalHeader.SectionAlignment - 1))
    Die(path, "PE SizeOfImage must be multiple of SectionAlignment");
  if (pe->OptionalHeader.SizeOfHeaders & (pe->OptionalHeader.FileAlignment - 1))
    Die(path, "PE SizeOfHeaders must be multiple of FileAlignment");
  if (pe->OptionalHeader.SizeOfHeaders > pe->OptionalHeader.AddressOfEntryPoint)
    Die(path, "PE SizeOfHeaders <= AddressOfEntryPoint must be the case");
  if (pe->OptionalHeader.SizeOfHeaders >= pe->OptionalHeader.SizeOfImage)
    Die(path, "PE SizeOfHeaders < SizeOfImage must be the case");
  if (pe->OptionalHeader.SizeOfStackCommit >> 32)
    Die(path, "PE SizeOfStackCommit can't exceed 4GB");
  if (pe->OptionalHeader.SizeOfStackReserve >> 32)
    Die(path, "PE SizeOfStackReserve can't exceed 4GB");
  if (pe->OptionalHeader.SizeOfHeapCommit >> 32)
    Die(path, "PE SizeOfHeapCommit can't exceed 4GB");
  if (pe->OptionalHeader.SizeOfHeapReserve >> 32)
    Die(path, "PE SizeOfHeapReserve can't exceed 4GB");

  // check pe section headers
  struct NtImageSectionHeader *sections =
      (struct NtImageSectionHeader *)((char *)&pe->OptionalHeader +
                                      pe->FileHeader.SizeOfOptionalHeader);
  for (int i = 0; i < pe->FileHeader.NumberOfSections; ++i) {
    if (sections[i].SizeOfRawData & (pe->OptionalHeader.FileAlignment - 1))
      Die(path, "PE SizeOfRawData should be multiple of FileAlignment");
    if (sections[i].PointerToRawData & (pe->OptionalHeader.FileAlignment - 1))
      Die(path, "PE PointerToRawData must be multiple of FileAlignment");
    if (img + sections[i].PointerToRawData >= img + imgsize)
      Die(path, "PE PointerToRawData points outside image");
    if (img + sections[i].PointerToRawData + sections[i].SizeOfRawData >
        img + imgsize)
      Die(path, "PE SizeOfRawData overlaps end of image");
    if (!sections[i].VirtualAddress)
      Die(path, "PE VirtualAddress shouldn't be zero");
    if (sections[i].VirtualAddress & (pe->OptionalHeader.SectionAlignment - 1))
      Die(path, "PE VirtualAddress must be multiple of SectionAlignment");
    if ((sections[i].Characteristics &
         (kNtPeSectionCntCode | kNtPeSectionCntInitializedData |
          kNtPeSectionCntUninitializedData)) ==
        kNtPeSectionCntUninitializedData) {
      if (sections[i].SizeOfRawData)
        Die(path, "PE SizeOfRawData should be zero for pure BSS section");
      if (sections[i].PointerToRawData)
        Die(path, "PE PointerToRawData should be zero for pure BSS section");
    }
    if (!i) {
      if (sections[i].VirtualAddress !=
          ((pe->OptionalHeader.SizeOfHeaders +
            (pe->OptionalHeader.SectionAlignment - 1)) &
           -pe->OptionalHeader.SectionAlignment))
        Die(path, "PE VirtualAddress of first section must be SizeOfHeaders "
                  "rounded up to SectionAlignment");
    } else {
      if (sections[i].VirtualAddress !=
          sections[i - 1].VirtualAddress +
              ((sections[i - 1].Misc.VirtualSize +
                (pe->OptionalHeader.SectionAlignment - 1)) &
               -pe->OptionalHeader.SectionAlignment))
        Die(path, "PE sections must be in ascending order and the virtual "
                  "memory they define must be adjacent after VirtualSize is "
                  "rounded up to the SectionAlignment");
    }
  }

  int size_of_pe_headers =
      (char *)(sections + pe->FileHeader.NumberOfSections) -
      (char *)&pe->Signature;
  return size_of_pe_headers;
}

void FixupPeImage(char *map, size_t size,       //
                  struct NtImageNtHeaders *pe,  //
                  const char *path,             //
                  Elf64_Sxword rva_skew,        //
                  Elf64_Sxword off_skew) {
  assert(!(rva_skew & 65535));

  // fixup pe header
  if (ckd_sub(&pe->OptionalHeader.ImageBase,  //
              pe->OptionalHeader.ImageBase, rva_skew))
    Die(path, "skewing PE ImageBase VA overflowed");
  if (ckd_add(&pe->OptionalHeader.AddressOfEntryPoint,
              pe->OptionalHeader.AddressOfEntryPoint, rva_skew))
    Die(path, "skewing PE AddressOfEntryPoint RVA overflowed");
  if (ckd_add(&pe->OptionalHeader.SizeOfImage,  //
              pe->OptionalHeader.SizeOfImage, rva_skew))
    Die(path, "skewing PE SizeOfImage VSZ overflowed");
  if (ckd_add(&pe->OptionalHeader.SizeOfHeaders,  //
              pe->OptionalHeader.SizeOfHeaders, off_skew))
    Die(path, "skewing PE SizeOfHeaders FSZ overflowed");

  // fixup dll imports
  //
  // this implementation currently only works on simple pe file layouts
  // where relative virtual addresses are equivalent to file offsets
  struct NtImageDataDirectory *ddImports =
      pe->OptionalHeader.DataDirectory + kNtImageDirectoryEntryImport;
  if (pe->OptionalHeader.NumberOfRvaAndSizes >= 2 && ddImports->Size) {
    struct NtImageImportDescriptor *idt;
    if (!(idt = GetPeRva(map, pe, ddImports->VirtualAddress)))
      Die(path, "couldn't resolve VirtualAddress/Size RVA of PE Import "
                "Directory Table to within a defined PE section");
    for (int i = 0; idt[i].ImportLookupTable; ++i) {
      uint64_t *ilt, *iat;
      if (!(ilt = GetPeRva(map, pe, idt[i].ImportLookupTable)))
        Die(path, "PE ImportLookupTable RVA didn't resolve to a section");
      for (int j = 0; ilt[j]; ++j) {
        if (ckd_add(&ilt[j], ilt[j], rva_skew))
          Die(path, "skewing PE Import Lookup Table RVA overflowed");
      }
      if (!(iat = GetPeRva(map, pe, idt[i].ImportAddressTable)))
        Die(path, "PE ImportAddressTable RVA didn't resolve to a section");
      for (int j = 0; iat[j]; ++j) {
        if (ckd_add(&iat[j], iat[j], rva_skew))
          Die(path, "skewing PE Import Lookup Table RVA overflowed");
      }
      if (ckd_add(&idt[i].DllNameRva, idt[i].DllNameRva, rva_skew))
        Die(path, "skewing PE IDT DllNameRva RVA overflowed");
      if (ckd_add(&idt[i].ImportLookupTable, idt[i].ImportLookupTable,
                  rva_skew))
        Die(path, "skewing PE IDT ImportLookupTable RVA overflowed");
      if (ckd_add(&idt[i].ImportAddressTable, idt[i].ImportAddressTable,
                  rva_skew))
        Die(path, "skewing PE IDT ImportAddressTable RVA overflowed");
    }
    if (ckd_add(&ddImports->VirtualAddress, ddImports->VirtualAddress,
                rva_skew))
      Die(path, "skewing PE IMAGE_DIRECTORY_ENTRY_IMPORT RVA overflowed");
  }

  // fixup pe segments table
  struct NtImageSectionHeader *sections =
      (struct NtImageSectionHeader *)((char *)&pe->OptionalHeader +
                                      pe->FileHeader.SizeOfOptionalHeader);
  for (int i = 0; i < pe->FileHeader.NumberOfSections; ++i) {
    if (ckd_add(&sections[i].VirtualAddress, sections[i].VirtualAddress,
                rva_skew))
      Die(path, "skewing PE section VirtualAddress overflowed");
    if (ckd_add(&sections[i].PointerToRawData, sections[i].PointerToRawData,
                off_skew))
      Die(path, "skewing PE section PointerToRawData overflowed");
  }
}

static bool FindLoaderEmbeddedMachoHeader(struct Loader *ape) {
  size_t i;
  struct MachoHeader *macho;
  for (i = 0; i + sizeof(struct MachoHeader) < ape->size; i += 64) {
    if (READ32LE((char *)ape->map + i) == 0xFEEDFACE + 1) {
      macho = (struct MachoHeader *)((char *)ape->map + i);
      ape->macho_offset = i;
      ape->macho_length = sizeof(*macho) + macho->loadsize;
      return true;
    }
  }
  return false;
}

static struct Input *GetInput(int machine) {
  int i;
  for (i = 0; i < inputs.n; ++i) {
    if (inputs.p[i].elf->e_machine == machine) {
      return inputs.p + i;
    }
  }
  return 0;
}

static struct Loader *GetLoader(int machine, int os) {
  int i;
  for (i = 0; i < loaders.n; ++i) {
    if ((loaders.p[i].os & os) && loaders.p[i].machine == machine) {
      return loaders.p + i;
    }
  }
  return 0;
}

static void AddLoader(const char *path) {
  if (loaders.n == ARRAYLEN(loaders.p)) {
    Die(prog, "too many loaders");
  }
  loaders.p[loaders.n++].path = path;
}

static void GetOpts(int argc, char *argv[]) {
  int opt, bits;
  bool got_support_vector = false;
  while ((opt = getopt(argc, argv, "hvgsGBo:l:S:M:V:")) != -1) {
    switch (opt) {
      case 'o':
        outpath = optarg;
        break;
      case 's':
        HashInputString("-s");
        want_stripped = true;
        break;
      case 'V':
        HashInputString("-V");
        HashInputString(optarg);
        if (ParseSupportVector(optarg, &bits)) {
          support_vector |= bits;
        } else {
          Die(prog, "unrecognized token passed to -s support vector flag");
          exit(1);
        }
        got_support_vector = true;
        break;
      case 'l':
        HashInputString("-l");
        AddLoader(optarg);
        break;
      case 'S':
        HashInputString("-S");
        HashInputString(optarg);
        custom_sh_code = optarg;
        break;
      case 'B':
        HashInputString("-B");
        force_bypass_binfmt_misc = true;
        break;
      case 'g':
        HashInputString("-g");
        generate_debuggable_binary = true;
        break;
      case 'G':
        HashInputString("-G");
        dont_path_lookup_ape_loader = true;
        break;
      case 'M':
        HashInputString("-M");
        macos_silicon_loader_source_path = optarg;
        macos_silicon_loader_source_text = LoadSourceCode(optarg);
        break;
      case 'v':
        tinyprint(0, VERSION, NULL);
        exit(0);
      case 'h':
        ShowUsage(0, 1);
      default:
        ShowUsage(1, 2);
    }
  }
  if (!outpath) {
    Die(prog, "need output path");
  }
  if (optind == argc) {
    Die(prog, "missing input argument");
  }
  if (!got_support_vector) {
    support_vector = -1;
  }
}

static void OpenLoader(struct Loader *ldr) {
  int fd;
  Elf64_Ehdr *elf;
  struct MachoHeader *macho;
  if ((fd = open(ldr->path, O_RDONLY)) == -1) {
    DieSys(ldr->path);
  }
  if ((ldr->size = lseek(fd, 0, SEEK_END)) == -1) {
    DieSys(ldr->path);
  }
  ldr->map = mmap(0, ldr->size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
  if (ldr->map == MAP_FAILED) {
    DieSys(ldr->path);
  }
  HashInput(ldr->map, ldr->size);
  close(fd);
  if (IsElf64Binary((elf = ldr->map), ldr->size)) {
    ValidateElfImage(ldr->map, ldr->size, ldr->path, true);
    ldr->os = _HOSTLINUX | _HOSTFREEBSD | _HOSTNETBSD | _HOSTOPENBSD;
    ldr->machine = elf->e_machine;
    if (ldr->machine == EM_NEXGEN32E && FindLoaderEmbeddedMachoHeader(ldr)) {
      ldr->os |= _HOSTXNU;
    }
  } else if (READ32LE(ldr->map) == 0xFEEDFACE + 1) {
    macho = (struct MachoHeader *)ldr->map;
    if (macho->arch == MAC_CPU_NEXGEN32E) {
      Die(ldr->path, "there's no reason to embed the x86 ape.macho loader, "
                     "because ape.elf for x86-64 works on x86-64 macos too");
    } else if (macho->arch == MAC_CPU_ARM64) {
      ldr->os = _HOSTXNU;
      ldr->machine = EM_AARCH64;
      if (macos_silicon_loader_source_path) {
        Die(ldr->path,
            "it doesn't make sense to embed a prebuilt ape loader executable "
            "for macos silicon when its source code was already passed so it "
            "can be built by the ape shell script on the fly");
      }
    } else {
      Die(ldr->path, "unrecognized mach-o ape loader architecture");
    }
  } else {
    Die(ldr->path, "ape loader file format must be elf or mach-o");
  }
}

static int PhdrFlagsToProt(Elf64_Word flags) {
  int prot = PROT_NONE;
  if (flags & PF_R) prot |= PROT_READ;
  if (flags & PF_W) prot |= PROT_WRITE;
  if (flags & PF_X) prot |= PROT_EXEC;
  return prot;
}

static char *EncodeWordAsPrintf(char *p, uint64_t w, int bytes) {
  int c;
  while (bytes-- > 0) {
    c = w & 255;
    w >>= 8;
    if (isascii(c) && isprint(c) && !isdigit(c) && c != '\\' && c != '%') {
      *p++ = c;
    } else {
      *p++ = '\\';
      if ((c & 0700)) *p++ = '0' + ((c & 0700) >> 6);
      if ((c & 0770)) *p++ = '0' + ((c & 070) >> 3);
      *p++ = '0' + (c & 7);
    }
  }
  return p;
}

static char *FixupWordAsBinary(char *p, uint64_t w) {
  do {
    *p++ = w;
  } while ((w >>= 8));
  return p;
}

static char *FixupWordAsDecimal(char *p, uint64_t w) {
  char buf[21];
  return mempcpy(p, buf, FormatInt64(buf, w) - buf);
}

static char *FixupWordAsPrintf(char *p, uint64_t w) {
  unassert(READ32LE(p) == READ32LE("\\000"));
  do {
    *p++ = '\\';
    *p++ = '0' + ((w & 0700) >> 6);
    *p++ = '0' + ((w & 070) >> 3);
    *p++ = '0' + ((w & 07));
  } while ((w >>= 8));
  return p;
}

static char *FixupPrintf(char *p, uint64_t w) {
  switch (strategy) {
    case kPe:
      return p;
    case kElf:
    case kMacho:
      return FixupWordAsBinary(p, w);
    case kApe:
      return FixupWordAsPrintf(p, w);
    default:
      __builtin_unreachable();
  }
}

static int PickElfOsAbi(void) {
  switch (support_vector) {
    case _HOSTLINUX:
      return ELFOSABI_LINUX;
    case _HOSTOPENBSD:
      return ELFOSABI_OPENBSD;
    case _HOSTNETBSD:
      return ELFOSABI_NETBSD;
    default:
      // If we're supporting multiple operating systems, then FreeBSD
      // has the only kernel that actually checks this field.
      return ELFOSABI_FREEBSD;
  }
}

static void AddOffsetReloc(struct Input *in, uint64_t *r) {
  if (in->offsetrelocs.n == ARRAYLEN(in->offsetrelocs.p)) {
    Die(in->path, "ran out of offset relocations");
  }
  in->offsetrelocs.p[in->offsetrelocs.n++] = r;
}

static char *GenerateDecimalOffsetRelocation(char *p) {
  size_t i, n = 10;
  for (i = 0; i < n; ++i) {
    *p++ = ' ';
  }
  return p;
}

static char *GenerateElf(char *p, struct Input *in) {
  Elf64_Ehdr *e;
  p = ALIGN(p, alignof(Elf64_Ehdr));
  e = (Elf64_Ehdr *)p;
  e->e_ident[EI_MAG0] = ELFMAG0;
  e->e_ident[EI_MAG1] = ELFMAG1;
  e->e_ident[EI_MAG2] = ELFMAG2;
  e->e_ident[EI_MAG3] = ELFMAG3;
  e->e_ident[EI_CLASS] = ELFCLASS64;
  e->e_ident[EI_DATA] = ELFDATA2LSB;
  e->e_ident[EI_VERSION] = 1;
  e->e_ident[EI_OSABI] = PickElfOsAbi();
  e->e_ident[EI_ABIVERSION] = 0;
  e->e_type = ET_EXEC;
  e->e_machine = in->elf->e_machine;
  e->e_version = 1;
  e->e_entry = in->elf->e_entry;
  e->e_ehsize = sizeof(Elf64_Ehdr);
  e->e_phentsize = sizeof(Elf64_Phdr);
  in->printf_phoff = (char *)&e->e_phoff;
  in->printf_phnum = (char *)&e->e_phnum;
  return p + sizeof(*e);
}

static bool IsPhdrAllocated(struct Input *in, Elf64_Phdr *phdr) {
  int i;
  Elf64_Shdr *shdr;
  if (1) return true;
  for (i = 0; i < in->elf->e_shnum; ++i) {
    if (!(shdr = GetElfSectionHeaderAddress(in->elf, in->size, i))) {
      Die(in->path, "elf section header overflow");
    }
    if ((shdr->sh_flags & SHF_ALLOC) &&
        MAX(shdr->sh_addr, phdr->p_vaddr) <
            MIN(shdr->sh_addr + shdr->sh_size, phdr->p_vaddr + phdr->p_memsz)) {
      return true;
    }
  }
  return false;
}

// finds non-local elf symbol by name
static struct Elf64_Sym *GetElfSymbol(struct Input *in, const char *name) {
  char *ss;
  Elf64_Sym *st;
  Elf64_Shdr *sh;
  Elf64_Xword i, n;
  ss = GetElfStringTable(in->elf, in->size, ".strtab");
  sh = GetElfSymbolTable(in->elf, in->size, SHT_SYMTAB, &n);
  st = GetElfSectionAddress(in->elf, in->size, sh);
  if (!st || !ss) Die(in->path, "missing elf symbol table");
  for (i = sh->sh_info; i < n; ++i) {
    if (st[i].st_name && !strcmp(ss + st[i].st_name, name)) {
      return st + i;
    }
  }
  return 0;
}

static char *DefineMachoNull(char *p) {
  struct MachoLoadSegment *load;
  load = (struct MachoLoadSegment *)p;
  ++macholoadcount;
  load->command = MAC_LC_SEGMENT_64;
  load->size = sizeof(*load);
  strcpy(load->name, "__PAGEZERO");
  load->memsz = 0x200000;
  return p + sizeof(*load);
}

static char *DefineMachoUuid(char *p) {
  struct MachoLoadUuid *load;
  load = (struct MachoLoadUuid *)p;
  ++macholoadcount;
  load->command = MAC_LC_UUID;
  load->size = sizeof(*load);
  if (!hashes) Die(outpath, "won't generate macho uuid");
  memcpy(load->uuid, hashpool, sizeof(load->uuid));
  return p + sizeof(*load);
}

static char *DefineMachoEntrypoint(char *p, struct Input *in) {
  struct Elf64_Sym *start;
  struct MachoLoadThread64 *load;
  if (!(start = GetElfSymbol(in, "_apple")) &&
      !(start = GetElfSymbol(in, "_start"))) {
    Die(in->path, "couldn't find _apple() or _start() function in elf "
                  "executable which can serve as the macho entrypoint");
  }
  load = (struct MachoLoadThread64 *)p;
  ++macholoadcount;
  load->thread.command = MAC_LC_UNIXTHREAD;
  load->thread.size = sizeof(*load);
  load->thread.flavor = MAC_THREAD_NEXGEN32E;
  load->thread.count = sizeof(load->regs) / 4;
  load->regs[16] = start->st_value;
  return p + sizeof(*load);
}

static char *GenerateMachoSegment(char *p, struct Input *in, Elf64_Phdr *phdr) {
  struct MachoLoadSegment *load;
  load = (struct MachoLoadSegment *)p;
  load->command = MAC_LC_SEGMENT_64;
  load->size = sizeof(*load);
  FormatInt32(__veil("r", stpcpy(load->name, "__APE")), macholoadcount);
  ++macholoadcount;
  load->vaddr = phdr->p_vaddr;
  load->memsz = phdr->p_memsz;
  load->offset = phdr->p_offset;
  load->filesz = phdr->p_filesz;
  load->maxprot = PROT_EXEC | PROT_READ | PROT_WRITE;
  load->initprot = PhdrFlagsToProt(phdr->p_flags);
  if (load->initprot & PROT_EXEC) {
    load->initprot |= PROT_READ;  // xnu wants it
  }
  AddOffsetReloc(in, &load->offset);
  if (!in->first_macho_load) {
    in->first_macho_load = load;
  }
  return p + sizeof(*load);
}

static char *GenerateMachoLoads(char *p, struct Input *in) {
  int i;
  struct Elf64_Phdr *phdr;
  p = DefineMachoNull(p);
  for (i = 0; i < in->elf->e_phnum; ++i) {
    phdr = GetElfProgramHeaderAddress(in->elf, in->size, i);
    if (phdr->p_type == PT_LOAD) {
      if (!IsPhdrAllocated(in, phdr)) {
        continue;
      }
      p = GenerateMachoSegment(p, in, phdr);
    }
  }
  p = DefineMachoUuid(p);
  p = DefineMachoEntrypoint(p, in);
  return p;
}

static char *GenerateMacho(char *p, struct Input *in) {
  char *pMacho, *pLoads;
  struct MachoHeader *macho;
  pMacho = p = ALIGN(p, 8);
  macho = (struct MachoHeader *)p;
  macho->magic = 0xFEEDFACE + 1;
  macho->arch = MAC_CPU_NEXGEN32E;
  macho->arch2 = MAC_CPU_NEXGEN32E_ALL;
  macho->filetype = MAC_EXECUTE;
  macho->flags = MAC_NOUNDEFS;
  macho->__reserved = 0;
  pLoads = p += sizeof(struct MachoHeader);
  p = GenerateMachoLoads(p, in);
  macho->loadcount = macholoadcount;
  macho->loadsize = p - pLoads;
  if (in->ddarg_macho_skip)
    FixupWordAsDecimal(in->ddarg_macho_skip, pMacho - prologue);
  if (in->ddarg_macho_count)
    FixupWordAsDecimal(in->ddarg_macho_count, p - pMacho);
  return p;
}

static void ValidatePortableExecutable(struct Input *in) {
  unassert(r_off32_e_lfanew);
  unassert(in->elf->e_machine == EM_NEXGEN32E);
  Elf64_Sym *ape_pe;
  if (!(ape_pe = GetElfSymbol(in, "ape_pe"))) {
    if (support_vector & _HOSTWINDOWS) {
      Die(in->path, "elf image needs to define `ape_pe` when windows is "
                    "in the support vector");
    }
    return;
  }
  int off;
  Elf64_Shdr *shdr =
      GetElfSectionHeaderAddress(in->elf, in->size, ape_pe->st_shndx);
  if (!shdr || ckd_sub(&off, ape_pe->st_value, shdr->sh_addr) ||
      ckd_add(&off, off, shdr->sh_offset) || off >= in->size ||
      off + sizeof(struct NtImageNtHeaders) > in->size)
    Die(in->path, "ape_pe symbol headers corrupted");
  in->pe_offset = off;
  in->pe = (struct NtImageNtHeaders *)(in->map + off);
  in->size_of_pe_headers = ValidatePeImage(
      in->map + in->minload, in->maxload - in->minload, in->pe, in->path);
}

static void LinkPortableExecutableHeader(struct Input *in,       //
                                         Elf64_Sxword rva_skew,  //
                                         Elf64_Sxword off_skew) {
  if (!in->pe) {
    // this can happen if (1) strategy is ape, (2) metal is in the
    // support vector and (3) cosmo's efi entrypoint wasn't linked
    return;
  }
  WRITE32LE(r_off32_e_lfanew, in->pe_offset - in->minload + off_skew);
  FixupPeImage(in->map + in->minload, in->maxload - in->minload, in->pe,
               in->path, rva_skew, off_skew);
}

static char *GenerateElfNote(char *p, const char *name, int type, int desc) {
  p = WRITE32LE(p, strlen(name) + 1);
  p = WRITE32LE(p, 4);
  p = WRITE32LE(p, type);
  p = stpcpy(p, name);
  p = ALIGN(p, 4);
  p = WRITE32LE(p, desc);
  return p;
}

static char *GenerateElfNotes(char *p) {
  char *save;
  save = p = ALIGN(p, 4);
  noteoff = p - prologue;
  p = GenerateElfNote(p, "APE", 1, APE_VERSION_NOTE);
  if (support_vector & _HOSTOPENBSD) {
    p = GenerateElfNote(p, "OpenBSD", 1, 0);
  }
  if (support_vector & _HOSTNETBSD) {
    p = GenerateElfNote(p, "NetBSD", 1, 901000000);
  }
  notesize = p - save;
  return p;
}

static char *SecondPass(char *p, struct Input *in) {
  int i, phnum;
  Elf64_Phdr *phdr, *phdr2;

  // plan elf program headers
  phnum = 0;
  in->minload = -1;
  p = ALIGN(p, alignof(Elf64_Phdr));
  FixupPrintf(in->printf_phoff, p - prologue);
  for (i = 0; i < in->elf->e_phnum; ++i) {
    phdr = GetElfProgramHeaderAddress(in->elf, in->size, i);
    if (phdr->p_type == PT_LOAD && !IsPhdrAllocated(in, phdr)) continue;
    *(phdr2 = (Elf64_Phdr *)p) = *phdr;
    p += sizeof(Elf64_Phdr);
    if (phdr->p_filesz) {
      in->minload = MIN(in->minload, phdr->p_offset);
      in->maxload = MAX(in->maxload, phdr->p_offset + phdr->p_filesz);
      AddOffsetReloc(in, &phdr2->p_offset);
    }
    ++phnum;
  }
  if (noteoff && notesize) {
    phdr = (Elf64_Phdr *)p;
    p += sizeof(Elf64_Phdr);
    phdr->p_type = PT_NOTE;
    phdr->p_flags = PF_R;
    phdr->p_offset = noteoff;
    phdr->p_vaddr = 0;
    phdr->p_paddr = 0;
    phdr->p_filesz = notesize;
    phdr->p_memsz = notesize;
    phdr->p_align = 4;
    ++phnum;
  }
  FixupPrintf(in->printf_phnum, phnum);

  // plan embedded mach-o executable
  if (strategy == kApe &&             //
      (support_vector & _HOSTXNU) &&  //
      in->elf->e_machine == EM_NEXGEN32E) {
    p = GenerateMacho(p, in);
  }

  return p;
}

static char *SecondPass2(char *p, struct Input *in) {

  // plan embedded portable executable
  in->we_are_generating_pe = (strategy == kApe || strategy == kPe) &&
                             in->elf->e_machine == EM_NEXGEN32E &&
                             (support_vector & (_HOSTWINDOWS | _HOSTMETAL));
  if (in->we_are_generating_pe) {
    ValidatePortableExecutable(in);
    // if the elf image has a non-alloc prologue that's being stripped
    // away then there's a chance we can avoid adding 64kb of bloat to
    // the new file size. that's only possible if all the fat ape hdrs
    // we generate are able to fit inside the prologue.
    p = ALIGN(p, 8);
    // TODO(jart): Figure out why not skewing corrupts pe import table
    in->we_must_skew_pe_vaspace =
        1 || ROUNDUP(p - prologue + in->size_of_pe_headers,
                     (int)in->pe->OptionalHeader.FileAlignment) > in->minload;
    if (!in->we_must_skew_pe_vaspace) {
      in->pe_e_lfanew = p - prologue;
      in->pe_SizeOfHeaders = in->pe->OptionalHeader.SizeOfHeaders;
      struct NtImageNtHeaders *pe2 = (struct NtImageNtHeaders *)p;
      p += in->size_of_pe_headers;
      memcpy(pe2, in->pe, in->size_of_pe_headers);
      in->pe = pe2;
      p = ALIGN(p, (int)in->pe->OptionalHeader.FileAlignment);
    }
  }

  return p;
}

// on the third pass, we stop generating prologue content and begin
// focusing on embedding the executable files passed via the flags.
static Elf64_Off ThirdPass(Elf64_Off offset, struct Input *in) {
  int i;
  Elf64_Addr vaddr;
  Elf64_Xword image_align;

  // determine microprocessor page size
  unsigned long pagesz;
  if (in->elf->e_machine == EM_AARCH64) {
    pagesz = 16384;  // apple m1 (xnu, linux)
  } else {           //
    pagesz = 4096;   // x86-64, raspberry pi
  }

  // determine file alignment of image
  //
  //   1. elf requires that file offsets be congruent with virtual
  //      addresses modulo the cpu page size. so when adding stuff
  //      to the beginning of the file, we need to round up to the
  //      page size in order to maintain elf's invariant, although
  //      no such roundup is required on the program segments once
  //      the invariant is restored. elf loaders will happily mmap
  //      program headers from arbitrary file intervals (which may
  //      overlap) onto arbitrarily virtual intervals (which don't
  //      need to be contiguous). in order to do that, the loaders
  //      will generally use unix's mmap() function which needs to
  //      have page aligned addresses. since program headers start
  //      and stop at potentially any byte, elf loaders shall work
  //      around the mmap() requirements by rounding out intervals
  //      as necessary in order to ensure both the mmap() size and
  //      offset parameters are page size aligned. this means with
  //      elf, we never need to insert any empty space into a file
  //      when we don't want to. we can simply allow the offset to
  //      to drift apart from the virtual offset.
  //
  //   2. pe doesn't care about congruency and instead specifies a
  //      second kind of alignment. the minimum alignment of files
  //      is 512 because that's what dos used. where it gets hairy
  //      with pe is SizeOfHeaders which has complex and confusing
  //      requirements we're still figuring out. in cases where we
  //      determine that it's necessary to skew the pe image base,
  //      windows imposes a separate 64kb alignment requirement on
  //      the image base.
  //
  //   3. macho is the strictest executable format. xnu won't even
  //      load executables that do anything special with alignment
  //      which must conform to the cpu page size. it applies with
  //      both the image base and the segments. xnu also wants the
  //      segments to be contiguous similar to portable executable
  //      except that applies to both the file and virtual spaces.
  //
  vaddr = 0;
  if (in->we_are_generating_pe) {
    if (in->we_must_skew_pe_vaspace) {
      image_align = 65535;
    } else {
      image_align = in->pe->OptionalHeader.FileAlignment;
    }
    image_align = MAX(image_align, pagesz);
  } else {
    image_align = pagesz;
  }
  while ((offset & (image_align - 1)) != (vaddr & (image_align - 1))) {
    ++offset;
  }

  // fixup the program header offsets
  for (i = 0; i < in->offsetrelocs.n; ++i) {
    *in->offsetrelocs.p[i] += offset - in->minload;
  }

  // fixup macho loads to account for skew
  if (in->first_macho_load) {
    if (in->first_macho_load->offset) {
      in->first_macho_load->vaddr -= in->first_macho_load->offset;
      in->first_macho_load->memsz += in->first_macho_load->offset;
      in->first_macho_load->filesz += in->first_macho_load->offset;
      in->first_macho_load->offset = 0;
    }
  }

  // fixup and link assembler generated portable executable headers
  if (in->we_are_generating_pe) {
    if (in->we_must_skew_pe_vaspace) {
      LinkPortableExecutableHeader(in, offset, offset);
    } else {
      LinkPortableExecutableHeader(in, 0, offset);
      WRITE32LE(r_off32_e_lfanew, in->pe_e_lfanew);
      in->pe->OptionalHeader.SizeOfHeaders = in->pe_SizeOfHeaders;
    }
  }

  // copy the stripped executable into the output
  Pwrite(in->map + in->minload, in->maxload - in->minload, offset);
  offset += in->maxload - in->minload;

  return offset;
}

static void OpenInput(const char *path) {
  int fd;
  struct Input *in;
  if (inputs.n == ARRAYLEN(inputs.p)) Die(prog, "too many input files");
  in = inputs.p + inputs.n++;
  in->path = path;
  if ((fd = open(path, O_RDONLY)) == -1) DieSys(path);
  if ((in->size = lseek(fd, 0, SEEK_END)) == -1) DieSys(path);
  in->map = mmap(0, in->size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
  if (in->map == MAP_FAILED) DieSys(path);
  if (!IsElf64Binary(in->elf, in->size)) Die(path, "not an elf64 binary");
  HashInput(in->map, in->size);
  close(fd);
}

static char *GenerateScriptIfMachine(char *p, struct Input *in) {
  if (in->elf->e_machine == EM_NEXGEN32E) {
    return stpcpy(p, "if [ \"$m\" = x86_64 ] || [ \"$m\" = amd64 ]; then\n");
  } else if (in->elf->e_machine == EM_AARCH64) {
    return stpcpy(p, "if [ \"$m\" = aarch64 ] || [ \"$m\" = arm64 ]; then\n");
  } else if (in->elf->e_machine == EM_PPC64) {
    return stpcpy(p, "if [ \"$m\" = ppc64le ]; then\n");
  } else {
    Die(in->path, "unsupported cpu architecture");
  }
}

static char *FinishGeneratingDosHeader(char *p) {
  p = WRITE16LE(p, 0x1000);  // 10: MZ: lowers upper bound load / 16
  p = WRITE16LE(p, 0xf800);  // 12: MZ: roll greed on bss
  p = WRITE16LE(p, 0);       // 14: MZ: lower bound on stack segment
  p = WRITE16LE(p, 0);       // 16: MZ: initialize stack pointer
  p = WRITE16LE(p, 0);       // 18: MZ: ∑bₙ checksum don't bother
  p = WRITE16LE(p, 0x0100);  // 20: MZ: initial ip value
  p = WRITE16LE(p, 0x0800);  // 22: MZ: increases cs load lower bound
  p = WRITE16LE(p, 0x0040);  // 24: MZ: reloc table offset
  p = WRITE16LE(p, 0);       // 26: MZ: overlay number
  p = WRITE16LE(p, 0);       // 28: MZ: overlay information
  p = WRITE16LE(p, 0);       // 30
  p = WRITE16LE(p, 0);       // 32
  p = WRITE16LE(p, 0);       // 34
  p = WRITE16LE(p, 0);       // 36
  p = WRITE16LE(p, 0);       // 38

  // terminate the shell quote started earlier in the ape magic. the big
  // concern with shell script quoting, is that binary content might get
  // generated in the dos stub which has an ascii value that is the same
  // as the end of quote. using a longer terminator reduces it to a very
  // low order of probability. tacking on an unpredictable deterministic
  // value makes it nearly impossible to break even with intent for that
  // another terminator exists, which dates back to every version of ape
  // ever released, which is "#'\"\n". programs wanting a simple way for
  // scanning over the actually portable executable mz stub can use that
  char *q = ape_heredoc;
  q = stpcpy(q, "justine");
  uint64_t w = READ64LE(hashpool);
  for (int i = 0; i < 6; ++i) {
    *q++ = "0123456789abcdefghijklmnopqrstuvwxyz"[w % 36];
    w /= 36;
  }
  p = stpcpy(p, "' <<'");
  p = stpcpy(p, ape_heredoc);
  p = stpcpy(p, "'\n");

  // here's our first unpredictable binary value, which is the offset of
  // the portable executable headers.
  r_off32_e_lfanew = p;
  return WRITE32LE(p, 0);
}

static char *CopyMasterBootRecord(char *p) {
  Elf64_Off off;
  Elf64_Shdr *shdr;
  struct Input *in;
  struct Elf64_Sym *stub;
  unassert(p == prologue + 64);
  if ((in = GetInput(EM_NEXGEN32E)) && (stub = GetElfSymbol(in, "stub"))) {
    shdr = GetElfSectionHeaderAddress(in->elf, in->size, stub->st_shndx);
    if (!shdr || ckd_sub(&off, stub->st_value, shdr->sh_addr) ||
        ckd_add(&off, off, shdr->sh_offset) || off >= in->size ||
        off + (512 - 64) > in->size) {
      Die(in->path, "corrupt symbol: stub");
    }
    p = mempcpy(p, in->map + off, 512 - 64);
  }
  return p;
}

static bool IsElfCarryingZipAssets(struct Input *in) {
  return !!FindElfSectionByName(in->elf, in->size,
                                GetElfSectionNameStringTable(in->elf, in->size),
                                ".zip");
}

static bool IsZipFileNamed(unsigned char *lfile, const char *name) {
  return ZIP_LFILE_NAMESIZE(lfile) == strlen(name) &&
         !memcmp(ZIP_LFILE_NAME(lfile), name, ZIP_LFILE_NAMESIZE(lfile));
}

static bool IsZipFileNameEqual(unsigned char *lfile1, unsigned char *lfile2) {
  return ZIP_LFILE_NAMESIZE(lfile1) == ZIP_LFILE_NAMESIZE(lfile2) &&
         !memcmp(ZIP_LFILE_NAME(lfile1), ZIP_LFILE_NAME(lfile2),
                 ZIP_LFILE_NAMESIZE(lfile1));
}

static bool IsZipFileContentEqual(unsigned char *lfile1,
                                  unsigned char *lfile2) {
  return ZIP_LFILE_CRC32(lfile1) == ZIP_LFILE_CRC32(lfile2) &&
         ZIP_LFILE_COMPRESSEDSIZE(lfile1) == ZIP_LFILE_COMPRESSEDSIZE(lfile2) &&
         !memcmp(lfile1 + ZIP_LFILE_HDRSIZE(lfile1),
                 lfile2 + ZIP_LFILE_HDRSIZE(lfile2),
                 ZIP_LFILE_COMPRESSEDSIZE(lfile1));
}

static bool HasZipAsset(unsigned char *lfile) {
  int i;
  for (i = 0; i < assets.n; ++i) {
    if (IsZipFileNameEqual(lfile, assets.p[i].lfile)) {
      if (IsZipFileContentEqual(lfile, assets.p[i].lfile)) {
        return true;
      } else {
        Die(outpath, "multiple ELF files define assets at the same ZIP path, "
                     "but these duplicated assets can't be merged because they "
                     "don't have exactly the same content; perhaps the build "
                     "system is in an inconsistent state");
      }
    }
  }
  return false;
}

static unsigned char *GetZipEndOfCentralDirectory(struct Input *in) {
  unsigned char *img = in->umap;
  unsigned char *eocd = img + in->size - kZipCdirHdrMinSize;
  unsigned char *stop = MAX(eocd - 65536, img);
  for (; eocd >= stop; --eocd) {
    if (READ32LE(eocd) == kZipCdirHdrMagic) {
      if (IsZipEocd32(img, in->size, eocd - img) != kZipOk) {
        Die(in->path, "found corrupted ZIP EOCD header at the end of an "
                      "ELF file with a .zip section");
      }
      return eocd;
    }
  }
  Die(in->path, "zip eocd not found in last 64kb of elf even though a .zip "
                "section exists; you may need to run fixupobj.com");
}

static void IndexZipAssetsFromElf(struct Input *in) {
  unsigned char *lfile;
  unsigned char *img = in->umap;
  unsigned char *eof = img + in->size;
  unsigned char *eocd = GetZipEndOfCentralDirectory(in);
  unsigned char *cdir = img + ZIP_CDIR_OFFSET(eocd);
  unsigned char *cfile = cdir;
  unsigned char *stop = cdir + ZIP_CDIR_SIZE(eocd);
  if (stop > eof) {
    Die(in->path, "zip central directory size overlaps image eof");
  }
  for (; cfile < stop; cfile += ZIP_CFILE_HDRSIZE(cfile)) {
    if (cfile + kZipCfileHdrMinSize > eof ||  //
        cfile + ZIP_CFILE_HDRSIZE(cfile) > eof) {
      Die(in->path, "zip central directory entry overlaps image eof");
    }
    if (READ32LE(cfile) != kZipCfileHdrMagic) {
      Die(in->path, "zip central directory entry magic corrupted");
    }
    if (ZIP_CFILE_OFFSET(cfile) == 0xFFFFFFFF) {
      Die(in->path, "zip64 file format not supported at link time");
    }
    if ((lfile = img + ZIP_CFILE_OFFSET(cfile)) > eof) {
      Die(in->path, "zip offset to local file points past image eof");
    }
    if (lfile + kZipLfileHdrMinSize > eof ||  //
        lfile + ZIP_LFILE_HDRSIZE(lfile) > eof) {
      Die(in->path, "zip local file header overlaps image eof");
    }
    if (READ32LE(lfile) != kZipLfileHdrMagic) {
      Die(in->path, "zip local file entry magic corrupted");
    }
    if (ZIP_LFILE_COMPRESSEDSIZE(lfile) == 0xFFFFFFFF ||
        ZIP_LFILE_UNCOMPRESSEDSIZE(lfile) == 0xFFFFFFFF) {
      Die(in->path, "zip64 file format not supported at link time");
    }
    if (lfile + ZIP_LFILE_SIZE(lfile) > eof) {
      Die(in->path, "zip local file content overlaps image eof");
    }
    if (!IsZipFileNamed(lfile, ".symtab") && !HasZipAsset(lfile)) {
      AppendZipAsset(lfile, cfile);
    }
  }
}

static void CopyZips(Elf64_Off offset) {
  int i;
  for (i = 0; i < inputs.n; ++i) {
    struct Input *in = inputs.p + i;
    if (IsElfCarryingZipAssets(in)) {
      IndexZipAssetsFromElf(in);
    }
  }
  if (!assets.n) {
    return;  // nothing to do
  }
  if (offset + assets.total_local_file_bytes + assets.total_centraldir_bytes +
          kZipCdirHdrMinSize >
      INT_MAX) {
    Die(outpath, "more than 2gb of zip files not supported yet");
  }
  Elf64_Off lp = offset;
  Elf64_Off midpoint = offset + assets.total_local_file_bytes;
  Elf64_Off cp = midpoint;
  for (i = 0; i < assets.n; ++i) {
    unsigned char *cfile = assets.p[i].cfile;
    WRITE32LE(cfile + kZipCfileOffsetOffset, lp);
    unsigned char *lfile = assets.p[i].lfile;
    Pwrite(lfile, ZIP_LFILE_SIZE(lfile), lp);
    lp += ZIP_LFILE_SIZE(lfile);
    Pwrite(cfile, ZIP_CFILE_HDRSIZE(cfile), cp);
    cp += ZIP_CFILE_HDRSIZE(cfile);
  }
  unassert(lp == midpoint);
  unsigned char eocd[kZipCdirHdrMinSize] = {0};
  WRITE32LE(eocd, kZipCdirHdrMagic);
  WRITE32LE(eocd + kZipCdirRecordsOnDiskOffset, assets.n);
  WRITE32LE(eocd + kZipCdirRecordsOffset, assets.n);
  WRITE32LE(eocd + kZipCdirSizeOffset, assets.total_centraldir_bytes);
  WRITE32LE(eocd + kZipCdirOffsetOffset,
            offset + assets.total_local_file_bytes);
  Pwrite(eocd, sizeof(eocd), cp);
}

int main(int argc, char *argv[]) {
  char *p;
  int i, j;
  Elf64_Off offset;
  Elf64_Xword prologue_bytes;

#ifdef MODE_DBG
  ShowCrashReports();
#endif

  prog = argv[0];
  if (!prog) prog = "apelink";

  // process flags
  GetOpts(argc, argv);

  // determine strategy
  //
  // if we're only targeting a single architecture, and we're not
  // targeting windows or macos then it's possible to make an elf
  // executable without a shell script.
  if (argc - optind == 1 && !(support_vector & (_HOSTWINDOWS | _HOSTXNU))) {
    strategy = kElf;
    loaders.n = 0;
  } else if (argc - optind == 1 &&
             !(support_vector & ~(_HOSTWINDOWS | _HOSTXNU))) {
    strategy = kPe;
    loaders.n = 0;
  } else if (inputs.n == 1 && support_vector == _HOSTXNU) {
    strategy = kMacho;
    loaders.n = 0;
  } else {
    strategy = kApe;
  }

  // open loaders
  for (i = 0; i < loaders.n; ++i) {
    OpenLoader(loaders.p + i);
  }
  for (i = 0; i < loaders.n; ++i) {
    for (j = i + 1; j < loaders.n; ++j) {
      if (loaders.p[i].os == loaders.p[j].os &&
          loaders.p[i].machine == loaders.p[j].machine) {
        Die(prog, "multiple ape loaders specified for the same platform");
      }
    }
  }

  // open input files
  for (i = optind; i < argc; ++i) {
    OpenInput(argv[i]);
  }
  for (i = 0; i < inputs.n; ++i) {
    for (j = i + 1; j < inputs.n; ++j) {
      if (inputs.p[i].elf->e_machine == inputs.p[j].elf->e_machine) {
        Die(prog, "multiple executables passed for the same machine type");
      }
    }
  }

  // validate input files
  for (i = 0; i < inputs.n; ++i) {
    struct Input *in = inputs.p + i;
    ValidateElfImage(in->elf, in->size, in->path, false);
  }

  // load symbols
  if (!want_stripped) {
    for (i = 0; i < inputs.n; ++i) {
      struct Input *in = inputs.p + i;
      if (GetElfSymbol(in, "__zipos_get")) {
        LoadSymbols(in->elf, in->size, in->path);
      }
    }
  }

  // generate the executable header
  p = prologue;
  if (strategy == kElf) {
    p = GenerateElf(p, inputs.p);
    if (support_vector & _HOSTMETAL) {
      p = CopyMasterBootRecord(p);
    }
  } else if (strategy == kMacho) {
    p = GenerateMacho(p, inputs.p);
  } else if (strategy == kPe) {
    p[0] = 'M';
    p[1] = 'Z';
    p += 64;
    r_off32_e_lfanew = p - 4;
  } else {
    assert(strategy == kApe);
    if (force_bypass_binfmt_misc) {
      p = stpcpy(p, "APEDBG='\n\n");
      if (support_vector & (_HOSTWINDOWS | _HOSTMETAL)) {
        p = FinishGeneratingDosHeader(p);
      }
    } else if (support_vector & _HOSTWINDOWS) {
      p = stpcpy(p, "MZqFpD='\n\n");
      p = FinishGeneratingDosHeader(p);
    } else {
      p = stpcpy(p, "jartsr='\n\n");
      if (support_vector & _HOSTMETAL) {
        p = FinishGeneratingDosHeader(p);
      }
    }
    if (support_vector & _HOSTMETAL) {
      p = CopyMasterBootRecord(p);
    }
    p = stpcpy(p, "\n");
    p = stpcpy(p, ape_heredoc);
    p = stpcpy(p, "\n");
    p = stpcpy(p, "#'\"\n");  // longstanding convention (see mz notes)
    p = stpcpy(p, "\n");
    if (custom_sh_code) {
      p = stpcpy(p, custom_sh_code);
      *p++ = '\n';
    }
    p = stpcpy(p, "o=$(command -v \"$0\")\n");

    // run this program using the systemwide ape loader if it exists
    if (loaders.n) {
      p = stpcpy(p, "[ x\"$1\" != x--assimilate ] && ");
    }
    if (!dont_path_lookup_ape_loader) {
      p = stpcpy(p, "type ape >/dev/null 2>&1 && "
                    "exec ape \"$o\" \"$@\"\n");
    }

    // otherwise try to use the ad-hoc self-extracted loader, securely
    if (loaders.n) {
      p = stpcpy(p, "t=\"${TMPDIR:-${HOME:-.}}/.ape-" APE_VERSION_STR "\"\n"
                    "[ x\"$1\" != x--assimilate ] && "
                    "[ -x \"$t\" ] && "
                    "exec \"$t\" \"$o\" \"$@\"\n");
    }

    // otherwise this is a fresh install so consider the platform
    p = stpcpy(p, "m=$(uname -m 2>/dev/null) || m=x86_64\n");
    if (support_vector & _HOSTXNU) {
      p = stpcpy(p, "if [ ! -d /Applications ]; then\n");
    }

    // generate shell script for elf operating systems
    //
    //   1. for classic ape binaries which assimilate, all we have to do
    //      is self-modify this executable file using the printf builtin
    //      and then we re-exec this file. we only need Elf64_Ehdr which
    //      is always exactly 64 bytes.
    //
    //   2. if an appropriate ape loader program was passed in the flags
    //      then we won't self-modify the executable, and we instead try
    //      to self-extract the loader binary to a safe location, and it
    //      is then used by re-launch this program. thanks to incredible
    //      greatness of the elf file format, our 12kb ape loader binary
    //      is able to run on all of our supported elf operating systems
    //      but, we do still need to embed multiple copies of ape loader
    //      when generating fat binaries that run on multiple cpu types.
    //
    for (i = 0; i < inputs.n; ++i) {
      struct Input *in = inputs.p + i;
      if (GetLoader(in->elf->e_machine, ~_HOSTXNU)) {
        // if an ape loader is available for this microprocessor for
        // this operating system, then put assimilate behind a flag.
        p = stpcpy(p, "if [ x\"$1\" = x--assimilate ]; then\n");
      }
      p = GenerateScriptIfMachine(p, in);  // if [ arch ]
      p = stpcpy(p, "exec 7<> \"$o\" || exit 121\n"
                    "printf '"
                    "\\177ELF"  // 0x0: ⌂ELF
                    "\\2"       //   4: long mode
                    "\\1"       //   5: little endian
                    "\\1");     //   6: elf v1.o
      p = EncodeWordAsPrintf(p, PickElfOsAbi(), 1);
      p = stpcpy(p, "\\0"           //   8: os/abi ver.
                    "\\0\\0\\0"     //   9: padding 3/7
                    "\\0\\0\\0\\0"  //      padding 4/7
                    "\\2\\0");      //  10: εxεcµταblε
      p = EncodeWordAsPrintf(p, in->elf->e_machine, 1);
      p = stpcpy(p, "\\0\\1\\0\\0\\0");  // elf v1.o
      p = EncodeWordAsPrintf(p, in->elf->e_entry, 8);
      in->printf_phoff = p;
      p = stpcpy(p, "\\000\\000\\000\\000\\0\\0\\0\\0");
      p = stpcpy(p, "\\0\\0\\0\\0\\0\\0\\0\\0"  // 28: e_shoff
                    "\\0\\0\\0\\0"              // 30: e_flags
                    "\\100\\0"                  // 34: e_ehsize
                    "\\70\\0");                 // 36: e_phentsize
      in->printf_phnum = p;
      p = stpcpy(p, "\\000\\000"  // 38: e_phnum
                    "\\0\\0"      // 3a: e_shentsize
                    "\\0\\0"      // 3c: e_shnum
                    "\\0\\0"      // 3e: e_shstrndx
                    "' >&7\n"
                    "exec 7<&-\n");
      if (GetLoader(in->elf->e_machine, ~_HOSTXNU)) {
        p = stpcpy(p, "fi\n");  // </GenerateScriptIfMachine>
        p = stpcpy(p, "exit\n");
        p = stpcpy(p, "fi\n");  // </--assimilate>
      } else {
        p = stpcpy(p, "exec \"$o\" \"$@\"\n");
        p = stpcpy(p, "fi\n");  // </GenerateScriptIfMachine>
      }
    }

    // generate shell script for xnu
    //
    //   1. for classic ape binaries which assimilate, ape will favor
    //      using `dd`, to memmove the embedded mach-o headers to the
    //      front of the this file, before re-exec'ing itself. that's
    //      because ape was originally designed using a linker script
    //      which couldn't generate printf statements outputting data
    //      of variable length. we are now stuck with the `dd` design
    //      because tools like assimilate.com expect it there. that's
    //      how it's able to determine the offset of the macho header
    //
    //   2. the x86 elf ape loader executable is able to runs on xnu,
    //      but we need to use an additional `dd` command after it is
    //      copied, which memmove's the embedded macho-o headers into
    //      the first bytes of the file.
    //
    //   3. xnu on arm64 has strict code signing requirements, and it
    //      means we can't embed a precompiled ape loader program :'(
    //      so what we do is embed the the ape loader source code and
    //      then hope the user has a c compiler installed, which will
    //      let our shell script compile the ape loader on first run.
    //
    if (support_vector & _HOSTXNU) {
      bool gotsome;
      p = stpcpy(p, "else\n");  // if [ -d /Applications ]; then

      // output native mach-o morph
      for (i = 0; i < inputs.n; ++i) {
        struct Input *in = inputs.p + i;
        if (in->elf->e_machine != EM_NEXGEN32E) continue;
        if (GetLoader(in->elf->e_machine, _HOSTXNU)) {
          p = stpcpy(p, "if [ x\"$1\" = x--assimilate ]; then\n");
        }
        p = GenerateScriptIfMachine(p, in);
        p = stpcpy(p, "dd if=\"$o\" of=\"$o\" bs=1");
        p = stpcpy(p, " skip=");
        in->ddarg_macho_skip = p;
        p = GenerateDecimalOffsetRelocation(p);
        p = stpcpy(p, " count=");
        in->ddarg_macho_count = p;
        p = GenerateDecimalOffsetRelocation(p);
        p = stpcpy(p, " conv=notrunc 2>/dev/null ||exit\n");
        if (GetLoader(in->elf->e_machine, _HOSTXNU)) {
          p = stpcpy(p, "exit\n");  // --assimilate
          p = stpcpy(p, "fi\n");
        } else {
          p = stpcpy(p, "exec \"$o\" \"$@\"\n");
        }
        p = stpcpy(p, "fi\n");
        gotsome = true;
        break;
      }

      // output mach-o ape loader binary
      for (i = 0; i < inputs.n; ++i) {
        struct Loader *loader;
        struct Input *in = inputs.p + i;
        if ((loader = GetLoader(in->elf->e_machine, _HOSTXNU))) {
          loader->used = true;
          p = GenerateScriptIfMachine(p, in);  // <if-machine>
          p = stpcpy(p, "mkdir -p \"${t%/*}\" ||exit\n"
                        "dd if=\"$o\"");
          p = stpcpy(p, " skip=");
          loader->ddarg_skip1 = p;
          p = GenerateDecimalOffsetRelocation(p);
          p = stpcpy(p, " count=");
          loader->ddarg_size1 = p;
          p = GenerateDecimalOffsetRelocation(p);
          p = stpcpy(p, " bs=1 2>/dev/null | gzip -dc >\"$t.$$\" ||exit\n");
          if (loader->macho_offset) {
            p = stpcpy(p, "dd if=\"$t.$$\" of=\"$t.$$\"");
            p = stpcpy(p, " skip=");
            p = FormatInt32(p, loader->macho_offset / 64);
            p = stpcpy(p, " count=");
            p = FormatInt32(p, ROUNDUP(loader->macho_length, 64) / 64);
            p = stpcpy(p, " bs=64 conv=notrunc 2>/dev/null ||exit\n");
          }
          p = stpcpy(p, "chmod 755 \"$t.$$\" ||exit\n"
                        "mv -f \"$t.$$\" \"$t\" ||exit\n");
          p = stpcpy(p, "exec \"$t\" \"$o\" \"$@\"\n"
                        "fi\n");  // </if-machine>
          gotsome = true;
        }
      }

      if (macos_silicon_loader_source_path) {
        struct Input *in;
        if (!(in = GetInput(EM_AARCH64))) {
          Die(macos_silicon_loader_source_path,
              "won't embed macos arm64 ape loader source code because a native "
              "build of your program for aarch64 wasn't passed as an argument");
        }
        p = GenerateScriptIfMachine(p, in);  // <if-machine>
        p = stpcpy(p, "if ! type cc >/dev/null 2>&1; then\n"
                      "echo \"$0: please run: xcode-select --install\" >&2\n"
                      "exit 1\n"
                      "fi\n"
                      "mkdir -p \"${t%/*}\" ||exit\n"
                      "dd if=\"$o\"");
        p = stpcpy(p, " skip=");
        macos_silicon_loader_source_ddarg_skip = p;
        p = GenerateDecimalOffsetRelocation(p);
        p = stpcpy(p, " count=");
        macos_silicon_loader_source_ddarg_size = p;
        p = GenerateDecimalOffsetRelocation(p);
        p = stpcpy(p, " bs=1 2>/dev/null | gzip -dc >\"$t.c.$$\" ||exit\n"
                      "mv -f \"$t.c.$$\" \"$t.c\" ||exit\n"
                      "cc -w -O -o \"$t.$$\" \"$t.c\" ||exit\n"
                      "mv -f \"$t.$$\" \"$t\" ||exit\n"
                      "exec \"$t\" \"$o\" \"$@\"\n"
                      "fi\n");  // </if-machine>
        gotsome = true;
      }

      if (!gotsome) {
        p = stpcpy(p, "true\n");
      }
      p = stpcpy(p, "fi\n");  // if [ -d /Applications ]; then
    } else {
      macos_silicon_loader_source_path = 0;
    }

    // extract the ape loader for open platforms
    if (inputs.n && (support_vector & _HOSTXNU)) {
      p = stpcpy(p, "if [ ! -d /Applications ]; then\n");
    }
    for (i = 0; i < inputs.n; ++i) {
      struct Loader *loader;
      struct Input *in = inputs.p + i;
      if ((loader = GetLoader(in->elf->e_machine, ~_HOSTXNU))) {
        loader->used = true;
        p = GenerateScriptIfMachine(p, in);
        p = stpcpy(p, "mkdir -p \"${t%/*}\" ||exit\n"
                      "dd if=\"$o\"");
        p = stpcpy(p, " skip=");
        loader->ddarg_skip2 = p;
        p = GenerateDecimalOffsetRelocation(p);
        p = stpcpy(p, " count=");
        loader->ddarg_size2 = p;
        p = GenerateDecimalOffsetRelocation(p);
        p = stpcpy(p, " bs=1 2>/dev/null | gzip -dc >\"$t.$$\" ||exit\n"
                      "chmod 755 \"$t.$$\" ||exit\n"
                      "mv -f \"$t.$$\" \"$t\" ||exit\n");
        p = stpcpy(p, "exec \"$t\" \"$o\" \"$@\"\n"
                      "fi\n");
      }
    }
    if (inputs.n && (support_vector & _HOSTXNU)) {
      p = stpcpy(p, "fi\n");
    }

    // the final failure
    p = stpcpy(p, "echo \"$0: this ape program lacks $m support\" >&2\n");
    p = stpcpy(p, "exit 127\n\n\n\n");
  }
  p = GenerateElfNotes(p);
  for (i = 0; i < inputs.n; ++i) {
    p = SecondPass(p, inputs.p + i);
  }
  for (i = 0; i < inputs.n; ++i) {
    p = SecondPass2(p, inputs.p + i);
  }
  prologue_bytes = p - prologue;

  // write the output file
  if ((outfd = creat(outpath, 0755)) == -1) {
    DieSys(outpath);
  }
  offset = prologue_bytes;
  for (i = 0; i < inputs.n; ++i) {
    offset = ThirdPass(offset, inputs.p + i);
  }

  // concatenate ape loader binaries
  for (i = 0; i < loaders.n; ++i) {
    char *compressed_data;
    size_t compressed_size;
    struct Loader *loader;
    loader = loaders.p + i;
    if (!loader->used) continue;
    compressed_data = Gzip(loader->map, loader->size, &compressed_size);
    if (loader->ddarg_skip1) {
      FixupWordAsDecimal(loader->ddarg_skip1, offset);
    }
    if (loader->ddarg_size1) {
      FixupWordAsDecimal(loader->ddarg_size1, compressed_size);
    }
    if (loader->ddarg_skip2) {
      FixupWordAsDecimal(loader->ddarg_skip2, offset);
    }
    if (loader->ddarg_size2) {
      FixupWordAsDecimal(loader->ddarg_size2, compressed_size);
    }
    Pwrite(compressed_data, compressed_size, offset);
    offset += compressed_size;
    free(compressed_data);
  }

  // concatenate ape loader source code
  if (macos_silicon_loader_source_path) {
    char *compressed_data;
    size_t compressed_size;
    compressed_data =
        Gzip(macos_silicon_loader_source_text,
             strlen(macos_silicon_loader_source_text), &compressed_size);
    FixupWordAsDecimal(macos_silicon_loader_source_ddarg_skip, offset);
    FixupWordAsDecimal(macos_silicon_loader_source_ddarg_size, compressed_size);
    Pwrite(compressed_data, compressed_size, offset);
    offset += compressed_size;
    free(compressed_data);
  }

  // add the zip files
  CopyZips(offset);

  // write the header
  Pwrite(prologue, prologue_bytes, 0);

  if (close(outfd)) {
    DieSys(outpath);
  }
}
