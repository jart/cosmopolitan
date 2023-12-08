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
#include "libc/dce.h"
#include "libc/macros.internal.h"
#include "libc/str/str.h"
/* clang-format off */

static const struct {
  unsigned short x;
  unsigned short y;
  short d;
} kLower[] = {
    {0x00c0, 0x00d6, +32},    /* 23x À ..Ö  → à ..ö  Watin */
    {0x00d8, 0x00de, +32},    /*  7x Ø ..Þ  → ø ..þ  Watin */
    {0x0178, 0x0178, -121},   /*  1x Ÿ ..Ÿ  → ÿ ..ÿ  Watin-A */
    {0x0179, 0x0179, +1},     /*  1x Ź ..Ź  → ź ..ź  Watin-A */
    {0x017b, 0x017b, +1},     /*  1x Ż ..Ż  → ż ..ż  Watin-A */
    {0x017d, 0x017d, +1},     /*  1x Ž ..Ž  → ž ..ž  Watin-A */
    {0x0181, 0x0181, +210},   /*  1x Ɓ ..Ɓ  → ɓ ..ɓ  Watin-B */
    {0x0182, 0x0182, +1},     /*  1x Ƃ ..Ƃ  → ƃ ..ƃ  Watin-B */
    {0x0184, 0x0184, +1},     /*  1x Ƅ ..Ƅ  → ƅ ..ƅ  Watin-B */
    {0x0186, 0x0186, +206},   /*  1x Ɔ ..Ɔ  → ɔ ..ɔ  Watin-B */
    {0x0187, 0x0187, +1},     /*  1x Ƈ ..Ƈ  → ƈ ..ƈ  Watin-B */
    {0x0189, 0x018a, +205},   /*  2x Ɖ ..Ɗ  → ɖ ..ɗ  Watin-B */
    {0x018b, 0x018b, +1},     /*  1x Ƌ ..Ƌ  → ƌ ..ƌ  Watin-B */
    {0x018e, 0x018e, +79},    /*  1x Ǝ ..Ǝ  → ǝ ..ǝ  Watin-B */
    {0x018f, 0x018f, +202},   /*  1x Ə ..Ə  → ə ..ə  Watin-B */
    {0x0190, 0x0190, +203},   /*  1x Ɛ ..Ɛ  → ɛ ..ɛ  Watin-B */
    {0x0191, 0x0191, +1},     /*  1x Ƒ ..Ƒ  → ƒ ..ƒ  Watin-B */
    {0x0193, 0x0193, +205},   /*  1x Ɠ ..Ɠ  → ɠ ..ɠ  Watin-B */
    {0x0194, 0x0194, +207},   /*  1x Ɣ ..Ɣ  → ɣ ..ɣ  Watin-B */
    {0x0196, 0x0196, +211},   /*  1x Ɩ ..Ɩ  → ɩ ..ɩ  Watin-B */
    {0x0197, 0x0197, +209},   /*  1x Ɨ ..Ɨ  → ɨ ..ɨ  Watin-B */
    {0x0198, 0x0198, +1},     /*  1x Ƙ ..Ƙ  → ƙ ..ƙ  Watin-B */
    {0x019c, 0x019c, +211},   /*  1x Ɯ ..Ɯ  → ɯ ..ɯ  Watin-B */
    {0x019d, 0x019d, +213},   /*  1x Ɲ ..Ɲ  → ɲ ..ɲ  Watin-B */
    {0x019f, 0x019f, +214},   /*  1x Ɵ ..Ɵ  → ɵ ..ɵ  Watin-B */
    {0x01a0, 0x01a0, +1},     /*  1x Ơ ..Ơ  → ơ ..ơ  Watin-B */
    {0x01a2, 0x01a2, +1},     /*  1x Ƣ ..Ƣ  → ƣ ..ƣ  Watin-B */
    {0x01a4, 0x01a4, +1},     /*  1x Ƥ ..Ƥ  → ƥ ..ƥ  Watin-B */
    {0x01a6, 0x01a6, +218},   /*  1x Ʀ ..Ʀ  → ʀ ..ʀ  Watin-B */
    {0x01a7, 0x01a7, +1},     /*  1x Ƨ ..Ƨ  → ƨ ..ƨ  Watin-B */
    {0x01a9, 0x01a9, +218},   /*  1x Ʃ ..Ʃ  → ʃ ..ʃ  Watin-B */
    {0x01ac, 0x01ac, +1},     /*  1x Ƭ ..Ƭ  → ƭ ..ƭ  Watin-B */
    {0x01ae, 0x01ae, +218},   /*  1x Ʈ ..Ʈ  → ʈ ..ʈ  Watin-B */
    {0x01af, 0x01af, +1},     /*  1x Ư ..Ư  → ư ..ư  Watin-B */
    {0x01b1, 0x01b2, +217},   /*  2x Ʊ ..Ʋ  → ʊ ..ʋ  Watin-B */
    {0x01b3, 0x01b3, +1},     /*  1x Ƴ ..Ƴ  → ƴ ..ƴ  Watin-B */
    {0x01b5, 0x01b5, +1},     /*  1x Ƶ ..Ƶ  → ƶ ..ƶ  Watin-B */
    {0x01b7, 0x01b7, +219},   /*  1x Ʒ ..Ʒ  → ʒ ..ʒ  Watin-B */
    {0x01b8, 0x01b8, +1},     /*  1x Ƹ ..Ƹ  → ƹ ..ƹ  Watin-B */
    {0x01bc, 0x01bc, +1},     /*  1x Ƽ ..Ƽ  → ƽ ..ƽ  Watin-B */
    {0x01c4, 0x01c4, +2},     /*  1x Ǆ ..Ǆ  → ǆ ..ǆ  Watin-B */
    {0x01c5, 0x01c5, +1},     /*  1x ǅ ..ǅ  → ǆ ..ǆ  Watin-B */
    {0x01c7, 0x01c7, +2},     /*  1x Ǉ ..Ǉ  → ǉ ..ǉ  Watin-B */
    {0x01c8, 0x01c8, +1},     /*  1x ǈ ..ǈ  → ǉ ..ǉ  Watin-B */
    {0x01ca, 0x01ca, +2},     /*  1x Ǌ ..Ǌ  → ǌ ..ǌ  Watin-B */
    {0x01cb, 0x01cb, +1},     /*  1x ǋ ..ǋ  → ǌ ..ǌ  Watin-B */
    {0x01cd, 0x01cd, +1},     /*  1x Ǎ ..Ǎ  → ǎ ..ǎ  Watin-B */
    {0x01f1, 0x01f1, +2},     /*  1x Ǳ ..Ǳ  → ǳ ..ǳ  Watin-B */
    {0x01f2, 0x01f2, +1},     /*  1x ǲ ..ǲ  → ǳ ..ǳ  Watin-B */
    {0x01f4, 0x01f4, +1},     /*  1x Ǵ ..Ǵ  → ǵ ..ǵ  Watin-B */
    {0x01f6, 0x01f6, -97},    /*  1x Ƕ ..Ƕ  → ƕ ..ƕ  Watin-B */
    {0x01f7, 0x01f7, -56},    /*  1x Ƿ ..Ƿ  → ƿ ..ƿ  Watin-B */
    {0x0220, 0x0220, -130},   /*  1x Ƞ ..Ƞ  → ƞ ..ƞ  Watin-B */
    {0x023b, 0x023b, +1},     /*  1x Ȼ ..Ȼ  → ȼ ..ȼ  Watin-B */
    {0x023d, 0x023d, -163},   /*  1x Ƚ ..Ƚ  → ƚ ..ƚ  Watin-B */
    {0x0241, 0x0241, +1},     /*  1x Ɂ ..Ɂ  → ɂ ..ɂ  Watin-B */
    {0x0243, 0x0243, -195},   /*  1x Ƀ ..Ƀ  → ƀ ..ƀ  Watin-B */
    {0x0244, 0x0244, +69},    /*  1x Ʉ ..Ʉ  → ʉ ..ʉ  Watin-B */
    {0x0245, 0x0245, +71},    /*  1x Ʌ ..Ʌ  → ʌ ..ʌ  Watin-B */
    {0x0246, 0x0246, +1},     /*  1x Ɇ ..Ɇ  → ɇ ..ɇ  Watin-B */
    {0x0248, 0x0248, +1},     /*  1x Ɉ ..Ɉ  → ɉ ..ɉ  Watin-B */
    {0x024a, 0x024a, +1},     /*  1x Ɋ ..Ɋ  → ɋ ..ɋ  Watin-B */
    {0x024c, 0x024c, +1},     /*  1x Ɍ ..Ɍ  → ɍ ..ɍ  Watin-B */
    {0x024e, 0x024e, +1},     /*  1x Ɏ ..Ɏ  → ɏ ..ɏ  Watin-B */
    {0x0386, 0x0386, +38},    /*  1x Ά ..Ά  → ά ..ά  Greek */
    {0x0388, 0x038a, +37},    /*  3x Έ ..Ί  → έ ..ί  Greek */
    {0x038c, 0x038c, +64},    /*  1x Ό ..Ό  → ό ..ό  Greek */
    {0x038e, 0x038f, +63},    /*  2x Ύ ..Ώ  → ύ ..ώ  Greek */
    {0x0391, 0x03a1, +32},    /* 17x Α ..Ρ  → α ..ρ  Greek */
    {0x03a3, 0x03ab, +32},    /*  9x Σ ..Ϋ  → σ ..ϋ  Greek */
    {0x03dc, 0x03dc, +1},     /*  1x Ϝ ..Ϝ  → ϝ ..ϝ  Greek */
    {0x03f4, 0x03f4, -60},    /*  1x ϴ ..ϴ  → θ ..θ  Greek */
    {0x0400, 0x040f, +80},    /* 16x Ѐ ..Џ  → ѐ ..џ  Cyrillic */
    {0x0410, 0x042f, +32},    /* 32x А ..Я  → а ..я  Cyrillic */
    {0x0460, 0x0460, +1},     /*  1x Ѡ ..Ѡ  → ѡ ..ѡ  Cyrillic */
    {0x0462, 0x0462, +1},     /*  1x Ѣ ..Ѣ  → ѣ ..ѣ  Cyrillic */
    {0x0464, 0x0464, +1},     /*  1x Ѥ ..Ѥ  → ѥ ..ѥ  Cyrillic */
    {0x0472, 0x0472, +1},     /*  1x Ѳ ..Ѳ  → ѳ ..ѳ  Cyrillic */
    {0x0490, 0x0490, +1},     /*  1x Ґ ..Ґ  → ґ ..ґ  Cyrillic */
    {0x0498, 0x0498, +1},     /*  1x Ҙ ..Ҙ  → ҙ ..ҙ  Cyrillic */
    {0x049a, 0x049a, +1},     /*  1x Қ ..Қ  → қ ..қ  Cyrillic */
    {0x0531, 0x0556, +48},    /* 38x Ա ..Ֆ  → ա ..ֆ  Armenian */
    {0x10a0, 0x10c5, +7264},  /* 38x Ⴀ ..Ⴥ  → ⴀ ..ⴥ  Georgian */
    {0x10c7, 0x10c7, +7264},  /*  1x Ⴧ ..Ⴧ  → ⴧ ..ⴧ  Georgian */
    {0x10cd, 0x10cd, +7264},  /*  1x Ⴭ ..Ⴭ  → ⴭ ..ⴭ  Georgian */
    {0x13f0, 0x13f5, +8},     /*  6x Ᏸ ..Ᏽ  → ᏸ ..ᏽ  Cherokee */
    {0x1c90, 0x1cba, -3008}, /* 43x Ა ..Ჺ  → ა ..ჺ  Georgian2 */
    {0x1cbd, 0x1cbf, -3008}, /*  3x Ჽ ..Ჿ  → ჽ ..ჿ  Georgian2 */
    {0x1f08, 0x1f0f, -8},    /*  8x Ἀ ..Ἇ  → ἀ ..ἇ  Greek2 */
    {0x1f18, 0x1f1d, -8},    /*  6x Ἐ ..Ἕ  → ἐ ..ἕ  Greek2 */
    {0x1f28, 0x1f2f, -8},    /*  8x Ἠ ..Ἧ  → ἠ ..ἧ  Greek2 */
    {0x1f38, 0x1f3f, -8},    /*  8x Ἰ ..Ἷ  → ἰ ..ἷ  Greek2 */
    {0x1f48, 0x1f4d, -8},    /*  6x Ὀ ..Ὅ  → ὀ ..ὅ  Greek2 */
    {0x1f59, 0x1f59, -8},    /*  1x Ὑ ..Ὑ  → ὑ ..ὑ  Greek2 */
    {0x1f5b, 0x1f5b, -8},    /*  1x Ὓ ..Ὓ  → ὓ ..ὓ  Greek2 */
    {0x1f5d, 0x1f5d, -8},    /*  1x Ὕ ..Ὕ  → ὕ ..ὕ  Greek2 */
    {0x1f5f, 0x1f5f, -8},    /*  1x Ὗ ..Ὗ  → ὗ ..ὗ  Greek2 */
    {0x1f68, 0x1f6f, -8},    /*  8x Ὠ ..Ὧ  → ὠ ..ὧ  Greek2 */
    {0x1f88, 0x1f8f, -8},    /*  8x ᾈ ..ᾏ  → ᾀ ..ᾇ  Greek2 */
    {0x1f98, 0x1f9f, -8},    /*  8x ᾘ ..ᾟ  → ᾐ ..ᾗ  Greek2 */
    {0x1fa8, 0x1faf, -8},    /*  8x ᾨ ..ᾯ  → ᾠ ..ᾧ  Greek2 */
    {0x1fb8, 0x1fb9, -8},    /*  2x Ᾰ ..Ᾱ  → ᾰ ..ᾱ  Greek2 */
    {0x1fba, 0x1fbb, -74},   /*  2x Ὰ ..Ά  → ὰ ..ά  Greek2 */
    {0x1fbc, 0x1fbc, -9},    /*  1x ᾼ ..ᾼ  → ᾳ ..ᾳ  Greek2 */
    {0x1fc8, 0x1fcb, -86},   /*  4x Ὲ ..Ή  → ὲ ..ή  Greek2 */
    {0x1fcc, 0x1fcc, -9},    /*  1x ῌ ..ῌ  → ῃ ..ῃ  Greek2 */
    {0x1fd8, 0x1fd9, -8},    /*  2x Ῐ ..Ῑ  → ῐ ..ῑ  Greek2 */
    {0x1fda, 0x1fdb, -100},  /*  2x Ὶ ..Ί  → ὶ ..ί  Greek2 */
    {0x1fe8, 0x1fe9, -8},    /*  2x Ῠ ..Ῡ  → ῠ ..ῡ  Greek2 */
    {0x1fea, 0x1feb, -112},  /*  2x Ὺ ..Ύ  → ὺ ..ύ  Greek2 */
    {0x1fec, 0x1fec, -7},    /*  1x Ῥ ..Ῥ  → ῥ ..ῥ  Greek2 */
    {0x1ff8, 0x1ff9, -128},  /*  2x Ὸ ..Ό  → ὸ ..ό  Greek2 */
    {0x1ffa, 0x1ffb, -126},  /*  2x Ὼ ..Ώ  → ὼ ..ώ  Greek2 */
    {0x1ffc, 0x1ffc, -9},    /*  1x ῼ ..ῼ  → ῳ ..ῳ  Greek2 */
    {0x2126, 0x2126, -7517}, /*  1x Ω ..Ω  → ω ..ω  Letterlike */
    {0x212a, 0x212a, -8383}, /*  1x K ..K  → k ..k  Letterlike */
    {0x212b, 0x212b, -8262}, /*  1x Å ..Å  → å ..å  Letterlike */
    {0x2132, 0x2132, +28}, /*  1x Ⅎ ..Ⅎ  → ⅎ ..ⅎ  Letterlike */
    {0x2160, 0x216f, +16}, /* 16x Ⅰ ..Ⅿ  → ⅰ ..ⅿ  Numbery */
    {0x2183, 0x2183, +1},  /*  1x Ↄ ..Ↄ  → ↄ ..ↄ  Numbery */
    {0x24b6, 0x24cf, +26}, /* 26x Ⓐ ..Ⓩ  → ⓐ ..ⓩ  Enclosed */
    {0x2c00, 0x2c2e, +48}, /* 47x Ⰰ ..Ⱞ  → ⰰ ..ⱞ  Glagolitic */
    {0xff21, 0xff3a, +32}, /* 26x Ａ..Ｚ → ａ..ｚ Dubs */
};

