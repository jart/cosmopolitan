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
#include "libc/assert.h"
#include "libc/calls/struct/timespec.h"
#include "libc/dce.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/crc32.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/testlib/hyperion.h"
#include "libc/testlib/testlib.h"
#include "third_party/aarch64/arm_acle.internal.h"
#include "third_party/aarch64/arm_neon.internal.h"
#include "third_party/intel/immintrin.internal.h"
#include "third_party/zlib/zlib.h"

#define FANATICS "Fanatics"

static const char hyperion[] =
    FANATICS " have their dreams, wherewith they weave / "
             "A paradise for a sect; the savage too / "
             "From forth the loftiest fashion of his sleep / "
             "...";

unsigned crc32_reference(unsigned crc, const void *data, size_t size) {
  static unsigned tab[256];
  if (!tab[255]) {
    for (int d = 0; d < 256; ++d) {
      unsigned r = d;
      for (int i = 0; i < 8; ++i)
        r = r >> 1 ^ (r & 1 ? 0xedb88320u : 0);
      tab[d] = r;
    }
  }
  const unsigned char *p = (const unsigned char *)data;
  const unsigned char *pe = p + size;
  crc = ~crc;
  while (p < pe)
    crc = crc >> 8 ^ tab[(crc & 255) ^ *p++];
  crc = ~crc;
  return crc;
}

