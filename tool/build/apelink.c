/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/calls/calls.h"
#include "libc/elf/elf.h"
#include "libc/elf/struct/ehdr.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "third_party/getopt/getopt.internal.h"

#define VERSION                     \
  "apelink v1.0\n"                  \
  "copyright 2023 justine tunney\n" \
  "https://github.com/jart/cosmopolitan\n"

#define MANUAL                                                   \
  " -o OUTPUT INPUT...\n"                                        \
  "\n"                                                           \
  "DESCRIPTION\n"                                                \
  "\n"                                                           \
  "  Actually Portable Executable Linker\n"                      \
  "\n"                                                           \
  "  This program may be used to turn ELF executables into\n"    \
  "  APE executables. It's useful for creating fat binaries\n"   \
  "  that run on multiple architectures.\n"                      \
  "\n"                                                           \
  "FLAGS\n"                                                      \
  "\n"                                                           \
  "  -h         show usage\n"                                    \
  "  -o OUTPUT  set output path\n"                               \
  "\n"                                                           \
  "ARGUMENTS\n"                                                  \
  "\n"                                                           \
  "  OUTPUT     is your ape executable\n"                        \
  "  INPUT      may specify multiple APE or ELF builds of the\n" \
  "             same program for different architectures that\n" \
  "             shall be merged into a single output file\n"     \
  "\n"

static const char *prog;
static const char *outpath;

static wontreturn void Die(const char *reason) {
  tinyprint(2, prog, ": ", reason, "\n", NULL);
  exit(1);
}

static wontreturn void DieSys(const char *thing) {
  perror(thing);
  exit(1);
}

static wontreturn void ShowUsage(int rc, int fd) {
  tinyprint(fd, VERSION, "\nUSAGE\n\n  ", prog, MANUAL, NULL);
  exit(rc);
}

static void GetOpts(int argc, char *argv[]) {
  int opt;
  while ((opt = getopt(argc, argv, "ho:")) != -1) {
    switch (opt) {
      case 'o':
        outpath = optarg;
        break;
      case 'h':
        ShowUsage(0, 1);
      default:
        ShowUsage(1, 2);
    }
  }
  if (!outpath) {
    Die("need output path");
  }
  if (optind == argc) {
    Die("missing input argument");
  }
}

static void HandleInput(const char *inpath) {
  int fildes;
  ssize_t esize;
  Elf64_Ehdr *elf;
  if ((fildes = open(inpath, O_RDONLY)) == -1) {
    DieSys("open");
  }
  if ((esize = lseek(fildes, 0, SEEK_END)) == -1) {
    DieSys("lseek");
  }
  if (esize) {
    if ((elf = mmap(0, esize, PROT_READ | PROT_WRITE, MAP_SHARED, fildes, 0)) ==
        MAP_FAILED) {
      DieSys("mmap");
    }
    if (IsElf64Binary(elf, esize)) {
    }
    if (munmap(elf, esize)) {
      DieSys("munmap");
    }
  }
  if (close(fildes)) {
    DieSys("close");
  }
}

int main(int argc, char *argv[]) {
  int i, opt;

  // get program name
  prog = argv[0];
  if (!prog) prog = "apelink";

  // read flags
  GetOpts(argc, argv);

  // read args
  for (i = optind; i < argc; ++i) {
    HandleInput(argv[i]);
  }

  return 0;
}
