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

#define PAGE_SIZE 4096
#define NULL_PAGE 2097152

#define LINUX   1
#define XNU     8
#define OPENBSD 16
#define FREEBSD 32
#define NETBSD  64

#define MIN(X, Y) ((Y) > (X) ? (X) : (Y))
#define MAX(X, Y) ((Y) < (X) ? (X) : (Y))

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
#define ELFCLASS32         1
#define ELFDATA2LSB        1
#define EM_NEXGEN32E       62
#define ET_EXEC            2
#define PT_LOAD            1
#define PT_DYNAMIC         2
#define PT_INTERP          3
#define EI_CLASS           4
#define EI_DATA            5
#define PF_X               1
#define PF_W               2
#define PF_R               4
#define AT_PHDR            3
#define AT_PHENT           4
#define AT_PHNUM           5
#define AT_EXECFN_LINUX    31
#define AT_EXECFN_NETBSD   2014
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

union ElfEhdrBuf {
  struct ElfEhdr ehdr;
  char buf[4096];
};

union ElfPhdrBuf {
  struct ElfPhdr phdr;
  char buf[4096];
};

struct PathSearcher {
  int os;
  const char *name;
  const char *syspath;
  unsigned long namelen;
  char path[1024];
};

struct ApeLoader {
  union ElfEhdrBuf ehdr;
  union ElfPhdrBuf phdr;
  struct PathSearcher ps;
  char path[1024];
};

long SystemCall(long arg1, long arg2, long arg3, long arg4, long arg5,
                long arg6, long arg7, long magi);

extern char __executable_start[];
extern char _end[];

