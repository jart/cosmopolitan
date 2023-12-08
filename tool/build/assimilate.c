/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/elf/def.h"
#include "libc/elf/struct/ehdr.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/serialize.h"
#include "libc/limits.h"
#include "libc/macho.internal.h"
#include "libc/macros.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/stdckdint.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/msync.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "third_party/getopt/getopt.internal.h"
#include "third_party/regex/regex.h"

#define VERSION                                    \
  "actually portable executable assimilate v1.6\n" \
  "copyright 2023 justine alexandra roberts tunney\n"

#define USAGE                                               \
  " [-xapembfchv] [-o PATH] FILE...\n"                      \
  "  -h          show help\n"                               \
  "  -v          show version\n"                            \
  "  -f          ignore soft errors\n"                      \
  "  -b          don't remove freebsd from elf os/abi\n"    \
  "  -c          clobber input path w/o making backup\n"    \
  "  -e          convert to elf regardless of host os\n"    \
  "  -m          convert to macho regardless of host os\n"  \
  "  -x          convert to amd64 regardless of host cpu\n" \
  "  -a          convert to arm64 regardless of host cpu\n" \
  "  -p          convert to ppc64 regardless of host cpu\n" \
  "  -o PATH     write modified binary to different file\n"

#define ARCH_NATIVE 0
#define ARCH_AMD64  1
#define ARCH_ARM64  2
#define ARCH_PPC64  3

#define FORMAT_NATIVE 0
#define FORMAT_ELF    1
#define FORMAT_MACHO  2
#define FORMAT_PE     3

static int g_arch;
static int g_format;
static bool g_force;
static bool g_clobber;
static bool g_freebsd;
static const char *prog;
static const char *path;
static const char *outpath;
static bool got_format_flag;
static char bakpath[PATH_MAX];

static wontreturn void Die(const char *thing, const char *reason) {
  const char *native_explainer;
  if (got_format_flag) {
    native_explainer = "";
  } else if (IsXnu()) {
    native_explainer = " (the host os uses macho natively)";
  } else if (IsLinux() || IsFreebsd() || IsNetbsd() || IsOpenbsd()) {
    native_explainer = " (the host os uses elf natively)";
  } else {
    native_explainer = " (the host os uses pe natively)";
  }
  tinyprint(2, thing, ": ", reason, native_explainer, "\n", NULL);
  exit(1);
}

static wontreturn void DieSys(const char *thing) {
  perror(thing);
  exit(1);
}

static int Atoi(const char *s) {
  int x;
  if ((x = atoi(s)) == INT_MAX) {
    Die(path, "integer overflow parsing ape macho dd argument");
  }
  return x;
}

static void GetOpts(int argc, char *argv[]) {
  int opt;
  if (IsFreebsd()) {
    g_freebsd = true;
  }
  while ((opt = getopt(argc, argv, "hvfemxapbco:")) != -1) {
    switch (opt) {
      case 'f':
        g_force = true;
        break;
      case 'c':
        g_clobber = true;
        break;
      case 'b':
        g_freebsd = true;
        break;
      case 'o':
        outpath = optarg;
        break;
      case 'e':
        g_format = FORMAT_ELF;
        got_format_flag = true;
        break;
      case 'm':
        g_format = FORMAT_MACHO;
        got_format_flag = true;
        break;
      case 'x':
        g_arch = ARCH_AMD64;
        break;
      case 'a':
        g_arch = ARCH_ARM64;
        break;
      case 'p':
        g_arch = ARCH_PPC64;
        break;
      case 'v':
        tinyprint(1, VERSION, NULL);
        exit(0);
      case 'h':
        tinyprint(1, VERSION, "usage: ", prog, USAGE, NULL);
        exit(0);
      default:
        tinyprint(2, VERSION, "usage: ", prog, USAGE, NULL);
        exit(1);
    }
  }
  if (optind == argc) {
    Die(prog, "missing operand");
  }
  if (g_format == FORMAT_NATIVE) {
    if (IsXnu()) {
      g_format = FORMAT_MACHO;
    } else if (IsLinux() || IsFreebsd() || IsNetbsd() || IsOpenbsd()) {
      g_format = FORMAT_ELF;
    } else {
      g_format = FORMAT_PE;
    }
  }
  if (g_arch == ARCH_NATIVE) {
#ifdef __aarch64__
    g_arch = ARCH_ARM64;
#elif defined(__powerpc64__)
    g_arch = ARCH_PPC64;
#else
    g_arch = ARCH_AMD64;
#endif
  }
  if (g_format == FORMAT_PE && g_arch != ARCH_AMD64) {
    Die(prog, "native non-x86 on windows not supported yet");
  }
}

