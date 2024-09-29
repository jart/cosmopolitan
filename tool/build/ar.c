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
#include "libc/intrin/bsr.h"
#include "libc/limits.h"
#include "libc/macros.h"
#include "libc/runtime/runtime.h"
#include "libc/serialize.h"
#include "libc/stdckdint.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/s.h"
#include "tool/build/lib/ar.h"
#include "tool/build/lib/getargs.h"

/**
 * @fileoverview cosmopolitan ar
 */

static wontreturn void ShowUsage(int rc, int fd) {
  tinyprint(  //
      fd,
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
      "  FILE       should be foo.o, lib.a, or @args.txt\n"
      "\n"
      "DOCUMENTATION\n"
      "\n"
      "  Your Cosmopolitan Archiver is superior:\n"
      "\n"
      "  - Isn't accidentally quadratic like GNU ar. Cosmopolitan Libc is\n"
      "    distributed as libcosmo.a which contains 5000+ object files and\n"
      "    is tens of megabytes in size. GNU ar isn't capable of making an\n"
      "    archive that large. So we invented this ar as a replacement.\n"
      "\n"
      "  - Goes 2x faster than LLVM ar thanks to modern system calls like\n"
      "    copy_file_range(). This ar should also use 100x less memory.\n"
      "\n"
      "  - Can be built as a 96kb APE binary that works well on six OSes.\n"
      "    Cosmopolitan uses the same dev tools on all OSes and archsr to\n"
      "    ensure compilations are simple and deterministic for everyone.\n"
      "\n"
      "  This static archiver introduces handy features:\n"
      "\n"
      "  - Arguments may be supplied in an `@args.txt` file. This is useful\n"
      "    for overcoming the `ARG_MAX` limit, which is especially important\n"
      "    on Windows, where only very few command arguments can be passed.\n"
      "    GNU Make can be easily configured to generate args files.\n"
      "\n"
      "  - You can merge many .a files into one big .a file. Args that end\n"
      "    with .a will be opened as static archives. The .o files inside it\n"
      "    will then be added to your new archive. It would be the same as if\n"
      "    you passed all the .o files as args. This is fast. For example, to\n"
      "    merge 37 .a files containing 5000 .o files takes ~38 milliseconds.\n"
      "\n"
      "  - Directory arguments are ignored. The biggest gotcha with makefiles\n"
      "    that use wildcard globbing is that it can't detect when files are\n"
      "    deleted, which means it can't invalidate the artifacts which had\n"
      "    depended on that file, leading to nondeterminism and surprising\n"
      "    build failures. The simplest way to solve that is to add the\n"
      "    directory to the prerequisites list, since the directory modified\n"
      "    time will be updated by the OS when files inside it are deleted.\n"
      "    When doing this, it's simple and elegant to not need to filter\n"
      "    the directory prerequisites before passing `$^` to `ar`.\n"
      "\n",
      NULL);
  exit(rc);
}

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
  if (!(errstr = _strerdoc(errno)))
    errstr = "Unknown error";
  tinyprint(2, path, ": ", func, ": ", errstr, "\n", NULL);
  exit(1);
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
  if (c < a || c > HEAP_SIZE || p + c > h + HEAP_SIZE)
    Die(program_invocation_name, "out of memory");
  used = p - h + c;
  if (resizable)
    memcpy((char *)p - sizeof(c), &c, sizeof(c));
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
  if (ckd_mul(&n, n, z))
    n = HEAP_SIZE;
  if (!p)
    return balloc(~n, z);
  memcpy(&c, (char *)p - sizeof(c), sizeof(c));
  assert(c >= z && c < HEAP_SIZE && !(c & (c - 1)));
  if (n <= c)
    return p;
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
static void CopyFileOrDie(const char *inpath, int infd,    //
                          const char *outpath, int outfd,  //
                          size_t offset, size_t size) {
  char buf[512];
  size_t exchanged;
  enum { CFR, RW } mode;
  ssize_t want, got, wrote;
  if (offset)
    if (lseek(infd, offset, SEEK_SET) == -1)
      SysDie(inpath, "lseek");
  for (mode = CFR; size; size -= exchanged) {
    if (mode == CFR) {
      want = 4194304;
      if (want > size)
        want = size;
      got = copy_file_range(infd, 0, outfd, 0, want, 0);
      if (!got)
        Die(inpath, "unexpected eof");
      if (got != -1) {
        exchanged = got;
      } else if (errno == EXDEV ||       // different partitions
                 errno == EINVAL ||      // possible w/ ecryptfs
                 errno == ENOSYS ||      // not linux or freebsd
                 errno == ENOTSUP ||     // probably a /zip file
                 errno == EOPNOTSUPP) {  // technically the same
        exchanged = 0;
        mode = RW;
      } else {
        SysDie(inpath, "copy_file_range");
      }
    } else {
      want = sizeof(buf);
      if (want > size)
        want = size;
      got = read(infd, buf, want);
      if (!got)
        Die(inpath, "unexpected eof");
      if (got == -1)
        SysDie(inpath, "read");
      wrote = write(outfd, buf, got);
      if (wrote == -1)
        SysDie(outpath, "write");
      if (wrote != got)
        Die(outpath, "posix violated");
      exchanged = wrote;
    }
  }
}

