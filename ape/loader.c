/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/bits/bits.h"
#include "libc/calls/struct/metastat.internal.h"
#include "libc/calls/struct/stat.h"
#include "libc/elf/def.h"
#include "libc/elf/struct/ehdr.h"
#include "libc/elf/struct/phdr.h"
#include "libc/sysv/consts/prot.h"

/**
 * @fileoverview APE embeddable loader for Linux and BSD, e.g.
 *
 *     m=tiny
 *     make -j8 MODE=$m o/$m/ape o/$m/examples/printargs.com
 *     o/$m/ape/loader.elf o/$m/examples/printargs.com
 *
 * @note this can probably be used as a binfmt_misc interpreter
 */

#define LINUX   0
#define FREEBSD 1
#define NETBSD  2
#define OPENBSD 3

#define O_RDONLY         0
#define PROT_READ        1
#define PROT_WRITE       2
#define PROT_EXEC        4
#define MAP_SHARED       1
#define MAP_PRIVATE      2
#define MAP_FIXED        16
#define MAP_ANONYMOUS    (os == LINUX ? 32 : 4096)
#define AT_EXECFN_LINUX  31
#define AT_EXECFN_NETBSD 2014

#define __NR_read  (os == LINUX ? 0 : 3)
#define __NR_write (os == LINUX ? 1 : 4)
#define __NR_open  (os == LINUX ? 2 : 5)
#define __NR_close (os == LINUX ? 3 : 6)
#define __NR_exit  (os == LINUX ? 60 : 1)
#define __NR_mmap  (os == LINUX ? 9 : os == FREEBSD ? 477 : 197)
#define __NR_fstat \
  (os == LINUX ? 5 : os == FREEBSD ? 551 : os == OPENBSD ? 53 : 440)

static wontreturn void Exit(int os, long rc) {
  asm volatile("syscall"
               : /* no outputs */
               : "a"(__NR_exit), "D"(rc)
               : "memory");
  unreachable;
}

static void Close(int os, long fd) {
  long ax, di;
  asm volatile("syscall"
               : "=a"(ax), "=D"(di)
               : "0"(__NR_close), "1"(fd)
               : "rcx", "rdx", "rsi", "r8", "r9", "r10", "r11", "memory", "cc");
}

static long Read(int os, long fd, void *data, unsigned long size) {
  bool cf;
  long ax, di, si, dx;
  asm volatile("clc\n\t"
               "syscall"
               : "=@ccc"(cf), "=a"(ax), "=D"(di), "=S"(si), "=d"(dx)
               : "1"(__NR_read), "2"(fd), "3"(data), "4"(size)
               : "rcx", "r8", "r9", "r10", "r11", "memory");
  if (cf) ax = -ax;
  return ax;
}

static void Write(int os, long fd, const void *data, unsigned long size) {
  long ax, di, si, dx;
  asm volatile("syscall"
               : "=a"(ax), "=D"(di), "=S"(si), "=d"(dx)
               : "0"(__NR_write), "1"(fd), "2"(data), "3"(size)
               : "rcx", "r8", "r9", "r10", "r11", "memory", "cc");
}

static long Fstat(int os, long fd, union metastat *st) {
  long ax, di, si;
  asm volatile("syscall"
               : "=a"(ax), "=D"(di), "=S"(si)
               : "0"(__NR_fstat), "1"(fd), "2"(st)
               : "rcx", "rdx", "r8", "r9", "r10", "r11", "memory", "cc");
  return ax;
}

static void Msyscall(int os, long p, long n) {
  long ax, di, si;
  if (os == OPENBSD) {
    asm volatile("syscall"
                 : "=a"(ax), "=D"(di), "=S"(si)
                 : "0"(37), "1"(p), "2"(n)
                 : "rcx", "rdx", "r8", "r9", "r10", "r11", "memory", "cc");
  }
}

static long Open(int os, const char *path, long flags, long mode) {
  bool cf;
  long ax, di, si, dx;
  asm volatile("clc\n\t"
               "syscall"
               : "=@ccc"(cf), "=a"(ax), "=D"(di), "=S"(si), "=d"(dx)
               : "1"(__NR_open), "2"(path), "3"(flags), "4"(mode)
               : "rcx", "r8", "r9", "r10", "r11", "memory");
  if (cf) ax = -ax;
  return ax;
}

static long Mmap(int os, long addr, long size, long prot, long flags, long fd,
                 long off) {
  bool cf;
  long ax;
  register long flags_ asm("r10") = flags;
  register long fd_ asm("r8") = fd;
  register long off_ asm("r9") = off;
  asm volatile("push\t%%r9\n\t"
               "push\t%%r9\n\t"
               "clc\n\t"
               "syscall\n\t"
               "pop\t%%r9\n\t"
               "pop\t%%r9"
               : "=@ccc"(cf), "=a"(ax)
               : "1"(__NR_mmap), "D"(addr), "S"(size), "d"(prot), "r"(flags_),
                 "r"(fd_), "r"(off_)
               : "rcx", "r11", "memory");
  if (cf) ax = -ax;
  return ax;
}

static size_t GetFdSize(int os, int fd) {
  union metastat st;
  if (!Fstat(os, fd, &st)) {
    if (os == LINUX) {
      return st.linux.st_size;
    } else if (os == FREEBSD) {
      return st.freebsd.st_size;
    } else if (os == OPENBSD) {
      return st.openbsd.st_size;
    } else {
      return st.netbsd.st_size;
    }
  } else {
    return 0;
  }
}

