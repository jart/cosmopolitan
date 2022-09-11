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
#include "ape/loader.h"

#define SET_EXE_FILE    0 /* needs root ;_; */
#define TROUBLESHOOT    0
#define TROUBLESHOOT_OS LINUX

/**
 * @fileoverview APE Loader for GNU/Systemd/XNU/FreeBSD/NetBSD/OpenBSD
 *
 * We recommend using the normal APE design, where binaries assimilate
 * themselves once by self-modifying the first 64 bytes. If that can't
 * meet your requirements then we provide an excellent alternative.
 *
 *     m=tiny
 *     make -j8 MODE=$m o/$m/ape o/$m/examples/printargs.com
 *     o/$m/ape/ape.elf o/$m/examples/printargs.com
 *
 * This is an embeddable Actually Portable Executable interpreter. The
 * `ape/ape.S` bootloader embeds this binary inside each binary that's
 * linked using `$(APE_NO_MODIFY_SELF)` so it is an automated seamless
 * process. the shell script at the top of the .COM files will copy it
 * to `${TMPDIR:-${HOME:-.}}/.ape` and call execve(). It's a zero copy
 * operation in praxis since this payload uses mmap() to load the rest
 * of your executable the same way the kernel does, based on ELF phdrs
 * which are located in accordance with the first sh printf statement.
 *
 * APE executables will look for this program on the system path first
 * so your APE loader may be installed to your system as follows:
 *
 *     m=tiny
 *     make -j8 MODE=$m o/$m/ape/ape.elf
 *     sudo cp o/$m/ape/ape.elf /usr/bin/ape
 *
 * For Mac OS X systems you should install the `ape.macho` executable:
 *
 *     sudo cp o/$m/ape/ape.macho /usr/bin/ape
 *
 * Your APE loader may be used as a shebang interpreter by doing this:
 *
 *     #!/usr/bin/ape python.com
 *     # -*- python -*-
 *     print("hello world")
 *
 * However you won't need to do that, if your APE Loader is registered
 * as a binfmt_misc interpreter. You can do that as follows with root:
 *
 *     sudo cp -f o/$m/ape/ape.elf /usr/bin
 *     f=/proc/sys/fs/binfmt_misc/register
 *     sudo sh -c "echo ':APE:M::MZqFpD::/usr/bin/ape:' >$f"
 *
 * If the register file doesn't exist on your Linux machine then you'd
 * load it using the following commands:
 *
 *     sudo modprobe binfmt_misc
 *     sudo mount -t binfmt_misc none /proc/sys/fs/binfmt_misc
 *
 * You should now experience a performance boost, and you can also now
 * use the regular shebang form:
 *
 *     #!/usr/bin/python.com
 *     # -*- python -*-
 *     print("hello world")
 *
 * @note this can probably be used as a binfmt_misc interpreter
 */

#define LINUX   1
#define XNU     8
#define OPENBSD 16
#define FREEBSD 32
#define NETBSD  64

#define SupportsLinux()   (SUPPORT_VECTOR & LINUX)
#define SupportsXnu()     (SUPPORT_VECTOR & XNU)
#define SupportsFreebsd() (SUPPORT_VECTOR & FREEBSD)
#define SupportsOpenbsd() (SUPPORT_VECTOR & OPENBSD)
#define SupportsNetbsd()  (SUPPORT_VECTOR & NETBSD)

#define IsLinux()   (SupportsLinux() && os == LINUX)
#define IsXnu()     (SupportsXnu() && os == XNU)
#define IsFreebsd() (SupportsFreebsd() && os == FREEBSD)
#define IsOpenbsd() (SupportsOpenbsd() && os == OPENBSD)
#define IsNetbsd()  (SupportsNetbsd() && os == NETBSD)

