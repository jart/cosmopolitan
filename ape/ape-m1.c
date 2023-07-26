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
#include <dispatch/dispatch.h>
#include <errno.h>
#include <fcntl.h>
#include <libkern/OSCacheControl.h>
#include <limits.h>
#include <pthread.h>
#include <signal.h>
#include <stdarg.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/random.h>
#include <sys/uio.h>
#include <time.h>
#include <unistd.h>

#define pagesz         16384
#define SYSLIB_MAGIC   ('s' | 'l' << 8 | 'i' << 16 | 'b' << 24)
#define SYSLIB_VERSION 1

struct Syslib {
  int magic;
  int version;
  long (*fork)(void);
  long (*pipe)(int[2]);
  long (*clock_gettime)(int, struct timespec *);
  long (*nanosleep)(const struct timespec *, struct timespec *);
  long (*mmap)(void *, size_t, int, int, int, off_t);
  int (*pthread_jit_write_protect_supported_np)(void);
  void (*pthread_jit_write_protect_np)(int);
  void (*sys_icache_invalidate)(void *, size_t);
  int (*pthread_create)(pthread_t *, const pthread_attr_t *, void *(*)(void *),
                        void *);
  void (*pthread_exit)(void *);
  int (*pthread_kill)(pthread_t, int);
  int (*pthread_sigmask)(int, const sigset_t *restrict, sigset_t *restrict);
  int (*pthread_setname_np)(const char *);
  dispatch_semaphore_t (*dispatch_semaphore_create)(long);
  long (*dispatch_semaphore_signal)(dispatch_semaphore_t);
  long (*dispatch_semaphore_wait)(dispatch_semaphore_t, dispatch_time_t);
  dispatch_time_t (*dispatch_walltime)(const struct timespec *, int64_t);
};

#define ELFCLASS32  1
#define ELFDATA2LSB 1
#define EM_AARCH64  183
#define ET_EXEC     2
#define ET_DYN      3
#define PT_LOAD     1
#define PT_DYNAMIC  2
#define PT_INTERP   3
#define EI_CLASS    4
#define EI_DATA     5
#define PF_X        1
#define PF_W        2
#define PF_R        4
#define AT_PHDR     3
#define AT_PHENT    4
#define AT_PHNUM    5
#define AT_PAGESZ   6
#define AT_BASE     7
#define AT_ENTRY    9
#define AT_UID      11
#define AT_EUID     12
#define AT_GID      13
#define AT_EGID     14
#define AT_HWCAP    16
#define AT_HWCAP2   16
#define AT_SECURE   23
#define AT_RANDOM   25
#define AT_EXECFN   31

#define STACK_SIZE  (8ul * 1024 * 1024)
#define STACK_ALIGN (sizeof(long) * 2)
#define AUXV_BYTES  (sizeof(long) * 2 * 15)

// from the xnu codebase
#define _COMM_PAGE_START_ADDRESS      0x0000000FFFFFC000ul
#define _COMM_PAGE_APRR_SUPPORT       (_COMM_PAGE_START_ADDRESS + 0x10C)
#define _COMM_PAGE_APRR_WRITE_ENABLE  (_COMM_PAGE_START_ADDRESS + 0x110)
#define _COMM_PAGE_APRR_WRITE_DISABLE (_COMM_PAGE_START_ADDRESS + 0x118)

#define MIN(X, Y) ((Y) > (X) ? (X) : (Y))
#define MAX(X, Y) ((Y) < (X) ? (X) : (Y))

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
  char buf[4096];
};

struct PathSearcher {
  unsigned long namelen;
  const char *name;
  const char *syspath;
  char path[1024];
};

struct ApeLoader {
  union ElfEhdrBuf ehdr;
  struct PathSearcher ps;
  // this memory shall be discarded by the guest
  //////////////////////////////////////////////
  // this memory shall be known to guest program
  union {
    char argblock[ARG_MAX];
    long numblock[ARG_MAX / sizeof(long)];
  };
  union ElfPhdrBuf phdr;
  struct Syslib lib;
  char rando[16];
};

