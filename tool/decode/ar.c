/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/fmt/conv.h"
#include "libc/serialize.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/x/x.h"

#define AR_MAGIC1 "!<arch>\n"
#define AR_MAGIC2 "`\n"

/**
 * ar rU doge.a NOTICE  # create archive and use non-deterministic stuff
 * o//tool/decode/ar.com doge.a
 */

static int fd;
static uint8_t *data;
static long size;
static const char *path;
static struct stat st;

static void PrintString(uint8_t *p, long n, const char *name) {
  char *s;
  s = xmalloc(n + 1);
  s[n] = '\0';
  memcpy(s, p, n);
  printf("\t.ascii\t%-`'*.*s# %s\n", 35, n, s, name);
  free(s);
}

static void Open(void) {
  if ((fd = open(path, O_RDONLY)) == -1) {
    fprintf(stderr, "error: open() failed: %s\n", path);
    exit(1);
  }
  CHECK_NE(-1, fstat(fd, &st));
  if (!(size = st.st_size)) exit(0);
  CHECK_NE(MAP_FAILED,
           (data = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0)));
  LOGIFNEG1(close(fd));
}

static void Close(void) {
  LOGIFNEG1(munmap(data, size));
}

static void Check(void) {
  if (size < 8 + 60 + 4 ||
      (memcmp(data, AR_MAGIC1, strlen(AR_MAGIC1)) != 0 ||
       memcmp(data + 66, AR_MAGIC2, strlen(AR_MAGIC2)) != 0)) {
    fprintf(stderr, "error: not a unix archive: %s\n", path);
    exit(1);
  }
}

static void PrintTable(void) {
}

static void PrintHeader(uint8_t *p) {
  PrintString(p + 0, 16, "file identifier [ascii]");
  PrintString(p + 16, 12, "file modification timestamp [decimal]");
  PrintString(p + 28, 6, "owner id [decimal]");
  PrintString(p + 34, 6, "group id [decimal]");
  PrintString(p + 40, 8, "file mode [octal] (type and permission)");
  PrintString(p + 48, 10, "file size in bytes [decimal]");
  PrintString(p + 58, 2, "ending characters");
}

static void Print(void) {
  int arsize;
  uint8_t *b, *p;
  uint64_t offset;
  uint32_t i, n, o, table, entries, symbols, symbolslen;
  arsize = atoi((char *)(data + 8 + 48));
  CHECK_LE(4, arsize);
  CHECK_LE(8 + 60 + arsize, size);
  entries = READ32BE(data + 8 + 60);
  CHECK_LE(4 + entries * 4 + 1, arsize);
  printf("\t# %'s\n", path);
  PrintString(data, 8, "file signature");
  PrintHeader(data + 8);

  printf("\n");
  printf("\t.long\t%-*.u# %s\n", 35, entries, "symbol table entries");
  table = 8 + 60 + 4;
  for (i = 0; i < entries; ++i) {
    printf("\t.long\t%#-*.x# %u\n", 35, READ32BE(data + table + i * 4), i);
  }
  symbols = table + entries * 4;
  symbolslen = arsize - (entries + 1) * 4;
  for (i = o = 0; o < symbolslen; ++i, o += n + 1) {
    b = data + symbols + o;
    CHECK_NOTNULL((p = memchr(b, '\0', symbolslen - o)), "%p %p %p %p %`.s", b,
                  data, symbols, o, b);
    n = p - b;
    printf("\t.asciz\t%#-`'*.*s# %u\n", 35, n, b, i);
  }

  offset = 8 + 60 + arsize;
  while (offset < size) {
    offset += offset & 1;
    CHECK_LE(offset + 60, size);
    CHECK_EQ(0, memcmp(data + offset + 58, AR_MAGIC2, strlen(AR_MAGIC2)));
    arsize = atoi((char *)(data + offset + 48));
    CHECK_LE(offset + 60 + arsize, size);
    printf("\n");
    PrintHeader(data + offset);
    offset += 60 + arsize;
  }
}

int main(int argc, char *argv[]) {
  if (argc < 2) return 1;
  path = argv[1];
  Open();
  Check();
  Print();
  Close();
  return 0;
}