#define O_RDONLY           0
#define PROT_READ          1
#define PROT_WRITE         2
#define PROT_EXEC          4
#define MAP_SHARED         1
#define MAP_PRIVATE        2
#define MAP_FIXED          16
#define MAP_ANONYMOUS      (IsLinux() ? 32 : 4096)
#define AT_EXECFN_LINUX    31
#define AT_EXECFN_NETBSD   2014
#define ELFCLASS64         2
#define ELFDATA2LSB        1
#define EM_NEXGEN32E       62
#define ET_EXEC            2
#define PT_LOAD            1
#define PT_DYNAMIC         2
#define EI_CLASS           4
#define EI_DATA            5
#define PF_X               1
#define PF_W               2
#define PF_R               4
#define X_OK               1
#define XCR0_SSE           2
#define XCR0_AVX           4
#define PR_SET_MM          35
#define PR_SET_MM_EXE_FILE 13

#define Read32(S)                                                      \
  ((unsigned)(255 & (S)[3]) << 030 | (unsigned)(255 & (S)[2]) << 020 | \
   (unsigned)(255 & (S)[1]) << 010 | (unsigned)(255 & (S)[0]) << 000)

#define Read64(S)                         \
  ((unsigned long)(255 & (S)[7]) << 070 | \
   (unsigned long)(255 & (S)[6]) << 060 | \
   (unsigned long)(255 & (S)[5]) << 050 | \
   (unsigned long)(255 & (S)[4]) << 040 | \
   (unsigned long)(255 & (S)[3]) << 030 | \
   (unsigned long)(255 & (S)[2]) << 020 | \
   (unsigned long)(255 & (S)[1]) << 010 | \
   (unsigned long)(255 & (S)[0]) << 000)

struct PathSearcher {
  char os;
  unsigned long namelen;
  const char *name;
  const char *syspath;
  char path[1024];
};

struct ElfEhdr {
  unsigned char e_ident[16];
  unsigned short e_type;
  unsigned short e_machine;
  unsigned e_version;
  unsigned long e_entry;
  unsigned long e_phoff;
  unsigned long e_shoff;
  unsigned e_flags;
  unsigned short e_ehsize;
  unsigned short e_phentsize;
  unsigned short e_phnum;
  unsigned short e_shentsize;
  unsigned short e_shnum;
  unsigned short e_shstrndx;
};

struct ElfPhdr {
  unsigned p_type;
  unsigned p_flags;
  unsigned long p_offset;
  unsigned long p_vaddr;
  unsigned long p_paddr;
  unsigned long p_filesz;
  unsigned long p_memsz;
  unsigned long p_align;
};

extern char ehdr[];
extern char _end[];
static void *syscall_;
static char relocated;
static struct PathSearcher ps;
extern char __syscall_loader[];

static int ToLower(int c) {
  return 'A' <= c && c <= 'Z' ? c + ('a' - 'A') : c;
}

char *MemCpy(char *d, const char *s, unsigned long n) {
  unsigned long i = 0;
  for (; i < n; ++i) d[i] = s[i];
  return d + n;
}

static unsigned long StrLen(const char *s) {
  unsigned long n = 0;
  while (*s++) ++n;
  return n;
}

static const char *MemChr(const char *s, unsigned char c, unsigned long n) {
  for (; n; --n, ++s) {
    if ((*s & 255) == c) {
      return s;
    }
  }
  return 0;
}

static char *GetEnv(char **p, const char *s) {
  unsigned long i, j;
  if (p) {
    for (i = 0; p[i]; ++i) {
      for (j = 0;; ++j) {
        if (!s[j]) {
          if (p[i][j] == '=') {
            return p[i] + j + 1;
          }
          break;
        }
        if (s[j] != p[i][j]) {
          break;
        }
      }
    }
  }
  return 0;
}

static char *Utoa(char p[21], unsigned long x) {
  char t;
  unsigned long i, a, b;
  i = 0;
  do {
    p[i++] = x % 10 + '0';
    x = x / 10;
  } while (x > 0);
  p[i] = '\0';
  if (i) {
    for (a = 0, b = i - 1; a < b; ++a, --b) {
      t = p[a];
      p[a] = p[b];
      p[b] = t;
    }
  }
  return p + i;
}