static int ToLower(int c) {
  return 'A' <= c && c <= 'Z' ? c + ('a' - 'A') : c;
}

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

static const char *BaseName(const char *s) {
  int c;
  const char *b = "";
  if (s) {
    while ((c = *s++)) {
      if (c == '/') {
        b = s;
      }
    }
  }
  return b;
}

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

static void Emit(const char *s) {
  write(2, s, StrLen(s));
}

static long Print(int fd, const char *s, ...) {
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
  return write(fd, b, n);
}

static void Perror(const char *thing, long rc, const char *reason) {
  char ibuf[21];
  ibuf[0] = 0;
  if (rc) Itoa(ibuf, -rc);
  Print(2, "ape error: ", thing, ": ", reason, rc ? " failed w/ errno " : "",
        ibuf, "\n", 0l);
}

__attribute__((__noreturn__)) static void Pexit(const char *c, int failed,
                                                const char *s) {
  Perror(c, failed, s);
  _exit(127);
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
  return !access(ps->path, X_OK);
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
  if (MemChr(ps->name, '/', ps->namelen)) {
    ps->path[0] = 0;
    return AccessCommand(ps, suffix, 0);
  }
  return SearchPath(ps, suffix);
}

static char *Commandv(struct PathSearcher *ps, const char *name,
                      const char *syspath) {
  ps->syspath = syspath ? syspath : "/bin:/usr/local/bin:/usr/bin";
  if (!(ps->namelen = StrLen((ps->name = name)))) return 0;
  if (ps->namelen + 1 > sizeof(ps->path)) return 0;
  if (FindCommand(ps, "") || (!IsComPath(ps) && FindCommand(ps, ".com"))) {
    return ps->path;
  } else {
    return 0;
  }
}

static void pthread_jit_write_protect_np_workaround(int enabled) {
  int count_start = 8192;
  volatile int count = count_start;
  unsigned long *addr, *other, val, val2, reread = -1;
  addr = (unsigned long *)(!enabled ? _COMM_PAGE_APRR_WRITE_ENABLE
                                    : _COMM_PAGE_APRR_WRITE_DISABLE);
  other = (unsigned long *)(enabled ? _COMM_PAGE_APRR_WRITE_ENABLE
                                    : _COMM_PAGE_APRR_WRITE_DISABLE);
  switch (*(volatile unsigned char *)_COMM_PAGE_APRR_SUPPORT) {
    case 1:
      do {
        val = *addr;
        reread = -1;
        asm volatile("msr\tS3_4_c15_c2_7,%0\n"
                     "isb\tsy\n"
                     : /* no outputs */
                     : "r"(val)
                     : "memory");
        val2 = *addr;
        asm volatile("mrs\t%0,S3_4_c15_c2_7\n"
                     : "=r"(reread)
                     : /* no inputs */
                     : "memory");
        if (val2 == reread) {
          return;
        }
        usleep(10);
      } while (count-- > 0);
      break;
    case 3:
      do {
        val = *addr;
        reread = -1;
        asm volatile("msr\tS3_6_c15_c1_5,%0\n"
                     "isb\tsy\n"
                     : /* no outputs */
                     : "r"(val)
                     : "memory");
        val2 = *addr;
        asm volatile("mrs\t%0,S3_6_c15_c1_5\n"
                     : "=r"(reread)
                     : /* no inputs */
                     : "memory");
        if (val2 == reread) {
          return;
        }
        usleep(10);
      } while (count-- > 0);
      break;
    default:
      pthread_jit_write_protect_np(enabled);
      return;
  }
  Pexit("ape", 0, "failed to set jit write protection");
}

