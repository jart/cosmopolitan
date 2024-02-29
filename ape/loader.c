/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "ape/ape.h"

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

#define MIN(X, Y) ((Y) > (X) ? (X) : (Y))
#define MAX(X, Y) ((Y) < (X) ? (X) : (Y))

#define PATH_MAX 1024 /* XXX verify */

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

#ifdef __aarch64__
#define IsAarch64() 1
#else
#define IsAarch64() 0
#endif

#ifdef __cplusplus
#define EXTERN_C extern "C"
#else
#define EXTERN_C
#endif

#define O_RDONLY                    0
#define PROT_NONE                   0
#define PROT_READ                   1
#define PROT_WRITE                  2
#define PROT_EXEC                   4
#define MAP_SHARED                  1
#define MAP_PRIVATE                 2
#define MAP_FIXED                   16
#define MAP_ANONYMOUS               (IsLinux() ? 32 : 4096)
#define MAP_NORESERVE               (IsLinux() ? 16384 : 0)
#define ELFCLASS32                  1
#define ELFDATA2LSB                 1
#define EM_NEXGEN32E                62
#define EM_AARCH64                  183
#define ET_EXEC                     2
#define ET_DYN                      3
#define PT_LOAD                     1
#define PT_DYNAMIC                  2
#define PT_INTERP                   3
#define EI_CLASS                    4
#define EI_DATA                     5
#define PF_X                        1
#define PF_W                        2
#define PF_R                        4
#define AT_PHDR                     3
#define AT_PHENT                    4
#define AT_PHNUM                    5
#define AT_PAGESZ                   6
#define AT_FLAGS                    8
#define AT_FLAGS_PRESERVE_ARGV0_BIT 0
#define AT_FLAGS_PRESERVE_ARGV0     (1 << AT_FLAGS_PRESERVE_ARGV0_BIT)
#define AT_EXECFN_LINUX             31
#define AT_EXECFN_NETBSD            2014
#define X_OK                        1
#define XCR0_SSE                    2
#define XCR0_AVX                    4
#define PR_SET_MM                   35
#define PR_SET_MM_EXE_FILE          13

#define READ32(S)                                                      \
  ((unsigned)(255 & (S)[3]) << 030 | (unsigned)(255 & (S)[2]) << 020 | \
   (unsigned)(255 & (S)[1]) << 010 | (unsigned)(255 & (S)[0]) << 000)

#define READ64(S)                         \
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
  char buf[8192];
};

union ElfPhdrBuf {
  struct ElfPhdr phdr;
  char buf[1024];
};

struct PathSearcher {
  int os;
  int literally;
  const char *name;
  const char *syspath;
  unsigned long namelen;
  char path[PATH_MAX];
};

struct ApeLoader {
  union ElfPhdrBuf phdr;
  struct PathSearcher ps;
  char path[PATH_MAX];
};

EXTERN_C long SystemCall(long, long, long, long, long, long, long, int);
EXTERN_C void Launch(void *, long, void *, int, void *)
    __attribute__((__noreturn__));

extern char __executable_start[];
extern char _end[];

static unsigned long StrLen(const char *s) {
  unsigned long n = 0;
  while (*s++) ++n;
  return n;
}

static int StrCmp(const char *l, const char *r) {
  unsigned long i = 0;
  while (l[i] == r[i] && r[i]) ++i;
  return (l[i] & 255) - (r[i] & 255);
}

#if 0

static const char *StrRChr(const char *s, int c) {
  const char *b = 0;
  if (s) {
    for (; *s; ++s) {
      if (*s == c) {
        b = s;
      }
    }
  }
  return b;
}

static const char *BaseName(const char *s) {
  const char *b = StrRChr(s, '/');
  return b ? b + 1 : s;
}

#endif

