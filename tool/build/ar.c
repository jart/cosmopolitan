/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/ar.h"
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/iovec.h"
#include "libc/calls/struct/stat.h"
#include "libc/elf/def.h"
#include "libc/elf/elf.h"
#include "libc/elf/scalar.h"
#include "libc/elf/struct/sym.h"
#include "libc/errno.h"
#include "libc/fmt/itoa.h"
#include "libc/fmt/libgen.h"
#include "libc/fmt/magnumstrs.internal.h"
#include "libc/serialize.h"
#include "libc/intrin/bsr.h"
#include "libc/limits.h"
#include "libc/macros.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/stdckdint.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/s.h"
#include "tool/build/lib/getargs.h"

/**
 * @fileoverview cosmopolitan ar
 *
 * This static archiver is superior:
 *
 * - Isn't "accidentally quadratic" like GNU ar
 * - Goes 2x faster than LLVM ar while using 100x less memory
 * - Can be built as a 52kb APE binary that works well on six OSes
 *
 * This static archiver introduces handy features:
 *
 * - Arguments may be supplied in an `@args.txt` file
 * - Directory arguments are ignored
 *
 * @see https://www.unix.com/man-page/opensolaris/3head/ar.h/
 * @see https://en.wikipedia.org/wiki/Ar_(Unix)
 */

#define VERSION                     \
  "cosmopolitan ar v2.0\n"          \
  "copyright 2023 justine tunney\n" \
  "https://github.com/jart/cosmopolitan\n"

#define HEAP_SIZE (256L * 1024 * 1024)

struct Ints {
  int *p;
  size_t i;
};

struct Args {
  char **p;
  size_t i;
};

struct Bytes {
  char *p;
  size_t i;
};

static void SortChars(char *A, long n) {
  long i, j, t;
  for (i = 1; i < n; i++) {
    t = A[i];
    j = i - 1;
    while (j >= 0 && A[j] > t) {
      A[j + 1] = A[j];
      j = j - 1;
    }
    A[j + 1] = t;
  }
}

static wontreturn void Die(const char *path, const char *reason) {
  tinyprint(2, path, ": ", reason, "\n", NULL);
  exit(1);
}

static wontreturn void SysDie(const char *path, const char *func) {
  const char *errstr;
  if (!(errstr = _strerdoc(errno))) errstr = "Unknown error";
  tinyprint(2, path, ": ", func, ": ", errstr, "\n", NULL);
  exit(1);
}

static wontreturn void ShowUsage(int rc, int fd) {
  tinyprint(fd, VERSION,
            "\n"
            "USAGE\n"
            "\n",
            "  ", program_invocation_name, " FLAGS ARCHIVE FILE...\n",
            "\n"
            "FLAGS\n"
            "\n"
            "  rcs        create new archive with index\n"
            "  rcsD       always deterministic\n"
            "  --help     show usage\n"
            "  --version  show program details\n"
            "\n"
            "ARGUMENTS\n"
            "\n"
            "  ARCHIVE    should be foo.a\n"
            "  FILE       should be foo.o or @args.txt\n"
            "\n",
            NULL);
  exit(rc);
}

// allocates 𝑛 bytes of memory aligned on 𝑎 from .bss
// - avoids binary bloat of mmap() and malloc()
// - dies if out of memory or overflow occurs
// - new memory is always zero-initialized
// - can't be resized; use reballoc api
// - can't be freed or reclaimed
static void *balloc(size_t n, size_t a) {
  size_t c;
  int resizable;
  uintptr_t h, p;
  static size_t used;
  static char heap[HEAP_SIZE];
  assert(a >= 1 && !(a & (a - 1)));
  h = (uintptr_t)heap;
  p = h + used;
  if ((resizable = (ssize_t)n < 0)) {
    n = ~n;
    p += sizeof(c);
  }
  p += a - 1;
  p &= -a;
  if (n <= a) {
    c = a;
  } else if (!resizable) {
    c = n;
  } else {
    c = 2ull << (__builtin_clzll(n - 1) ^ (sizeof(long long) * CHAR_BIT - 1));
  }
  if (c < a || c > HEAP_SIZE || p + c > h + HEAP_SIZE) {
    Die(program_invocation_name, "out of memory");
  }
  used = p - h + c;
  if (resizable) {
    memcpy((char *)p - sizeof(c), &c, sizeof(c));
  }
  return (void *)p;
}

