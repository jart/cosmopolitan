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
#include "libc/bits/bits.h"
#include "libc/intrin/repmovsb.h"
#include "libc/macros.h"
#include "libc/nexgen32e/kompressor.h"
#include "libc/str/str.h"
#include "libc/str/undeflate.h"

#define kDeflateCompressionTypeNone           0b00
#define kDeflateCompressionTypeFixedHuffman   0b01
#define kDeflateCompressionTypeDynamicHuffman 0b10

#define kDeflateCodeLengthCopyPrevious3To6Times  16
#define kDeflateCodeLengthRepeatZero3To10Times   17
#define kDeflateCodeLengthRepeatZero11To138Times 18

#define CONSUME(BITS) \
  hold.word >>= BITS; \
  hold.bits -= BITS

#define MOAR(BITS)                        \
  while (hold.bits < BITS) {              \
    al = *ip++;                           \
    hold.word |= (size_t)al << hold.bits; \
    hold.bits += 8;                       \
  }

struct DeflateHold {
  size_t word;
  size_t bits;
};

static const struct DeflateConsts {
  uint16_t lenbase[32];
  uint16_t distbase[32];
  uint8_t lenbits[32];
  uint8_t distbits[32];
  uint8_t orders[19];
  struct RlDecode lensrl[6];
} kDeflate = {
    {3,  4,  5,  6,  7,  8,  9,  10, 11,  13,  15,  17,  19,  23, 27,
     31, 35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258},
    {1,    2,    3,    4,    5,    7,    9,    13,    17,    25,
     33,   49,   65,   97,   129,  193,  257,  385,   513,   769,
     1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577},
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2,
     2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5},
    {0, 0, 0, 0, 1, 1, 2, 2,  3,  3,  4,  4,  5,  5,  6,
     6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13},
    {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15},
    {{144, 8}, {112, 9}, {24, 7}, {8, 8}, {32, 5}, {0, 0}},
};

static uint32_t undeflatetree(struct DeflateState *ds, uint32_t *tree,
                              const uint8_t *lens, size_t symcount) {
  size_t i, len;
  uint32_t code, slot;
  uint16_t codes[16], first[16], counts[16];
  memset(counts, 0, sizeof(counts));
  for (i = 0; i < symcount; i++) {
    counts[lens[i]]++;
  }
  codes[0] = 0;
  first[0] = 0;
  counts[0] = 0;
  for (i = 1; i < ARRAYLEN(codes); i++) {
    codes[i] = (codes[i - 1] + counts[i - 1]) << 1;
    first[i] = first[i - 1] + counts[i - 1];
  }
  assert(first[15] + counts[15] <= symcount);
  for (i = 0; i < symcount; i++) {
    if ((len = lens[i])) {
      code = codes[len]++;
      slot = first[len]++;
      tree[slot] = code << (32 - len) | i << 4 | len;
    }
  }
  return first[15];
}

static struct DeflateHold undeflatesymbol(struct DeflateHold hold,
                                          const uint32_t *tree,
                                          size_t treecount,
                                          uint32_t *out_symbol) {
  size_t left, right, m;
  uint32_t search, key;
  left = 0;
  right = treecount;
  search = bitreverse16(hold.word);
  search <<= 16;
  search |= 0xffff;
  while (left < right) { /* TODO(jart): Make this O(1) like Zlib. */
    m = (left + right) >> 1;
    if (search < tree[m]) {
      right = m;
    } else {
      left = m + 1;
    }
  }
  key = tree[left - 1];
  assert(!((search ^ key) >> (32 - (key & 0xf))));
  *out_symbol = (key >> 4) & 0xfff;
  CONSUME(key & 0xf);
  return hold;
}

/**
 * Decompresses raw DEFLATE data.
 *
 * This is 10x smaller and 10x slower than chromium zlib.
 *
 * @param output should be followed by a single guard page, and have
 *     36kb of guard pages preceding it too
 * @note h/t Phil Katz, David Huffman, Claude Shannon
 */
