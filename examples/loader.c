#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/bits/bits.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/elf/def.h"
#include "libc/elf/struct/ehdr.h"
#include "libc/elf/struct/phdr.h"
#include "libc/linux/close.h"
#include "libc/linux/exit.h"
#include "libc/linux/fstat.h"
#include "libc/linux/mmap.h"
#include "libc/linux/open.h"

/**
 * @fileoverview 704-byte APE executing payload for Linux, e.g.
 *
 *     m=tiny
 *     make -j8 MODE=$m o/$m/examples
 *     o/$m/examples/loader.elf o/$m/examples/printargs.com
 *
 * @note this can probably be used as a binfmt_misc interpreter
 */

#define O_RDONLY      0
#define PROT_READ     1
#define PROT_WRITE    2
#define PROT_EXEC     4
#define MAP_SHARED    1
#define MAP_PRIVATE   2
#define MAP_FIXED     16
#define MAP_ANONYMOUS 32

asm(".globl\t_start\n\t"
    "_start:\n\t"
    "mov\t%rsp,%rdi\n\t"
    "jmp\tloader");

static noinstrument noasan noubsan void spawn(long *sp, char *b) {
  struct Elf64_Ehdr *e;
  struct Elf64_Phdr *h;
  e = (void *)b;
  h = (void *)(b + e->e_phoff);
  if (LinuxMmap((void *)(h[1].p_vaddr + h[1].p_filesz),
                h[1].p_memsz - h[1].p_filesz, PROT_READ | PROT_WRITE,
                MAP_PRIVATE | MAP_FIXED | MAP_ANONYMOUS, -1, 0) > 0) {
    sp[1] = sp[0] - 1;
    asm volatile("mov\t%2,%%rsp\n\t"
                 "jmpq\t*%1"
                 : /* no outputs */
                 : "D"(0), "S"((void *)e->e_entry), "d"(sp + 1)
                 : "memory");
    unreachable;
  }
}

noinstrument noasan noubsan void loader(long *sp) {
  struct stat st;
  int c, i, fd, argc;
  char *b, *p, *q, **argv;
  argc = *sp;
  argv = (char **)(sp + 1);
  if (argc > 1 && (fd = LinuxOpen(argv[1], O_RDONLY, 0)) >= 0 &&
      !LinuxFstat(fd, &st) &&
      (b = (char *)LinuxMmap((void *)0x400000, st.st_size,
                             PROT_READ | PROT_WRITE | PROT_EXEC,
                             MAP_PRIVATE | MAP_FIXED, fd, 0)) > 0) {
    LinuxClose(fd);
    if (READ32LE(b) == READ32LE("\177ELF")) {
      spawn(sp, b);
    } else {
      for (p = b; p < b + st.st_size; ++p) {
        if (READ64LE(p) == READ64LE("printf '")) {
          for (q = b, p += 8; (c = *p++) != '\'';) {
            if (c == '\\') {
              c = *p++ - '0';
              if ('0' <= *p && *p <= '7') c *= 8, c += *p++ - '0';
              if ('0' <= *p && *p <= '7') c *= 8, c += *p++ - '0';
            }
            *q++ = c;
          }
          if (READ32LE(b) == READ32LE("\177ELF")) {
            spawn(sp, b);
          }
          break;
        }
      }
    }
  }
  LinuxExit(127);
}
