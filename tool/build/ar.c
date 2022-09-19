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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/mem/copyfd.internal.h"
#include "libc/calls/struct/iovec.h"
#include "libc/calls/struct/stat.h"
#include "libc/elf/elf.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/bits.h"
#include "libc/intrin/safemacros.internal.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/mem/arraylist2.internal.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/sock.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/ex.h"
#include "libc/sysv/consts/madv.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/s.h"
#include "libc/x/x.h"
#include "tool/build/lib/getargs.h"

/**
 * @fileoverview System Five Static Archive Builder.
 *
 * GNU ar has a bug which causes it to take hundreds of milliseconds to
 * build archives like ntdll.a and several minutes for cosmopolitan.a.
 * This goes quadratically faster taking 1ms to do ntdll w/ hot cache.
 *
 * Compared to LLVM ar this tool goes 10x faster because it uses madvise
 * and copy_file_range which give us the optimal page cached file system
 * beahvior that a build environment needs.
 *
 * This tool also adds a feature: it ignores directory parameters. This
 * is important because good Makefiles on Linux will generally have the
 * directory be a .a prerequisite so archives rebuild on file deletion.
 *
 * @see https://www.unix.com/man-page/opensolaris/3head/ar.h/
 */

struct Args {
  size_t i, n;
  char **p;
};

struct String {
  size_t i, n;
  char *p;
};

struct Ints {
  size_t i, n;
  int *p;
};

struct Header {
  char name[16];
  char date[12];
  char uid[6];
  char gid[6];
  char mode[8];
  char size[10];
  char fmag[2];
};

static void *Realloc(void *p, size_t n) {
  void *q;
  if (!(q = realloc(p, n))) {
    fputs("error: ar: out of memory\n", stderr);
    exit(1);
  }
  return q;
}

static void *Malloc(size_t n) {
  return Realloc(0, n);
}

static void NewArgs(struct Args *l, size_t n) {
  l->i = 0;
  l->n = MAX(2, n);
  l->p = Malloc(l->n * sizeof(*l->p));
  l->p[0] = 0;
}

static void NewInts(struct Ints *l, size_t n) {
  l->i = 0;
  l->n = MAX(2, n);
  l->p = Malloc(l->n * sizeof(*l->p));
  l->p[0] = 0;
}

static void NewString(struct String *s, size_t n) {
  s->i = 0;
  s->n = MAX(2, n);
  s->p = Malloc(s->n * sizeof(*s->p));
  s->p[0] = 0;
}

static void AppendInt(struct Ints *l, int i) {
  assert(l->n > 1);
  if (l->i + 1 >= l->n) {
    do {
      l->n += l->n >> 1;
    } while (l->i + 1 >= l->n);
    l->p = Realloc(l->p, l->n * sizeof(*l->p));
  }
  l->p[l->i++] = i;
  l->p[l->i] = 0;
}

static void AppendArg(struct Args *l, char *s) {
  assert(l->n > 1);
  if (l->i + 1 >= l->n) {
    do {
      l->n += l->n >> 1;
    } while (l->i + 1 >= l->n);
    l->p = Realloc(l->p, l->n * sizeof(*l->p));
  }
  l->p[l->i++] = s;
  l->p[l->i] = 0;
}

static void MakeHeader(struct Header *h, const char *name, int ref, int mode,
                       int size) {
  size_t n;
  char ibuf[13], *p;
  memset(h, ' ', sizeof(*h));
  n = strlen(name);
  memcpy(h->name, name, n);
  if (ref != -1) {
    FormatUint32(h->name + n, ref);
  }
  if (strcmp(name, "//") != 0) {
    h->date[0] = '0';
    h->uid[0] = '0';
    h->gid[0] = '0';
    p = FormatOctal32(ibuf, mode & 0777, false);
    CHECK_LE(p - ibuf, sizeof(h->mode));
    memcpy(h->mode, ibuf, p - ibuf);
  }
  h->fmag[0] = '`';
  h->fmag[1] = '\n';
  p = FormatUint32(ibuf, size);
  CHECK_LE(p - ibuf, sizeof(h->size));
  memcpy(h->size, ibuf, p - ibuf);
}