// reallocates 𝑛 𝑧-sized elements aligned on 𝑧 from .bss
// - avoids binary bloat of mmap() and realloc()
// - dies if out of memory or overflow occurs
// - new memory is always zero-initialized
// - abstracts multiply overflow check
// - shrinking always happens in-place
// - growing cost is always amortized
// - can't be freed or reclaimed
static void *reballoc(void *p, size_t n, size_t z) {
  size_t c;
  assert(n >= 0);
  assert(z >= 1 && !(z & (z - 1)));
  if (ckd_mul(&n, n, z)) n = HEAP_SIZE;
  if (!p) return balloc(~n, z);
  memcpy(&c, (char *)p - sizeof(c), sizeof(c));
  assert(c >= z && c < HEAP_SIZE && !(c & (c - 1)));
  if (n <= c) return p;
  return memcpy(balloc(~n, z), p, c);
}

static char *StrDup(const char *s) {
  size_t n = strlen(s) + 1;
  return memcpy(balloc(n, 1), s, n);
}

static char *StrCat(const char *a, const char *b) {
  char *p;
  size_t n, m;
  n = strlen(a);
  m = strlen(b);
  p = balloc(n + m + 1, 1);
  memcpy(p, a, n);
  memcpy(p + n, b, m + 1);
  return p;
}

static void AppendInt(struct Ints *l, int i) {
  l->p = reballoc(l->p, l->i + 2, sizeof(*l->p));
  l->p[l->i++] = i;
}

static void AppendArg(struct Args *l, char *s) {
  l->p = reballoc(l->p, l->i + 2, sizeof(*l->p));
  l->p[l->i++] = s;
}

static void AppendBytes(struct Bytes *l, const char *s, size_t n) {
  l->p = reballoc(l->p, l->i + n + 1, sizeof(*l->p));
  memcpy(l->p + l->i, s, n);
  l->i += n;
}

static int IsEqual(const char *a, const char *b) {
  return !strcmp(a, b);
}

static void MakeArHeader(struct ar_hdr *h,  //
                         const char *name,  //
                         int mode,          //
                         size_t size) {     //
  size_t n;
  char b[21];
  memset(h, ' ', sizeof(*h));
  n = strlen(name);
  if (n > ARRAYLEN(h->ar_name)) {
    Die(program_invocation_name, "ar_name overflow");
  }
  memcpy(h->ar_name, name, n);
  if (!IsEqual(name, "//")) {
    h->ar_date[0] = '0';
    h->ar_uid[0] = '0';
    h->ar_gid[0] = '0';
    memcpy(h->ar_mode, b, FormatOctal32(b, mode & 0777, false) - b);
  }
  if (size > 9999999999) {
    Die(program_invocation_name, "ar_size overflow");
  }
  memcpy(h->ar_size, b, FormatUint64(b, size) - b);
  memcpy(h->ar_fmag, ARFMAG, sizeof(h->ar_fmag));
}

// copies data between file descriptors until end of file
// - assumes signal handlers aren't in play
// - uses copy_file_range() if possible
// - returns number of bytes exchanged
// - dies if operation fails
static int64_t CopyFileOrDie(const char *inpath, int infd,  //
                             const char *outpath, int outfd) {
  int64_t toto;
  char buf[512];
  size_t exchanged;
  ssize_t got, wrote;
  enum { CFR, RW } mode;
  for (mode = CFR, toto = 0;; toto += exchanged) {
    if (mode == CFR) {
      got = copy_file_range(infd, 0, outfd, 0, 4194304, 0);
      if (!got) break;
      if (got != -1) {
        exchanged = got;
      } else if (errno == EXDEV ||       // different partitions
                 errno == ENOSYS ||      // not linux or freebsd
                 errno == ENOTSUP ||     // probably a /zip file
                 errno == EOPNOTSUPP) {  // technically the same
        exchanged = 0;
        mode = RW;
      } else {
        SysDie(inpath, "copy_file_range");
      }
    } else {
      got = read(infd, buf, sizeof(buf));
      if (!got) break;
      if (got == -1) SysDie(inpath, "read");
      wrote = write(outfd, buf, got);
      if (wrote == -1) SysDie(outpath, "write");
      if (wrote != got) Die(outpath, "posix violated");
      exchanged = wrote;
    }
  }
  return toto;
}

