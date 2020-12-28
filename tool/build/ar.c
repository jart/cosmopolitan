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
#include "libc/alg/arraylist2.internal.h"
#include "libc/bits/bits.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/iovec.h"
#include "libc/calls/struct/stat.h"
#include "libc/elf/elf.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/fmt/itoa.h"
#include "libc/log/check.h"
#include "libc/macros.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/sock.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/madv.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/x/x.h"

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
 */

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

static void MakeHeader(struct Header *h, const char *name, int ref, int mode,
                       int size) {
  size_t n;
  char buf[21];
  memset(h, ' ', sizeof(*h));
  n = strlen(name);
  memcpy(h->name, name, n);
  if (ref != -1) {
    memcpy(h->name + n, buf, uint64toarray_radix10(ref, buf));
  }
  if (strcmp(name, "//") != 0) {
    h->date[0] = '0';
    h->uid[0] = '0';
    h->gid[0] = '0';
    memcpy(h->mode, buf, uint64toarray_radix8(mode & 0777, buf));
  }
  h->fmag[0] = '`';
  h->fmag[1] = '\n';
  memcpy(h->size, buf, uint64toarray_radix10(size, buf));
}

int main(int argc, char *argv[]) {
  void *elf;
  char *strs;
  ssize_t rc;
  struct stat *st;
  uint32_t outpos;
  Elf64_Sym *syms;
  uint64_t outsize;
  char **objectargs;
  uint8_t *tablebuf;
  struct iovec iov[7];
  const char *symname;
  const char *outpath;
  Elf64_Xword symcount;
  struct Ints symnames;
  struct String symbols;
  struct String filenames;
  struct Header *header1, *header2;
  size_t wrote, remain, objectargcount;
  int *offsets, *modes, *sizes, *names;
  int i, j, fd, err, name, outfd, tablebufsize;

  if (!(argc > 2 && strcmp(argv[1], "rcsD") == 0)) {
    fprintf(stderr, "%s%s%s\n", "Usage: ", argv[0], " rcsD ARCHIVE FILE...");
    return 1;
  }

  st = xmalloc(sizeof(struct stat));
  symbols.i = 0;
  symbols.n = 4096;
  symbols.p = xmalloc(symbols.n);
  filenames.i = 0;
  filenames.n = 1024;
  filenames.p = xmalloc(filenames.n);
  symnames.i = 0;
  symnames.n = 1024;
  symnames.p = xmalloc(symnames.n * sizeof(int));

  outpath = argv[2];
  objectargs = argv + 3;
  objectargcount = argc - 3;
  modes = xmalloc(sizeof(int) * objectargcount);
  names = xmalloc(sizeof(int) * objectargcount);
  sizes = xmalloc(sizeof(int) * objectargcount);

  // load global symbols and populate page cache
  for (i = 0; i < objectargcount; ++i) {
  TryAgain:
    CHECK_NE(-1, (fd = open(objectargs[i], O_RDONLY)));
    CHECK_NE(-1, fstat(fd, st));
    CHECK_LT(st->st_size, 0x7ffff000);
    if (!st->st_size || S_ISDIR(st->st_mode) ||
        endswith(objectargs[i], ".pkg")) {
      close(fd);
      for (j = i; j + 1 < objectargcount; ++j) {
        objectargs[j] = objectargs[j + 1];
      }
      --objectargcount;
      goto TryAgain;
    }
    names[i] = filenames.i;
    sizes[i] = st->st_size;
    modes[i] = st->st_mode;
    CONCAT(&filenames.p, &filenames.i, &filenames.n, basename(objectargs[i]),
           strlen(basename(objectargs[i])));
    CONCAT(&filenames.p, &filenames.i, &filenames.n, "/\n", 2);
    CHECK_NE(MAP_FAILED,
             (elf = mmap(NULL, st->st_size, PROT_READ, MAP_PRIVATE, fd, 0)));
    CHECK(IsElf64Binary(elf, st->st_size));
    CHECK_NOTNULL((strs = GetElfStringTable(elf, st->st_size)));
    CHECK_NOTNULL((syms = GetElfSymbolTable(elf, st->st_size, &symcount)));
    for (j = 0; j < symcount; ++j) {
      if (syms[j].st_shndx == SHN_UNDEF) continue;
      if (syms[j].st_other == STV_INTERNAL) continue;
      if (ELF64_ST_BIND(syms[j].st_info) == STB_LOCAL) continue;
      symname = GetElfString(elf, st->st_size, strs, syms[j].st_name);
      CONCAT(&symbols.p, &symbols.i, &symbols.n, symname, strlen(symname) + 1);
      APPEND(&symnames.p, &symnames.i, &symnames.n, &i);
    }
    CHECK_NE(-1, munmap(elf, st->st_size));
    close(fd);
  }
  APPEND(&filenames.p, &filenames.i, &filenames.n, "\n");

  // compute length of output archive
  outsize = 0;
  tablebufsize = 4 + symnames.i * 4;
  tablebuf = xmalloc(tablebufsize);
  offsets = xmalloc(objectargcount * 4);
  header1 = xmalloc(sizeof(struct Header));
  header2 = xmalloc(sizeof(struct Header));
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
  for (i = 0; i < objectargcount; ++i) {
    outsize += outsize & 1;
    offsets[i] = outsize;
    outsize += 60;
    outsize += sizes[i];
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
  CHECK_NE(-1, (outfd = open(outpath, O_WRONLY | O_TRUNC | O_CREAT, 0644)));
  ftruncate(outfd, outsize);
  if ((outsize = writev(outfd, iov, ARRAYLEN(iov))) == -1) goto fail;
  for (i = 0; i < objectargcount; ++i) {
    if ((fd = open(objectargs[i], O_RDONLY)) == -1) goto fail;
    iov[0].iov_base = "\n";
    outsize += (iov[0].iov_len = outsize & 1);
    iov[1].iov_base = header1;
    outsize += (iov[1].iov_len = 60);
    MakeHeader(header1, "/", names[i], modes[i], sizes[i]);
    if (writev(outfd, iov, 2) == -1) goto fail;
    outsize += (remain = sizes[i]);
    if (copy_file_range(fd, NULL, outfd, NULL, remain, 0) != remain) goto fail;
    close(fd);
  }
  close(outfd);

  free(header2);
  free(header1);
  free(offsets);
  free(tablebuf);
  free(sizes);
  free(names);
  free(modes);
  free(symbols.p);
  free(filenames.p);
  free(symnames.p);
  free(st);
  return 0;

fail:
  err = errno;
  if (!err) err = 1;
  unlink(outpath);
  fputs("error: ar failed\n", stderr);
  return err;
}
