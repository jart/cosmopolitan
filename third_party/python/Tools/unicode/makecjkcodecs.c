/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/assert.h"
#include "libc/fmt/leb128.h"
#include "libc/log/check.h"
#include "libc/nexgen32e/crc32.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/append.h"
#include "libc/stdio/stdio.h"
#include "libc/x/x.h"
#include "third_party/zlib/zlib.h"

#define MAC(x) m(#x, x, sizeof(x))
#define DZD(x) dzd(#x, x, sizeof(x), sizeof(x[0]))

static void *Deflate(const void *data, size_t size, size_t *out_size) {
  void *res;
  z_stream zs;
  zs.zfree = 0;
  zs.zalloc = 0;
  CHECK_EQ(Z_OK, deflateInit2(&zs, Z_BEST_COMPRESSION, Z_DEFLATED, -MAX_WBITS,
                              DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY));
  zs.next_in = data;
  zs.avail_in = size;
  zs.avail_out = compressBound(size);
  zs.next_out = res = xmalloc(zs.avail_out);
  CHECK_EQ(Z_STREAM_END, deflate(&zs, Z_FINISH));
  CHECK_EQ(Z_OK, deflateEnd(&zs));
  *out_size = zs.total_out;
  return xrealloc(res, zs.total_out);
}

void m(const char *s, void *p, size_t n) {
  FILE *f;
  size_t i, m;
  unsigned char *q;
  q = Deflate(p, n, &m);
  f = fopen(xstrcat("third_party/python/Modules/cjkcodecs/", s, ".c"), "wb");
  fprintf(f, "#include \"libc/x/x.h\"\n");
  fprintf(f, "\n");
  fprintf(f, "static _Atomic(void *) %s_ptr;\n", s);
  fprintf(f, "static const unsigned char %s_rodata[] = {", s);
  for (i = 0; i < m; ++i) {
    if (i % 13 == 0) fprintf(f, "\n ");
    fprintf(f, " 0x%02x,", q[i]);
  }
  fprintf(f, "\n};\n");
  fprintf(f, "\n");
  fprintf(f, "optimizesize void *%s(void) {\n", s);
  fprintf(f, "  return xload(&%s_ptr,\n", s);
  fprintf(f, "               %s_rodata,\n", s);
  fprintf(f, "               %d, %d); /* %g%% profit */\n", m, n,
          (double)m / n * 100);
  fprintf(f, "}\n");
  fclose(f);
  printf("\tthird_party/python/Modules/cjkcodecs/%s.c\t\\\n", s);
}

void dzd(const char *s, void *p, size_t n, size_t z) {
  FILE *f;
  uint32_t S;
  size_t i, m;
  int64_t x, y;
  char *r, *q, *b, t[19];
  S = crc32_z(0, p, n);
  for (r = 0, y = i = 0; i < n / z; ++i) {
    if (z == 2) {
      x = ((const uint16_t *)p)[i];
    } else if (z == 4) {
      x = ((const uint32_t *)p)[i];
    } else {
      __builtin_unreachable();
    }
    appendd(&r, t, zleb64(t, x - y) - t);
    y = x;
  }
  q = Deflate(r, appendz(r).i, &m);
  f = fopen(xstrcat("third_party/python/Modules/cjkcodecs/", s, ".c"), "wb");
  fprintf(f, "#include \"libc/x/x.h\"\n");
  fprintf(f, "\n");
  fprintf(f, "static _Atomic(void *) %s_ptr;\n", s);
  fprintf(f, "static const unsigned char %s_rodata[%zu] = {", s, m);
  for (i = 0; i < m; ++i) {
    if (i % 13 == 0) fprintf(f, "\n ");
    fprintf(f, " 0x%02x,", q[i] & 255);
  }
  fprintf(f, "\n};\n");
  fprintf(f, "\n");
  fprintf(f, "optimizesize void *%s(void) {\n", s);
  fprintf(f, "  return xloadzd(&%s_ptr,\n", s);
  fprintf(f, "                 %s_rodata,\n", s);
  fprintf(f, "                 %d, %d, %d, %d, 0x%08xu); /* %g%% profit */\n",
          m, appendz(r).i, n / z, z, S, (double)m / n * 100);
  fprintf(f, "}\n");
  fclose(f);
  printf("\tthird_party/python/Modules/cjkcodecs/%s.c\t\\\n", s);
}