static int ToLower(int c) {
  return 'A' <= c && c <= 'Z' ? c + ('a' - 'A') : c;
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

static void *MemMove(void *a, const void *b, unsigned long n) {
  char *d = a;
  unsigned long i;
  const char *s = b;
  if (d > s) {
    for (i = n; i--;) {
      d[i] = s[i];
    }
  } else {
    for (i = 0; i < n; ++i) {
      d[i] = s[i];
    }
  }
  return d;
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

__attribute__((__noreturn__)) static void Exit(int rc, int os) {
  SystemCall(rc, 0, 0, 0, 0, 0, 0,
             (IsLinux() ? 60 : 1) | (IsXnu() ? 0x2000000 : 0));
  __builtin_unreachable();
}

static int Close(int fd, int os) {
  return SystemCall(fd, 0, 0, 0, 0, 0, 0,
                    (IsLinux() ? 3 : 6) | (IsXnu() ? 0x2000000 : 0));
}

static long Pread(int fd, void *data, unsigned long size, long off, int os) {
  long magi;
  if (IsLinux()) {
    magi = 0x011;
  } else if (IsXnu()) {
    magi = 0x2000099;
  } else if (IsFreebsd()) {
    magi = 0x1db;
  } else if (IsOpenbsd()) {
    magi = 0x0a9; /* OpenBSD v7.3+ */
  } else if (IsNetbsd()) {
    magi = 0x0ad;
  } else {
    __builtin_unreachable();
  }
  return SystemCall(fd, (long)data, size, off, off, 0, 0, magi);
}

static long Write(int fd, const void *data, unsigned long size, int os) {
  return SystemCall(fd, (long)data, size, 0, 0, 0, 0,
                    (IsLinux() ? 1 : 4) | (IsXnu() ? 0x2000000 : 0));
}

static int Execve(const char *prog, char **argv, char **envp, int os) {
  return SystemCall((long)prog, (long)argv, (long)envp, 0, 0, 0, 0,
                    59 | (IsXnu() ? 0x2000000 : 0));
}

static int Access(const char *path, int mode, int os) {
  return SystemCall((long)path, mode, 0, 0, 0, 0, 0,
                    (IsLinux() ? 21 : 33) | (IsXnu() ? 0x2000000 : 0));
}

static int Msyscall(long p, unsigned long n, int os) {
  if (IsOpenbsd()) {
    return SystemCall(p, n, 0, 0, 0, 0, 0, 37);
  } else {
    return 0;
  }
}

static int Open(const char *path, int flags, int mode, int os) {
  return SystemCall((long)path, flags, mode, 0, 0, 0, 0,
                    (IsLinux() ? 2 : 5) | (IsXnu() ? 0x2000000 : 0));
}

static int Mprotect(void *addr, unsigned long size, int prot, int os) {
  return SystemCall((long)addr, size, prot, 0, 0, 0, 0,
                    (IsLinux() ? 10 : 74) | (IsXnu() ? 0x2000000 : 0));
}

static long Mmap(void *addr, unsigned long size, int prot, int flags, int fd,
                 long off, int os) {
  long magi;
  if (IsLinux()) {
    magi = 9;
  } else if (IsXnu()) {
    magi = 0x2000000 | 197;
  } else if (IsFreebsd()) {
    magi = 477;
  } else if (IsOpenbsd()) {
    magi = 49; /* OpenBSD v7.3+ */
  } else if (IsNetbsd()) {
    magi = 197;
  } else {
    __builtin_unreachable();
  }
  return SystemCall((long)addr, size, prot, flags, fd, off, off, magi);
}

static long Print(int os, int fd, const char *s, ...) {
  int c;
  unsigned n;
  char b[512];
  __builtin_va_list va;
  __builtin_va_start(va, s);
  for (n = 0; s; s = __builtin_va_arg(va, const char *)) {
    while ((c = *s++)) {
      if (n < sizeof(b)) {
        b[n++] = c;
      }
    }
  }
  __builtin_va_end(va);
  return Write(fd, b, n, os);
}

static void Perror(int os, const char *thing, long rc, const char *reason) {
  char ibuf[21];
  ibuf[0] = 0;
  if (rc) Itoa(ibuf, -rc);
  Print(os, 2, "ape error: ", thing, ": ", reason,
        rc ? " failed w/ errno " : "", ibuf, "\n", 0l);
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

static void *MemSet(void *a, int c, unsigned long n) {
  char *d = a;
  unsigned long i;
  for (i = 0; i < n; ++i) {
    d[i] = c;
  }
  return d;
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
  MemMove(ps->path + pathlen, ps->name, ps->namelen);
  MemMove(ps->path + pathlen + ps->namelen, suffix, suffixlen + 1);
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
                                                long *sp, struct ElfEhdr *e,
                                                struct ElfPhdr *p) {
  long rc;
  int prot;
  int flags;
  int found_code;
  int found_entry;
  long code, codesize;
  unsigned long a, b, c, d, i, j;

  /* load elf */
  code = 0;
  codesize = 0;
  found_code = 0;
  found_entry = 0;
  for (i = 0; i < e->e_phnum; ++i) {

    /* validate program header */
    if (p[i].p_type == PT_INTERP) {
      Pexit(os, exe, 0, "ELF has PT_INTERP which is unsupported");
    }
    if (p[i].p_type == PT_DYNAMIC) {
      Pexit(os, exe, 0, "ELF has PT_DYNAMIC which is unsupported");
    }
    if (p[i].p_type != PT_LOAD) {
      continue;
    }
    if (!p[i].p_memsz) {
      continue;
    }
    if (p[i].p_filesz > p[i].p_memsz) {
      Pexit(os, exe, 0, "ELF phdr filesz exceeds memsz");
    }
    if ((p[i].p_vaddr & (PAGE_SIZE - 1)) != (p[i].p_offset & (PAGE_SIZE - 1))) {
      Pexit(os, exe, 0, "ELF phdr virt/off skew mismatch w.r.t. pagesize");
    }
    if (p[i].p_vaddr + p[i].p_memsz < p[i].p_vaddr ||
        p[i].p_vaddr + p[i].p_memsz + (PAGE_SIZE - 1) < p[i].p_vaddr) {
      Pexit(os, exe, 0, "ELF phdr vaddr+memsz overflow");
    }
    if (p[i].p_vaddr + p[i].p_filesz < p[i].p_vaddr ||
        p[i].p_vaddr + p[i].p_filesz + (PAGE_SIZE - 1) < p[i].p_vaddr) {
      Pexit(os, exe, 0, "ELF phdr vaddr+files overflow");
    }
    a = p[i].p_vaddr & -PAGE_SIZE;
    b = (p[i].p_vaddr + p[i].p_memsz + (PAGE_SIZE - 1)) & -PAGE_SIZE;
    if (MAX(a, 0) < MIN(b, NULL_PAGE)) {
      Pexit(os, exe, 0, "ELF overlaps NULL page");
    }
    if (MAX(a, (unsigned long)__executable_start) <
        MIN(b, (unsigned long)_end)) {
      Pexit(os, exe, 0, "ELF overlaps your APE loader");
    }
    for (j = i + 1; j < e->e_phnum; ++j) {
      if (p[j].p_type != PT_LOAD) continue;
      c = p[j].p_vaddr & -PAGE_SIZE;
      d = (p[j].p_vaddr + p[j].p_memsz + (PAGE_SIZE - 1)) & -PAGE_SIZE;
      if (MAX(a, c) < MIN(b, d)) {
        Pexit(os, exe, 0, "ELF overlaps its own vaspace");
      }
    }

    /* configure mapping */
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
      if (!found_code) {
        code = p[i].p_vaddr;
        codesize = p[i].p_filesz;
      }
      if (p[i].p_vaddr <= e->e_entry &&
          e->e_entry < p[i].p_vaddr + p[i].p_memsz) {
        found_entry = 1;
      }
    }

    /* load from file */
    if (p[i].p_filesz) {
      void *addr;
      unsigned long size;
      int dirty, prot1, prot2;
      dirty = 0;
      prot1 = prot;
      prot2 = prot;
      a = p[i].p_vaddr + p[i].p_filesz;
      b = (a + (PAGE_SIZE - 1)) & -PAGE_SIZE;
      c = p[i].p_vaddr + p[i].p_memsz;
      if (b > c) b = c;
      if (c > b) {
        dirty = 1;
        if (~prot1 & PROT_WRITE) {
          prot1 = PROT_READ | PROT_WRITE;
        }
      }
      addr = (void *)(p[i].p_vaddr & -PAGE_SIZE);
      size = (p[i].p_vaddr & (PAGE_SIZE - 1)) + p[i].p_filesz;
      rc = Mmap(addr, size, prot1, flags, fd, p[i].p_offset & -PAGE_SIZE, os);
      if (rc < 0) Pexit(os, exe, rc, "prog mmap");
      if (dirty) MemSet((void *)a, 0, b - a);
      if (prot2 != prot1) {
        rc = Mprotect(addr, size, prot2, os);
        if (rc < 0) Pexit(os, exe, rc, "prog mprotect");
      }
    }

    /* allocate extra bss */
    a = p[i].p_vaddr + p[i].p_filesz;
    a = (a + (PAGE_SIZE - 1)) & -PAGE_SIZE;
    b = p[i].p_vaddr + p[i].p_memsz;
    if (b > a) {
      rc = Mmap((void *)a, b - a, prot, flags | MAP_ANONYMOUS, 0, 0, os);
      if (rc < 0) Pexit(os, exe, rc, "bss mmap");
    }
  }

  /* finish up */
  if (!found_entry) {
    Pexit(os, exe, 0, "ELF entrypoint not found in PT_LOAD with PF_X");
  }
  Close(fd, os);
  Msyscall(code, codesize, os);

  /* we clear all the general registers we can to have some wiggle room
     to extend the behavior of this loader in the future. we don't need
     to clear the xmm registers since the ape loader should be compiled
     with the -mgeneral-regs-only flag. */
  asm volatile("xor\t%%eax,%%eax\n\t"
               "xor\t%%r8d,%%r8d\n\t"
               "xor\t%%r9d,%%r9d\n\t"
               "xor\t%%r10d,%%r10d\n\t"
               "xor\t%%r11d,%%r11d\n\t"
               "xor\t%%ebx,%%ebx\n\t"   /* netbsd doesnt't clear this */
               "xor\t%%r12d,%%r12d\n\t" /* netbsd doesnt't clear this */
               "xor\t%%r13d,%%r13d\n\t" /* netbsd doesnt't clear this */
               "xor\t%%r14d,%%r14d\n\t" /* netbsd doesnt't clear this */
               "xor\t%%r15d,%%r15d\n\t" /* netbsd doesnt't clear this */
               "mov\t%%rdx,%%rsp\n\t"
               "xor\t%%edx,%%edx\n\t"
               "push\t%%rsi\n\t"
               "xor\t%%esi,%%esi\n\t"
               "xor\t%%ebp,%%ebp\n\t"
               "ret"
               : /* no outputs */
               : "D"(IsFreebsd() ? sp : 0), "S"(e->e_entry), "d"(sp), "c"(os)
               : "memory");
  __builtin_unreachable();
}

static void TryElf(struct ApeLoader *M, const char *exe, int fd, long *sp,
                   long *auxv, int os) {
  unsigned size = M->ehdr.ehdr.e_phnum;
  if (Read32(M->ehdr.buf) == Read32("\177ELF") &&
      M->ehdr.ehdr.e_type == ET_EXEC &&
      M->ehdr.ehdr.e_machine == EM_NEXGEN32E &&
      M->ehdr.ehdr.e_ident[EI_CLASS] != ELFCLASS32 &&
      M->ehdr.ehdr.e_phentsize >= sizeof(M->phdr.phdr) &&
      (size *= M->ehdr.ehdr.e_phentsize) <= sizeof(M->phdr.buf) &&
      Pread(fd, M->phdr.buf, size, M->ehdr.ehdr.e_phoff, os) == size) {
    for (; *auxv; auxv += 2) {
      switch (*auxv) {
        case AT_PHDR:
          auxv[1] = (unsigned long)&M->phdr;
          break;
        case AT_PHENT:
          auxv[1] = M->ehdr.ehdr.e_phentsize;
          break;
        case AT_PHNUM:
          auxv[1] = M->ehdr.ehdr.e_phnum;
          break;
        default:
          break;
      }
    }
    Spawn(os, exe, fd, sp, &M->ehdr.ehdr, &M->phdr.phdr);
  }
}

__attribute__((__noreturn__)) void ApeLoader(long di, long *sp, char dl) {
  int rc;
  unsigned i, n;
  int c, fd, os, argc;
  struct ApeLoader *M;
  long *auxv, *ap, *ew;
  char *p, *exe, *prog, **argv, **envp;

  /* detect freebsd */
  if (SupportsXnu() && dl == XNU) {
    os = XNU;
  } else if (SupportsFreebsd() && di) {
    os = FREEBSD;
    sp = (long *)di;
  } else {
    os = 0;
  }

  /* extract arguments */
  argc = *sp;
  argv = (char **)(sp + 1);
  envp = (char **)(sp + 1 + argc + 1);
  auxv = (long *)(sp + 1 + argc + 1);
  for (;;) {
    if (!*auxv++) {
      break;
    }
  }

  /* detect openbsd */
  if (SupportsOpenbsd() && !os && !auxv[0]) {
    os = OPENBSD;
  }

  /* detect netbsd and find end of words */
  for (ap = auxv; ap[0]; ap += 2) {
    if (SupportsNetbsd() && !os && ap[0] == AT_EXECFN_NETBSD) {
      os = NETBSD;
    }
  }
  ew = ap + 1;

  /* allocate loader memory */
  n = sizeof(*M) / sizeof(long);
  MemMove(sp - n, sp, (char *)ew - (char *)sp);
  sp -= n, argv -= n, envp -= n, auxv -= n;
  M = (struct ApeLoader *)(ew - n);

  /* default operating system */
  if (!os) {
    os = LINUX;
  }

  /* we can load via shell, shebang, or binfmt_misc */
  if (argc >= 3 && !StrCmp(argv[1], "-")) {
    /* if the first argument is a hyphen then we give the user the
       power to change argv[0] or omit it entirely. most operating
       systems don't permit the omission of argv[0] but we do, b/c
       it's specified by ANSI X3.159-1988. */
    prog = (char *)sp[3];
    argc = sp[3] = sp[0] - 3;
    argv = (char **)((sp += 3) + 1);
  } else if (argc < 2) {
    Print(os, 2,
          "usage: ape   PROG [ARGV1,ARGV2,...]\n"
          "       ape - PROG [ARGV0,ARGV1,...]\n"
          "αcτµαlly pδrταblε εxεcµταblε loader v1.2\n"
          "copyright 2022 justine alexandra roberts tunney\n"
          "https://justine.lol/ape.html\n",
          0l);
    Exit(1, os);
  } else {
    prog = (char *)sp[2];
    argc = sp[1] = sp[0] - 1;
    argv = (char **)((sp += 1) + 1);
  }

  /* resolve path of executable and read its first page */
  if (!(exe = Commandv(&M->ps, os, prog, GetEnv(envp, "PATH")))) {
    Pexit(os, prog, 0, "not found (maybe chmod +x)");
  } else if ((fd = Open(exe, O_RDONLY, 0, os)) < 0) {
    Pexit(os, exe, fd, "open");
  } else if ((rc = Pread(fd, M->ehdr.buf, sizeof(M->ehdr.buf), 0, os)) < 0) {
    Pexit(os, exe, rc, "read");
  } else if (rc != sizeof(M->ehdr.buf)) {
    Pexit(os, exe, 0, "too small");
  }

  /* change argv[0] to resolved path if it's ambiguous */
  if (argc > 0 && *prog != '/' && *exe == '/' && !StrCmp(prog, argv[0])) {
    argv[0] = exe;
  }

  /* ape intended behavior
     1. if file is a native executable, try to run it natively
     2. if ape, will scan shell script for elf printf statements
     3. shell script may have multiple lines producing elf headers
     4. all elf printf lines must exist in the first 4096 bytes of file
     5. elf program headers may appear anywhere in the binary */
  if ((IsXnu() && Read32(M->ehdr.buf) == 0xFEEDFACE + 1) ||
      (!IsXnu() && Read32(M->ehdr.buf) == Read32("\177ELF"))) {
    Close(fd, os);
    Execve(exe, argv, envp, os);
  }
  if (Read64(M->ehdr.buf) == Read64("MZqFpD='") ||
      Read64(M->ehdr.buf) == Read64("jartsr='")) {
    for (p = M->ehdr.buf; p < M->ehdr.buf + sizeof(M->ehdr.buf); ++p) {
      if (Read64(p) != Read64("printf '")) {
        continue;
      }
      for (i = 0, p += 8;
           p + 3 < M->ehdr.buf + sizeof(M->ehdr.buf) && (c = *p++) != '\'';) {
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
        M->ehdr.buf[i++] = c;
      }
      if (i >= sizeof(M->ehdr.ehdr)) {
        TryElf(M, exe, fd, sp, auxv, os);
      }
    }
  }
  TryElf(M, exe, fd, sp, auxv, os);
  Pexit(os, exe, 0, "Not an acceptable APE/ELF executable for x86-64");
}