static void AppendName(const char *name, struct Args *names,
                       struct Bytes *filenames) {
  struct ar_hdr header1;
  char bnbuf[PATH_MAX + 1];
  strlcpy(bnbuf, name, sizeof(bnbuf));
  char *aname = StrCat(basename(bnbuf), "/");
  if (strlen(aname) <= sizeof(header1.ar_name)) {
    AppendArg(names, aname);
  } else {
    char ibuf[21];
    FormatUint64(ibuf, filenames->i);
    AppendArg(names, StrCat("/", ibuf));
    AppendBytes(filenames, aname, strlen(aname));
    AppendBytes(filenames, "\n", 1);
  }
}

static void AppendSymbols(const char *path, const Elf64_Ehdr *elf,
                          size_t elfsize, struct Bytes *symbols,
                          struct Ints *symnames, int objid) {
  if (!IsElf64Binary(elf, elfsize))
    Die(path, "not an elf64 binary");
  char *strs = GetElfStringTable(elf, elfsize, ".strtab");
  if (!strs)
    Die(path, "elf .strtab not found");
  Elf64_Xword symcount;
  Elf64_Shdr *symsec = GetElfSymbolTable(elf, elfsize, SHT_SYMTAB, &symcount);
  Elf64_Sym *syms = GetElfSectionAddress(elf, elfsize, symsec);
  if (!syms)
    Die(path, "elf symbol table not found");
  for (Elf64_Xword j = symsec->sh_info; j < symcount; ++j) {
    if (!syms[j].st_name)
      continue;
    if (syms[j].st_shndx == SHN_UNDEF)
      continue;
    if (syms[j].st_shndx == SHN_COMMON)
      continue;
    const char *symname = GetElfString(elf, elfsize, strs, syms[j].st_name);
    if (!symname)
      Die(path, "elf symbol name corrupted");
    AppendBytes(symbols, symname, strlen(symname) + 1);
    AppendInt(symnames, objid);
  }
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
    if (IsEqual(argv[1], "-n"))
      exit(0);
    if (IsEqual(argv[1], "-h") ||  //
        IsEqual(argv[1], "-?") ||  //
        IsEqual(argv[1], "--help")) {
      ShowUsage(0, 1);
    }
    if (IsEqual(argv[1], "--version")) {
      tinyprint(1,
                "cosmopolitan ar v3.0\n"
                "copyright 2024 justine tunney\n"
                "https://github.com/jart/cosmopolitan\n",
                NULL);
      exit(0);
    }
  }

  // get flags and output path
  if (argc < 3)
    Die(argv[0], "missing argument");
  char *flags = argv[1];
  const char *outpath = argv[2];

  // we only support one mode of operation, which is creating a new
  // deterministic archive. computing the full archive goes so fast
  // on modern systems that it isn't worth supporting the byzantine
  // standard posix ar flags intended to improve cassette tape perf
  SortChars(flags, strlen(flags));
  if (*flags == 'D')
    ++flags;
  if (!IsEqual(flags, "cr") &&    //
      !IsEqual(flags, "cru") &&   //
      !IsEqual(flags, "crsu") &&  //
      !IsEqual(flags, "crs")) {
    tinyprint(2, program_invocation_name, ": flags should be rcsD\n", NULL);
    ShowUsage(1, 2);
  }

  struct Args args = {reballoc(0, 4096, sizeof(char *))};
  struct Args names = {reballoc(0, 4096, sizeof(char *))};
  struct Ints sizes = {reballoc(0, 4096, sizeof(int))};
  struct Ints foffsets = {reballoc(0, 4096, sizeof(int))};
  struct Ints symnames = {reballoc(0, 16384, sizeof(int))};
  struct Bytes symbols = {reballoc(0, 131072, sizeof(char))};
  struct Bytes filenames = {reballoc(0, 16384, sizeof(char))};

  // perform analysis pass on input files
  struct GetArgs ga;
  getargs_init(&ga, argv + 3);
  for (objectid = 0;;) {
    struct stat st;
    const char *arg;
    if (!(arg = getargs_next(&ga)))
      break;
    if (endswith(arg, "/"))
      continue;
    if (endswith(arg, ".pkg"))
      continue;
    if (endswith(arg, ".a")) {
      struct Ar ar;
      struct ArFile arf;
      openar(&ar, arg);
      while (readar(&ar, &arf)) {
        AppendArg(&args, StrDup(arg));
        AppendInt(&sizes, arf.size);
        AppendInt(&foffsets, arf.offset);
        AppendName(arf.name, &names, &filenames);
        AppendSymbols(arg, arf.data, arf.size, &symbols, &symnames, objectid++);
      }
      closear(&ar);
    } else {
      if (stat(arg, &st))
        SysDie(arg, "stat");
      if (S_ISDIR(st.st_mode))
        continue;
      if (!st.st_size)
        Die(arg, "file is empty");
      if (st.st_size > 0x7ffff000)
        Die(arg, "file too large");
      if ((fd = open(arg, O_RDONLY)) == -1)
        SysDie(arg, "open");
      AppendArg(&args, StrDup(arg));
      AppendInt(&sizes, st.st_size);
      AppendInt(&foffsets, 0);
      AppendName(arg, &names, &filenames);
      void *elf = mmap(0, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
      if (elf == MAP_FAILED)
        SysDie(arg, "mmap");
      AppendSymbols(arg, elf, st.st_size, &symbols, &symnames, objectid++);
      if (munmap(elf, st.st_size))
        SysDie(arg, "munmap");
      if (close(fd))
        SysDie(arg, "close");
    }
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
  for (size_t i = 0; i < symnames.i; ++i)
    WRITE32BE(tablebuf + 4 + i * 4, offsets[symnames.p[i]]);

  // write output archive
  int outfd;
  if ((outfd = creat(outpath, 0644)) == -1)
    SysDie(outpath, "creat");
  if (ftruncate(outfd, outsize))
    SysDie(outpath, "ftruncate");
  if ((outsize = writev(outfd, iov, ARRAYLEN(iov))) == -1)
    SysDie(outpath, "writev[1]");
  for (size_t i = 0; i < args.i; ++i) {
    const char *inpath = args.p[i];
    if (!(i && IsEqual(inpath, args.p[i - 1])))
      if ((fd = open(inpath, O_RDONLY)) == -1)
        SysDie(inpath, "open");
    iov[0].iov_base = "\n";
    outsize += (iov[0].iov_len = outsize & 1);
    iov[1].iov_base = &header1;
    outsize += (iov[1].iov_len = sizeof(struct ar_hdr));
    MakeArHeader(&header1, names.p[i], 0100644, sizes.p[i]);
    if (writev(outfd, iov, 2) == -1)
      SysDie(outpath, "writev[2]");
    outsize += sizes.p[i];
    CopyFileOrDie(inpath, fd, outpath, outfd, foffsets.p[i], sizes.p[i]);
    if (!(i + 1 < args.i && IsEqual(inpath, args.p[i + 1])))
      if (close(fd))
        SysDie(inpath, "close");
  }
  if (close(outfd))
    SysDie(outpath, "close");

  return 0;
}