// Calculates CRC32 at 73 GiB/s on AMD Ryzen Threadripper PRO 7995WX.
// "Fast CRC Computation for Generic Polynomials Using PCLMULQDQ Instruction"
//  V. Gopal, E. Ozturk, et al., 2009, http://intel.ly/2ySEwL0
uint32_t crc32_uber_alles(uint32_t crc, const void *data, size_t len) {
  const uint8_t *buf = (const uint8_t *)data;
  crc = ~crc;
#if defined(__AVX512F__) && defined(__VPCLMULQDQ__) && defined(__PCLMUL__)
  if (len >= 256) {
    __attribute__((__aligned__(64))) static const uint64_t k1k2[] = {
        0x011542778a, 0x01322d1430, 0x011542778a, 0x01322d1430,
        0x011542778a, 0x01322d1430, 0x011542778a, 0x01322d1430,
    };
    __attribute__((__aligned__(64))) static const uint64_t k3k4[] = {
        0x0154442bd4, 0x01c6e41596, 0x0154442bd4, 0x01c6e41596,
        0x0154442bd4, 0x01c6e41596, 0x0154442bd4, 0x01c6e41596,
    };
    __attribute__((__aligned__(64))) static const uint64_t k5k6[] = {
        0x01751997d0,
        0x00ccaa009e,
    };
    __attribute__((__aligned__(64))) static const uint64_t k7k8[] = {
        0x0163cd6124,
        0x0000000000,
    };
    __attribute__((__aligned__(64))) static const uint64_t poly[] = {
        0x01db710641,
        0x01f7011641,
    };
    __m512i x0, x1, x2, x3, x4, x5, x6, x7, x8, y5, y6, y7, y8;
    __m128i a0, a1, a2, a3;
    x1 = _mm512_loadu_si512((__m512i *)(buf + 0x00));
    x2 = _mm512_loadu_si512((__m512i *)(buf + 0x40));
    x3 = _mm512_loadu_si512((__m512i *)(buf + 0x80));
    x4 = _mm512_loadu_si512((__m512i *)(buf + 0xC0));
    x1 = _mm512_xor_si512(x1, _mm512_castsi128_si512(_mm_cvtsi32_si128(crc)));
    x0 = _mm512_load_si512((__m512i *)k1k2);
    buf += 256;
    len -= 256;
    while (len >= 256) {
      x5 = _mm512_clmulepi64_epi128(x1, x0, 0x00);
      x6 = _mm512_clmulepi64_epi128(x2, x0, 0x00);
      x7 = _mm512_clmulepi64_epi128(x3, x0, 0x00);
      x8 = _mm512_clmulepi64_epi128(x4, x0, 0x00);
      x1 = _mm512_clmulepi64_epi128(x1, x0, 0x11);
      x2 = _mm512_clmulepi64_epi128(x2, x0, 0x11);
      x3 = _mm512_clmulepi64_epi128(x3, x0, 0x11);
      x4 = _mm512_clmulepi64_epi128(x4, x0, 0x11);
      y5 = _mm512_loadu_si512((__m512i *)(buf + 0x00));
      y6 = _mm512_loadu_si512((__m512i *)(buf + 0x40));
      y7 = _mm512_loadu_si512((__m512i *)(buf + 0x80));
      y8 = _mm512_loadu_si512((__m512i *)(buf + 0xC0));
      x1 = _mm512_xor_si512(x1, x5);
      x2 = _mm512_xor_si512(x2, x6);
      x3 = _mm512_xor_si512(x3, x7);
      x4 = _mm512_xor_si512(x4, x8);
      x1 = _mm512_xor_si512(x1, y5);
      x2 = _mm512_xor_si512(x2, y6);
      x3 = _mm512_xor_si512(x3, y7);
      x4 = _mm512_xor_si512(x4, y8);
      buf += 256;
      len -= 256;
    }
    x0 = _mm512_load_si512((__m512i *)k3k4);
    x5 = _mm512_clmulepi64_epi128(x1, x0, 0x00);
    x1 = _mm512_clmulepi64_epi128(x1, x0, 0x11);
    x1 = _mm512_xor_si512(x1, x2);
    x1 = _mm512_xor_si512(x1, x5);
    x5 = _mm512_clmulepi64_epi128(x1, x0, 0x00);
    x1 = _mm512_clmulepi64_epi128(x1, x0, 0x11);
    x1 = _mm512_xor_si512(x1, x3);
    x1 = _mm512_xor_si512(x1, x5);
    x5 = _mm512_clmulepi64_epi128(x1, x0, 0x00);
    x1 = _mm512_clmulepi64_epi128(x1, x0, 0x11);
    x1 = _mm512_xor_si512(x1, x4);
    x1 = _mm512_xor_si512(x1, x5);
    while (len >= 64) {
      x2 = _mm512_loadu_si512((__m512i *)buf);
      x5 = _mm512_clmulepi64_epi128(x1, x0, 0x00);
      x1 = _mm512_clmulepi64_epi128(x1, x0, 0x11);
      x1 = _mm512_xor_si512(x1, x2);
      x1 = _mm512_xor_si512(x1, x5);
      buf += 64;
      len -= 64;
    }
    a0 = _mm_load_si128((__m128i *)k5k6);
    a1 = _mm512_extracti32x4_epi32(x1, 0);
    a2 = _mm512_extracti32x4_epi32(x1, 1);
    a3 = _mm_clmulepi64_si128(a1, a0, 0x00);
    a1 = _mm_clmulepi64_si128(a1, a0, 0x11);
    a1 = _mm_xor_si128(a1, a3);
    a1 = _mm_xor_si128(a1, a2);
    a2 = _mm512_extracti32x4_epi32(x1, 2);
    a3 = _mm_clmulepi64_si128(a1, a0, 0x00);
    a1 = _mm_clmulepi64_si128(a1, a0, 0x11);
    a1 = _mm_xor_si128(a1, a3);
    a1 = _mm_xor_si128(a1, a2);
    a2 = _mm512_extracti32x4_epi32(x1, 3);
    a3 = _mm_clmulepi64_si128(a1, a0, 0x00);
    a1 = _mm_clmulepi64_si128(a1, a0, 0x11);
    a1 = _mm_xor_si128(a1, a3);
    a1 = _mm_xor_si128(a1, a2);
    a2 = _mm_clmulepi64_si128(a1, a0, 0x10);
    a3 = _mm_setr_epi32(~0, 0, ~0, 0);
    a1 = _mm_srli_si128(a1, 8);
    a1 = _mm_xor_si128(a1, a2);
    a0 = _mm_loadl_epi64((__m128i *)k7k8);
    a2 = _mm_srli_si128(a1, 4);
    a1 = _mm_and_si128(a1, a3);
    a1 = _mm_clmulepi64_si128(a1, a0, 0x00);
    a1 = _mm_xor_si128(a1, a2);
    a0 = _mm_load_si128((__m128i *)poly);
    a2 = _mm_and_si128(a1, a3);
    a2 = _mm_clmulepi64_si128(a2, a0, 0x10);
    a2 = _mm_and_si128(a2, a3);
    a2 = _mm_clmulepi64_si128(a2, a0, 0x00);
    a1 = _mm_xor_si128(a1, a2);
    crc = _mm_extract_epi32(a1, 1);
  }
#endif
#if defined(__SSE4_2__) && defined(__PCLMUL__)
  if (len >= 64) {
    __attribute__((__aligned__(16))) static const uint64_t k1k2[] = {
        0x0154442bd4,
        0x01c6e41596,
    };
    __attribute__((__aligned__(16))) static const uint64_t k3k4[] = {
        0x01751997d0,
        0x00ccaa009e,
    };
    __attribute__((__aligned__(16))) static const uint64_t k5k0[] = {
        0x0163cd6124,
        0x0000000000,
    };
    __attribute__((__aligned__(16))) static const uint64_t poly[] = {
        0x01db710641,
        0x01f7011641,
    };
    __m128i x0, x1, x2, x3, x4, x5, x6, x7, x8, y5, y6, y7, y8;
    x1 = _mm_loadu_si128((__m128i *)(buf + 0x00));
    x2 = _mm_loadu_si128((__m128i *)(buf + 0x10));
    x3 = _mm_loadu_si128((__m128i *)(buf + 0x20));
    x4 = _mm_loadu_si128((__m128i *)(buf + 0x30));
    x1 = _mm_xor_si128(x1, _mm_cvtsi32_si128(crc));
    x0 = _mm_load_si128((__m128i *)k1k2);
    buf += 64;
    len -= 64;
    while (len >= 64) {
      x5 = _mm_clmulepi64_si128(x1, x0, 0x00);
      x6 = _mm_clmulepi64_si128(x2, x0, 0x00);
      x7 = _mm_clmulepi64_si128(x3, x0, 0x00);
      x8 = _mm_clmulepi64_si128(x4, x0, 0x00);
      x1 = _mm_clmulepi64_si128(x1, x0, 0x11);
      x2 = _mm_clmulepi64_si128(x2, x0, 0x11);
      x3 = _mm_clmulepi64_si128(x3, x0, 0x11);
      x4 = _mm_clmulepi64_si128(x4, x0, 0x11);
      y5 = _mm_loadu_si128((__m128i *)(buf + 0x00));
      y6 = _mm_loadu_si128((__m128i *)(buf + 0x10));
      y7 = _mm_loadu_si128((__m128i *)(buf + 0x20));
      y8 = _mm_loadu_si128((__m128i *)(buf + 0x30));
      x1 = _mm_xor_si128(x1, x5);
      x2 = _mm_xor_si128(x2, x6);
      x3 = _mm_xor_si128(x3, x7);
      x4 = _mm_xor_si128(x4, x8);
      x1 = _mm_xor_si128(x1, y5);
      x2 = _mm_xor_si128(x2, y6);
      x3 = _mm_xor_si128(x3, y7);
      x4 = _mm_xor_si128(x4, y8);
      buf += 64;
      len -= 64;
    }
    x0 = _mm_load_si128((__m128i *)k3k4);
    x5 = _mm_clmulepi64_si128(x1, x0, 0x00);
    x1 = _mm_clmulepi64_si128(x1, x0, 0x11);
    x1 = _mm_xor_si128(x1, x2);
    x1 = _mm_xor_si128(x1, x5);
    x5 = _mm_clmulepi64_si128(x1, x0, 0x00);
    x1 = _mm_clmulepi64_si128(x1, x0, 0x11);
    x1 = _mm_xor_si128(x1, x3);
    x1 = _mm_xor_si128(x1, x5);
    x5 = _mm_clmulepi64_si128(x1, x0, 0x00);
    x1 = _mm_clmulepi64_si128(x1, x0, 0x11);
    x1 = _mm_xor_si128(x1, x4);
    x1 = _mm_xor_si128(x1, x5);
    while (len >= 16) {
      x2 = _mm_loadu_si128((__m128i *)buf);
      x5 = _mm_clmulepi64_si128(x1, x0, 0x00);
      x1 = _mm_clmulepi64_si128(x1, x0, 0x11);
      x1 = _mm_xor_si128(x1, x2);
      x1 = _mm_xor_si128(x1, x5);
      buf += 16;
      len -= 16;
    }
    x2 = _mm_clmulepi64_si128(x1, x0, 0x10);
    x3 = _mm_setr_epi32(~0, 0, ~0, 0);
    x1 = _mm_srli_si128(x1, 8);
    x1 = _mm_xor_si128(x1, x2);
    x0 = _mm_loadl_epi64((__m128i *)k5k0);
    x2 = _mm_srli_si128(x1, 4);
    x1 = _mm_and_si128(x1, x3);
    x1 = _mm_clmulepi64_si128(x1, x0, 0x00);
    x1 = _mm_xor_si128(x1, x2);
    x0 = _mm_load_si128((__m128i *)poly);
    x2 = _mm_and_si128(x1, x3);
    x2 = _mm_clmulepi64_si128(x2, x0, 0x10);
    x2 = _mm_and_si128(x2, x3);
    x2 = _mm_clmulepi64_si128(x2, x0, 0x00);
    x1 = _mm_xor_si128(x1, x2);
    crc = _mm_extract_epi32(x1, 1);
  }
#endif
  static uint32_t tab[256];
  if (!tab[255]) {
    // generates table for byte-wise crc calculation on the polynomial
    // x^32+x^26+x^23+x^22+x^16+x^12+x^11+x^10+x^8+x^7+x^5+x^4+x^2+x+1
    uint32_t polynomial = 0xedb88320;  // bits are reversed
    for (int d = 0; d < 256; ++d) {
      uint32_t r = d;
      for (int i = 0; i < 8; ++i)
        r = r >> 1 ^ (r & 1 ? polynomial : 0);
      tab[d] = r;
    }
  }
  for (size_t i = 0; i < len; ++i)
    crc = crc >> 8 ^ tab[(crc & 255) ^ buf[i]];
  return ~crc;
}