static char *Itoa(char p[21], long x) {
  if (x < 0) *p++ = '-', x = -(unsigned long)x;
  return Utoa(p, x);
}

#if TROUBLESHOOT
const char *DescribeOs(int os) {
  if (IsLinux()) {
    return "GNU/SYSTEMD";
  } else if (IsXnu()) {
    return "XNU";
  } else if (IsFreebsd()) {
    return "FREEBSD";
  } else if (IsOpenbsd()) {
    return "OPENBSD";
  } else if (IsNetbsd()) {
    return "NETBSD";
  } else {
    return "WUT";
  }
}
#endif

__attribute__((__noreturn__)) static void Exit(int rc, int os) {
  asm volatile("call\t*%2"
               : /* no outputs */
               : "a"((IsLinux() ? 60 : 1) | (IsXnu() ? 0x2000000 : 0)), "D"(rc),
                 "rm"(syscall_)
               : "memory");
  __builtin_unreachable();
}

static void Close(int fd, int os) {
  int ax, di;
  asm volatile("call\t*%4"
               : "=a"(ax), "=D"(di)
               : "0"((IsLinux() ? 3 : 6) | (IsXnu() ? 0x2000000 : 0)), "1"(fd),
                 "rm"(syscall_)
               : "rcx", "rdx", "rsi", "r8", "r9", "r10", "r11", "memory", "cc");
}

static int Read(int fd, void *data, int size, int os) {
  long si;
  int ax, di, dx;
  asm volatile("call\t*%8"
               : "=a"(ax), "=D"(di), "=S"(si), "=d"(dx)
               : "0"((IsLinux() ? 0 : 3) | (IsXnu() ? 0x2000000 : 0)), "1"(fd),
                 "2"(data), "3"(size), "rm"(syscall_)
               : "rcx", "r8", "r9", "r10", "r11", "memory");
  return ax;
}

static void Write(int fd, const void *data, int size, int os) {
  long si;
  int ax, di, dx;
  asm volatile("call\t*%8"
               : "=a"(ax), "=D"(di), "=S"(si), "=d"(dx)
               : "0"((IsLinux() ? 1 : 4) | (IsXnu() ? 0x2000000 : 0)), "1"(fd),
                 "2"(data), "3"(size), "rm"(syscall_)
               : "rcx", "r8", "r9", "r10", "r11", "memory", "cc");
}

static void Execve(const char *prog, char **argv, char **envp, int os) {
  long ax, di, si, dx;
  asm volatile("call\t*%8"
               : "=a"(ax), "=D"(di), "=S"(si), "=d"(dx)
               : "0"(59 | (IsXnu() ? 0x2000000 : 0)), "1"(prog), "2"(argv),
                 "3"(envp), "rm"(syscall_)
               : "rcx", "r8", "r9", "r10", "r11", "memory", "cc");
}

static int Access(const char *path, int mode, int os) {
  int ax, si;
  long dx, di;
  asm volatile("call\t*%7"
               : "=a"(ax), "=D"(di), "=S"(si), "=d"(dx)
               : "0"((IsLinux() ? 21 : 33) | (IsXnu() ? 0x2000000 : 0)),
                 "1"(path), "2"(mode), "rm"(syscall_)
               : "rcx", "r8", "r9", "r10", "r11", "memory", "cc");
  return ax;
}

static int Msyscall(long p, long n, int os) {
  int ax;
  long di, si;
  if (!IsOpenbsd()) {
    return 0;
  } else {
    asm volatile("call\t*%6"
                 : "=a"(ax), "=D"(di), "=S"(si)
                 : "0"(37), "1"(p), "2"(n), "rm"(syscall_)
                 : "rcx", "rdx", "r8", "r9", "r10", "r11", "memory", "cc");
    return ax;
  }
}