__attribute__((__noreturn__)) static void Spawn(const char *exe, int fd,
                                                long *sp, struct ElfEhdr *e,
                                                struct ElfPhdr *p,
                                                struct Syslib *lib) {
  long rc;
  int prot;
  int flags;
  int found_entry;
  unsigned long dynbase;
  unsigned long virtmin, virtmax;
  unsigned long a, b, c, d, i, j;

  /* validate elf */
  found_entry = 0;
  virtmin = virtmax = 0;
  for (i = 0; i < e->e_phnum; ++i) {
    if (p[i].p_type != PT_LOAD) {
      continue;
    }
    if (p[i].p_filesz > p[i].p_memsz) {
      Pexit(exe, 0, "ELF p_filesz exceeds p_memsz");
    }
    if ((p[i].p_flags & (PF_W | PF_X)) == (PF_W | PF_X)) {
      Pexit(exe, 0, "Apple Silicon doesn't allow RWX memory");
    }
    if ((p[i].p_vaddr & (pagesz - 1)) != (p[i].p_offset & (pagesz - 1))) {
      Pexit(exe, 0, "ELF p_vaddr incongruent w/ p_offset modulo 16384");
    }
    if (p[i].p_vaddr + p[i].p_memsz < p[i].p_vaddr ||
        p[i].p_vaddr + p[i].p_memsz + (pagesz - 1) < p[i].p_vaddr) {
      Pexit(exe, 0, "ELF p_vaddr + p_memsz overflow");
    }
    if (p[i].p_offset + p[i].p_filesz < p[i].p_offset ||
        p[i].p_offset + p[i].p_filesz + (pagesz - 1) < p[i].p_offset) {
      Pexit(exe, 0, "ELF p_offset + p_filesz overflow");
    }
    a = p[i].p_vaddr & -pagesz;
    b = (p[i].p_vaddr + p[i].p_memsz + (pagesz - 1)) & -pagesz;
    for (j = i + 1; j < e->e_phnum; ++j) {
      if (p[j].p_type != PT_LOAD) continue;
      c = p[j].p_vaddr & -pagesz;
      d = (p[j].p_vaddr + p[j].p_memsz + (pagesz - 1)) & -pagesz;
      if (MAX(a, c) < MIN(b, d)) {
        Pexit(exe, 0, "ELF segments overlap each others virtual memory");
      }
    }
    if (p[i].p_flags & PF_X) {
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
    Pexit(exe, 0, "ELF entrypoint not found in PT_LOAD with PF_X");
  }

  /* choose loading address for dynamic elf executables
     that maintains relative distances between segments */
  if (e->e_type == ET_DYN) {
    rc = (long)mmap(0, virtmax - virtmin, PROT_NONE,
                    MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (rc < 0) Pexit(exe, rc, "pie mmap");
    dynbase = rc;
    if (dynbase & (pagesz - 1)) {
      Pexit(exe, 0, "OS mmap incongruent w/ AT_PAGESZ");
    }
    if (dynbase + virtmin < dynbase) {
      Pexit(exe, 0, "ELF dynamic base overflow");
    }
  } else {
    dynbase = 0;
  }

  /* load elf */
  for (i = 0; i < e->e_phnum; ++i) {
    if (p[i].p_type != PT_LOAD) continue;
    if (!p[i].p_memsz) continue;

    /* configure mapping */
    prot = 0;
    flags = MAP_FIXED | MAP_PRIVATE;
    if (p[i].p_flags & PF_R) prot |= PROT_READ;
    if (p[i].p_flags & PF_W) prot |= PROT_WRITE;
    if (p[i].p_flags & PF_X) prot |= PROT_EXEC;

    /* load from file */
    if (p[i].p_filesz) {
      void *addr;
      int prot1, prot2;
      unsigned long size;
      prot1 = prot;
      prot2 = prot;
      a = p[i].p_vaddr + p[i].p_filesz; /* end of file content */
      b = (a + (pagesz - 1)) & -pagesz; /* first pure bss page */
      c = p[i].p_vaddr + p[i].p_memsz;  /* end of segment data */
      if (b > c) b = c;
      if (c > b && (~prot1 & PROT_WRITE)) {
        prot1 = PROT_READ | PROT_WRITE;
      }
      addr = (void *)(dynbase + (p[i].p_vaddr & -pagesz));
      size = (p[i].p_vaddr & (pagesz - 1)) + p[i].p_filesz;
      rc = (long)mmap(addr, size, prot1, flags, fd, p[i].p_offset & -pagesz);
      if (rc < 0) Pexit(exe, rc, "prog mmap");
      if (c > b) Bzero((void *)(dynbase + a), b - a);
      if (prot2 != prot1) {
        rc = mprotect(addr, size, prot2);
        if (rc < 0) Pexit(exe, rc, "prog mprotect");
      }
    }

    /* allocate extra bss */
    a = p[i].p_vaddr + p[i].p_filesz;
    a = (a + (pagesz - 1)) & -pagesz;
    b = p[i].p_vaddr + p[i].p_memsz;
    if (b > a) {
      flags |= MAP_ANONYMOUS;
      rc = (long)mmap((void *)(dynbase + a), b - a, prot, flags, -1, 0);
      if (rc < 0) Pexit(exe, rc, "bss mmap");
    }
  }

  /* finish up */
  close(fd);

  register long *x0 asm("x0") = sp;
  register struct Syslib *x15 asm("x15") = lib;
  register long x16 asm("x16") = e->e_entry;
  asm volatile("mov\tx1,#0\n\t"
               "mov\tx2,#0\n\t"
               "mov\tx3,#0\n\t"
               "mov\tx4,#0\n\t"
               "mov\tx5,#0\n\t"
               "mov\tx6,#0\n\t"
               "mov\tx7,#0\n\t"
               "mov\tx8,#0\n\t"
               "mov\tx9,#0\n\t"
               "mov\tx10,#0\n\t"
               "mov\tx11,#0\n\t"
               "mov\tx12,#0\n\t"
               "mov\tx13,#0\n\t"
               "mov\tx14,#0\n\t"
               "mov\tx17,#0\n\t"
               "mov\tx19,#0\n\t"
               "mov\tx20,#0\n\t"
               "mov\tx21,#0\n\t"
               "mov\tx22,#0\n\t"
               "mov\tx23,#0\n\t"
               "mov\tx24,#0\n\t"
               "mov\tx25,#0\n\t"
               "mov\tx26,#0\n\t"
               "mov\tx27,#0\n\t"
               "mov\tx28,#0\n\t"
               "mov\tx29,#0\n\t"
               "mov\tx30,#0\n\t"
               "mov\tsp,x0\n\t"
               "mov\tx0,#0\n\t"
               "br\tx16"
               : /* no outputs */
               : "r"(x0), "r"(x15), "r"(x16)
               : "memory");
  __builtin_unreachable();
}

static const char *TryElf(struct ApeLoader *M, const char *exe, int fd,
                          long *sp, long *bp, char *execfn) {
  long i, rc;
  unsigned size;
  struct ElfEhdr *e;
  struct ElfPhdr *p;

  /* validate elf header */
  e = &M->ehdr.ehdr;
  if (READ32(M->ehdr.buf) != READ32("\177ELF")) {
    return "didn't embed ELF magic";
  }
  if (e->e_ident[EI_CLASS] == ELFCLASS32) {
    return "32-bit ELF isn't supported";
  }
  if (e->e_type != ET_EXEC && e->e_type != ET_DYN) {
    return "ELF not ET_EXEC or ET_DYN";
  }
  if (e->e_machine != EM_AARCH64) {
    return "couldn't find ELF header with ARM64 machine type";
  }
  if (e->e_phentsize != sizeof(struct ElfPhdr)) {
    Pexit(exe, 0, "e_phentsize is wrong");
  }
  size = e->e_phnum;
  if ((size *= sizeof(struct ElfPhdr)) > sizeof(M->phdr.buf)) {
    Pexit(exe, 0, "too many ELF program headers");
  }

  /* read program headers */
  rc = pread(fd, M->phdr.buf, size, M->ehdr.ehdr.e_phoff);
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

  /* simulate linux auxiliary values */
  long auxv[][2] = {
      {AT_PHDR, (long)&M->phdr.phdr},        //
      {AT_PHENT, M->ehdr.ehdr.e_phentsize},  //
      {AT_PHNUM, M->ehdr.ehdr.e_phnum},      //
      {AT_ENTRY, M->ehdr.ehdr.e_entry},      //
      {AT_PAGESZ, pagesz},                   //
      {AT_UID, getuid()},                    //
      {AT_EUID, geteuid()},                  //
      {AT_GID, getgid()},                    //
      {AT_EGID, getegid()},                  //
      {AT_HWCAP, 0xffb3ffffu},               //
      {AT_HWCAP2, 0x181},                    //
      {AT_SECURE, issetugid()},              //
      {AT_RANDOM, (long)M->rando},           //
      {AT_EXECFN, (long)execfn},             //
      {0, 0},                                //
  };
  _Static_assert(sizeof(auxv) == AUXV_BYTES,
                 "Please update the AUXV_BYTES constant");
  MemMove(bp, auxv, sizeof(auxv));

  /* we're now ready to load */
  Spawn(exe, fd, sp, e, p, &M->lib);
}

__attribute__((__noinline__)) static long sysret(long rc) {
  return rc == -1 ? -errno : rc;
}

static long sys_fork(void) {
  return sysret(fork());
}

static long sys_pipe(int pfds[2]) {
  return sysret(pipe(pfds));
}

static long sys_clock_gettime(int clock, struct timespec *ts) {
  return sysret(clock_gettime(clock, ts));
}

static long sys_nanosleep(const struct timespec *req, struct timespec *rem) {
  return sysret(nanosleep(req, rem));
}

static long sys_mmap(void *addr, size_t size, int prot, int flags, int fd,
                     off_t off) {
  return sysret((long)mmap(addr, size, prot, flags, fd, off));
}

int main(int argc, char **argv, char **envp) {
  long z;
  void *map;
  long *sp, *bp, *ip;
  int c, i, n, fd, rc;
  struct ApeLoader *M;
  unsigned char rando[24];
  char *p, *pe, *tp, *exe, *prog, *execfn;

  // generate some hard random data
  if (getentropy(rando, sizeof(rando))) {
    Pexit(argv[0], -1, "getentropy");
  }

  // make the stack look like a linux one
  map = mmap((void *)(0x7f0000000000 | (long)rando[23] << 32), STACK_SIZE,
             PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  if (map == MAP_FAILED) {
    Pexit(argv[0], -1, "stack mmap");
  }

  // put argument block at top of allocated stack
  z = (long)map;
  z += STACK_SIZE - sizeof(struct ApeLoader);
  z &= -_Alignof(struct ApeLoader);
  M = (struct ApeLoader *)z;

  // expose screwy apple libs
  M->lib.magic = SYSLIB_MAGIC;
  M->lib.version = SYSLIB_VERSION;
  M->lib.fork = sys_fork;
  M->lib.pipe = sys_pipe;
  M->lib.clock_gettime = sys_clock_gettime;
  M->lib.nanosleep = sys_nanosleep;
  M->lib.mmap = sys_mmap;
  M->lib.pthread_jit_write_protect_supported_np =
      pthread_jit_write_protect_supported_np;
  M->lib.pthread_jit_write_protect_np = pthread_jit_write_protect_np_workaround;
  M->lib.pthread_create = pthread_create;
  M->lib.pthread_exit = pthread_exit;
  M->lib.pthread_kill = pthread_kill;
  M->lib.pthread_sigmask = pthread_sigmask;
  M->lib.pthread_setname_np = pthread_setname_np;
  M->lib.dispatch_semaphore_create = dispatch_semaphore_create;
  M->lib.dispatch_semaphore_signal = dispatch_semaphore_signal;
  M->lib.dispatch_semaphore_wait = dispatch_semaphore_wait;
  M->lib.dispatch_walltime = dispatch_walltime;

  // copy system provided argument block
  bp = M->numblock;
  tp = M->argblock + sizeof(M->argblock);
  *bp++ = argc;
  for (i = 0; i < argc; ++i) {
    tp -= (n = StrLen(argv[i]) + 1);
    MemMove(tp, argv[i], n);
    *bp++ = (long)tp;
  }
  *bp++ = 0;
  for (i = 0; envp[i]; ++i) {
    tp -= (n = StrLen(envp[i]) + 1);
    MemMove(tp, envp[i], n);
    *bp++ = (long)tp;
  }
  *bp++ = 0;

  // get arguments that point into our block
  sp = M->numblock;
  argc = *sp;
  argv = (char **)(sp + 1);
  envp = (char **)(sp + 1 + argc + 1);

  // xnu stores getauxval(at_execfn) in getenv("_")
  execfn = argc > 0 ? argv[0] : 0;
  for (i = 0; envp[i]; ++i) {
    if (envp[i][0] == '_' && envp[i][1] == '=') {
      execfn = envp[i] + 2;
      break;
    }
  }

  // interpret command line arguments
  if (argc >= 3 && !StrCmp(argv[1], "-")) {
    // if the first argument is a hyphen then we give the user the
    // power to change argv[0] or omit it entirely. most operating
    // systems don't permit the omission of argv[0] but we do, b/c
    // it's specified by ANSI X3.159-1988.
    prog = (char *)sp[3];
    argc = sp[3] = sp[0] - 3;
    argv = (char **)((sp += 3) + 1);
  } else if (argc < 2) {
    Emit("usage: ape   PROG [ARGV1,ARGV2,...]\n"
         "       ape - PROG [ARGV0,ARGV1,...]\n"
         "actually portable executable loader silicon 1.5\n"
         "copyright 2023 justine alexandra roberts tunney\n"
         "https://justine.lol/ape.html\n");
    _exit(1);
  } else {
    prog = (char *)sp[2];
    argc = sp[1] = sp[0] - 1;
    argv = (char **)((sp += 1) + 1);
  }

  // search for executable
  if (!(exe = Commandv(&M->ps, prog, GetEnv(envp, "PATH")))) {
    Pexit(prog, 0, "not found (maybe chmod +x)");
  } else if ((fd = openat(AT_FDCWD, exe, O_RDONLY)) < 0) {
    Pexit(exe, -1, "open");
  } else if ((rc = read(fd, M->ehdr.buf, sizeof(M->ehdr.buf))) < 0) {
    Pexit(exe, -1, "read");
  } else if ((unsigned long)rc < sizeof(M->ehdr.ehdr)) {
    Pexit(exe, 0, "too small");
  }
  pe = M->ehdr.buf + rc;

  // resolve argv[0] to reflect path search
  if ((argc > 0 && *prog != '/' && *exe == '/' && !StrCmp(prog, argv[0])) ||
      !StrCmp(BaseName(prog), argv[0])) {
    tp -= (n = StrLen(exe) + 1);
    MemMove(tp, exe, n);
    argv[0] = tp;
  }

  // squeeze and align the argument block
  ip = (long *)(((long)tp - AUXV_BYTES) & -sizeof(long));
  ip -= (n = bp - sp);
  ip = (long *)((long)ip & -STACK_ALIGN);
  MemMove(ip, sp, n * sizeof(long));
  bp = ip + n;
  sp = ip;

  // relocate the guest's random numbers
  MemMove(M->rando, rando, sizeof(M->rando));
  Bzero(rando, sizeof(rando));

  // ape intended behavior
  // 1. if file is an elf executable, it'll be used as-is
  // 2. if ape, will scan shell script for elf printf statements
  // 3. shell script may have multiple lines producing elf headers
  // 4. all elf printf lines must exist in the first 8192 bytes of file
  // 5. elf program headers may appear anywhere in the binary
  if (READ64(M->ehdr.buf) == READ64("MZqFpD='") ||
      READ64(M->ehdr.buf) == READ64("jartsr='") ||
      READ64(M->ehdr.buf) == READ64("APEDBG='")) {
    for (p = M->ehdr.buf; p < pe; ++p) {
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
        M->ehdr.buf[i++] = c;
        if (i >= sizeof(M->ehdr.buf)) {
          break;
        }
      }
      if (i >= sizeof(M->ehdr.ehdr)) {
        TryElf(M, exe, fd, sp, bp, execfn);
      }
    }
  }
  Pexit(exe, 0, TryElf(M, exe, fd, sp, bp, execfn));
}