static const int kAstralLower[][3] = {
    {0x10400, 0x10427, +40}, /* 40x 𐐀 ..𐐧  → 𐐨 ..𐑏  Deseret */
    {0x104b0, 0x104d3, +40}, /* 36x 𐒰 ..𐓓  → 𐓘 ..𐓻  Osage */
    {0x1d400, 0x1d419, +26}, /* 26x 𝐀 ..𝐙  → 𝐚 ..𝐳  Math */
    {0x1d43c, 0x1d44d, +26}, /* 18x 𝐼 ..𝑍  → 𝑖 ..𝑧  Math */
    {0x1d468, 0x1d481, +26}, /* 26x 𝑨 ..𝒁  → 𝒂 ..𝒛  Math */
    {0x1d4ae, 0x1d4b5, +26}, /*  8x 𝒮 ..𝒵  → 𝓈 ..𝓏  Math */
    {0x1d4d0, 0x1d4e9, +26}, /* 26x 𝓐 ..𝓩  → 𝓪 ..𝔃  Math */
    {0x1d50d, 0x1d514, +26}, /*  8x 𝔍 ..𝔔  → 𝔧 ..𝔮  Math */
    {0x1d56c, 0x1d585, +26}, /* 26x 𝕬 ..𝖅  → 𝖆 ..𝖟  Math */
    {0x1d5a0, 0x1d5b9, +26}, /* 26x 𝖠 ..𝖹  → 𝖺 ..𝗓  Math */
    {0x1d5d4, 0x1d5ed, +26}, /* 26x 𝗔 ..𝗭  → 𝗮 ..𝘇  Math */
    {0x1d608, 0x1d621, +26}, /* 26x 𝘈 ..𝘡  → 𝘢 ..𝘻  Math */
    {0x1d63c, 0x1d655, -442}, /* 26x 𝘼 ..𝙕  → 𝒂 ..𝒛  Math */
    {0x1d670, 0x1d689, +26},  /* 26x 𝙰 ..𝚉  → 𝚊 ..𝚣  Math */
    {0x1d6a8, 0x1d6b8, +26},  /* 17x 𝚨 ..𝚸  → 𝛂 ..𝛒  Math */
    {0x1d6e2, 0x1d6f2, +26},  /* 17x 𝛢 ..𝛲  → 𝛼 ..𝜌  Math */
    {0x1d71c, 0x1d72c, +26},  /* 17x 𝜜 ..𝜬  → 𝜶 ..𝝆  Math */
    {0x1d756, 0x1d766, +26},  /* 17x 𝝖 ..𝝦  → 𝝰 ..𝞀  Math */
    {0x1d790, 0x1d7a0, -90},  /* 17x 𝞐 ..𝞠  → 𝜶 ..𝝆  Math */
};

