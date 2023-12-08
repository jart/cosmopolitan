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
// clang-format off

static const struct {
  unsigned short x;
  unsigned short y;
  short d;
} kUpper[] = {
    {0x00b5, 0x00b5, +743},   /*  1x µ ..µ  → Μ ..Μ  Watin */
    {0x00e0, 0x00f6, -32},    /* 23x à ..ö  → À ..Ö  Watin */
    {0x00f8, 0x00fe, -32},    /*  7x ø ..þ  → Ø ..Þ  Watin */
    {0x00ff, 0x00ff, +121},   /*  1x ÿ ..ÿ  → Ÿ ..Ÿ  Watin */
    {0x017a, 0x017a, -1},     /*  1x ź ..ź  → Ź ..Ź  Watin-A */
    {0x017c, 0x017c, -1},     /*  1x ż ..ż  → Ż ..Ż  Watin-A */
    {0x017e, 0x017e, -1},     /*  1x ž ..ž  → Ž ..Ž  Watin-A */
    {0x017f, 0x017f, -300},   /*  1x ſ ..ſ  → S ..S  Watin-A */
    {0x0180, 0x0180, +195},   /*  1x ƀ ..ƀ  → Ƀ ..Ƀ  Watin-B */
    {0x0183, 0x0183, -1},     /*  1x ƃ ..ƃ  → Ƃ ..Ƃ  Watin-B */
    {0x0185, 0x0185, -1},     /*  1x ƅ ..ƅ  → Ƅ ..Ƅ  Watin-B */
    {0x0188, 0x0188, -1},     /*  1x ƈ ..ƈ  → Ƈ ..Ƈ  Watin-B */
    {0x018c, 0x018c, -1},     /*  1x ƌ ..ƌ  → Ƌ ..Ƌ  Watin-B */
    {0x0192, 0x0192, -1},     /*  1x ƒ ..ƒ  → Ƒ ..Ƒ  Watin-B */
    {0x0195, 0x0195, +97},    /*  1x ƕ ..ƕ  → Ƕ ..Ƕ  Watin-B */
    {0x0199, 0x0199, -1},     /*  1x ƙ ..ƙ  → Ƙ ..Ƙ  Watin-B */
    {0x019a, 0x019a, +163},   /*  1x ƚ ..ƚ  → Ƚ ..Ƚ  Watin-B */
    {0x019e, 0x019e, +130},   /*  1x ƞ ..ƞ  → Ƞ ..Ƞ  Watin-B */
    {0x01a1, 0x01a1, -1},     /*  1x ơ ..ơ  → Ơ ..Ơ  Watin-B */
    {0x01a3, 0x01a3, -1},     /*  1x ƣ ..ƣ  → Ƣ ..Ƣ  Watin-B */
    {0x01a5, 0x01a5, -1},     /*  1x ƥ ..ƥ  → Ƥ ..Ƥ  Watin-B */
    {0x01a8, 0x01a8, -1},     /*  1x ƨ ..ƨ  → Ƨ ..Ƨ  Watin-B */
    {0x01ad, 0x01ad, -1},     /*  1x ƭ ..ƭ  → Ƭ ..Ƭ  Watin-B */
    {0x01b0, 0x01b0, -1},     /*  1x ư ..ư  → Ư ..Ư  Watin-B */
    {0x01b4, 0x01b4, -1},     /*  1x ƴ ..ƴ  → Ƴ ..Ƴ  Watin-B */
    {0x01b6, 0x01b6, -1},     /*  1x ƶ ..ƶ  → Ƶ ..Ƶ  Watin-B */
    {0x01b9, 0x01b9, -1},     /*  1x ƹ ..ƹ  → Ƹ ..Ƹ  Watin-B */
    {0x01bd, 0x01bd, -1},     /*  1x ƽ ..ƽ  → Ƽ ..Ƽ  Watin-B */
    {0x01bf, 0x01bf, +56},    /*  1x ƿ ..ƿ  → Ƿ ..Ƿ  Watin-B */
    {0x01c5, 0x01c5, -1},     /*  1x ǅ ..ǅ  → Ǆ ..Ǆ  Watin-B */
    {0x01c6, 0x01c6, -2},     /*  1x ǆ ..ǆ  → Ǆ ..Ǆ  Watin-B */
    {0x01c8, 0x01c8, -1},     /*  1x ǈ ..ǈ  → Ǉ ..Ǉ  Watin-B */
    {0x01c9, 0x01c9, -2},     /*  1x ǉ ..ǉ  → Ǉ ..Ǉ  Watin-B */
    {0x01cb, 0x01cb, -1},     /*  1x ǋ ..ǋ  → Ǌ ..Ǌ  Watin-B */
    {0x01cc, 0x01cc, -2},     /*  1x ǌ ..ǌ  → Ǌ ..Ǌ  Watin-B */
    {0x01ce, 0x01ce, -1},     /*  1x ǎ ..ǎ  → Ǎ ..Ǎ  Watin-B */
    {0x01dd, 0x01dd, -79},    /*  1x ǝ ..ǝ  → Ǝ ..Ǝ  Watin-B */
    {0x01f2, 0x01f2, -1},     /*  1x ǲ ..ǲ  → Ǳ ..Ǳ  Watin-B */
    {0x01f3, 0x01f3, -2},     /*  1x ǳ ..ǳ  → Ǳ ..Ǳ  Watin-B */
    {0x01f5, 0x01f5, -1},     /*  1x ǵ ..ǵ  → Ǵ ..Ǵ  Watin-B */
    {0x023c, 0x023c, -1},     /*  1x ȼ ..ȼ  → Ȼ ..Ȼ  Watin-B */
    {0x023f, 0x0240, +10815}, /*  2x ȿ ..ɀ  → Ȿ ..Ɀ  Watin-B */
    {0x0242, 0x0242, -1},     /*  1x ɂ ..ɂ  → Ɂ ..Ɂ  Watin-B */
    {0x0247, 0x0247, -1},     /*  1x ɇ ..ɇ  → Ɇ ..Ɇ  Watin-B */
    {0x0249, 0x0249, -1},     /*  1x ɉ ..ɉ  → Ɉ ..Ɉ  Watin-B */
    {0x024b, 0x024b, -1},     /*  1x ɋ ..ɋ  → Ɋ ..Ɋ  Watin-B */
    {0x024d, 0x024d, -1},     /*  1x ɍ ..ɍ  → Ɍ ..Ɍ  Watin-B */
    {0x024f, 0x024f, -1},     /*  1x ɏ ..ɏ  → Ɏ ..Ɏ  Watin-B */
    {0x037b, 0x037d, +130},   /*  3x ͻ ..ͽ  → Ͻ ..Ͽ  Greek */
    {0x03ac, 0x03ac, -38},    /*  1x ά ..ά  → Ά ..Ά  Greek */
    {0x03ad, 0x03af, -37},    /*  3x έ ..ί  → Έ ..Ί  Greek */
    {0x03b1, 0x03c1, -32},    /* 17x α ..ρ  → Α ..Ρ  Greek */
    {0x03c2, 0x03c2, -31},    /*  1x ς ..ς  → Σ ..Σ  Greek */
    {0x03c3, 0x03cb, -32},    /*  9x σ ..ϋ  → Σ ..Ϋ  Greek */
    {0x03cc, 0x03cc, -64},    /*  1x ό ..ό  → Ό ..Ό  Greek */
    {0x03cd, 0x03ce, -63},    /*  2x ύ ..ώ  → Ύ ..Ώ  Greek */
    {0x03d0, 0x03d0, -62},    /*  1x ϐ ..ϐ  → Β ..Β  Greek */
    {0x03d1, 0x03d1, -57},    /*  1x ϑ ..ϑ  → Θ ..Θ  Greek */
    {0x03d5, 0x03d5, -47},    /*  1x ϕ ..ϕ  → Φ ..Φ  Greek */
    {0x03d6, 0x03d6, -54},    /*  1x ϖ ..ϖ  → Π ..Π  Greek */
    {0x03dd, 0x03dd, -1},     /*  1x ϝ ..ϝ  → Ϝ ..Ϝ  Greek */
    {0x03f0, 0x03f0, -86},    /*  1x ϰ ..ϰ  → Κ ..Κ  Greek */
    {0x03f1, 0x03f1, -80},    /*  1x ϱ ..ϱ  → Ρ ..Ρ  Greek */
    {0x03f5, 0x03f5, -96},    /*  1x ϵ ..ϵ  → Ε ..Ε  Greek */
    {0x0430, 0x044f, -32},    /* 32x а ..я  → А ..Я  Cyrillic */
    {0x0450, 0x045f, -80},    /* 16x ѐ ..џ  → Ѐ ..Џ  Cyrillic */
    {0x0461, 0x0461, -1},     /*  1x ѡ ..ѡ  → Ѡ ..Ѡ  Cyrillic */
    {0x0463, 0x0463, -1},     /*  1x ѣ ..ѣ  → Ѣ ..Ѣ  Cyrillic */
    {0x0465, 0x0465, -1},     /*  1x ѥ ..ѥ  → Ѥ ..Ѥ  Cyrillic */
    {0x0473, 0x0473, -1},     /*  1x ѳ ..ѳ  → Ѳ ..Ѳ  Cyrillic */
    {0x0491, 0x0491, -1},     /*  1x ґ ..ґ  → Ґ ..Ґ  Cyrillic */
    {0x0499, 0x0499, -1},     /*  1x ҙ ..ҙ  → Ҙ ..Ҙ  Cyrillic */
    {0x049b, 0x049b, -1},     /*  1x қ ..қ  → Қ ..Қ  Cyrillic */
    {0x0561, 0x0586, -48},    /* 38x ա ..ֆ  → Ա ..Ֆ  Armenian */
    {0x10d0, 0x10fa, +3008},  /* 43x ა ..ჺ  → Ა ..Ჺ  Georgian */
    {0x10fd, 0x10ff, +3008},  /*  3x ჽ ..ჿ  → Ჽ ..Ჿ  Georgian */
    {0x13f8, 0x13fd, -8},     /*  6x ᏸ ..ᏽ  → Ᏸ ..Ᏽ  Cherokee */
    {0x214e, 0x214e, -28},    /*  1x ⅎ ..ⅎ  → Ⅎ ..Ⅎ  Letterlike */
    {0x2170, 0x217f, -16},    /* 16x ⅰ ..ⅿ  → Ⅰ ..Ⅿ  Numbery */
    {0x2184, 0x2184, -1},     /*  1x ↄ ..ↄ  → Ↄ ..Ↄ  Numbery */
    {0x24d0, 0x24e9, -26},    /* 26x ⓐ ..ⓩ  → Ⓐ ..Ⓩ  Enclosed */
    {0x2c30, 0x2c5e, -48},    /* 47x ⰰ ..ⱞ  → Ⰰ ..Ⱞ  Glagolitic */
    {0x2d00, 0x2d25, -7264},  /* 38x ⴀ ..ⴥ  → Ⴀ ..Ⴥ  Georgian2 */
    {0x2d27, 0x2d27, -7264},  /*  1x ⴧ ..ⴧ  → Ⴧ ..Ⴧ  Georgian2 */
    {0x2d2d, 0x2d2d, -7264},  /*  1x ⴭ ..ⴭ  → Ⴭ ..Ⴭ  Georgian2 */
    {0xff41, 0xff5a, -32},    /* 26x ａ..ｚ → Ａ..Ｚ Dubs */
};