static int GetElfArch(void) {
  switch (g_arch) {
    case ARCH_AMD64:
      return EM_NEXGEN32E;
    case ARCH_ARM64:
      return EM_AARCH64;
    case ARCH_PPC64:
      return EM_PPC64;
    default:
      unassert(false);
  }
}

static void GetElfHeader(Elf64_Ehdr *ehdr, const char *image, size_t n) {
  int c, i;
  const char *p, *e;
  for (p = image, e = p + MIN(n, 8192); p < e; ++p) {
  TryAgain:
    if (READ64LE(p) != READ64LE("printf '")) continue;
    for (i = 0, p += 8; p + 3 < e && (c = *p++) != '\'';) {
      if (c == '\\') {
        if ('0' <= *p && *p <= '7') {
          c = *p++ - '0';
          if ('0' <= *p && *p <= '7') {
            c *= 8;
            c += *p++ - '0';
            if ('0' <= *p && *p <= '7') {
              c *= 8;
              c += *p++ - '0';
            }
          }
        }
      }
      if (i < 64) {
        ((char *)ehdr)[i++] = c;
      } else {
        goto TryAgain;
      }
    }
    if (i != sizeof(*ehdr) ||                              //
        READ32LE(ehdr->e_ident) != READ32LE("\177ELF") ||  //
        ehdr->e_ident[EI_CLASS] == ELFCLASS32 ||           //
        ehdr->e_machine != GetElfArch()) {
      goto TryAgain;
    }
    return;
  }
  switch (g_arch) {
    case ARCH_AMD64:
      Die(path, "printf statement not found in first 8192 bytes of image "
                "containing elf64 ehdr for amd64");
    case ARCH_ARM64:
      Die(path, "printf statement not found in first 8192 bytes of image "
                "containing elf64 ehdr for arm64");
    case ARCH_PPC64:
      Die(path, "printf statement not found in first 8192 bytes of image "
                "containing elf64 ehdr for ppc64");
    default:
      unassert(false);
  }
}

static int GetMachoArch(void) {
  switch (g_arch) {
    case ARCH_AMD64:
      return MAC_CPU_NEXGEN32E;
    case ARCH_ARM64:
      return MAC_CPU_ARM64;
    case ARCH_PPC64:
      return MAC_CPU_POWERPC64;
    default:
      unassert(false);
  }
}

