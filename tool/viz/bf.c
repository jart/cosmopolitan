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
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/bits.h"
#include "libc/intrin/bsr.h"
#include "libc/intrin/tpenc.h"
#include "libc/macros.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/str/tab.internal.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/s.h"

/**
 * @fileoverview BingFold - A Hexdump Tool
 */

#define W 64
#define C 253

void bf(int fd) {
  ssize_t rc;
  uint64_t w;
  struct stat st;
  size_t i, n, o;
  int c, fg, fg2, bits;
  char ibuf[W], obuf[12 + 1 + W * 6 + 1 + W * (2 + 11) + 11 + 1];
  o = 0;
  if (fstat(fd, &st) != -1 && S_ISREG(st.st_mode) && st.st_size) {
    bits = _bsr(_roundup2pow(st.st_size));
    bits = ROUNDUP(bits, 4);
  } else {
    bits = 48;
  }
  do {
    if ((rc = read(fd, ibuf, W)) == -1) exit(2);
    if (rc) {
      n = 0;
      n += uint64toarray_fixed16(o, obuf + n, bits);
      o += rc;
      obuf[n++] = ' ';
      for (i = 0; i < rc; ++i) {
        c = ibuf[i] & 0xff;
        w = _tpenc(kCp437[c]);
        do {
          obuf[n++] = w;
        } while ((w >>= 8));
      }
      for (i = 0; i < W - rc + 1; ++i) {
        obuf[n++] = ' ';
      }
      for (fg = -1, i = 0; i < rc; ++i) {
        c = ibuf[i] & 0xff;
        if (c < 32) {
          fg2 = 237 + c * ((C - 237) / 32.);
        } else if (c >= 232) {
          fg2 = C + (c - 232) * ((255 - C) / (256. - 232));
        } else {
          fg2 = C;
        }
        if (fg2 != fg) {
          fg = fg2;
          obuf[n++] = 033;
          obuf[n++] = '[';
          obuf[n++] = '3';
          obuf[n++] = '8';
          obuf[n++] = ';';
          obuf[n++] = '5';
          obuf[n++] = ';';
          n = FormatInt64(obuf + n, fg) - (obuf + n);
          obuf[n++] = 'm';
        }
        obuf[n++] = "0123456789abcdef"[c >> 4];
        obuf[n++] = "0123456789abcdef"[c & 15];
        /* obuf[n++] = ' '; */
      }
      obuf[n++] = 033;
      obuf[n++] = '[';
      obuf[n++] = '0';
      obuf[n++] = 'm';
      obuf[n++] = '\n';
      write(1, obuf, n);
    }
  } while (rc == W);
}

int main(int argc, char *argv[]) {
  int i, fd;
  if (argc > 1) {
    for (i = 1; i < argc; ++i) {
      if (i > 1) write(1, "\n", 1);
      if ((fd = open(argv[i], O_RDONLY)) == -1) exit(1);
      bf(fd);
    }
  } else {
    bf(0);
  }
  return 0;
}
