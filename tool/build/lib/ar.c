/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2024 Justine Alexandra Roberts Tunney                              │
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
#include "tool/build/lib/ar.h"
#include "libc/ar.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/ctype.h"
#include "libc/fmt/conv.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"

/**
 * @fileoverview static archive reader
 *
 * This file implements an API similar to opendir() for raeding the .o
 * files within your .a file. This works by mapping the .a file into
 * memory and then yielding pointers into the map where embedded files
 * reside, along with their decoded filenames.
 *
 * To try this library:
 *
 *     make -j o//tool/decode/ar2 o//libc/str/str.a
 *     o//tool/decode/ar2 o//libc/str/str.a
 *
 * This implementation currently dies on error. The intent is to make
 * this as simple of an abstraction as possible for simple command line
 * utilities like o//tool/build/ar. It shouldn't be considered a serious
 * general purpose library. Another thing it can't do is decode symbol
 * table information, since Cosmopolitan Ar currently doesn't need it.
 */

void openar(struct Ar *ar, const char *path) {
  memset(ar, 0, sizeof(*ar));
  ar->path = path;
  if ((ar->fd = open(path, O_RDONLY)) == -1) {
    perror(ar->path);
    exit(1);
  }
  if (fstat(ar->fd, &ar->st)) {
    perror(ar->path);
    exit(1);
  }
  ar->map = mmap(0, ar->st.st_size, PROT_READ, MAP_PRIVATE, ar->fd, 0);
  if (ar->map == MAP_FAILED) {
    perror(ar->path);
    exit(1);
  }
  if (!startswith(ar->map, ARMAG)) {
    tinyprint(2, ar->path, ": not an ar file\n", NULL);
    exit(1);
  }
  ar->offset = SARMAG;
}

void closear(struct Ar *ar) {
  if (munmap(ar->map, ar->st.st_size)) {
    perror(ar->path);
    exit(1);
  }
  if (close(ar->fd)) {
    perror(ar->path);
    exit(1);
  }
}

bool readar(struct Ar *ar, struct ArFile *arf) {
  for (;;) {
    ar->offset += 1;
    ar->offset &= -2;
    if (ar->offset + sizeof(struct ar_hdr) > ar->st.st_size)
      return false;

    struct ar_hdr *hdr = (struct ar_hdr *)(ar->map + ar->offset);
    ar->offset += sizeof(struct ar_hdr);

    char ar_fmag[sizeof(hdr->ar_fmag) + 1] = {0};
    memcpy(ar_fmag, hdr->ar_fmag, sizeof(hdr->ar_fmag));
    if (strcmp(ar_fmag, ARFMAG)) {
      tinyprint(2, ar->path, ": corrupt ar file fmag\n", NULL);
      exit(1);
    }

    char ar_name[sizeof(hdr->ar_name) + 1] = {0};
    memcpy(ar_name, hdr->ar_name, sizeof(hdr->ar_name));
    for (int j = sizeof(hdr->ar_name) - 1 + 1; j-- && isspace(ar_name[j]);)
      ar_name[j] = '\0';

    char ar_size[sizeof(hdr->ar_size) + 1] = {0};
    memcpy(ar_size, hdr->ar_size, sizeof(hdr->ar_size));
    int size = atoi(ar_size);
    if (size < 0 || ar->offset + size > ar->st.st_size) {
      tinyprint(2, ar->path, ": ar size overlaps eof\n", NULL);
      exit(1);
    }

    // symbol table
    if (!strcmp(ar_name, "/")) {
      ar->offset += size;
      continue;
    }

    // filename table
    if (!strcmp(ar_name, "//")) {
      ar->filenames = ar->map + ar->offset;
      ar->filenames_size = size;
      ar->offset += size;
      continue;
    }

    // get name of object file
    size_t len;
    const char *e;
    if (ar_name[0] == '/') {
      int off = atoi(ar_name + 1);
      if (off < 0 || off >= ar->filenames_size) {
        tinyprint(2, ar->path, ": ar filename not found\n", NULL);
        exit(1);
      }
      if (!(e = memchr(ar->filenames + off, '\n', ar->filenames_size - off))) {
        tinyprint(2, ar->path, ": ar filename overlaps end\n", NULL);
        exit(1);
      }
      if ((len = e - (ar->filenames + off)) >= PATH_MAX) {
        tinyprint(2, ar->path, ": ar filename too long\n", NULL);
        exit(1);
      }
      memcpy(arf->name, ar->filenames + off, len);
      arf->name[len] = '\0';
      if (len && arf->name[len - 1] == '/')
        arf->name[--len] = '\0';
    } else if ((len = strlen(ar_name)) && ar_name[len - 1] == '/') {
      memcpy(arf->name, ar_name, len - 1);
      arf->name[len - 1] = '\0';
    } else {
      tinyprint(2, ar->path, ": unsupported ar name: ", ar_name, "\n", NULL);
      exit(1);
    }

    // return pointer to embedded file
    arf->size = size;
    arf->offset = ar->offset;
    arf->data = ar->map + ar->offset;
    ar->offset += size;
    return true;
  }
}
