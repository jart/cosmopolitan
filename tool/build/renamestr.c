/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2024 Justine Alexandra Roberts Tunney                              │
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
#include "libc/elf/def.h"
#include "libc/elf/elf.h"
#include "libc/elf/scalar.h"
#include "libc/elf/struct/ehdr.h"
#include "libc/elf/struct/phdr.h"
#include "libc/intrin/kprintf.h"
#include "libc/macros.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/symbols.internal.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "third_party/getopt/getopt.internal.h"

#define VERSION      \
  "renamestr v0.1\n" \
  "https://github.com/jart/cosmopolitan\n"

#define MANUAL                                                \
  " -f FROM -t TO -o OUTPUT INPUT \n"                         \
  "\n"                                                        \
  "DESCRIPTION\n"                                             \
  "\n"                                                        \
  "  string replacement in ELF binary .rodata\n"              \
  "\n"                                                        \
  "  this program may be used to replace strings in the\n"    \
  "  .rodata sections of ELF binaries.\n"                     \
  "\n"                                                        \
  "FLAGS\n"                                                   \
  "\n"                                                        \
  "  -h         show usage\n"                                 \
  "\n"                                                        \
  "  -v         show version\n"                               \
  "\n"                                                        \
  "  -f FROM    source string to replace\n"                   \
  "\n"                                                        \
  "  -t TO      target string replacement. must be shorter\n" \
  "             than FROM string for replacement to work\n"   \
  "\n"                                                        \
  "  -o OUTPUT  output file\n"                                \
  "\n"                                                        \
  "  INPUT      ELF binary containing strings to replace\n"   \
  "\n"

static const char *prog;
static const char *exepath;
static Elf64_Shdr *rodata;
static char *rostart;
static char *roend;
static const char *outpath;
static int outfd;

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
  Die("renamestr", "out of memory");
}

static void *Malloc(size_t n) {
  void *p;
  if (!(p = malloc(n)))
    DieOom();
  return p;
}