int main(int argc, char *argv[]) {
  FILE *f;
  void *elf;
  char *line;
  char *strs;
  ssize_t rc;
  int *offsets;
  size_t wrote;
  size_t remain;
  struct stat *st;
  uint32_t outpos;
  Elf64_Sym *syms;
  const char *arg;
  struct Args args;
  uint64_t outsize;
  uint8_t *tablebuf;
  struct GetArgs ga;
  struct Ints modes;
  struct Ints names;
  struct Ints sizes;
  const char *reason;
  struct iovec iov[7];
  const char *symname;
  const char *outpath;
  Elf64_Xword symcount;
  struct Ints symnames;
  struct String symbols;
  struct String filenames;
  struct Header *header1, *header2;
  int i, j, fd, err, name, outfd, tablebufsize;

  // TODO(jart): Delete this.
  if (argc == 2 && !strcmp(argv[1], "-n")) {
    exit(0);
  }

  // we only support one mode of operation, which is creating a new
  // deterministic archive. this tool is so fast that we don't need
  // database-like tools when editing static archives
  if (!(argc > 2 && strcmp(argv[1], "rcsD") == 0)) {
    fputs("usage: ", stderr);
    if (argc > 0) fputs(argv[0], stderr);
    fputs(" rcsD ARCHIVE FILE...", stderr);
    exit(EX_USAGE);
  }

  outpath = argv[2];
  NewArgs(&args, 4);
  st = Malloc(sizeof(struct stat));
  NewInts(&modes, 128);
  NewInts(&names, 128);
  NewInts(&sizes, 128);
  NewInts(&symnames, 1024);
  NewString(&symbols, 4096);
  NewString(&filenames, 1024);
  getargs_init(&ga, argv + 3);

  // load global symbols and populate page cache
  for (i = 0;; ++i) {
  TryAgain:
    if (!(arg = getargs_next(&ga))) break;
    if (_endswith(arg, "/")) goto TryAgain;
    if (_endswith(arg, ".pkg")) goto TryAgain;
    CHECK_NE(-1, stat(arg, st), "%s", arg);
    if (!st->st_size || S_ISDIR(st->st_mode)) goto TryAgain;
    CHECK_NE(-1, (fd = open(arg, O_RDONLY)), "%s", arg);
    CHECK_LT(st->st_size, 0x7ffff000);
    AppendArg(&args, xstrdup(arg));
    AppendInt(&names, filenames.i);
    AppendInt(&sizes, st->st_size);
    AppendInt(&modes, st->st_mode);
    CONCAT(&filenames.p, &filenames.i, &filenames.n, basename(arg),
           strlen(basename(arg)));
    CONCAT(&filenames.p, &filenames.i, &filenames.n, "/\n", 2);
    CHECK_NE(MAP_FAILED,
             (elf = mmap(0, st->st_size, PROT_READ, MAP_PRIVATE, fd, 0)));
    CHECK(IsElf64Binary(elf, st->st_size), "%s", arg);
    CHECK_NOTNULL((strs = GetElfStringTable(elf, st->st_size)));
    CHECK_NOTNULL((syms = GetElfSymbolTable(elf, st->st_size, &symcount)));
    for (j = 0; j < symcount; ++j) {
      if (syms[j].st_shndx == SHN_UNDEF) continue;
      if (syms[j].st_other == STV_INTERNAL) continue;
      if (ELF64_ST_BIND(syms[j].st_info) == STB_LOCAL) continue;
      symname = GetElfString(elf, st->st_size, strs, syms[j].st_name);
      CONCAT(&symbols.p, &symbols.i, &symbols.n, symname, strlen(symname) + 1);
      AppendInt(&symnames, i);
    }
    CHECK_NE(-1, munmap(elf, st->st_size));
    close(fd);
  }
  APPEND(&filenames.p, &filenames.i, &filenames.n, "\n");

  // compute length of output archive
  outsize = 0;
  tablebufsize = 4 + symnames.i * 4;
  tablebuf = Malloc(tablebufsize);
  offsets = Malloc(args.i * 4);
  header1 = Malloc(sizeof(struct Header));
  header2 = Malloc(sizeof(struct Header));
  iov[0].iov_base = "!<arch>\n";
  outsize += (iov[0].iov_len = 8);
  iov[1].iov_base = header1;
  outsize += (iov[1].iov_len = 60);
  iov[2].iov_base = tablebuf;
  outsize += (iov[2].iov_len = tablebufsize);
  iov[3].iov_base = symbols.p;
  outsize += (iov[3].iov_len = symbols.i);
  iov[4].iov_base = "\n";
  outsize += (iov[4].iov_len = outsize & 1);
  iov[5].iov_base = header2;
  outsize += (iov[5].iov_len = 60);
  iov[6].iov_base = filenames.p;
  outsize += (iov[6].iov_len = filenames.i);
  for (i = 0; i < args.i; ++i) {
    outsize += outsize & 1;
    offsets[i] = outsize;
    outsize += 60;
    outsize += sizes.p[i];
  }
  CHECK_LE(outsize, 0x7ffff000);

  // serialize metadata
  MakeHeader(header1, "/", -1, 0, tablebufsize + symbols.i);
  MakeHeader(header2, "//", -1, 0, filenames.i);
  WRITE32BE(tablebuf, symnames.i);
  for (i = 0; i < symnames.i; ++i) {
    WRITE32BE(tablebuf + 4 + i * 4, offsets[symnames.p[i]]);
  }

  // write output archive
  CHECK_NE(-1, (outfd = creat(outpath, 0644)));
  ftruncate(outfd, outsize);
  if ((outsize = writev(outfd, iov, ARRAYLEN(iov))) == -1) {
    reason = "writev1 failed";
    goto fail;
  }
  for (i = 0; i < args.i; ++i) {
    if ((fd = open(args.p[i], O_RDONLY)) == -1) {
      reason = "open failed";
      goto fail;
    }
    iov[0].iov_base = "\n";
    outsize += (iov[0].iov_len = outsize & 1);
    iov[1].iov_base = header1;
    outsize += (iov[1].iov_len = 60);
    MakeHeader(header1, "/", names.p[i], modes.p[i], sizes.p[i]);
    if (writev(outfd, iov, 2) == -1) {
      reason = "writev2 failed";
      goto fail;
    }
    outsize += (remain = sizes.p[i]);
    if (_copyfd(fd, outfd, remain) == -1) {
      reason = "copy_file_range failed";
      goto fail;
    }
    close(fd);
  }
  close(outfd);

  for (i = 0; i < args.i; ++i) free(args.p[i]);
  getargs_destroy(&ga);
  free(filenames.p);
  free(symnames.p);
  free(symbols.p);
  free(tablebuf);
  free(modes.p);
  free(names.p);
  free(sizes.p);
  free(offsets);
  free(header1);
  free(header2);
  free(args.p);
  free(st);
  return 0;

fail:
  err = errno;
  unlink(outpath);
  fputs("error: ar failed: ", stderr);
  fputs(reason, stderr);
  fputs(": ", stderr);
  fputs(strerror(err), stderr);
  fputs("\n", stderr);
  return 1;
}