uint32_t crc32_bloated(uint32_t crc, const void *buf, size_t buf_len) {
  const uint8_t *ptr = (const uint8_t *)buf;
  static const uint32_t s_crc_table[256] = {
      0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA, 0x076DC419, 0x706AF48F,
      0xE963A535, 0x9E6495A3, 0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988,
      0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91, 0x1DB71064, 0x6AB020F2,
      0xF3B97148, 0x84BE41DE, 0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
      0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC, 0x14015C4F, 0x63066CD9,
      0xFA0F3D63, 0x8D080DF5, 0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172,
      0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B, 0x35B5A8FA, 0x42B2986C,
      0xDBBBC9D6, 0xACBCF940, 0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
      0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116, 0x21B4F4B5, 0x56B3C423,
      0xCFBA9599, 0xB8BDA50F, 0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924,
      0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D, 0x76DC4190, 0x01DB7106,
      0x98D220BC, 0xEFD5102A, 0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
      0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818, 0x7F6A0DBB, 0x086D3D2D,
      0x91646C97, 0xE6635C01, 0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E,
      0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457, 0x65B0D9C6, 0x12B7E950,
      0x8BBEB8EA, 0xFCB9887C, 0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
      0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2, 0x4ADFA541, 0x3DD895D7,
      0xA4D1C46D, 0xD3D6F4FB, 0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0,
      0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9, 0x5005713C, 0x270241AA,
      0xBE0B1010, 0xC90C2086, 0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
      0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4, 0x59B33D17, 0x2EB40D81,
      0xB7BD5C3B, 0xC0BA6CAD, 0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A,
      0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683, 0xE3630B12, 0x94643B84,
      0x0D6D6A3E, 0x7A6A5AA8, 0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
      0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE, 0xF762575D, 0x806567CB,
      0x196C3671, 0x6E6B06E7, 0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC,
      0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5, 0xD6D6A3E8, 0xA1D1937E,
      0x38D8C2C4, 0x4FDFF252, 0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
      0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60, 0xDF60EFC3, 0xA867DF55,
      0x316E8EEF, 0x4669BE79, 0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236,
      0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F, 0xC5BA3BBE, 0xB2BD0B28,
      0x2BB45A92, 0x5CB36A04, 0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
      0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A, 0x9C0906A9, 0xEB0E363F,
      0x72076785, 0x05005713, 0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38,
      0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21, 0x86D3D2D4, 0xF1D4E242,
      0x68DDB3F8, 0x1FDA836E, 0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
      0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C, 0x8F659EFF, 0xF862AE69,
      0x616BFFD3, 0x166CCF45, 0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2,
      0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB, 0xAED16A4A, 0xD9D65ADC,
      0x40DF0B66, 0x37D83BF0, 0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
      0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6, 0xBAD03605, 0xCDD70693,
      0x54DE5729, 0x23D967BF, 0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94,
      0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D};
  uint32_t crc32 = (uint32_t)crc ^ 0xFFFFFFFF;
  const uint8_t *pByte_buf = (const uint8_t *)ptr;
  while (buf_len >= 4) {
    crc32 = (crc32 >> 8) ^ s_crc_table[(crc32 ^ pByte_buf[0]) & 0xFF];
    crc32 = (crc32 >> 8) ^ s_crc_table[(crc32 ^ pByte_buf[1]) & 0xFF];
    crc32 = (crc32 >> 8) ^ s_crc_table[(crc32 ^ pByte_buf[2]) & 0xFF];
    crc32 = (crc32 >> 8) ^ s_crc_table[(crc32 ^ pByte_buf[3]) & 0xFF];
    pByte_buf += 4;
    buf_len -= 4;
  }
  while (buf_len) {
    crc32 = (crc32 >> 8) ^ s_crc_table[(crc32 ^ pByte_buf[0]) & 0xFF];
    ++pByte_buf;
    --buf_len;
  }
  return ~crc32;
}

