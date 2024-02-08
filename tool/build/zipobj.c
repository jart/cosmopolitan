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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/elf/def.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/fmt/libgen.h"
#include "libc/limits.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/mem/gc.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/clock.h"
#include "libc/sysv/consts/ex.h"
#include "libc/sysv/consts/exit.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/s.h"
#include "libc/time/time.h"
#include "libc/x/x.h"
#include "libc/zip.internal.h"
#include "third_party/getopt/getopt.internal.h"
#include "tool/build/lib/elfwriter.h"
#include "tool/build/lib/stripcomponents.h"

int arch_;
char *name_;
char *yoink_;
char *symbol_;
char *outpath_;
bool nocompress_;
bool basenamify_;
int strip_components_;
const char *path_prefix_;
struct timespec timestamp;

wontreturn void PrintUsage(int fd, int rc) {
  tinyprint(fd, "\n\
NAME\n\
\n\
  Cosmpolitan Zip File Compiler\n\
\n\
SYNOPSIS\n\
\n\
  ",
            program_invocation_name, " [FLAGS] FILE...\n\
\n\
DESCRIPTION\n\
\n\
  This program may be used to turn arbitrary files into .zip.o files\n\
  which can be incrementally linked into binaries, without quadratic\n\
  compression complexity.\n\
\n\
FLAGS\n\
\n\
  -h              show help\n\
  -o PATH         output path\n\
  -0              disable compression\n\
  -B              basename-ify zip filename\n\
  -a ARCH         microprocessor architecture\n\
  -N ZIPPATH      zip filename (defaults to input arg)\n\
  -P ZIPPATH      prepend path zip filename using join\n\
  -C INTEGER      strips leading path components from zip filename\n\
  -y SYMBOL       generate yoink for symbol (default __zip_eocd)\n\
\n\
",
            NULL);
  exit(rc);
}

static int ParseArch(const char *s) {
  if (startswith(s, "x86")) {
    return EM_NEXGEN32E;
  } else if (startswith(s, "arm") || !strcmp(s, "aarch64")) {
    return EM_AARCH64;
  } else if (startswith(s, "ppc") || startswith(s, "powerpc")) {
    return EM_PPC64;
  } else if (startswith(s, "riscv")) {
    return EM_RISCV;
  } else if (startswith(s, "s390")) {
    return EM_S390;
  } else {
    tinyprint(2, "error: unrecognized microprocessor architecture\n", NULL);
    exit(1);
  }
}

void GetOpts(int *argc, char ***argv) {
  int opt;
  yoink_ = "__zip_eocd";
  while ((opt = getopt(*argc, *argv, "?0nhBN:C:P:o:s:y:a:")) != -1) {
    switch (opt) {
      case 'o':
        outpath_ = optarg;
        break;
      case 'a':
        arch_ = ParseArch(optarg);
        break;
      case 'n':
        exit(0);
      case 's':
        symbol_ = optarg;
        break;
      case 'y':
        yoink_ = optarg;
        break;
      case 'N':
        name_ = optarg;
        break;
      case 'P':
        path_prefix_ = optarg;
        break;
      case 'C':
        strip_components_ = atoi(optarg);
        break;
      case 'B':
        basenamify_ = true;
        break;
      case '0':
        nocompress_ = true;
        break;
      case '?':
      case 'h':
        PrintUsage(1, EXIT_SUCCESS);
      default:
        PrintUsage(2, EX_USAGE);
    }
  }
  *argc -= optind;
  *argv += optind;
  if (!outpath_) {
    tinyprint(2,
              "error: no output path specified\n"
              "run ",
              program_invocation_name, " -h for usage\n", NULL);
    exit(1);
  }
}

void ProcessFile(struct ElfWriter *elf, const char *path) {
  int fd;
  void *map;
  struct stat st;
  const char *name;
  if (stat(path, &st)) {
    perror(path);
    exit(1);
  }
  if (S_ISDIR(st.st_mode)) {
    if ((fd = open(path, O_RDONLY | O_DIRECTORY)) == -1) {
      perror(path);
      exit(1);
    }
    map = "";
    st.st_size = 0;
  } else if (st.st_size) {
    if ((fd = open(path, O_RDONLY)) == -1 ||
        (map = mmap(0, st.st_size, PROT_READ, MAP_SHARED, fd, 0)) ==
            MAP_FAILED) {
      perror(path);
      exit(1);
    }
  } else {
    fd = -1;
    map = 0;
  }
  if (name_) {
    name = name_;
  } else {
    name = path;
    if (basenamify_) name = basename(gc(xstrdup(name)));
    name = StripComponents(name, strip_components_);
    if (path_prefix_) name = gc(xjoinpaths(path_prefix_, name));
  }
  if (S_ISDIR(st.st_mode)) {
    st.st_size = 0;
    if (!endswith(name, "/")) {
      name = gc(xstrcat(name, '/'));
    }
  }
  elfwriter_zip(elf, name, name, strlen(name), map, st.st_size, st.st_mode,
                timestamp, timestamp, timestamp, nocompress_);
  if (st.st_size) {
    unassert(!munmap(map, st.st_size));
  }
  close(fd);
}

void PullEndOfCentralDirectoryIntoLinkage(struct ElfWriter *elf) {
  elfwriter_align(elf, 1, 0);
  elfwriter_startsection(elf, ".yoink", SHT_PROGBITS, SHF_EXECINSTR);
  elfwriter_yoink(elf, yoink_, STB_GLOBAL);
  elfwriter_finishsection(elf);
}

void CheckFilenameKosher(const char *path) {
  unassert(kZipCfileHdrMinSize + strlen(path) <= 65535);
  unassert(!startswith(path, "/"));
  unassert(!strstr(path, ".."));
}

void zipobj(int argc, char **argv) {
  size_t i;
  struct ElfWriter *elf;
  unassert(argc < UINT16_MAX / 3 - 64); /* ELF 64k section limit */
  GetOpts(&argc, &argv);
  for (i = 0; i < argc; ++i) CheckFilenameKosher(argv[i]);
  elf = elfwriter_open(outpath_, 0644, arch_);
  elfwriter_cargoculting(elf);
  for (i = 0; i < argc; ++i) ProcessFile(elf, argv[i]);
  PullEndOfCentralDirectoryIntoLinkage(elf);
  elfwriter_close(elf);
}

int main(int argc, char **argv) {
  timestamp.tv_sec = 1647414000; /* determinism */
  /* clock_gettime(CLOCK_REALTIME, &timestamp); */
  zipobj(argc, argv);
  return 0;
}
