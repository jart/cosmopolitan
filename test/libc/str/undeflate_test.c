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
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/errno.h"
#include "libc/log/check.h"
#include "libc/macros.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/crc32.h"
#include "libc/runtime/gc.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/symbols.internal.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/str/undeflate.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/hyperion.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"
#include "libc/zip.h"
#include "third_party/zlib/zlib.h"

STATIC_YOINK("zip_uri_support");
STATIC_YOINK("libc/testlib/hyperion.txt");

TEST(undeflate, testEmbeddedPlaintextConstant) {
  EXPECT_STARTSWITH("The fall of Hyperion - a Dream", kHyperion);
}

TEST(undeflate, testStatCentralDirectory_notFound_noSysCalls) {
  uint64_t c;
  struct stat st;
  stat("zip:doge.txt", &st); /* warmup */
  c = g_syscount;
  ASSERT_EQ(-1, stat("zip:doge.txt", &st));
  ASSERT_EQ(0, g_syscount - c);
  ASSERT_EQ(ENOENT, errno);
}

TEST(undeflate, testStatCentralDirectory_isFound_noSysCalls) {
  uint64_t c;
  struct stat st = {0};
  c = g_syscount;
  ASSERT_NE(-1, stat("zip:libc/testlib/hyperion.txt", &st));
  ASSERT_TRUE(S_ISREG(st.st_mode));
  ASSERT_EQ(kHyperionSize, st.st_size);
  ASSERT_EQ(0, g_syscount - c);
}

TEST(undeflate, testOpenReadCloseEmbeddedZip) {
  int fd;
  char *data;
  ASSERT_NE(-1, (fd = open("zip:libc/testlib/hyperion.txt", O_RDONLY)));
  ASSERT_NE(NULL, (data = gc(malloc(kHyperionSize))));
  ASSERT_EQ(kHyperionSize, read(fd, data, kHyperionSize));
  EXPECT_EQ(0, memcmp(kHyperion, data, kHyperionSize));
  EXPECT_NE(-1, close(fd));
}

TEST(undeflate, testEmbeddedCompressedZipFile_theHardWay) {
  int fd;
  size_t i;
  bool found;
  struct stat st;
  struct DeflateState ds;
  uint8_t *map, *cd, *cf, *lf, *data;
  found = false;
  ASSERT_NE(-1, (fd = open(FindComBinary(), O_RDONLY)));
  ASSERT_NE(-1, fstat(fd, &st));
  ASSERT_NE(MAP_FAILED,
            (map = mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, fd, 0)));
  ASSERT_NE(NULL, (cd = zipfindcentraldir(map, st.st_size)));
  ASSERT_GE(ZIP_CDIR_RECORDS(cd), 1);
  for (i = 0, cf = map + ZIP_CDIR_OFFSET(cd); i < ZIP_CDIR_RECORDS(cd);
       ++i, cf += ZIP_CFILE_HDRSIZE(cf)) {
    if (strncmp("libc/testlib/hyperion.txt", ZIP_CFILE_NAME(cf),
                ZIP_CFILE_NAMESIZE(cf)) == 0) {
      lf = map + ZIP_CFILE_OFFSET(cf);
      ASSERT_EQ(kZipCompressionDeflate, ZIP_LFILE_COMPRESSIONMETHOD(lf));
      ASSERT_EQ(kHyperionSize, ZIP_LFILE_UNCOMPRESSEDSIZE(lf));
      undeflate((data = gc(xmalloc(ZIP_LFILE_UNCOMPRESSEDSIZE(lf) * 24))),
                ZIP_LFILE_UNCOMPRESSEDSIZE(lf), ZIP_LFILE_CONTENT(lf),
                ZIP_LFILE_COMPRESSEDSIZE(lf), &ds);
      ASSERT_EQ(ZIP_LFILE_CRC32(lf),
                crc32_z(0, data, ZIP_LFILE_UNCOMPRESSEDSIZE(lf)));
      ASSERT_EQ(0, memcmp(kHyperion, data, ZIP_LFILE_UNCOMPRESSEDSIZE(lf)));
      found = true;
      break;
    }
  }
  ASSERT_NE(-1, munmap(map, st.st_size));
  ASSERT_NE(-1, close(fd));
  ASSERT_TRUE(found);
}

////////////////////////////////////////////////////////////////////////////////

uint8_t *buf_;
size_t bufsize_;
uint8_t *data_;
size_t compressedsize_;
size_t uncompressedsize_;
struct DeflateState ds_;

void Inflate(void) {
  z_stream stream;
  stream.next_in = data_;
  stream.zalloc = Z_NULL;
  stream.zfree = Z_NULL;
  stream.avail_in = compressedsize_;
  stream.total_in = compressedsize_;
  stream.next_out = buf_;
  stream.avail_out = bufsize_;
  stream.total_out = bufsize_;
  CHECK_EQ(Z_OK, inflateInit2(&stream, -MAX_WBITS));
  CHECK_NE(Z_BUF_ERROR, inflate(&stream, Z_NO_FLUSH));
  CHECK_EQ(Z_OK, inflateEnd(&stream));
}

void Undeflate(void) {
  undeflate(buf_, uncompressedsize_, data_, compressedsize_, &ds_);
}

static size_t GetLocalFile(const char *name) {
  size_t i, cf, namesize;
  namesize = strlen(name);
  for (i = 0, cf = ZIP_CDIR_OFFSET(__zip_end); i < ZIP_CDIR_RECORDS(__zip_end);
       ++i, cf += ZIP_CFILE_HDRSIZE(cf)) {
    if (namesize == ZIP_CFILE_NAMESIZE(&_base[0] + cf) &&
        memcmp(name, ZIP_CFILE_NAME(&_base[0] + cf), namesize) == 0) {
      return ZIP_CFILE_OFFSET(&_base[0] + cf);
    }
  }
  abort();
}

BENCH(undeflate, bench) {
  size_t lf;
  lf = GetLocalFile("libc/testlib/hyperion.txt");
  data_ = ZIP_LFILE_CONTENT(&_base[0] + lf);
  compressedsize_ = ZIP_LFILE_COMPRESSEDSIZE(&_base[0] + lf);
  uncompressedsize_ = ZIP_LFILE_UNCOMPRESSEDSIZE(&_base[0] + lf);
  bufsize_ = ROUNDUP(uncompressedsize_, FRAMESIZE / 2);
  buf_ = gc(malloc(bufsize_));
  EZBENCH(donothing, Inflate());
  EZBENCH(donothing, Undeflate());
}