static int Open(const char *path, int flags, int mode, int os) {
  long di;
  int ax, dx, si;
  asm volatile("call\t*%8"
               : "=a"(ax), "=D"(di), "=S"(si), "=d"(dx)
               : "0"((IsLinux() ? 2 : 5) | (IsXnu() ? 0x2000000 : 0)),
                 "1"(path), "2"(flags), "3"(mode), "rm"(syscall_)
               : "rcx", "r8", "r9", "r10", "r11", "memory", "cc");
  return ax;
}

__attribute__((__noinline__)) static long Mmap(long addr, long size, int prot,
                                               int flags, int fd, long off,
                                               int os) {
  long ax, di, si, dx;
  register int flags_ asm("r10") = flags;
  register int fd_ asm("r8") = fd;
  register long off_ asm("r9") = off;
  asm volatile("push\t%%r9\n\t"
               "push\t%%r9\n\t"
               "call\t*%7\n\t"
               "pop\t%%r9\n\t"
               "pop\t%%r9"
               : "=a"(ax), "=D"(di), "=S"(si), "=d"(dx), "+r"(flags_),
                 "+r"(fd_), "+r"(off_)
               : "rm"(syscall_),
                 "0"((IsLinux()     ? 9
                      : IsFreebsd() ? 477
                                    : 197) |
                     (IsXnu() ? 0x2000000 : 0)),
                 "1"(addr), "2"(size), "3"(prot)
               : "rcx", "r11", "memory", "cc");
  return ax;
}

int MunmapLinux(const void *addr, unsigned long size) {
  int ax;
  asm volatile("syscall"
               : "=a"(ax)
               : "0"(11), "D"(addr), "S"(size)
               : "rcx", "r11", "memory");
  return ax;
}

int PrctlLinux(int op, long a, long b, long c, long d) {
  int rc;
  asm volatile("mov\t%5,%%r10\n\t"
               "mov\t%6,%%r8\n\t"
               "syscall"
               : "=a"(rc)
               : "0"(157), "D"(op), "S"(a), "d"(b), "g"(c), "g"(d)
               : "rcx", "r8", "r10", "r11", "memory");
  return rc;
}

static void Emit(int os, const char *s) {
  Write(2, s, StrLen(s), os);
}

static void Perror(int os, const char *c, int rc, const char *s) {
  char ibuf[21];
  Emit(os, "ape error: ");
  Emit(os, c);
  Emit(os, ": ");
  Emit(os, s);
  if (rc) {
    Emit(os, " failed errno=");
    Itoa(ibuf, -rc);
    Emit(os, ibuf);
  }
  Emit(os, "\n");
}

__attribute__((__noreturn__)) static void Pexit(int os, const char *c, int rc,
                                                const char *s) {
  Perror(os, c, rc, s);
  Exit(127, os);
}

static int StrCmp(const char *l, const char *r) {
  unsigned long i = 0;
  while (l[i] == r[i] && r[i]) ++i;
  return (l[i] & 255) - (r[i] & 255);
}

static char EndsWithIgnoreCase(const char *p, unsigned long n, const char *s) {
  unsigned long i, m;
  if (n >= (m = StrLen(s))) {
    for (i = n - m; i < n; ++i) {
      if (ToLower(p[i]) != *s++) {
        return 0;
      }
    }
    return 1;
  } else {
    return 0;
  }
}

static char IsComPath(struct PathSearcher *ps) {
  return EndsWithIgnoreCase(ps->name, ps->namelen, ".com") ||
         EndsWithIgnoreCase(ps->name, ps->namelen, ".exe") ||
         EndsWithIgnoreCase(ps->name, ps->namelen, ".com.dbg");
}

static char AccessCommand(struct PathSearcher *ps, const char *suffix,
                          unsigned long pathlen) {
  unsigned long suffixlen;
  suffixlen = StrLen(suffix);
  if (pathlen + 1 + ps->namelen + suffixlen + 1 > sizeof(ps->path)) return 0;
  if (pathlen && ps->path[pathlen - 1] != '/') ps->path[pathlen++] = '/';
  MemCpy(ps->path + pathlen, ps->name, ps->namelen);
  MemCpy(ps->path + pathlen + ps->namelen, suffix, suffixlen + 1);
  return !Access(ps->path, X_OK, ps->os);
}