/**
 * Converts wide character to lower case.
 */
wint_t towlower(wint_t c) {
  int m, l, r, n;
  if (c < 0200) {
    if ('A' <= c && c <= 'Z') {
      return c + 32;
    } else {
      return c;
    }
  } else if (c <= 0xffff) {
    if ((0x0100 <= c && c <= 0x0176) || /* 60x Ā..ā → ā..ŵ Watin-A */
        (0x01de <= c && c <= 0x01ee) || /*  9x Ǟ..Ǯ → ǟ..ǯ Watin-B */
        (0x01f8 <= c && c <= 0x021e) || /* 20x Ǹ..Ȟ → ǹ..ȟ Watin-B */
        (0x0222 <= c && c <= 0x0232) || /*  9x Ȣ..Ȳ → ȣ..ȳ Watin-B */
        (0x1e00 <= c && c <= 0x1eff)) { /*256x Ḁ..Ỿ → ḁ..ỿ Watin-C */
      if (c == 0x0130) return c - 199;
      if (c == 0x1e9e) return c;
      return c + (~c & 1);
    } else if (0x01cf <= c && c <= 0x01db) {
      return c + (c & 1); /* 7x Ǐ..Ǜ → ǐ..ǜ Watin-B */
    } else if (0x13a0 <= c && c <= 0x13ef) {
      return c + 38864; /* 80x Ꭰ ..Ꮿ  → ꭰ ..ꮿ  Cherokee */
    } else {
      l = 0;
      r = n = sizeof(kLower) / sizeof(kLower[0]);
      while (l < r) {
        m = (l & r) + ((l ^ r) >> 1);  // floor((a+b)/2)
        if (kLower[m].y < c) {
          l = m + 1;
        } else {
          r = m;
        }
      }
      if (l < n && kLower[l].x <= c && c <= kLower[l].y) {
        return c + kLower[l].d;
      } else {
        return c;
      }
    }
  } else {
    l = 0;
    r = n = sizeof(kAstralLower) / sizeof(kAstralLower[0]);
    while (l < r) {
      m = (l & r) + ((l ^ r) >> 1);  // floor((a+b)/2)
      if (kAstralLower[m][1] < c) {
        l = m + 1;
      } else {
        r = m;
      }
    }
    if (l < n && kAstralLower[l][0] <= c && c <= kAstralLower[l][1]) {
      return c + kAstralLower[l][2];
    } else {
      return c;
    }
  }
}

__weak_reference(towlower, towlower_l);