ssize_t undeflate(void *output, size_t outputsize, void *input,
                  size_t inputsize, struct DeflateState *ds) {
  struct DeflateHold hold;
  bool isfinalblock;
  size_t i, nlit, ndist;
  uint8_t *ip, *op, *si, b, al, last, compressiontype;
  uint32_t j, l, len, sym, tlit, tdist, tlen, nlen;

  op = output;
  ip = input;
  hold.word = 0;
  hold.bits = 0;
  isfinalblock = 0;

  while (!isfinalblock) {
    MOAR(3);
    isfinalblock = hold.word & 0b1;
    CONSUME(1);
    compressiontype = hold.word & 0b11;
    CONSUME(2);

    switch (compressiontype) {
      case kDeflateCompressionTypeNone:
        CONSUME(hold.bits & 7);
        MOAR(32);
        len = hold.word & 0xffff;
        nlen = (hold.word >> 16) & 0xffff;
        assert(len == ~nlen);
        CONSUME(32);
        while (len--) {
          if (hold.bits) {
            *op++ = hold.word;
            CONSUME(8);
          } else {
            *op++ = *ip++;
          }
        }
        continue;

      case kDeflateCompressionTypeFixedHuffman:
        nlit = 288;
        ndist = 32;
        rldecode(ds->lens, kDeflate.lensrl);
        break;

      case kDeflateCompressionTypeDynamicHuffman:
        MOAR(5 + 5 + 4);
        nlit = (hold.word & 0b11111) + 257;
        CONSUME(5);
        ndist = (hold.word & 0b11111) + 1;
        CONSUME(5);
        nlen = (hold.word & 0b1111) + 4;
        CONSUME(4);
        for (i = 0; i < nlen; i++) {
          MOAR(3);
          ds->lenlens[kDeflate.orders[i]] = hold.word & 0b111;
          CONSUME(3);
        }
        for (; i < ARRAYLEN(ds->lenlens); i++) {
          ds->lenlens[kDeflate.orders[i]] = 0;
        }
        tlen =
            undeflatetree(ds, ds->lencodes, ds->lenlens, ARRAYLEN(ds->lenlens));
        i = 0;
        last = 0;
        while (i < nlit + ndist) {
          MOAR(16);
          hold = undeflatesymbol(hold, ds->lencodes, tlen, &sym);
          b = 2;
          j = 1;
          switch (sym) {
            case kDeflateCodeLengthRepeatZero11To138Times:
              b += 4;
              j += 8;
              /* fallthrough */
            case kDeflateCodeLengthRepeatZero3To10Times:
              b += 1;
              last = 0;
              /* fallthrough */
            case kDeflateCodeLengthCopyPrevious3To6Times:
              MOAR(b);
              j += (hold.word & ((1u << b) - 1u)) + 2u;
              CONSUME(b);
              break;
            default:
              last = sym;
              break;
          }
          while (j--) {
            ds->lens[i++] = last;
          }
        }
        break;
      default:
        return -1;
    }

    tlit = undeflatetree(ds, ds->litcodes, ds->lens, nlit);
    tdist = undeflatetree(ds, ds->distcodes, &ds->lens[nlit], ndist);

    do {
      MOAR(16);
      hold = undeflatesymbol(hold, ds->litcodes, tlit, &sym);
      if (sym < 256) {
        *op++ = sym;
      } else if (sym > 256) {
        sym -= 257;
        b = kDeflate.lenbits[sym];
        MOAR(b);
        l = kDeflate.lenbase[sym] + (hold.word & ((1u << b) - 1));
        CONSUME(b);
        MOAR(16);
        hold = undeflatesymbol(hold, ds->distcodes, tdist, &sym);
        b = kDeflate.distbits[sym];
        MOAR(b);
        /* max readback: 24577 + 2**13-1 = 32768 */
        si = op - ((uint32_t)kDeflate.distbase[sym] +
                   (uint32_t)(hold.word & ((1u << b) - 1)));
        CONSUME(b);
        repmovsb(&op, &si, l);
      }
    } while (sym != 256);
  }

  return 0;
}