#define JISX0213_ENCPAIRS 46

#define UNIINV 0xFFFE
#define NOCHAR 0xFFFF
#define MULTIC 0xFFFE
#define DBCINV 0xFFFD
#define U      UNIINV
#define N      NOCHAR
#define M      MULTIC
#define D      DBCINV

typedef uint16_t ucs2_t;
typedef uint16_t DBCHAR;
typedef uint32_t Py_UCS4;

struct CjkIndex {
  uint16_t map;
  unsigned char bottom, top;
};

struct CjkPairEncodeMap {
  uint32_t uniseq;
  uint16_t code;
};

#include "mappings_cn.inc"
#include "mappings_hk.inc"
#include "mappings_jisx0213_pair.inc"
#include "mappings_jp.inc"
#include "mappings_kr.inc"
#include "mappings_tw.inc"

int main(int argc, char *argv[]) {
  MAC(big5_decmap);
  MAC(big5_encmap);
  MAC(big5hkscs_bmp_encmap);
  MAC(big5hkscs_decmap);
  MAC(big5hkscs_nonbmp_encmap);
  MAC(cp932ext_decmap);
  MAC(cp932ext_encmap);
  MAC(cp949_encmap);
  MAC(cp949ext_decmap);
  MAC(cp950ext_decmap);
  MAC(cp950ext_encmap);
  MAC(gb18030ext_decmap);
  MAC(gb18030ext_encmap);
  MAC(gb2312_decmap);
  MAC(gbcommon_encmap);
  MAC(gbkext_decmap);
  MAC(jisx0208_decmap);
  MAC(jisx0212_decmap);
  MAC(jisx0213_1_bmp_decmap);
  MAC(jisx0213_1_emp_decmap);
  MAC(jisx0213_2_bmp_decmap);
  MAC(jisx0213_2_emp_decmap);
  MAC(jisx0213_bmp_encmap);
  MAC(jisx0213_emp_encmap);
  MAC(jisx0213_pair_decmap);
  MAC(jisxcommon_encmap);
  MAC(ksx1001_decmap);
  MAC(jisx0213_pair_encmap);
  DZD(__big5_decmap);
  MAC(__big5_encmap);
  MAC(__big5hkscs_bmp_encmap);
  MAC(__big5hkscs_decmap);
  MAC(__big5hkscs_nonbmp_encmap);
  DZD(__cp932ext_decmap);
  MAC(__cp932ext_encmap);
  DZD(__cp949_encmap);
  DZD(__cp949ext_decmap);
  DZD(__cp950ext_decmap);
  MAC(__cp950ext_encmap);
  DZD(__gb18030ext_decmap);
  DZD(__gb18030ext_encmap);
  DZD(__gb2312_decmap);
  DZD(__gbcommon_encmap);
  DZD(__gbkext_decmap);
  DZD(__jisx0208_decmap);
  DZD(__jisx0212_decmap);
  DZD(__jisx0213_1_bmp_decmap);
  MAC(__jisx0213_1_emp_decmap);
  DZD(__jisx0213_2_bmp_decmap);
  MAC(__jisx0213_2_emp_decmap);
  MAC(__jisx0213_bmp_encmap);
  MAC(__jisx0213_emp_encmap);
  DZD(__jisx0213_pair_decmap);
  MAC(__jisxcommon_encmap);
  DZD(__ksx1001_decmap);
  return 0;
}