static void GetMachoPayload(const char *image, size_t imagesize,
                            int *out_offset, int *out_size) {
  regex_t rx;
  const char *script;
  regmatch_t rm[1 + 13] = {0};
  int rc, skip, count, bs, offset, size;

  if ((script = memmem(image, MIN(imagesize, 4096), "'\n#'\"\n", 6))) {
    script += 6;
  } else if ((script = memmem(image, MIN(imagesize, 4096), "#'\"\n", 4))) {
    script += 4;
  } else {
    Die(path, "ape shell script not found");
  }

  // the ape shell script has always historically used `dd` to
  // assimilate binaries to the mach-o file format but we have
  // formatted the arguments in a variety of different ways eg
  //
  //   - `arg="  9293"` is how we originally had ape do it
  //   - `arg=$((  9293))` b/c busybox sh disliked quoted space
  //   - `arg=9293  ` is generated by modern apelink.com program
  //
  unassert(regcomp(&rx,
                   "bs="              // dd block size arg
                   "(['\"] *)?"       //   #1 optional quote w/ space
                   "(\\$\\(\\( *)?"   //   #2 optional math w/ space
                   "([[:digit:]]+)"   //   #3
                   "( *\\)\\))?"      //   #4 optional math w/ space
                   "( *['\"])?"       //   #5 optional quote w/ space
                   " +"               //
                   "skip="            // dd skip arg
                   "(['\"] *)?"       //   #6 optional quote w/ space
                   "(\\$\\(\\( *)?"   //   #7 optional math w/ space
                   "([[:digit:]]+)"   //   #8
                   "( *\\)\\))?"      //   #9 optional math w/ space
                   "( *['\"])?"       //  #10 optional quote w/ space
                   " +"               //
                   "count="           // dd count arg
                   "(['\"] *)?"       //  #11 optional quote w/ space
                   "(\\$\\(\\( *)?"   //  #12 optional math w/ space
                   "([[:digit:]]+)",  //  #13
                   REG_EXTENDED) == REG_OK);

  int i = 0;
TryAgain:
  rc = regexec(&rx, script + i, 1 + 13, rm, 0);
  if (rc != REG_OK) {
    unassert(rc == REG_NOMATCH);
    switch (g_arch) {
      case ARCH_AMD64:
        Die(path, "ape macho dd command for amd64 not found");
      case ARCH_ARM64:
        Die(path, "ape macho dd command for arm64 not found; by convention ape "
                  "executables are run on apple silicon only as elf binaries, "
                  "which are loaded by the ape-m1.c ape loader program; thus "
                  "consider passing the -ae flags to assimilate to arm64 elf");
      case ARCH_PPC64:
        Die(path, "ape macho dd command for ppc64 not found");
      default:
        unassert(false);
    }
  }
  i += rm[13].rm_eo;

  bs = Atoi(script + rm[3].rm_so);
  skip = Atoi(script + rm[8].rm_so);
  count = Atoi(script + rm[13].rm_so);

  if (ckd_mul(&offset, skip, bs)) {
    Die(path, "integer overflow computing ape macho dd offset");
  }
  if (ckd_mul(&size, count, bs)) {
    Die(path, "integer overflow computing ape macho dd size");
  }

  if (offset < 64) {
    Die(path, "ape macho dd offset must be at least 64");
  }
  if (offset >= imagesize) {
    Die(path, "ape macho dd offset points outside image");
  }
  if (size < 32) {
    Die(path, "ape macho dd size must be at least 32");
  }
  if (size > imagesize - offset) {
    Die(path, "ape macho dd size overlaps end of image");
    exit(1);
  }

  if (READ32LE(image + offset) != 0xFEEDFACE + 1 ||
      READ32LE(image + offset + 4) != GetMachoArch()) {
    goto TryAgain;
  }

  *out_offset = offset;
  *out_size = size;
  regfree(&rx);
}

static ssize_t Write(int fd, const void *data, size_t size) {
  ssize_t rc;
  const char *p, *e;
  for (p = data, e = p + size; p < e; p += (size_t)rc) {
    if ((rc = write(fd, p, e - p)) == -1) {
      return -1;
    }
  }
  return size;
}

static ssize_t Pwrite(int fd, const void *data, size_t size, uint64_t offset) {
  ssize_t rc;
  const char *p, *e;
  for (p = data, e = p + size; p < e; p += (size_t)rc, offset += (size_t)rc) {
    if ((rc = pwrite(fd, p, e - p, offset)) == -1) {
      return -1;
    }
  }
  return size;
}

static int GetMode(int fd) {
  struct stat st;
  if (fstat(fd, &st)) DieSys(path);
  return st.st_mode & 0777;
}

static void CopyFile(int infd, const char *map, size_t size,  //
                     const void *hdr, size_t hdrsize) {
  int outfd;
  if (!outpath) return;
  if ((outfd = creat(outpath, GetMode(infd))) == -1) DieSys(outpath);
  if (hdrsize && Write(outfd, hdr, hdrsize) == -1) DieSys(outpath);
  if (Write(outfd, map + hdrsize, size - hdrsize) == -1) DieSys(outpath);
  if (close(outfd)) DieSys(outpath);
}