static size_t Length(const char *s) {
  size_t n = 0;
  while (*s++) ++n;
  return n;
}

static void Emit(int os, const char *s) {
  Write(os, 2, s, Length(s));
}

static void Log(int os, const char *s) {
#ifndef NDEBUG
  Emit(os, "ape loader error: ");
  Emit(os, s);
#endif
}

static void Spawn(int os, int fd, long *sp, char *b, struct Elf64_Ehdr *e) {
  size_t i;
  int prot, flags;
  long code, codesize;
  struct Elf64_Phdr *p;
  if (e->e_ident[EI_CLASS] != ELFCLASS64) {
    Log(os, "EI_CLASS != ELFCLASS64\n");
    return;
  }
  if (e->e_ident[EI_DATA] != ELFDATA2LSB) {
    Log(os, "EI_CLASS != ELFCLASS64\n");
    return;
  }
  if (e->e_machine != EM_NEXGEN32E) {
    Log(os, "e_machine != EM_NEXGEN32E\n");
    return;
  }
  if (e->e_type != ET_EXEC) {
    Log(os, "e_type != ET_EXEC\n");
    return;
  }
  if (e->e_phoff + e->e_phnum * sizeof(*p) > 0x1000) {
    Log(os, "phnum out of bounds\n");
    return;
  }
  code = 0;
  codesize = 0;
  for (p = (struct Elf64_Phdr *)(b + e->e_phoff), i = e->e_phnum; i--;) {
    if (p[i].p_type != PT_LOAD) continue;
    if ((p[i].p_vaddr | p[i].p_filesz | p[i].p_memsz | p[i].p_offset) & 0xfff) {
      Log(os, "ape requires strict page size padding and alignment\n");
      return;
    }
    prot = 0;
    flags = MAP_FIXED | MAP_PRIVATE;
    if (p[i].p_flags & PF_R) {
      prot |= PROT_READ;
    }
    if (p[i].p_flags & PF_W) {
      prot |= PROT_WRITE;
    }
    if (p[i].p_flags & PF_X) {
      prot |= PROT_EXEC;
      code = p[i].p_vaddr;
      codesize = p[i].p_filesz;
    }
    if (p[i].p_memsz > p[i].p_filesz) {
      if (Mmap(os, p[i].p_vaddr + p[i].p_filesz, p[i].p_memsz - p[i].p_filesz,
               prot, flags | MAP_ANONYMOUS, -1, 0) < 0) {
        Log(os, "bss mmap failed\n");
        return;
      }
    }
    if (p[i].p_filesz) {
      if (Mmap(os, p[i].p_vaddr, p[i].p_filesz, prot, flags, fd,
               p[i].p_offset) < 0) {
        Log(os, "image mmap failed\n");
        return;
      }
    }
  }
  Close(os, fd);
  Msyscall(os, code, codesize);
  sp[1] = sp[0] - 1;
  ++sp;
  asm volatile("mov\t%2,%%rsp\n\t"
               "jmpq\t*%1"
               : /* no outputs */
               : "D"(os == FREEBSD ? sp : 0), "S"(e->e_entry), "d"(sp)
               : "memory");
  unreachable;
}

void loader(long di, long *sp) {
  size_t size;
  long rc, *auxv;
  char *p, **argv;
  int c, i, fd, os, argc;
  union {
    struct Elf64_Ehdr ehdr;
    char p[0x1000];
  } u;
  os = 0;
  if (di) {
    os = FREEBSD;
    sp = (long *)di;
  }
  argc = *sp;
  argv = (char **)(sp + 1);
  auxv = (long *)(argv + argc + 1);
  for (;;) {
    if (!*auxv++) {
      break;
    }
  }
  if (!auxv[0]) {
    os = OPENBSD;
  }
  for (; auxv[0]; auxv += 2) {
    if (!os) {
      if (auxv[0] == AT_EXECFN_NETBSD) {
        os = NETBSD;
        if (argc > 1) {
          auxv[1] = (long)argv[1];
        }
      } else if (auxv[0] == AT_EXECFN_LINUX) {
        if (argc > 1) {
          auxv[1] = (long)argv[1];
        }
      }
    }
  }
  if (argc < 2) {
    Emit(os, "usage: loader PROG [ARGS...]\n");
  } else if ((fd = Open(os, argv[1], O_RDONLY, 0)) < 0) {
    Log(os, "open failed\n");
  } else if ((rc = Read(os, fd, u.p, sizeof(u.p))) < 0) {
    Log(os, "read failed\n");
  } else if (rc != sizeof(u.p)) {
    Log(os, "file too small\n");
  } else if (READ32LE(u.p) == READ32LE("\177ELF")) {
    Spawn(os, fd, sp, u.p, &u.ehdr);
  } else {
    for (p = u.p; p < u.p + sizeof(u.p); ++p) {
      if (READ64LE(p) == READ64LE("printf '")) {
        for (i = 0, p += 8; p + 3 < u.p + sizeof(u.p) && (c = *p++) != '\'';) {
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
          u.p[i++] = c;
        }
        if (i >= 64 && READ32LE(u.p) == READ32LE("\177ELF")) {
          Spawn(os, fd, sp, u.p, &u.ehdr);
          Exit(os, 127);
        }
      }
    }
    Log(os, "could not find printf elf in first page\n");
  }
  Exit(os, 127);
}
