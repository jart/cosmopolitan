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
#include "libc/bits/safemacros.h"
#include "libc/calls/calls.h"
#include "libc/log/check.h"
#include "libc/nexgen32e/kompressor.h"
#include "libc/nexgen32e/lz4.h"
#include "libc/runtime/ezmap.internal.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"

TEST(lz4, decompress_emptyStringWithoutChecksum) {
  /* lz4 -9 --content-size --no-frame-crc /tmp/empty - | hexdump -C */
  static char kLz4Data[] = {0x04, 0x22, 0x4d, 0x18, 0x60, 0x40,
                            0x82, 0x00, 0x00, 0x00, 0x00};
  char *src = memcpy(tmalloc(sizeof(kLz4Data)), kLz4Data, sizeof(kLz4Data));
  char *dst = tmalloc(1);
  *dst = 'z';
  ASSERT_EQ(dst, lz4decode(dst, src));
  ASSERT_EQ('z', *dst);
  tfree(dst);
  tfree(src);
}

TEST(lz4, decompress_oneLetterWithoutChecksum) {
  /* printf a >oneletter */
  /* lz4 -9 --content-size --no-frame-crc oneletter /dev/stdout | hexdump -C */
  static char kLz4Data[] = {0x04, 0x22, 0x4d, 0x18, 0x68, 0x40, 0x01, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2c, 0x01,
                            0x00, 0x00, 0x80, 0x61, 0x00, 0x00, 0x00, 0x00};
  char *src = memcpy(tmalloc(sizeof(kLz4Data)), kLz4Data, sizeof(kLz4Data));
  char *dst = tmalloc(1);
  ASSERT_EQ(dst + 1, lz4decode(dst, src));
  ASSERT_EQ('a', *dst);
  tfree(dst);
  tfree(src);
}

TEST(lz4, decompress_runLengthDecode) {
  /* printf aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa >/tmp/a */
  /* lz4 -9 --content-size --no-frame-crc /tmp/a - | hexdump -vC */
  static char kLz4Data[] = {
      0x04, 0x22, 0x4d, 0x18, 0x68, 0x40, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x30, 0x0b, 0x00, 0x00, 0x00, 0x1f, 0x61, 0x01, 0x00, 0x07,
      0x50, 0x61, 0x61, 0x61, 0x61, 0x61, 0x00, 0x00, 0x00, 0x00};
  char *src = memcpy(tmalloc(sizeof(kLz4Data)), kLz4Data, sizeof(kLz4Data));
  const char *want = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
  char *dst = tmalloc(strlen(want));
  ASSERT_EQ(dst + strlen(want), lz4decode(dst, src));
  ASSERT_STREQN(want, dst, strlen(want));
  tfree(dst);
  tfree(src);
}

TEST(lz4, zoneFileGmt) {
  if (!fileexists("usr/share/zoneinfo.dict.lz4")) return;
  struct MappedFile dict, gmt;
  CHECK_NE(-1, MapFileRead("usr/share/zoneinfo.dict.lz4", &dict));
  CHECK_NE(-1, MapFileRead("usr/share/zoneinfo/GMT.lz4", &gmt));
  size_t mapsize, gmtsize;
  char *mapping, *gmtdata;
  lz4decode((gmtdata = lz4decode(
                 (mapping = mapanon(
                      (mapsize = roundup(
                           LZ4_FRAME_BLOCKCONTENTSIZE(lz4check(dict.addr)) +
                               (gmtsize = LZ4_FRAME_BLOCKCONTENTSIZE(
                                    lz4check(gmt.addr))),
                           FRAMESIZE)))),
                 dict.addr)),
            gmt.addr);
  ASSERT_BINEQ(
      u"TZif2                  ☺   ☺           ☺   ♦      GMT   TZif2   "
      u"               ☺   ☺       ☺   ☺   ♦°              GMT   ◙GMT0◙",
      gmtdata);
  munmap(mapping, mapsize);
  UnmapFile(&dict);
  UnmapFile(&gmt);
}