int main(int argc, char *argv[]) {
  int fd, objectid;
  struct ar_hdr header1;
  struct ar_hdr header2;

#ifdef MODE_DBG
  ShowCrashReports();
#endif

  // handle hardcoded flags
  if (argc == 2) {
    if (IsEqual(argv[1], "-n")) {
      exit(0);
    }
    if (IsEqual(argv[1], "-h") ||  //
        IsEqual(argv[1], "-?") ||  //
        IsEqual(argv[1], "--help")) {
      ShowUsage(0, 1);
    }
    if (IsEqual(argv[1], "--version")) {
      tinyprint(1, VERSION, NULL);
      exit(0);
    }
  }

  // get flags and output path
  if (argc < 3) {
    ShowUsage(1, 2);
  }
  char *flags = argv[1];
  const char *outpath = argv[2];

  // we only support one mode of operation, which is creating a new
  // deterministic archive. computing the full archive goes so fast
  // on modern systems that it isn't worth supporting the byzantine
  // standard posix ar flags intended to improve cassette tape perf
  SortChars(flags, strlen(flags));
  if (*flags == 'D') ++flags;
  if (!IsEqual(flags, "cr") &&    //
      !IsEqual(flags, "cru") &&   //
      !IsEqual(flags, "crsu") &&  //
      !IsEqual(flags, "crs")) {
    tinyprint(2, program_invocation_name, ": flags should be rcsD\n", NULL);
    ShowUsage(1, 2);
  }

  struct Args args = {reballoc(0, 4096, sizeof(char *))};
  struct Args names = {reballoc(0, 4096, sizeof(char *))};
  struct Ints modes = {reballoc(0, 4096, sizeof(int))};
  struct Ints sizes = {reballoc(0, 4096, sizeof(int))};
  struct Ints symnames = {reballoc(0, 16384, sizeof(int))};
  struct Bytes symbols = {reballoc(0, 131072, sizeof(char))};
  struct Bytes filenames = {reballoc(0, 16384, sizeof(char))};

  // perform analysis pass on input files
  struct GetArgs ga;
  getargs_init(&ga, argv + 3);
  for (objectid = 0;;) {
    struct stat st;
    const char *arg;
    if (!(arg = getargs_next(&ga))) break;
    if (endswith(arg, "/")) continue;
    if (endswith(arg, ".pkg")) continue;
    if (stat(arg, &st)) SysDie(arg, "stat");
    if (S_ISDIR(st.st_mode)) continue;
    if (!st.st_size) Die(arg, "file is empty");
    if (st.st_size > 0x7ffff000) Die(arg, "file too large");
    if ((fd = open(arg, O_RDONLY)) == -1) SysDie(arg, "open");
    AppendArg(&args, StrDup(arg));
    AppendInt(&sizes, st.st_size);
    AppendInt(&modes, st.st_mode);
    char bnbuf[PATH_MAX + 1];
    strlcpy(bnbuf, arg, sizeof(bnbuf));
    char *aname = StrCat(basename(bnbuf), "/");
    if (strlen(aname) <= sizeof(header1.ar_name)) {
      AppendArg(&names, aname);
    } else {
      char ibuf[21];
      FormatUint64(ibuf, filenames.i);
      AppendArg(&names, StrCat("/", ibuf));
      AppendBytes(&filenames, aname, strlen(aname));
      AppendBytes(&filenames, "\n", 1);
    }
    size_t mapsize = st.st_size;
    void *elf = mmap(0, mapsize, PROT_READ, MAP_PRIVATE, fd, 0);
    if (elf == MAP_FAILED) SysDie(arg, "mmap");
    if (!IsElf64Binary(elf, mapsize)) Die(arg, "not an elf64 binary");
    char *strs = GetElfStringTable(elf, mapsize, ".strtab");
    if (!strs) Die(arg, "elf .strtab not found");
    Elf64_Xword symcount;
    Elf64_Shdr *symsec = GetElfSymbolTable(elf, mapsize, SHT_SYMTAB, &symcount);
    Elf64_Sym *syms = GetElfSectionAddress(elf, mapsize, symsec);
    if (!syms) Die(arg, "elf symbol table not found");
    for (Elf64_Xword j = symsec->sh_info; j < symcount; ++j) {
      if (!syms[j].st_name) continue;
      if (syms[j].st_shndx == SHN_UNDEF) continue;
      if (syms[j].st_shndx == SHN_COMMON) continue;
      const char *symname = GetElfString(elf, mapsize, strs, syms[j].st_name);
      if (!symname) Die(arg, "elf symbol name corrupted");
      AppendBytes(&symbols, symname, strlen(symname) + 1);
      AppendInt(&symnames, objectid);
    }
    if (munmap(elf, mapsize)) SysDie(arg, "munmap");
    if (close(fd)) SysDie(arg, "close");
    ++objectid;
  }
  getargs_destroy(&ga);

  // compute length of output archive
  size_t outsize = 0;
  struct iovec iov[8];
  int tablebufsize = 4 + symnames.i * 4;
  char *tablebuf = balloc(tablebufsize, 1);
  int *offsets = balloc(args.i * sizeof(int), sizeof(int));
  iov[0].iov_base = ARMAG;
  outsize += (iov[0].iov_len = SARMAG);
  iov[1].iov_base = &header1;
  outsize += (iov[1].iov_len = sizeof(struct ar_hdr));
  iov[2].iov_base = tablebuf;
  outsize += (iov[2].iov_len = tablebufsize);
  iov[3].iov_base = symbols.p;
  outsize += (iov[3].iov_len = symbols.i);
  iov[4].iov_base = "";
  outsize += (iov[4].iov_len = outsize & 1);
  iov[5].iov_base = &header2;
  outsize += (iov[5].iov_len = filenames.i ? sizeof(struct ar_hdr) : 0);
  iov[6].iov_base = filenames.p;
  outsize += (iov[6].iov_len = filenames.i);
  iov[7].iov_base = "\n";
  outsize += (iov[7].iov_len = filenames.i & 1);
  for (size_t i = 0; i < args.i; ++i) {
    outsize += outsize & 1;
    if (outsize > INT_MAX) {
      Die(outpath, "archive too large");
    }
    offsets[i] = outsize;
    outsize += sizeof(struct ar_hdr);
    outsize += sizes.p[i];
  }

  // serialize metadata
  MakeArHeader(&header1, "/", 0, tablebufsize + ROUNDUP(symbols.i, 2));
  MakeArHeader(&header2, "//", 0, ROUNDUP(filenames.i, 2));
  WRITE32BE(tablebuf, symnames.i);
  for (size_t i = 0; i < symnames.i; ++i) {
    WRITE32BE(tablebuf + 4 + i * 4, offsets[symnames.p[i]]);
  }

  // write output archive
  int outfd;
  if ((outfd = creat(outpath, 0644)) == -1) {
    SysDie(outpath, "creat");
  }
  if (ftruncate(outfd, outsize)) {
    SysDie(outpath, "ftruncate");
  }
  if ((outsize = writev(outfd, iov, ARRAYLEN(iov))) == -1) {
    SysDie(outpath, "writev[1]");
  }
  for (size_t i = 0; i < args.i; ++i) {
    const char *inpath = args.p[i];
    if ((fd = open(inpath, O_RDONLY)) == -1) {
      SysDie(inpath, "open");
    }
    iov[0].iov_base = "\n";
    outsize += (iov[0].iov_len = outsize & 1);
    iov[1].iov_base = &header1;
    outsize += (iov[1].iov_len = sizeof(struct ar_hdr));
    MakeArHeader(&header1, names.p[i], modes.p[i], sizes.p[i]);
    if (writev(outfd, iov, 2) == -1) {
      SysDie(outpath, "writev[2]");
    }
    outsize += sizes.p[i];
    if (CopyFileOrDie(inpath, fd, outpath, outfd) != sizes.p[i]) {
      Die(inpath, "file size changed");
    }
    if (close(fd)) {
      SysDie(inpath, "close");
    }
  }
  if (close(outfd)) {
    SysDie(outpath, "close");
  }

  return 0;
}
