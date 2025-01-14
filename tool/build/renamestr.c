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
#include "libc/intrin/likely.h"
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
  " -f FROM -t TO INPUT \n"                                   \
  "\n"                                                        \
  "DESCRIPTION\n"                                             \
  "\n"                                                        \
  "  in-place string replacement in ELF binary .rodata\n"     \
  "\n"                                                        \
  "  this program may be used to replace strings in the\n"    \
  "  .rodata sections of ELF binaries, in-place.\n"           \
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
  "  INPUT      ELF binary containing strings to replace\n"   \
  "\n"

static const char *prog;
static const char *exepath;
static Elf64_Shdr *rodata;
static char *rostart;
static char *roend;
static int exefd;

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

static void Pwrite(const void *data, size_t size, uint64_t offset) {
  ssize_t rc;
  const char *p, *e;
  for (p = data, e = p + size; p < e; p += (size_t)rc, offset += (size_t)rc) {
    if ((rc = pwrite(exefd, p, e - p, offset)) == -1) {
      DieSys(exepath);
    }
  }
}

struct String {
  const char *str;
  size_t len;
};

struct Param {
  struct String from;
  struct String to;
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
  while ((opt = getopt(argc, argv, "hvf:t:")) != -1) {
    if (params.n >= ARRAYLEN(params.p)) {
      param = NULL;
    } else {
      param = &(params.p[params.n]);
    }
    switch (opt) {
      case 'f':
        if (!param) {
          Die(prog, "too many replacements provided");
        }
        if (param->from.str) {
          Die(prog, "from string already provided");
        }
        param->from.str = optarg;
        param->from.len = strlen(optarg);
        partial = !partial;
        break;
      case 't':
        if (!param) {
          Die(prog, "too many replacements provided");
        }
        if (param->to.str) {
          Die(prog, "to string already provided");
        }
        param->to.str = optarg;
        param->to.len = strlen(optarg);
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
    if (param->from.str && param->to.str) {
      if (param->from.len < param->to.len) {
        Die(prog, "to.str longer than from.str, cannot replace");
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
  if (optind == argc) {
    Die(prog, "missing input argument");
  }
  if (optind != argc - 1) {
    Die(prog, "too many args");
  }
  exepath = argv[optind];
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
  if ((fd = open(path, O_RDWR)) == -1)
    DieSys(path);
  if ((input.size = lseek(fd, 0, SEEK_END)) == -1)
    DieSys(path);
  input.path = path;
  input.map = mmap(0, input.size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
  if (input.map == MAP_FAILED)
    DieSys(path);
  if (!IsElf64Binary(input.elf, input.size))
    Die(path, "not an elf64 binary");
  exefd = fd;
}

static void ReplaceString(struct Param *param) {
  size_t len;
  char *x = (char *)memchr(param->roloc, 0, roend - param->roloc);
  memmove(param->roloc, param->to.str, param->to.len);
  if (UNLIKELY(x == NULL)) {
    len = roend - param->roloc;
    memmove(param->roloc + param->to.len, param->roloc + param->from.len,
            len - param->from.len);
  } else {
    len = x - param->roloc;
    memmove(param->roloc + param->to.len, param->roloc + param->from.len,
            len + 1 - param->from.len);
  }
  param->roloc += param->to.len;
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
    kprintf("need to replace '%s' with '%s'\n", param->from.str, param->to.str);
#endif
  }

#define NEXT_ROLOC(z) \
  memmem((z)->roloc, roend - (z)->roloc, (z)->from.str, (z)->from.len)
  for (int i = 0; i < params.n; ++i) {
    struct Param *param = &(params.p[i]);
    for (param->roloc = NEXT_ROLOC(param); param->roloc != NULL;
         param->roloc = NEXT_ROLOC(param)) {
      ReplaceString(param);
      param->count++;
    }
  }
#undef NEXT_ROLOC

  Pwrite(input.map, input.size, 0);
  if (close(exefd)) {
    Die(prog, "unable to close file after writing");
  }

  for (int i = 0; i < params.n; ++i) {
    struct Param *param = &(params.p[i]);
    printf("'%s' -> '%s': %d replacements\n", param->from.str, param->to.str,
           param->count);
  }
  return 0;
}