static char SearchPath(struct PathSearcher *ps, const char *suffix) {
  const char *p;
  unsigned long i;
  for (p = ps->syspath;;) {
    for (i = 0; p[i] && p[i] != ':'; ++i) {
      if (i < sizeof(ps->path)) {
        ps->path[i] = p[i];
      }
    }
    if (AccessCommand(ps, suffix, i)) {
      return 1;
    } else if (p[i] == ':') {
      p += i + 1;
    } else {
      return 0;
    }
  }
}

static char FindCommand(struct PathSearcher *ps, const char *suffix) {
  if (MemChr(ps->name, '/', ps->namelen) ||
      MemChr(ps->name, '\\', ps->namelen)) {
    ps->path[0] = 0;
    return AccessCommand(ps, suffix, 0);
  } else {
    if (AccessCommand(ps, suffix, 0)) return 1;
  }
  return SearchPath(ps, suffix);
}

static char *Commandv(struct PathSearcher *ps, int os, const char *name,
                      const char *syspath) {
  ps->os = os;
  ps->syspath = syspath ? syspath : "/bin:/usr/local/bin:/usr/bin";
  if (!(ps->namelen = StrLen((ps->name = name)))) return 0;
  if (ps->namelen + 1 > sizeof(ps->path)) return 0;
  if (FindCommand(ps, "") || (!IsComPath(ps) && FindCommand(ps, ".com"))) {
    return ps->path;
  } else {
    return 0;
  }
}

__attribute__((__noreturn__)) static void Spawn(int os, const char *exe, int fd,
                                                long *sp, char *page,
                                                struct ElfEhdr *e) {
  long rc;
  unsigned long i;
  int prot, flags;
  struct ElfPhdr *p;
  long code, codesize;
  if (e->e_type != ET_EXEC) {
    Pexit(os, exe, 0, "ELF e_type != ET_EXEC");
  }
  if (e->e_machine != EM_NEXGEN32E) {
    Pexit(os, exe, 0, "ELF e_machine != EM_NEXGEN32E");
  }
  if (e->e_ident[EI_CLASS] != ELFCLASS64) {
    Pexit(os, exe, 0, "ELF e_ident[EI_CLASS] != ELFCLASS64");
  }
  if (e->e_ident[EI_DATA] != ELFDATA2LSB) {
    Pexit(os, exe, 0, "ELF e_ident[EI_DATA] != ELFDATA2LSB");
  }
  if (e->e_phoff + e->e_phnum * sizeof(*p) > 0x1000) {
    Pexit(os, exe, 0, "ELF phdrs need to be in first page");
  }
  code = 0;
  codesize = 0;
  for (p = (struct ElfPhdr *)(page + e->e_phoff), i = e->e_phnum; i--;) {
    if (p[i].p_type == PT_DYNAMIC) {
      Pexit(os, exe, 0, "not a real executable");
    }
    if (p[i].p_type != PT_LOAD) continue;
    if ((p[i].p_vaddr | p[i].p_filesz | p[i].p_memsz | p[i].p_offset) & 0xfff) {
      Pexit(os, exe, 0, "APE phdrs must be 4096-aligned and 4096-padded");
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
      if ((rc = Mmap(p[i].p_vaddr + p[i].p_filesz, p[i].p_memsz - p[i].p_filesz,
                     prot, flags | MAP_ANONYMOUS, -1, 0, os)) < 0) {
        Pexit(os, exe, rc, "bss mmap()");
      }
    }
    if (p[i].p_filesz) {
      if ((rc = Mmap(p[i].p_vaddr, p[i].p_filesz, prot, flags, fd,
                     p[i].p_offset, os)) < 0) {
        Pexit(os, exe, rc, "image mmap()");
      }
    }
  }
  if (!code) {
    Pexit(os, exe, 0, "ELF needs PT_LOAD phdr w/ PF_X");
  }

#if SET_EXE_FILE
  // change /proc/pid/exe to new executable path
  if (IsLinux() && relocated) {
    MunmapLinux((char *)0x200000, (long)(_end - ehdr));
    PrctlLinux(PR_SET_MM, PR_SET_MM_EXE_FILE, fd, 0, 0);
  }
#endif

  if (relocated) {
    int ax;
    asm volatile("syscall"
                 : "=a"(ax)
                 : "0"(1), "D"(1), "S"("hello\n"), "d"(6)
                 : "rcx", "r11", "memory");
  }

  Close(fd, os);

  // authorize only the loaded program to issue system calls. if this
  // fails, then we pass a link to our syscall function to the program
  // since it probably means a userspace program executed this loader
  // and passed us a custom syscall function earlier.
  if (Msyscall(code, codesize, os) != -1) {
    syscall_ = 0;
  }

#if TROUBLESHOOT
  Emit(TROUBLESHOOT_OS, "preparing to jump\n");
#endif

  // we clear all the general registers we can to have some wiggle room
  // to extend the behavior of this loader in the future. we don't need
  // to clear the xmm registers since the ape loader should be compiled
  // with the -mgeneral-regs-only flag.
  register void *r8 asm("r8") = syscall_;
  asm volatile("xor\t%%eax,%%eax\n\t"
               "xor\t%%r9d,%%r9d\n\t"
               "xor\t%%r10d,%%r10d\n\t"
               "xor\t%%r11d,%%r11d\n\t"
               "xor\t%%ebx,%%ebx\n\t"    // netbsd doesnt't clear this
               "xor\t%%r12d,%%r12d\n\t"  // netbsd doesnt't clear this
               "xor\t%%r13d,%%r13d\n\t"  // netbsd doesnt't clear this
               "xor\t%%r14d,%%r14d\n\t"  // netbsd doesnt't clear this
               "xor\t%%r15d,%%r15d\n\t"  // netbsd doesnt't clear this
               "mov\t%%rdx,%%rsp\n\t"
               "xor\t%%edx,%%edx\n\t"
               "push\t%%rsi\n\t"
               "xor\t%%esi,%%esi\n\t"
               "xor\t%%ebp,%%ebp\n\t"
               "ret"
               : /* no outputs */
               : "D"(IsFreebsd() ? sp : 0), "S"(e->e_entry), "d"(sp), "c"(os),
                 "r"(r8)
               : "memory");
  __builtin_unreachable();
}