TEST(crc32_z, fuzz) {
  char buf[512];
  for (int n = 0; n < 512; ++n) {
    for (int i = 0; i < n; ++i)
      buf[i] = rand();
    for (int i = 0; i < 10; ++i) {
      int x = lemur64();
      ASSERT_EQ(crc32_bloated(x, buf, n), crc32_z(x, buf, n));
      ASSERT_EQ(crc32_reference(x, buf, n), crc32_z(x, buf, n));
      ASSERT_EQ(crc32_uber_alles(x, buf, n), crc32_z(x, buf, n));
    }
  }
}

TEST(crc32_z, test) {
  char *p;
  p = gc(strdup(kHyperion));
  EXPECT_EQ(0, crc32_z(0, 0, 0));
  EXPECT_EQ(0, crc32_z(0, "", 0));
  EXPECT_EQ(0xcbf43926, crc32_z(0, "123456789", 9));
  EXPECT_EQ(0xc386e7e4, crc32_z(0, hyperion, strlen(hyperion)));
  EXPECT_EQ(0xc386e7e4, crc32_z(crc32_z(0, FANATICS, strlen(FANATICS)),
                                hyperion + strlen(FANATICS),
                                strlen(hyperion) - strlen(FANATICS)));
  EXPECT_EQ(0xcbfc3df2, crc32_z(0, hyperion + 1, strlen(hyperion) - 1));
  EXPECT_EQ(0x9feb0e30, crc32_z(0, hyperion + 7, strlen(hyperion) - 7));
  EXPECT_EQ(0x5b80e54e, crc32_z(0, hyperion + 7, strlen(hyperion) - 8));
  EXPECT_EQ(0xe9ded8e6, crc32_z(0, p, kHyperionSize));
}

