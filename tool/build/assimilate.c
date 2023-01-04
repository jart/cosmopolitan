/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/fmt/conv.h"
#include "libc/intrin/bits.h"
#include "libc/intrin/kprintf.h"
#include "libc/log/check.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/msync.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "third_party/getopt/getopt.h"
#include "third_party/regex/regex.h"

STATIC_YOINK("strerror_wr");

// options used: fhem
// letters not used: ABCDEFGHIJKLMNOPQRSTUVWXYZabcdgijklnopqrstuvwxyz
// digits not used: 0123456789
// puncts not used: !"#$%&'()*+,-./;<=>@[\]^_`{|}~
// letters duplicated: none
#define GETOPTS "em"

#define USAGE \
  "\
Usage: assimilate.com [-hfem] COMFILE...\n\
  -h          show help\n\
  -e          force elf\n\
  -m          force macho\n\
  -f          continue on soft errors\n\
\n\
αcτµαlly pδrταblε εxεcµταblε assimilate v1.o\n\
copyright 2022 justine alexandra roberts tunney\n\
https://twitter.com/justinetunney\n\
https://linkedin.com/in/jtunney\n\
https://justine.lol/ape.html\n\
https://github.com/jart\n\
\n\
This program converts Actually Portable Executable files so they're\n\
in the platform-local executable format, rather than the multiplatform\n\
APE shell script format. This is useful on UNIX operating systems when\n\
you want to use your APE programs as script interpreter or for setuid.\n\
"

#define MODE_NATIVE 0
#define MODE_ELF    1
#define MODE_MACHO  2
#define MODE_PE     3

int g_mode;
bool g_force;
int exitcode;
const char *prog;
char errstr[128];

void GetOpts(int argc, char *argv[]) {
  int opt;
  while ((opt = getopt(argc, argv, GETOPTS)) != -1) {
    switch (opt) {
      case 'f':
        g_force = true;
        break;
      case 'e':
        g_mode = MODE_ELF;
        break;
      case 'm':
        g_mode = MODE_MACHO;
        break;
      case 'h':
      case '?':
        write(1, USAGE, sizeof(USAGE) - 1);
        exit(0);
      default:
        write(2, USAGE, sizeof(USAGE) - 1);
        exit(64);
    }
  }
  if (g_mode == MODE_NATIVE) {
    if (IsXnu()) {
      g_mode = MODE_MACHO;
    } else if (IsLinux() || IsFreebsd() || IsNetbsd() || IsOpenbsd()) {
      g_mode = MODE_ELF;
    } else {
      g_mode = MODE_PE;
    }
  }
}

void GetElfHeader(char ehdr[hasatleast 64], const char *image) {
  char *p;
  int c, i;
  for (p = image; p < image + 4096; ++p) {
    if (READ64LE(p) != READ64LE("printf '")) continue;
    for (i = 0, p += 8; p + 3 < image + 4096 && (c = *p++) != '\'';) {
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
        ehdr[i++] = c;
      } else {
        kprintf("%s: ape printf elf header too long\n", prog);
        exit(1);
      }
    }
    if (i != 64) {
      kprintf("%s: ape printf elf header too short\n", prog);
      exit(2);
    }
    if (READ32LE(ehdr) != READ32LE("\177ELF")) {
      kprintf("%s: ape printf elf header didn't have elf magic\n", prog);
      exit(3);
    }
    return;
  }
  kprintf("%s: printf statement not found in first 4096 bytes\n", prog);
  exit(4);
}