static const int kAstralUpper[][3] = {
    {0x10428, 0x1044f, -40},  /* 40x 𐐨..𐑏 → 𐐀..𐐧 Deseret */
    {0x104d8, 0x104fb, -40},  /* 36x 𐓘..𐓻 → 𐒰..𐓓 Osage */
    {0x1d41a, 0x1d433, -26},  /* 26x 𝐚..𝐳 → 𝐀..𝐙 Math */
    {0x1d456, 0x1d467, -26},  /* 18x 𝑖..𝑧 → 𝐼..𝑍 Math */
    {0x1d482, 0x1d49b, -26},  /* 26x 𝒂..𝒛 → 𝑨..𝒁 Math */
    {0x1d4c8, 0x1d4cf, -26},  /*  8x 𝓈..𝓏 → 𝒮..𝒵 Math */
    {0x1d4ea, 0x1d503, -26},  /* 26x 𝓪..𝔃 → 𝓐..𝓩 Math */
    {0x1d527, 0x1d52e, -26},  /*  8x 𝔧..𝔮 → 𝔍..𝔔 Math */
    {0x1d586, 0x1d59f, -26},  /* 26x 𝖆..𝖟 → 𝕬..𝖅 Math */
    {0x1d5ba, 0x1d5d3, -26},  /* 26x 𝖺..𝗓 → 𝖠..𝖹 Math */
    {0x1d5ee, 0x1d607, -26},  /* 26x 𝗮..𝘇 → 𝗔..𝗭 Math */
    {0x1d622, 0x1d63b, -26},  /* 26x 𝘢..𝘻 → 𝘈..𝘡 Math */
    {0x1d68a, 0x1d6a3, +442}, /* 26x 𝒂..𝒛 → 𝘼..𝙕 Math */
    {0x1d6c2, 0x1d6d2, -26},  /* 26x 𝚊..𝚣 → 𝙰..𝚉 Math */
    {0x1d6fc, 0x1d70c, -26},  /* 17x 𝛂..𝛒 → 𝚨..𝚸 Math */
    {0x1d736, 0x1d746, -26},  /* 17x 𝛼..𝜌 → 𝛢..𝛲 Math */
    {0x1d770, 0x1d780, -26},  /* 17x 𝜶..𝝆 → 𝜜..𝜬 Math */
    {0x1d770, 0x1d756, -26},  /* 17x 𝝰..𝞀 → 𝝖..𝝦 Math */
    {0x1d736, 0x1d790, -90},  /* 17x 𝜶..𝝆 → 𝞐..𝞠 Math */
};

