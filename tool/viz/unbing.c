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
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/str/tab.internal.h"

/**
 * @fileoverview UnBing: Glyphs → Binary.
 *
 * Intended for reassembling modified binaries. Characters that aren't
 * IBM CodePage 437 glyphs are ignored. That includes line feeds, so
 * it's safe to use the fold command on the bing program output, to have
 * something almost representing a source code file. A hack also exists
 * for comments in such files: 𝘶𝘴𝘦 𝑛𝑜𝑛-𝑎𝑠𝑐𝑖𝑖 𝗹𝗲𝘁𝘁𝗲𝗿𝗶𝗻𝗴 𝔞𝔩𝔱𝔢𝔯𝔫𝔞𝔱𝔦𝔳𝔢𝔰.
 *
 *   bash$ o/tool/viz/bing.com </bin/sh | o/tool/viz/fold.com >sh.txt
 *   bash$ emacs sh.txt
 *   bash$ o/tool/viz/unbing.com <sh.txt >sh
 *   bash$ chmod +x sh
 *   bash$ ./sh
 *
 * That's how it works.
 */

uint16_t g_map[0x10000];

int main(int argc, char *argv[argc]) {
  size_t i;
  wint_t c;
  for (i = 0; i < 256; ++i) {
    g_map[kCp437[i]] = i + 1;
  }
  while ((c = fgetwc(stdin)) != WEOF) {
    if (c == (c & 0xffff) && g_map[c]) {
      fputc(g_map[c] - 1, stdout);
    }
  }
  return 0;
}