static void Bzero(void *a, unsigned long n) {
  long z;
  char *p, *e;
  p = (char *)a;
  e = p + n;
  z = 0;
  while (p + sizeof(z) <= e) {
    __builtin_memcpy(p, &z, sizeof(z));
    p += sizeof(z);
  }
  while (p < e) {
    *p++ = 0;
  }
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
  long w;
  char *d;
  const char *s;
  unsigned long i;
  d = (char *)a;
  s = (const char *)b;
  if (d > s) {
    while (n >= sizeof(w)) {
      n -= sizeof(w);
      __builtin_memcpy(&w, s + n, sizeof(n));
      __builtin_memcpy(d + n, &w, sizeof(n));
    }
    while (n--) {
      d[n] = s[n];
    }
  } else {
    i = 0;
    while (i + sizeof(w) <= n) {
      __builtin_memcpy(&w, s + i, sizeof(i));
      __builtin_memcpy(d + i, &w, sizeof(i));
      i += sizeof(w);
    }
    for (; i < n; ++i) {
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

static char *Utoa(char p[20], unsigned long x) {
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

__attribute__((__noinline__)) static long CallSystem(long arg1, long arg2,
                                                     long arg3, long arg4,
                                                     long arg5, long arg6,
                                                     long arg7, int numba,
                                                     char os) {
  if (IsXnu()) numba |= 0x2000000;
  return SystemCall(arg1, arg2, arg3, arg4, arg5, arg6, arg7, numba);
}

__attribute__((__noreturn__)) static void Exit(long rc, int os) {
  int numba;
  if (IsLinux()) {
    if (IsAarch64()) {
      numba = 94;
    } else {
      numba = 60;
    }
  } else {
    numba = 1;
  }
  CallSystem(rc, 0, 0, 0, 0, 0, 0, numba, os);
  __builtin_unreachable();
}

static int Close(int fd, int os) {
  int numba;
  if (IsLinux()) {
    if (IsAarch64()) {
      numba = 57;
    } else {
      numba = 3;
    }
  } else {
    numba = 6;
  }
  return CallSystem(fd, 0, 0, 0, 0, 0, 0, numba, os);
}

static long Pread(int fd, void *data, unsigned long size, long off, int os) {
  long numba;
  if (IsLinux()) {
    if (IsAarch64()) {
      numba = 0x043;
    } else {
      numba = 0x011;
    }
  } else if (IsXnu()) {
    numba = 0x2000099;
  } else if (IsFreebsd()) {
    numba = 0x1db;
  } else if (IsOpenbsd()) {
    numba = 0x0a9; /* OpenBSD v7.3+ */
  } else if (IsNetbsd()) {
    numba = 0x0ad;
  } else {
    __builtin_unreachable();
  }
  return SystemCall(fd, (long)data, size, off, off, 0, 0, numba);
}

static long Write(int fd, const void *data, unsigned long size, int os) {
  int numba;
  if (IsLinux()) {
    if (IsAarch64()) {
      numba = 64;
    } else {
      numba = 1;
    }
  } else {
    numba = 4;
  }
  return CallSystem(fd, (long)data, size, 0, 0, 0, 0, numba, os);
}

static int Access(const char *path, int mode, int os) {
  if (IsLinux() && IsAarch64()) {
    return SystemCall(-100, (long)path, mode, 0, 0, 0, 0, 48);
  } else {
    return CallSystem((long)path, mode, 0, 0, 0, 0, 0, IsLinux() ? 21 : 33, os);
  }
}

static int Msyscall(long p, unsigned long n, int os) {
  if (IsOpenbsd()) {
    return SystemCall(p, n, 0, 0, 0, 0, 0, 37);
  } else {
    return 0;
  }
}

static int Open(const char *path, int flags, int mode, int os) {
  if (IsLinux() && IsAarch64()) {
    return SystemCall(-100, (long)path, flags, mode, 0, 0, 0, 56);
  } else {
    return CallSystem((long)path, flags, mode, 0, 0, 0, 0, IsLinux() ? 2 : 5,
                      os);
  }
}

static int Mprotect(void *addr, unsigned long size, int prot, int os) {
  int numba;
  if (IsLinux()) {
    if (IsAarch64()) {
      numba = 226;
    } else {
      numba = 10;
    }
  } else {
    numba = 74;
  }
  return CallSystem((long)addr, size, prot, 0, 0, 0, 0, numba, os);
}

static long Mmap(void *addr, unsigned long size, int prot, int flags, int fd,
                 long off, int os) {
  long numba;
  if (IsLinux()) {
    if (IsAarch64()) {
      numba = 222;
    } else {
      numba = 9;
    }
  } else if (IsXnu()) {
    numba = 0x2000000 | 197;
  } else if (IsFreebsd()) {
    numba = 477;
  } else if (IsOpenbsd()) {
    numba = 49; /* OpenBSD v7.3+ */
  } else if (IsNetbsd()) {
    numba = 197;
  } else {
    __builtin_unreachable();
  }
  return SystemCall((long)addr, size, prot, flags, fd, off, off, numba);
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

static long Printf(int os, int fd, const char *fmt, ...) {
  int i;
  char c;
  int k = 0;
  unsigned u;
  char b[512];
  const char *s;
  unsigned long d;
  __builtin_va_list va;
  __builtin_va_start(va, fmt);
  for (;;) {
    switch ((c = *fmt++)) {
      case '\0':
        __builtin_va_end(va);
        return Write(fd, b, k, os);
      case '%':
        switch ((c = *fmt++)) {
          case 's':
            for (s = __builtin_va_arg(va, const char *); s && *s; ++s) {
              if (k < 512) b[k++] = *s;
            }
            break;
          case 'd':
            d = __builtin_va_arg(va, unsigned long);
            for (i = 16; i--;) {
              u = (d >> (i * 4)) & 15;
              if (u < 10) {
                c = '0' + u;
              } else {
                u -= 10;
                c = 'a' + u;
              }
              if (k < 512) b[k++] = c;
            }
            break;
          default:
            if (k < 512) b[k++] = c;
            break;
        }
        break;
      default:
        if (k < 512) b[k++] = c;
        break;
    }
  }
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

static char AccessCommand(struct PathSearcher *ps, unsigned long pathlen) {
  if (pathlen + 1 + ps->namelen + 1 > sizeof(ps->path)) return 0;
  if (pathlen && ps->path[pathlen - 1] != '/') ps->path[pathlen++] = '/';
  MemMove(ps->path + pathlen, ps->name, ps->namelen);
  ps->path[pathlen + ps->namelen] = 0;
  return !Access(ps->path, X_OK, ps->os);
}

static char SearchPath(struct PathSearcher *ps) {
  const char *p;
  unsigned long i;
  for (p = ps->syspath;;) {
    for (i = 0; p[i] && p[i] != ':'; ++i) {
      if (i < sizeof(ps->path)) {
        ps->path[i] = p[i];
      }
    }
    if (AccessCommand(ps, i)) {
      return 1;
    } else if (p[i] == ':') {
      p += i + 1;
    } else {
      return 0;
    }
  }
}

static char *Commandv(struct PathSearcher *ps, int os, char *name,
                      const char *syspath) {
  if (!(ps->namelen = StrLen((ps->name = name)))) return 0;
  if (ps->literally || MemChr(ps->name, '/', ps->namelen)) return name;
  ps->os = os;
  ps->syspath = syspath ? syspath : "/bin:/usr/local/bin:/usr/bin";
  if (ps->namelen + 1 > sizeof(ps->path)) return 0;
  ps->path[0] = 0;
  if (SearchPath(ps)) {
    return ps->path;
  } else {
    return 0;
  }
}

__attribute__((__noreturn__)) static void Spawn(int os, char *exe, int fd,
                                                long *sp, unsigned long pagesz,
                                                struct ElfEhdr *e,
                                                struct ElfPhdr *p) {
  long rc;
  int prot;
  int flags;
  int found_code;
  int found_entry;
  long code, codesize;
  unsigned long dynbase;
  unsigned long virtmin, virtmax;
  unsigned long a, b, c, d, i, j;

  /* validate elf */
  code = 0;
  codesize = 0;
  found_code = 0;
  found_entry = 0;
  virtmin = virtmax = 0;
  if (pagesz & (pagesz - 1)) {
    Pexit(os, exe, 0, "AT_PAGESZ isn't two power");
  }
  for (i = 0; i < e->e_phnum; ++i) {
    if (p[i].p_type != PT_LOAD) {
      continue;
    }
    if (p[i].p_filesz > p[i].p_memsz) {
      Pexit(os, exe, 0, "ELF p_filesz exceeds p_memsz");
    }
    if ((p[i].p_vaddr & (pagesz - 1)) != (p[i].p_offset & (pagesz - 1))) {
      Pexit(os, exe, 0, "ELF p_vaddr incongruent w/ p_offset modulo AT_PAGESZ");
    }
    if (p[i].p_vaddr + p[i].p_memsz < p[i].p_vaddr ||
        p[i].p_vaddr + p[i].p_memsz + (pagesz - 1) < p[i].p_vaddr) {
      Pexit(os, exe, 0, "ELF p_vaddr + p_memsz overflow");
    }
    if (p[i].p_offset + p[i].p_filesz < p[i].p_offset ||
        p[i].p_offset + p[i].p_filesz + (pagesz - 1) < p[i].p_offset) {
      Pexit(os, exe, 0, "ELF p_offset + p_filesz overflow");
    }
    a = p[i].p_vaddr & -pagesz;
    b = (p[i].p_vaddr + p[i].p_memsz + (pagesz - 1)) & -pagesz;
    if (MAX(a, (unsigned long)__executable_start) <
        MIN(b, (unsigned long)_end)) {
      Pexit(os, exe, 0, "ELF segments overlap your APE loader");
    }
    for (j = i + 1; j < e->e_phnum; ++j) {
      if (p[j].p_type != PT_LOAD) continue;
      c = p[j].p_vaddr & -pagesz;
      d = (p[j].p_vaddr + p[j].p_memsz + (pagesz - 1)) & -pagesz;
      if (MAX(a, c) < MIN(b, d)) {
        Pexit(os, exe, 0, "ELF segments overlap each others virtual memory");
      }
    }
    if (p[i].p_flags & PF_X) {
      if (!found_code) {
        code = p[i].p_vaddr;
        codesize = p[i].p_filesz;
      }
      if (p[i].p_vaddr <= e->e_entry &&
          e->e_entry < p[i].p_vaddr + p[i].p_memsz) {
        found_entry = 1;
      }
    }
    if (p[i].p_vaddr < virtmin) {
      virtmin = p[i].p_vaddr;
    }
    if (p[i].p_vaddr + p[i].p_memsz > virtmax) {
      virtmax = p[i].p_vaddr + p[i].p_memsz;
    }
  }
  if (!found_entry) {
    Pexit(os, exe, 0, "ELF entrypoint not found in PT_LOAD with PF_X");
  }

  /* choose loading address for dynamic elf executables
     that maintains relative distances between segments */
  if (e->e_type == ET_DYN) {
    rc = Mmap(0, virtmax - virtmin, PROT_NONE,
              MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE, -1, 0, os);
    if (rc < 0) Pexit(os, exe, rc, "pie mmap");
    dynbase = rc;
    if (dynbase & (pagesz - 1)) {
      Pexit(os, exe, 0, "OS mmap incongruent w/ AT_PAGESZ");
    }
    if (dynbase + virtmin < dynbase) {
      Pexit(os, exe, 0, "ELF dynamic base overflow");
    }
  } else {
    dynbase = 0;
  }

  /* load elf */
  for (i = 0; i < e->e_phnum; ++i) {
    void *addr;
    unsigned long size;
    if (p[i].p_type != PT_LOAD) continue;

    /* configure mapping */
    prot = 0;
    flags = MAP_FIXED | MAP_PRIVATE;
    if (p[i].p_flags & PF_R) prot |= PROT_READ;
    if (p[i].p_flags & PF_W) prot |= PROT_WRITE;
    if (p[i].p_flags & PF_X) prot |= PROT_EXEC;

    if (p[i].p_filesz) {
      /* load from file */
      int prot1, prot2;
      unsigned long wipe;
      prot1 = prot;
      prot2 = prot;
      /* when we ask the system to map the interval [vaddr,vaddr+filesz)
         it might schlep extra file content into memory on both the left
         and the righthand side. that's because elf doesn't require that
         either side of the interval be aligned on the system page size.
         normally we can get away with ignoring these junk bytes. but if
         the segment defines bss memory (i.e. memsz > filesz) then we'll
         need to clear the extra bytes in the page, if they exist. since
         we can't do that if we're mapping a read-only page, we can just
         map it with write permissions and call mprotect on it afterward */
      a = p[i].p_vaddr + p[i].p_filesz; /* end of file content */
      b = (a + (pagesz - 1)) & -pagesz; /* first pure bss page */
      c = p[i].p_vaddr + p[i].p_memsz;  /* end of segment data */
      wipe = MIN(b - a, c - a);
      if (wipe && (~prot1 & PROT_WRITE)) {
        prot1 = PROT_READ | PROT_WRITE;
      }
      addr = (void *)(dynbase + (p[i].p_vaddr & -pagesz));
      size = (p[i].p_vaddr & (pagesz - 1)) + p[i].p_filesz;
      rc = Mmap(addr, size, prot1, flags, fd, p[i].p_offset & -pagesz, os);
      if (rc < 0) Pexit(os, exe, rc, "prog mmap");
      if (wipe) Bzero((void *)(dynbase + a), wipe);
      if (prot2 != prot1) {
        rc = Mprotect(addr, size, prot2, os);
        if (rc < 0) Pexit(os, exe, rc, "prog mprotect");
      }
      /* allocate extra bss */
      if (c > b) {
        flags |= MAP_ANONYMOUS;
        rc = Mmap((void *)(dynbase + b), c - b, prot, flags, -1, 0, os);
        if (rc < 0) Pexit(os, exe, rc, "extra bss mmap");
      }
    } else {
      /* allocate pure bss */
      addr = (void *)(dynbase + (p[i].p_vaddr & -pagesz));
      size = (p[i].p_vaddr & (pagesz - 1)) + p[i].p_memsz;
      flags |= MAP_ANONYMOUS;
      rc = Mmap(addr, size, prot, flags, -1, 0, os);
      if (rc < 0) Pexit(os, exe, rc, "bss mmap");
    }
  }

  /* finish up */
  Close(fd, os);
  Msyscall(dynbase + code, codesize, os);

  /* call program entrypoint */
  Launch(IsFreebsd() ? sp : 0, dynbase + e->e_entry, exe, os, sp);
}

static const char *TryElf(struct ApeLoader *M, union ElfEhdrBuf *ebuf,
                          char *exe, int fd, long *sp, long *auxv,
                          unsigned long pagesz, int os) {
  long i, rc;
  unsigned size;
  struct ElfEhdr *e;
  struct ElfPhdr *p;

  /* validate page size */
  if (!pagesz) pagesz = 4096;
  if (pagesz & (pagesz - 1)) {
    Pexit(os, exe, 0, "AT_PAGESZ isn't two power");
  }

  /* validate elf header */
  e = &ebuf->ehdr;
  if (READ32(ebuf->buf) != READ32("\177ELF")) {
    return "didn't embed ELF magic";
  }
  if (e->e_ident[EI_CLASS] == ELFCLASS32) {
    return "32-bit ELF isn't supported";
  }
  if (e->e_type != ET_EXEC && e->e_type != ET_DYN) {
    return "ELF not ET_EXEC or ET_DYN";
  }
#ifdef __aarch64__
  if (e->e_machine != EM_AARCH64) {
    return "couldn't find ELF header with AARCH64 machine type";
  }
#else
  if (e->e_machine != EM_NEXGEN32E) {
    return "couldn't find ELF header with x86-64 machine type";
  }
#endif
  if (e->e_phentsize != sizeof(struct ElfPhdr)) {
    Pexit(os, exe, 0, "e_phentsize is wrong");
  }
  size = e->e_phnum;
  if ((size *= sizeof(struct ElfPhdr)) > sizeof(M->phdr.buf)) {
    Pexit(os, exe, 0, "too many ELF program headers");
  }

  /* read program headers */
  rc = Pread(fd, M->phdr.buf, size, e->e_phoff, os);
  if (rc < 0) return "failed to read ELF program headers";
  if (rc != size) return "truncated read of ELF program headers";

  /* bail on recoverable program header errors */
  p = &M->phdr.phdr;
  for (i = 0; i < e->e_phnum; ++i) {
    if (p[i].p_type == PT_INTERP) {
      return "ELF has PT_INTERP which isn't supported";
    }
    if (p[i].p_type == PT_DYNAMIC) {
      return "ELF has PT_DYNAMIC which isn't supported";
    }
  }

  /* remove empty program headers */
  for (i = 0; i < e->e_phnum;) {
    if (p[i].p_type == PT_LOAD && !p[i].p_memsz) {
      if (i + 1 < e->e_phnum) {
        MemMove(p + i, p + i + 1,
                (e->e_phnum - (i + 1)) * sizeof(struct ElfPhdr));
      }
      --e->e_phnum;
    } else {
      ++i;
    }
  }

  /*
   * merge adjacent loads that are contiguous with equal protection,
   * which prevents our program header overlap check from needlessly
   * failing later on; it also shaves away a microsecond of latency,
   * since every program header requires invoking at least 1 syscall
   */
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
        MemMove(p + i + 1, p + i + 2,
                (e->e_phnum - (i + 2)) * sizeof(struct ElfPhdr));
      }
      --e->e_phnum;
    } else {
      ++i;
    }
  }

  /* fixup operating system provided auxiliary values */
  for (; *auxv; auxv += 2) {
    switch (*auxv) {
      case AT_PHDR:
        auxv[1] = (unsigned long)&M->phdr;
        break;
      case AT_PHENT:
        auxv[1] = e->e_phentsize;
        break;
      case AT_PHNUM:
        auxv[1] = e->e_phnum;
        break;
      default:
        break;
    }
  }

  /* we're now ready to load */
  Spawn(os, exe, fd, sp, pagesz, e, p);
}

__attribute__((__noreturn__)) static void ShowUsage(int os, int fd, int rc) {
  Print(os, fd,
        "NAME\n"
        "\n"
        "  actually portable executable loader version " APE_VERSION_STR "\n"
        "  copyrights 2024 justine alexandra roberts tunney\n"
        "  https://justine.lol/ape.html\n"
        "\n"
        "USAGE\n"
        "\n"
        "  ape   PROG [ARGV1,ARGV2,...]\n"
        "  ape - PROG [ARGV0,ARGV1,...]\n"
        "\n",
        0l);
  Exit(rc, os);
}

EXTERN_C __attribute__((__noreturn__)) void ApeLoader(long di, long *sp,
                                                      char dl) {
  int rc, n;
  unsigned i;
  const char *ape;
  int c, fd, os, argc;
  struct ApeLoader *M;
  char arg0, literally;
  unsigned long pagesz;
  union ElfEhdrBuf *ebuf;
  long *auxv, *ap, *endp, *sp2;
  char *p, *pe, *exe, *prog, **argv, **envp;

  (void)Printf;

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
  auxv = sp + 1 + argc + 1;
  for (;;) {
    if (!*auxv++) {
      break;
    }
  }

  /* determine ape loader program name */
  ape = argv[0];
  if (!ape) ape = "ape";

  /* detect openbsd */
  if (SupportsOpenbsd() && !os && !auxv[0]) {
    os = OPENBSD;
  }

  /* xnu passes auxv as an array of strings */
  if (os == XNU) {
    *auxv = 0;
  }

  /* detect netbsd and find end of words */
  pagesz = 0;
  arg0 = 0;
  for (ap = auxv; ap[0]; ap += 2) {
    if (ap[0] == AT_PAGESZ) {
      pagesz = ap[1];
    } else if (SupportsNetbsd() && !os && ap[0] == AT_EXECFN_NETBSD) {
      os = NETBSD;
    } else if (SupportsLinux() && ap[0] == AT_FLAGS) {
      // TODO(mrdomino): maybe set/insert this when we are called as "ape -".
      arg0 = !!(ap[1] & AT_FLAGS_PRESERVE_ARGV0);
    }
  }
  if (!pagesz) {
    pagesz = 4096;
  }
  endp = ap + 1;

  /* the default operating system */
  if (!os) {
    os = LINUX;
  }

  /* we can load via shell, shebang, or binfmt_misc */
  if (arg0) {
    literally = 1;
    prog = (char *)sp[2];
    argc = sp[2] = sp[0] - 2;
    argv = (char **)((sp += 2) + 1);
  } else if ((literally = argc >= 3 && !StrCmp(argv[1], "-"))) {
    /* if the first argument is a hyphen then we give the user the
       power to change argv[0] or omit it entirely. most operating
       systems don't permit the omission of argv[0] but we do, b/c
       it's specified by ANSI X3.159-1988. */
    prog = (char *)sp[3];
    argc = sp[3] = sp[0] - 3;
    argv = (char **)((sp += 3) + 1);
  } else if (argc < 2) {
    ShowUsage(os, 2, 1);
  } else {
    if (argv[1][0] == '-') {
      rc = !((argv[1][1] == 'h' && !argv[1][2]) ||
             !StrCmp(argv[1] + 1, "-help"));
      ShowUsage(os, 1 + rc, rc);
    }
    prog = (char *)sp[2];
    argc = sp[1] = sp[0] - 1;
    argv = (char **)((sp += 1) + 1);
  }

  /* allocate loader memory in program's arg block */
  n = sizeof(struct ApeLoader);
  M = (struct ApeLoader *)__builtin_alloca(n);
  M->ps.literally = literally;

  /* create new bottom of stack for spawned program
     system v abi aligns this on a 16-byte boundary
     grows down the alloc by poking the guard pages */
  n = (endp - sp + 1) * sizeof(long);
  sp2 = (long *)__builtin_alloca(n);
  if ((long)sp2 & 15) ++sp2;
  for (; n > 0; n -= pagesz) {
    ((char *)sp2)[n - 1] = 0;
  }
  MemMove(sp2, sp, (endp - sp) * sizeof(long));
  argv = (char **)(sp2 + 1);
  envp = (char **)(sp2 + 1 + argc + 1);
  auxv = sp2 + (auxv - sp);
  sp = sp2;

  /* allocate ephemeral memory for reading file */
  n = sizeof(union ElfEhdrBuf);
  ebuf = (union ElfEhdrBuf *)__builtin_alloca(n);
  for (; n > 0; n -= pagesz) {
    ((char *)ebuf)[n - 1] = 0;
  }

  /* resolve path of executable and read its first page */
  if (!(exe = Commandv(&M->ps, os, prog, GetEnv(envp, "PATH")))) {
    Pexit(os, prog, 0, "not found (maybe chmod +x or ./ needed)");
  } else if ((fd = Open(exe, O_RDONLY, 0, os)) < 0) {
    Pexit(os, exe, fd, "open");
  } else if ((rc = Pread(fd, ebuf->buf, sizeof(ebuf->buf), 0, os)) < 0) {
    Pexit(os, exe, rc, "read");
  } else if ((unsigned long)rc < sizeof(ebuf->ehdr)) {
    Pexit(os, exe, 0, "too small");
  }
  pe = ebuf->buf + rc;

  /* ape intended behavior
     1. if ape, will scan shell script for elf printf statements
     2. shell script may have multiple lines producing elf headers
     3. all elf printf lines must exist in the first 8192 bytes of file
     4. elf program headers may appear anywhere in the binary */
  if (READ64(ebuf->buf) == READ64("MZqFpD='") ||
      READ64(ebuf->buf) == READ64("jartsr='") ||
      READ64(ebuf->buf) == READ64("APEDBG='")) {
    for (p = ebuf->buf; p < pe; ++p) {
      if (READ64(p) != READ64("printf '")) {
        continue;
      }
      for (i = 0, p += 8; p + 3 < pe && (c = *p++) != '\'';) {
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
        ebuf->buf[i++] = c;
        if (i >= sizeof(ebuf->buf)) {
          break;
        }
      }
      if (i >= sizeof(ebuf->ehdr)) {
        TryElf(M, ebuf, exe, fd, sp, auxv, pagesz, os);
      }
    }
  }
  Pexit(os, exe, 0, TryElf(M, ebuf, exe, fd, sp, auxv, pagesz, os));
}