static void WriteOutput(int infd, const char *map, size_t size,  //
                        const void *hdr, size_t hdrsize) {
  int outfd, oflags, omode;
  if (outpath) {
    CopyFile(infd, map, size, hdr, hdrsize);
  } else if (g_clobber) {
    if (Pwrite(infd, hdr, hdrsize, 0) == -1) DieSys(path);
  } else {
    omode = GetMode(infd);
    oflags = O_WRONLY | O_CREAT | (g_force ? O_TRUNC : O_EXCL);
    strlcat(bakpath, path, sizeof(bakpath));
    if (strlcat(bakpath, ".bak", sizeof(bakpath)) >= sizeof(bakpath)) {
      Die(path, "filename too long");
    }
    if ((outfd = open(bakpath, oflags, omode)) == -1) DieSys(bakpath);
    if (Write(outfd, map, size) == -1) DieSys(bakpath);
    if (close(outfd)) DieSys(bakpath);
    if (Pwrite(infd, hdr, hdrsize, 0) == -1) DieSys(path);
  }
}

static void AssimilateElf(int infd, const char *map, size_t size) {
  Elf64_Ehdr ehdr;
  GetElfHeader(&ehdr, map, size);
  if (!g_freebsd && ehdr.e_ident[EI_OSABI] == ELFOSABI_FREEBSD) {
    // none of the kernels except freebsd care about the osabi. however
    // gdb does whine about it when trying to debug ape elf programs on
    // other platforms like linux. so when assimilating, it makes sense
    // to fall back to the more generic system v os abi.
    ehdr.e_ident[EI_OSABI] = ELFOSABI_SYSV;
  }
  WriteOutput(infd, map, size, &ehdr, sizeof(ehdr));
}

static void AssimilateMacho(int infd, const char *map, size_t size) {
  int macho_offset, macho_size;
  GetMachoPayload(map, size, &macho_offset, &macho_size);
  WriteOutput(infd, map, size, map + macho_offset, macho_size);
}