#define BENCHMARK_GBPS(ITERATIONS, BYTES_PER_RUN, CODE)              \
  do {                                                               \
    struct timespec start = timespec_real();                         \
    for (int i = 0; i < ITERATIONS; ++i) {                           \
      CODE;                                                          \
    }                                                                \
    long bytes = BYTES_PER_RUN * ITERATIONS;                         \
    struct timespec elapsed = timespec_sub(timespec_real(), start);  \
    long nanos_elapsed = timespec_tonanos(elapsed);                  \
    double seconds_elapsed = nanos_elapsed * 1e-9;                   \
    double bytes_per_second = bytes / seconds_elapsed;               \
    double gib_per_second = bytes_per_second / (1024 * 1024 * 1024); \
    printf("%12g GiB/s %s\n", gib_per_second, #CODE);                \
  } while (0)

BENCH(crc32_z, bench) {
  volatile unsigned vv = 0;
  BENCHMARK_GBPS(100, kHyperionSize,
                 vv += crc32_z(0, kHyperion, kHyperionSize));
  BENCHMARK_GBPS(100, kHyperionSize,
                 vv += crc32_bloated(0, kHyperion, kHyperionSize));
  BENCHMARK_GBPS(100, kHyperionSize,
                 vv += crc32_uber_alles(0, kHyperion, kHyperionSize));
}