static void *Realloc(void *p, size_t n) {
  if (!(p = realloc(p, n)))
    DieOom();
  return p;
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

struct Param {
  const char *from_string;
  size_t from_len;
  const char *to_string;
  size_t to_len;
  int count;
  char *roloc;
};

struct Params {
  int n;
  struct Param p[4];
};

static struct Params params;

static void GetOpts(int argc, char *argv[]) {
  int opt;
  bool partial = false;
  params.n = 0;
  struct Param *param;
  while ((opt = getopt(argc, argv, "hvf:t:o:")) != -1) {
    if (params.n >= ARRAYLEN(params.p)) {
      param = NULL;
    } else {
      param = &(params.p[params.n]);
    }
    switch (opt) {
      case 'o':
        if (outpath) {
          Die(prog, "outpath already provided");
        }
        outpath = optarg;
        break;
      case 'f':
        if (!param) {
          Die(prog, "too many replacements provided");
        }
        if (param->from_string) {
          Die(prog, "from string already provided");
        }
        param->from_string = optarg;
        param->from_len = strlen(optarg);
        partial = !partial;
        break;
      case 't':
        if (!param) {
          Die(prog, "too many replacements provided");
        }
        if (param->to_string) {
          Die(prog, "to string already provided");
        }
        param->to_string = optarg;
        param->to_len = strlen(optarg);
        partial = !partial;
        break;
      case 'v':
        tinyprint(0, VERSION, NULL);
        exit(0);
      case 'h':
        ShowUsage(0, 1);
      default:
        ShowUsage(1, 2);
    }
    if (param->from_string && param->to_string) {
      if (param->from_len < param->to_len) {
        Die(prog, "to_string longer than from_string, cannot replace");
      }
      params.n++;
    }
  }
  if (params.n == 0) {
    Die(prog, "no replacements provided");
  }
  if (partial) {
    Die(prog, "partial replacement provided");
  }
  for (int i = 0; i < params.n; ++i) {  // no-op
    if (!params.p[i].from_string) {
      Die(prog, "need from_string for replacement");
    }
    if (!params.p[i].to_string) {
      Die(prog, "need to_string for replacement");
    }
  }
  if (!outpath) {
    Die(prog, "need outpath to write file");
  }
  if (optind == argc) {
    Die(prog, "missing input argument");
  }
  if (optind != argc - 1) {
    Die(prog, "too many args");
  }
  exepath = argv[optind];
}

static void ValidateElfImage(Elf64_Ehdr *e, Elf64_Off esize,  //
                             const char *epath) {

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
    if (p[i].p_type != PT_LOAD)
      continue;
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
    if (p[i].p_type == PT_INTERP) {
      Die(epath, "ELF has PT_INTERP which isn't supported");
    }
    if (p[i].p_type == PT_DYNAMIC) {
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
      if (p[j].p_type != PT_LOAD)
        continue;
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

struct Input {
  union {
    char *map;
    Elf64_Ehdr *elf;
    unsigned char *umap;
  };
  size_t size;
  const char *path;
};

static struct Input input;

static void OpenInput(const char *path) {
  int fd;
  if ((fd = open(path, O_RDONLY)) == -1)
    DieSys(path);
  if ((input.size = lseek(fd, 0, SEEK_END)) == -1)
    DieSys(path);
  input.path = path;
  input.map = mmap(0, input.size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
  if (input.map == MAP_FAILED)
    DieSys(path);
  if (!IsElf64Binary(input.elf, input.size))
    Die(path, "not an elf64 binary");
  close(fd);
}

static void ReplaceString(struct Param *param) {
  Elf64_Xword len = strnlen(param->roloc, roend - param->roloc);
  char *targstr = Malloc(len + 1);
  for (Elf64_Xword i = 0; i < len + 1; ++i)
    targstr[i] = 0;
  strcpy(targstr, param->to_string);
  strcat(targstr, param->roloc + param->from_len);
#ifdef MODE_DBG
  kprintf("'%s' should be '%s'\n", param->roloc, targstr);
#endif
  strcpy(param->roloc, targstr);
  param->roloc += param->to_len;
  free(targstr);
}

int main(int argc, char *argv[]) {
#ifdef MODE_DBG
  ShowCrashReports();
#endif

  prog = argv[0];

  if (!prog)
    prog = "renamestr";

  GetOpts(argc, argv);
  OpenInput(exepath);
  ValidateElfImage(input.elf, input.size, input.path);
  rodata = FindElfSectionByName(
      input.elf, input.size,
      GetElfSectionNameStringTable(input.elf, input.size), ".rodata");
  if (!rodata)
    Die(exepath, "doesn't have .rodata");

  rostart = GetElfSectionAddress(input.elf, input.size, rodata);
  if (!rostart)
    Die(prog, "could not get to start of .rodata");
  roend = rostart + rodata->sh_size;

#ifdef MODE_DBG
  kprintf("elf file to process: %s\n", exepath);
  kprintf("file size is %ld\n", input.size);
#endif
  for (int i = 0; i < params.n; ++i) {
    struct Param *param = &(params.p[i]);
    param->roloc = rostart;
    param->count = 0;
#ifdef MODE_DBG
    kprintf("need to replace '%s' with '%s'\n", param->from_string,
            param->to_string);
#endif
  }

#define NEXT_ROLOC(z) \
  memmem((z)->roloc, roend - (z)->roloc, (z)->from_string, (z)->from_len)
  for (int i = 0; i < params.n; ++i) {
    struct Param *param = &(params.p[i]);
    for (param->roloc = NEXT_ROLOC(param); param->roloc != NULL;
         param->roloc = NEXT_ROLOC(param)) {
      if (param->roloc == rostart ||
          param->roloc[-1] == '\0') {  // confirm start of string
        ReplaceString(param);
        param->count++;
      } else {
        // in the middle of a string, so move forward
        param->roloc += param->from_len;
      }
    }
  }
#undef NEXT_ROLOC

  if ((outfd = creat(outpath, 0755)) == -1) {
    Die(prog, "unable to open file for writing");
  }
  Pwrite(input.map, input.size, 0);
  if (close(outfd)) {
    Die(prog, "unable to close file after writing");
  }

  for (int i = 0; i < params.n; ++i) {
    struct Param *param = &(params.p[i]);
    printf("'%s' -> '%s': %d replacements\n", param->from_string,
           param->to_string, param->count);
  }
  return 0;
}