static void Assimilate(void) {
  int fd;
  char *p;
  int oflags;
  ssize_t size;
  oflags = outpath ? O_RDONLY : O_RDWR;
  if ((fd = open(path, oflags)) == -1) DieSys(path);
  if ((size = lseek(fd, 0, SEEK_END)) == -1) DieSys(path);
  if (size < 64) Die(path, "ape executables must be at least 64 bytes");
  p = mmap(0, size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (p == MAP_FAILED) DieSys(path);

  if (READ32LE(p) == READ32LE("\177ELF")) {
    Elf64_Ehdr *ehdr;
    switch (g_format) {
      case FORMAT_ELF:
        ehdr = (Elf64_Ehdr *)p;
        if (ehdr->e_ident[EI_CLASS] == ELFCLASS32) {
          Die(path, "32-bit elf not supported");
        }
        switch (g_arch) {
          case ARCH_AMD64:
            switch (ehdr->e_machine) {
              case EM_NEXGEN32E:
                if (g_force) {
                  CopyFile(fd, p, size, 0, 0);
                  exit(0);
                } else {
                  Die(path, "already an elf amd64 executable");
                }
              case EM_AARCH64:
                Die(path, "can't assimilate elf arm64 to elf amd64");
              case EM_PPC64:
                Die(path, "can't assimilate elf ppc64 to elf amd64");
              default:
                Die(path, "elf has unsupported architecture");
            }
          case ARCH_ARM64:
            switch (ehdr->e_machine) {
              case EM_AARCH64:
                if (g_force) {
                  CopyFile(fd, p, size, 0, 0);
                  exit(0);
                } else {
                  Die(path, "already an elf arm64 executable");
                }
              case EM_NEXGEN32E:
                Die(path, "can't assimilate elf amd64 to elf arm64");
              case EM_PPC64:
                Die(path, "can't assimilate elf ppc64 to elf arm64");
              default:
                Die(path, "elf has unsupported architecture");
            }
          case ARCH_PPC64:
            switch (ehdr->e_machine) {
              case EM_PPC64:
                if (g_force) {
                  CopyFile(fd, p, size, 0, 0);
                  exit(0);
                } else {
                  Die(path, "already an elf ppc64 executable");
                }
              case EM_NEXGEN32E:
                Die(path, "can't assimilate elf amd64 to elf ppc64");
              case EM_AARCH64:
                Die(path, "can't assimilate elf arm64 to elf ppc64");
              default:
                Die(path, "elf has unsupported architecture");
            }
          default:
            unassert(false);
        }
      case FORMAT_MACHO:
        Die(path, "can't assimilate elf to macho");
      case FORMAT_PE:
        Die(path, "can't assimilate elf to pe (try elf2pe.com)");
      default:
        unassert(false);
    }
  }

  if (READ32LE(p) == 0xFEEDFACE + 1) {
    struct MachoHeader *macho;
    switch (g_format) {
      case FORMAT_MACHO:
        macho = (struct MachoHeader *)p;
        switch (g_arch) {
          case ARCH_AMD64:
            switch (macho->arch) {
              case MAC_CPU_NEXGEN32E:
                if (g_force) {
                  CopyFile(fd, p, size, 0, 0);
                  exit(0);
                } else {
                  Die(path, "already a macho amd64 executable");
                }
              case MAC_CPU_ARM64:
                Die(path, "can't assimilate macho arm64 to macho amd64");
              case MAC_CPU_POWERPC64:
                Die(path, "can't assimilate macho ppc64 to macho amd64");
              default:
                Die(path, "macho has unsupported architecture");
            }
          case ARCH_ARM64:
            switch (macho->arch) {
              case MAC_CPU_ARM64:
                if (g_force) {
                  CopyFile(fd, p, size, 0, 0);
                  exit(0);
                } else {
                  Die(path, "already a macho arm64 executable");
                }
              case MAC_CPU_NEXGEN32E:
                Die(path, "can't assimilate macho amd64 to macho arm64");
              case MAC_CPU_POWERPC64:
                Die(path, "can't assimilate macho ppc64 to macho arm64");
              default:
                Die(path, "macho has unsupported architecture");
            }
          case ARCH_PPC64:
            switch (macho->arch) {
              case MAC_CPU_POWERPC64:
                if (g_force) {
                  CopyFile(fd, p, size, 0, 0);
                  exit(0);
                } else {
                  Die(path, "already a macho ppc64 executable");
                }
              case MAC_CPU_NEXGEN32E:
                Die(path, "can't assimilate macho amd64 to macho ppc64");
              case MAC_CPU_ARM64:
                Die(path, "can't assimilate macho arm64 to macho ppc64");
              default:
                Die(path, "macho has unsupported architecture");
            }
          default:
            unassert(false);
        }
      case FORMAT_ELF:
        Die(path, "can't assimilate macho to elf");
      case FORMAT_PE:
        Die(path, "can't assimilate macho to pe");
      default:
        unassert(false);
    }
  }

  if (READ64LE(p) != READ64LE("MZqFpD='") &&  //
      READ64LE(p) != READ64LE("jartsr='") &&  //
      READ64LE(p) != READ64LE("APEDBG='")) {
    Die(path, "not an actually portable executable");
  }

  if (g_format == FORMAT_PE) {
    if (READ16LE(p) == READ16LE("MZ")) {
      if (g_force) {
        CopyFile(fd, p, size, 0, 0);
        exit(0);
      } else {
        Die(path, "this ape file is already a pe file");
      }
    } else {
      Die(path, "this ape file was built without pe support");
    }
  }

  if (g_format == FORMAT_ELF) {
    AssimilateElf(fd, p, size);
  } else if (g_format == FORMAT_MACHO) {
    AssimilateMacho(fd, p, size);
  }

  if (munmap(p, size)) DieSys(path);
  if (close(fd)) DieSys(path);
}

int main(int argc, char *argv[]) {
  prog = program_invocation_short_name;
  GetOpts(argc, argv);
  for (int i = optind; i < argc; ++i) {
    path = argv[i];
    Assimilate();
  }
}