/**
 * Converts wide character to upper case.
 */
wint_t towupper(wint_t c) {
  int m, l, r, n;
  if (c < 0200) {
    if ('a' <= c && c <= 'z') {
      return c - 32;
    } else {
      return c;
    }
  } else if (c <= 0xffff) {
    if ((0x0101 <= c && c <= 0x0177) || /* 60x ā..ŵ → Ā..ā Watin-A */
        (0x01df <= c && c <= 0x01ef) || /*  9x ǟ..ǯ → Ǟ..Ǯ Watin-B */
        (0x01f8 <= c && c <= 0x021e) || /* 20x ǹ..ȟ → Ǹ..Ȟ Watin-B */
        (0x0222 <= c && c <= 0x0232) || /*  9x ȣ..ȳ → Ȣ..Ȳ Watin-B */
        (0x1e01 <= c && c <= 0x1eff)) { /*256x ḁ..ỿ → Ḁ..Ỿ Watin-C */
      if (c == 0x0131) return c + 232;
      if (c == 0x1e9e) return c;
      return c - (c & 1);
    } else if (0x01d0 <= c && c <= 0x01dc) {
      return c - (~c & 1); /* 7x ǐ..ǜ → Ǐ..Ǜ Watin-B */
    } else if (0xab70 <= c && c <= 0xabbf) {
      return c - 38864; /* 80x ꭰ ..ꮿ  → Ꭰ ..Ꮿ  Cherokee Supplement */
    } else {
      l = 0;
      r = n = sizeof(kUpper) / sizeof(kUpper[0]);
      while (l < r) {
        m = (l & r) + ((l ^ r) >> 1);  // floor((a+b)/2)
        if (kUpper[m].y < c) {
          l = m + 1;
        } else {
          r = m;
        }
      }
      if (l < n && kUpper[l].x <= c && c <= kUpper[l].y) {
        return c + kUpper[l].d;
      } else {
        return c;
      }
    }
  } else {
    l = 0;
    r = n = sizeof(kAstralUpper) / sizeof(kAstralUpper[0]);
    while (l < r) {
      m = (l & r) + ((l ^ r) >> 1);  // floor((a+b)/2)
      if (kAstralUpper[m][1] < c) {
        l = m + 1;
      } else {
        r = m;
      }
    }
    if (l < n && kAstralUpper[l][0] <= c && c <= kAstralUpper[l][1]) {
      return c + kAstralUpper[l][2];
    } else {
      return c;
    }
  }
}

__weak_reference(towupper, towupper_l);