__attribute__((__noreturn__)) void ApeLoader(long di, long *sp, char dl,
                                             struct ApeLoader *handoff) {
  int rc;
  long *auxv;
  struct ElfEhdr *eh;
  int c, i, fd, os, argc;
  char *p, *exe, *prog, **argv, **envp, *page;
  static union {
    struct ElfEhdr ehdr;
    char p[0x1000];
  } u;

  // detect freebsd
  if (handoff) {
    os = handoff->os;
  } else if (SupportsXnu() && dl == XNU) {
    os = XNU;
  } else if (SupportsFreebsd() && di) {
    os = FREEBSD;
    sp = (long *)di;
  } else {
    os = 0;
  }

  // extract arguments
  argc = *sp;
  argv = (char **)(sp + 1);
  envp = (char **)(sp + 1 + argc + 1);
  auxv = (long *)(sp + 1 + argc + 1);
  for (;;) {
    if (!*auxv++) {
      break;
    }
  }

  // get syscall function pointer
  if (handoff && handoff->systemcall) {
    syscall_ = handoff->systemcall;
  } else {
    syscall_ = __syscall_loader;
  }

  if (handoff) {
    // we were called by ape_execve()
    // no argument parsing is needed
    // no path searching is needed
    exe = handoff->prog;
    fd = handoff->fd;
    exe = handoff->prog;
    page = handoff->page;
    eh = (struct ElfEhdr *)handoff->page;
  } else {

    // detect openbsd
    if (SupportsOpenbsd() && !os && !auxv[0]) {
      os = OPENBSD;
    }

    // detect netbsd
    if (SupportsNetbsd() && !os) {
      for (; auxv[0]; auxv += 2) {
        if (auxv[0] == AT_EXECFN_NETBSD) {
          os = NETBSD;
          break;
        }
      }
    }

    // default operating system
    if (!os) {
      os = LINUX;
    }

#if SET_EXE_FILE
    if (IsLinux() && !relocated) {
      char *b1 = (char *)0x200000;
      char *b2 = (char *)0x300000;
      void (*pApeLoader)(long, long *, char, struct ApeLoader *);
      Mmap((long)b2, (long)(_end - ehdr), PROT_READ | PROT_WRITE | PROT_EXEC,
           MAP_FIXED | MAP_PRIVATE | MAP_ANONYMOUS, -1, 0, os);
      relocated = 1;
      MemCpy(b2, b1, (long)(_end - ehdr));
      pApeLoader = (void *)((char *)&ApeLoader - b1 + b2);
      pApeLoader(di, sp, dl, handoff);
    }
#endif

    // we can load via shell, shebang, or binfmt_misc
    if (argc >= 3 && !StrCmp(argv[1], "-")) {
      // if the first argument is a hyphen then we give the user the
      // power to change argv[0] or omit it entirely. most operating
      // systems don't permit the omission of argv[0] but we do, b/c
      // it's specified by ANSI X3.159-1988.
      prog = (char *)sp[3];
      argc = sp[3] = sp[0] - 3;
      argv = (char **)((sp += 3) + 1);
    } else if (argc < 2) {
      Emit(os, "usage: ape   PROG [ARGV1,ARGV2,...]\n"
               "       ape - PROG [ARGV0,ARGV1,...]\n"
               "αcτµαlly pδrταblε εxεcµταblε loader v1.o\n"
               "copyright 2022 justine alexandra roberts tunney\n"
               "https://justine.lol/ape.html\n");
      Exit(1, os);
    } else {
      prog = (char *)sp[2];
      argc = sp[1] = sp[0] - 1;
      argv = (char **)((sp += 1) + 1);
    }

    if (!(exe = Commandv(&ps, os, prog, GetEnv(envp, "PATH")))) {
      Pexit(os, prog, 0, "not found (maybe chmod +x)");
    } else if ((fd = Open(exe, O_RDONLY, 0, os)) < 0) {
      Pexit(os, exe, fd, "open");
    } else if ((rc = Read(fd, u.p, sizeof(u.p), os)) < 0) {
      Pexit(os, exe, rc, "read");
    } else if (rc != sizeof(u.p) && Read32(u.p) != Read32("\177ELF")) {
      Pexit(os, exe, 0, "too small");
    }

    page = u.p;
    eh = &u.ehdr;
  }

#if TROUBLESHOOT
  Emit(TROUBLESHOOT_OS, "os = ");
  Emit(TROUBLESHOOT_OS, DescribeOs(os));
  Emit(TROUBLESHOOT_OS, "\n");
  for (i = 0; i < argc; ++i) {
    Emit(TROUBLESHOOT_OS, "argv = ");
    Emit(TROUBLESHOOT_OS, argv[i]);
    Emit(TROUBLESHOOT_OS, "\n");
  }
#endif

  if ((IsXnu() && Read32(page) == 0xFEEDFACE + 1) ||
      (!IsXnu() && Read32(page) == Read32("\177ELF"))) {
    Close(fd, os);
    Execve(exe, argv, envp, os);
  }

  // TODO(jart): Parse Mach-O for old APE binary support on XNU.
  for (p = page; p < page + sizeof(u.p); ++p) {
    if (Read64(p) != Read64("printf '")) continue;
    for (i = 0, p += 8; p + 3 < page + sizeof(u.p) && (c = *p++) != '\'';) {
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
      page[i++] = c;
    }
    if (i >= 64 && Read32(page) == Read32("\177ELF")) {
      Spawn(os, exe, fd, sp, page, eh);
    }
  }

  Pexit(os, exe, 0, "could not find printf elf in first page");
}
