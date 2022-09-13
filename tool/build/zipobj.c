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
#include "libc/elf/def.h"
#include "libc/fmt/conv.h"
#include "libc/intrin/kprintf.h"
#include "libc/limits.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/mem/gc.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/clock.h"
#include "libc/sysv/consts/ex.h"
#include "libc/sysv/consts/exit.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/s.h"
#include "libc/time/time.h"
#include "libc/x/x.h"
#include "libc/zip.h"
#include "third_party/getopt/getopt.h"
#include "tool/build/lib/elfwriter.h"
#include "tool/build/lib/stripcomponents.h"

char *name_;
char *yoink_;
char *symbol_;
char *outpath_;
bool nocompress_;
bool basenamify_;
int64_t image_base_;
int strip_components_;
const char *path_prefix_;
struct timespec timestamp;
size_t kZipCdirHdrLinkableSizeBootstrap;

wontreturn void PrintUsage(int rc) {
  kprintf("%s%s%s\n", "Usage: ", program_invocation_name,
          " [-n] [-B] [-C INT] [-P PREFIX] [-o FILE] [-s SYMBOL] [-y YOINK] "
          "[FILE...]");
  exit(rc);
}

void GetOpts(int *argc, char ***argv) {
  int opt;
  yoink_ = "__zip_start";
  image_base_ = IMAGE_BASE_VIRTUAL;
  kZipCdirHdrLinkableSizeBootstrap = kZipCdirHdrLinkableSize;
  while ((opt = getopt(*argc, *argv, "?0nhBL:N:C:P:o:s:y:b:")) != -1) {
    switch (opt) {
      case 'o':
        outpath_ = optarg;
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
      case 'b':
        image_base_ = strtol(optarg, NULL, 0);
        break;
      case '0':
        nocompress_ = true;
        break;
      case 'L':
        kZipCdirHdrLinkableSizeBootstrap = strtoul(optarg, NULL, 0);
        break;
      case '?':
      case 'h':
        PrintUsage(EXIT_SUCCESS);
      default:
        PrintUsage(EX_USAGE);
    }
  }
  *argc -= optind;
  *argv += optind;
  CHECK_NOTNULL(outpath_);
}

void ProcessFile(struct ElfWriter *elf, const char *path) {
  int fd;
  void *map;
  size_t pathlen;
  struct stat st;
  const char *name;
  CHECK_NE(-1, (fd = open(path, O_RDONLY)));
  CHECK_NE(-1, fstat(fd, &st));
  if (S_ISDIR(st.st_mode)) {
    map = "";
    st.st_size = 0;
  } else if (st.st_size) {
    CHECK_NE(MAP_FAILED,
             (map = mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, fd, 0)));
  } else {
    map = NULL;
  }
  if (name_) {
    name = name_;
  } else {
    name = path;
    if (basenamify_) name = basename(name);
    name = StripComponents(name, strip_components_);
    if (path_prefix_) name = gc(xjoinpaths(path_prefix_, name));
  }
  if (S_ISDIR(st.st_mode)) {
    st.st_size = 0;
    if (!_endswith(name, "/")) {
      name = gc(xstrcat(name, '/'));
    }
  }
  elfwriter_zip(elf, name, name, strlen(name), map, st.st_size, st.st_mode,
                timestamp, timestamp, timestamp, nocompress_, image_base_,
                kZipCdirHdrLinkableSizeBootstrap);
  if (st.st_size) CHECK_NE(-1, munmap(map, st.st_size));
  close(fd);
}

void PullEndOfCentralDirectoryIntoLinkage(struct ElfWriter *elf) {
  elfwriter_align(elf, 1, 0);
  elfwriter_startsection(elf, ".yoink", SHT_PROGBITS,
                         SHF_ALLOC | SHF_EXECINSTR);
  elfwriter_yoink(elf, yoink_, STB_GLOBAL);
  elfwriter_finishsection(elf);
}

void CheckFilenameKosher(const char *path) {
  CHECK_LE(kZipCfileHdrMinSize + strlen(path),
           kZipCdirHdrLinkableSizeBootstrap);
  CHECK(!_startswith(path, "/"));
  CHECK(!strstr(path, ".."));
}

void zipobj(int argc, char **argv) {
  size_t i;
  struct ElfWriter *elf;
  CHECK_LT(argc, UINT16_MAX / 3 - 64); /* ELF 64k section limit */
  GetOpts(&argc, &argv);
  for (i = 0; i < argc; ++i) CheckFilenameKosher(argv[i]);
  elf = elfwriter_open(outpath_, 0644);
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
