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
#include "libc/str/str.h"

/**
 * Returns nonzero if c is uppercase letter.
 */
int iswupper(wint_t c) {
  if (c < 0200) {
    return 'A' <= c && c <= 'Z';
  } else {
    if (towlower(c) != c) return 1;
    switch (c) {
      case 0x03d2:  /* ϒ Greek */
      case 0x03d3:  /* ϓ Greek */
      case 0x03d4:  /* ϔ Greek */
      case 0x2102:  /* ℂ Letterlike */
      case 0x2107:  /* ℇ Letterlike */
      case 0x210b:  /* ℋ Letterlike */
      case 0x210c:  /* ℌ Letterlike */
      case 0x210d:  /* ℍ Letterlike */
      case 0x2110:  /* ℐ Letterlike */
      case 0x2111:  /* ℑ Letterlike */
      case 0x2112:  /* ℒ Letterlike */
      case 0x2115:  /* ℕ Letterlike */
      case 0x2119:  /* ℙ Letterlike */
      case 0x211a:  /* ℚ Letterlike */
      case 0x211b:  /* ℛ Letterlike */
      case 0x211c:  /* ℜ Letterlike */
      case 0x211d:  /* ℝ Letterlike */
      case 0x2124:  /* ℤ Letterlike */
      case 0x2128:  /* ℨ Letterlike */
      case 0x212c:  /* ℬ Letterlike */
      case 0x212d:  /* ℭ Letterlike */
      case 0x2130:  /* ℰ Letterlike */
      case 0x2131:  /* ℱ Letterlike */
      case 0x2133:  /* ℳ Letterlike */
      case 0x213e:  /* ℾ Letterlike */
      case 0x213f:  /* ℿ Letterlike */
      case 0x2145:  /* ⅅ Letterlike */
      case 0x1d434: /* 𝐴 Math */
      case 0x1d435: /* 𝐵 Math */
      case 0x1d436: /* 𝐶 Math */
      case 0x1d437: /* 𝐷 Math */
      case 0x1d438: /* 𝐸 Math */
      case 0x1d439: /* 𝐹 Math */
      case 0x1d43a: /* 𝐺 Math */
      case 0x1d43b: /* 𝐻 Math */
      case 0x1d49c: /* 𝒜 Math */
      case 0x1d49e: /* 𝒞 Math */
      case 0x1d49f: /* 𝒟 Math */
      case 0x1d4a2: /* 𝒢 Math */
      case 0x1d4a5: /* 𝒥 Math */
      case 0x1d4a6: /* 𝒦 Math */
      case 0x1d4a9: /* 𝒩 Math */
      case 0x1d4aa: /* 𝒪 Math */
      case 0x1d4ab: /* 𝒫 Math */
      case 0x1d4ac: /* 𝒬 Math */
      case 0x1d504: /* 𝔄 Math */
      case 0x1d505: /* 𝔅 Math */
      case 0x1d507: /* 𝔇 Math */
      case 0x1d508: /* 𝔈 Math */
      case 0x1d509: /* 𝔉 Math */
      case 0x1d50a: /* 𝔊 Math */
      case 0x1d516: /* 𝔖 Math */
      case 0x1d517: /* 𝔗 Math */
      case 0x1d518: /* 𝔘 Math */
      case 0x1d519: /* 𝔙 Math */
      case 0x1d51a: /* 𝔚 Math */
      case 0x1d51b: /* 𝔛 Math */
      case 0x1d51c: /* 𝔜 Math */
      case 0x1d538: /* 𝔸 Math */
      case 0x1d539: /* 𝔹 Math */
      case 0x1d53b: /* 𝔻 Math */
      case 0x1d53c: /* 𝔼 Math */
      case 0x1d53d: /* 𝔽 Math */
      case 0x1d53e: /* 𝔾 Math */
      case 0x1d540: /* 𝕀 Math */
      case 0x1d541: /* 𝕁 Math */
      case 0x1d542: /* 𝕂 Math */
      case 0x1d543: /* 𝕃 Math */
      case 0x1d544: /* 𝕄 Math */
      case 0x1d546: /* 𝕆 Math */
      case 0x1d54a: /* 𝕊 Math */
      case 0x1d54b: /* 𝕋 Math */
      case 0x1d54c: /* 𝕌 Math */
      case 0x1d54d: /* 𝕍 Math */
      case 0x1d54e: /* 𝕎 Math */
      case 0x1d54f: /* 𝕏 Math */
      case 0x1d550: /* 𝕐 Math */
      case 0x1d6e3: /* 𝛣 Math */
      case 0x1d6e4: /* 𝛤 Math */
      case 0x1d6e5: /* 𝛥 Math */
      case 0x1d6e6: /* 𝛦 Math */
      case 0x1d6e7: /* 𝛧 Math */
      case 0x1d6e8: /* 𝛨 Math */
      case 0x1d6e9: /* 𝛩 Math */
      case 0x1d6ea: /* 𝛪 Math */
      case 0x1d6eb: /* 𝛫 Math */
      case 0x1d6ec: /* 𝛬 Math */
      case 0x1d6ed: /* 𝛭 Math */
      case 0x1d6ee: /* 𝛮 Math */
      case 0x1d6ef: /* 𝛯 Math */
      case 0x1d6f0: /* 𝛰 Math */
      case 0x1d6f1: /* 𝛱 Math */
      case 0x1d6f2: /* 𝛲 Math */
      case 0x1d6f3: /* 𝛳 Math */
      case 0x1d6f4: /* 𝛴 Math */
      case 0x1d6f5: /* 𝛵 Math */
      case 0x1d6f6: /* 𝛶 Math */
      case 0x1d6f7: /* 𝛷 Math */
      case 0x1d6f8: /* 𝛸 Math */
      case 0x1d6f9: /* 𝛹 Math */
      case 0x1d6fa: /* 𝛺 Math */
      case 0x1d72d: /* 𝜭 Math */
      case 0x1d72e: /* 𝜮 Math */
      case 0x1d72f: /* 𝜯 Math */
      case 0x1d730: /* 𝜰 Math */
      case 0x1d731: /* 𝜱 Math */
      case 0x1d732: /* 𝜲 Math */
      case 0x1d733: /* 𝜳 Math */
      case 0x1d734: /* 𝜴 Math */
      case 0x1d767: /* 𝝧 Math */
      case 0x1d768: /* 𝝨 Math */
      case 0x1d769: /* 𝝩 Math */
      case 0x1d76a: /* 𝝪 Math */
      case 0x1d76b: /* 𝝫 Math */
      case 0x1d76c: /* 𝝬 Math */
      case 0x1d76d: /* 𝝭 Math */
      case 0x1d76e: /* 𝝮 Math */
      case 0x1d7a1: /* 𝞡 Math */
      case 0x1d7a2: /* 𝞢 Math */
      case 0x1d7a3: /* 𝞣 Math */
      case 0x1d7a4: /* 𝞤 Math */
      case 0x1d7a5: /* 𝞥 Math */
      case 0x1d7a6: /* 𝞦 Math */
      case 0x1d7a7: /* 𝞧 Math */
      case 0x1d7a8: /* 𝞨 Math */
      case 0x1d7ca: /* 𝟊 Math */
        return 1;
      default:
        return 0;
    }
  }
}

__weak_reference(iswupper, iswupper_l);