void GetMachoPayload(const char *image, size_t imagesize, int *out_offset,
                     int *out_size) {
  regex_t rx;
  const char *script;
  regmatch_t rm[1 + 3] = {0};
  int rc, skip, count, bs, offset, size;

  if ((script = memmem(image, imagesize, "'\n#'\"\n", 6))) {
    script += 6;
  } else if ((script = memmem(image, imagesize, "#'\"\n", 4))) {
    script += 4;
  } else {
    kprintf("%s: ape shell script not found\n", prog);
    exit(5);
  }

  DCHECK_EQ(REG_OK, regcomp(&rx,
                            "bs=([ [:digit:]]+) "
                            "skip=\"?([ [:digit:]]+)\"? "
                            "count=\"?([ [:digit:]]+)\"?",
                            REG_EXTENDED));
  rc = regexec(&rx, script, 4, rm, 0);
  if (rc != REG_OK) {
    if (rc == REG_NOMATCH) {
      kprintf("%s: ape macho dd command not found\n", prog);
      exit(6);
    }
    regerror(rc, &rx, errstr, sizeof(errstr));
    kprintf("%s: ape macho dd regex failed: %s\n", prog, errstr);
    exit(7);
  }
  bs = atoi(script + rm[1].rm_so);
  skip = atoi(script + rm[2].rm_so);
  count = atoi(script + rm[3].rm_so);
  if (__builtin_mul_overflow(skip, bs, &offset) ||
      __builtin_mul_overflow(count, bs, &size)) {
    kprintf("%s: integer overflow parsing macho\n");
    exit(8);
  }
  if (offset < 64) {
    kprintf("%s: ape macho dd offset should be ≥64: %d\n", prog, offset);
    exit(9);
  }
  if (offset >= imagesize) {
    kprintf("%s: ape macho dd offset is outside file: %d\n", prog, offset);
    exit(10);
  }
  if (size < 32) {
    kprintf("%s: ape macho dd size should be ≥32: %d\n", prog, size);
    exit(11);
  }
  if (size > imagesize - offset) {
    kprintf("%s: ape macho dd size is outside file: %d\n", prog, size);
    exit(12);
  }
  *out_offset = offset;
  *out_size = size;
  regfree(&rx);
}

void AssimilateElf(char *p, size_t n) {
  char ehdr[64];
  GetElfHeader(ehdr, p);
  memcpy(p, ehdr, 64);
  msync(p, 4096, MS_SYNC);
}

void AssimilateMacho(char *p, size_t n) {
  int offset, size;
  GetMachoPayload(p, n, &offset, &size);
  memmove(p, p + offset, size);
  msync(p, n, MS_SYNC);
}

void Assimilate(void) {
  int fd;
  char *p;
  struct stat st;
  if ((fd = open(prog, O_RDWR)) == -1) {
    kprintf("%s: open(O_RDWR) failed: %m\n", prog);
    exit(13);
  }
  if (fstat(fd, &st) == -1) {
    kprintf("%s: fstat() failed: %m\n", prog);
    exit(14);
  }
  if (st.st_size < 8192) {
    kprintf("%s: ape binaries must be at least 4096 bytes\n", prog);
    exit(15);
  }
  if ((p = mmap(0, st.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) ==
      MAP_FAILED) {
    kprintf("%s: mmap failed: %m\n", prog);
    exit(16);
  }
  if (g_mode == MODE_PE) {
    if (READ16LE(p) == READ16LE("MZ")) {
      if (!g_force) {
        kprintf("%s: program is already an elf binary\n", prog);
        if (g_mode != MODE_ELF) {
          exitcode = 1;
        }
      }
      goto Finish;
    } else {
      kprintf("%s: currently cannot back-convert to pe\n", prog);
      exit(17);
    }
  }
  if (READ32LE(p) == READ32LE("\177ELF")) {
    if (!g_force) {
      kprintf("%s: program is already an elf binary\n", prog);
      if (g_mode != MODE_ELF) {
        exitcode = 1;
      }
    }
    goto Finish;
  }
  if (READ32LE(p) == 0xFEEDFACE + 1) {
    if (!g_force) {
      kprintf("%s: program is already a mach-o binary\n", prog);
      if (g_mode != MODE_MACHO) {
        exitcode = 1;
      }
    }
    goto Finish;
  }
  if (READ64LE(p) != READ64LE("MZqFpD='")) {
    kprintf("%s: this file is not an actually portable executable\n", prog);
    exit(17);
  }
  if (g_mode == MODE_ELF) {
    AssimilateElf(p, st.st_size);
  } else if (g_mode == MODE_MACHO) {
    AssimilateMacho(p, st.st_size);
  }
Finish:
  if (munmap(p, st.st_size) == -1) {
    kprintf("%s: munmap() failed: %m\n", prog);
    exit(18);
  }
}

int main(int argc, char *argv[]) {
  int i;
  GetOpts(argc, argv);
  if (optind == argc) {
    kprintf("error: need at least one program path to assimilate\n");
    write(2, USAGE, sizeof(USAGE) - 1);
    exit(64);
  }
  for (i = optind; i < argc; ++i) {
    prog = argv[i];
    Assimilate();
  }
  return exitcode;
}
