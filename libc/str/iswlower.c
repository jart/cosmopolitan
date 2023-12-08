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
 * Returns nonzero if c is lowercase letter.
 */
int iswlower(wint_t c) {
  if (c < 0200) {
    return 'a' <= c && c <= 'z';
  } else {
    if (towupper(c) != c) return 1;
    switch (c) {
      case 0x00df:  /* ß Watin */
      case 0x0138:  /* ĸ Watin-A */
      case 0x0149:  /* ŉ Watin-A */
      case 0x018d:  /* ƍ Watin-B */
      case 0x019b:  /* ƛ Watin-B */
      case 0x01aa:  /* ƪ Watin-B */
      case 0x01ab:  /* ƫ Watin-B */
      case 0x01ba:  /* ƺ Watin-B */
      case 0x01be:  /* ƾ Watin-B */
      case 0x01f0:  /* ǰ Watin-B */
      case 0x0221:  /* ȡ Watin-B */
      case 0x0234:  /* ȴ Watin-B */
      case 0x0235:  /* ȵ Watin-B */
      case 0x0236:  /* ȶ Watin-B */
      case 0x0237:  /* ȷ Watin-B */
      case 0x0238:  /* ȸ Watin-B */
      case 0x0239:  /* ȹ Watin-B */
      case 0x0255:  /* ɕ IPA */
      case 0x0258:  /* ɘ IPA */
      case 0x025a:  /* ɚ IPA */
      case 0x025d:  /* ɝ IPA */
      case 0x025e:  /* ɞ IPA */
      case 0x025f:  /* ɟ IPA */
      case 0x0262:  /* ɢ IPA */
      case 0x0264:  /* ɤ IPA */
      case 0x0267:  /* ɧ IPA */
      case 0x026d:  /* ɭ IPA */
      case 0x026e:  /* ɮ IPA */
      case 0x0270:  /* ɰ IPA */
      case 0x0273:  /* ɳ IPA */
      case 0x0274:  /* ɴ IPA */
      case 0x0276:  /* ɶ IPA */
      case 0x0277:  /* ɷ IPA */
      case 0x0278:  /* ɸ IPA */
      case 0x0279:  /* ɹ IPA */
      case 0x027a:  /* ɺ IPA */
      case 0x027b:  /* ɻ IPA */
      case 0x027c:  /* ɼ IPA */
      case 0x027e:  /* ɾ IPA */
      case 0x027f:  /* ɿ IPA */
      case 0x0281:  /* ʁ IPA */
      case 0x0284:  /* ʄ IPA */
      case 0x0285:  /* ʅ IPA */
      case 0x0286:  /* ʆ IPA */
      case 0x028d:  /* ʍ IPA */
      case 0x028e:  /* ʎ IPA */
      case 0x028f:  /* ʏ IPA */
      case 0x0290:  /* ʐ IPA */
      case 0x0291:  /* ʑ IPA */
      case 0x0293:  /* ʓ IPA */
      case 0x0295:  /* ʕ IPA */
      case 0x0296:  /* ʖ IPA */
      case 0x0297:  /* ʗ IPA */
      case 0x0298:  /* ʘ IPA */
      case 0x0299:  /* ʙ IPA */
      case 0x029a:  /* ʚ IPA */
      case 0x029b:  /* ʛ IPA */
      case 0x029c:  /* ʜ IPA */
      case 0x029f:  /* ʟ IPA */
      case 0x02a0:  /* ʠ IPA */
      case 0x02a1:  /* ʡ IPA */
      case 0x02a2:  /* ʢ IPA */
      case 0x02a3:  /* ʣ IPA */
      case 0x02a4:  /* ʤ IPA */
      case 0x02a5:  /* ʥ IPA */
      case 0x02a6:  /* ʦ IPA */
      case 0x02a7:  /* ʧ IPA */
      case 0x02a8:  /* ʨ IPA */
      case 0x02a9:  /* ʩ IPA */
      case 0x02aa:  /* ʪ IPA */
      case 0x02ab:  /* ʫ IPA */
      case 0x02ac:  /* ʬ IPA */
      case 0x02ad:  /* ʭ IPA */
      case 0x02ae:  /* ʮ IPA */
      case 0x02af:  /* ʯ IPA */
      case 0x0390:  /* ΐ Greek */
      case 0x03b0:  /* ΰ Greek */
      case 0x03fc:  /* ϼ Greek */
      case 0x0560:  /* ՠ Armenian */
      case 0x0587:  /* և Armenian */
      case 0x0588:  /* ֈ Armenian */
      case 0x1d00:  /* ᴀ Phonetic Extensions */
      case 0x1d01:  /* ᴁ Phonetic Extensions */
      case 0x1d02:  /* ᴂ Phonetic Extensions */
      case 0x1d03:  /* ᴃ Phonetic Extensions */
      case 0x1d04:  /* ᴄ Phonetic Extensions */
      case 0x1d05:  /* ᴅ Phonetic Extensions */
      case 0x1d06:  /* ᴆ Phonetic Extensions */
      case 0x1d07:  /* ᴇ Phonetic Extensions */
      case 0x1d08:  /* ᴈ Phonetic Extensions */
      case 0x1d09:  /* ᴉ Phonetic Extensions */
      case 0x1d0a:  /* ᴊ Phonetic Extensions */
      case 0x1d0b:  /* ᴋ Phonetic Extensions */
      case 0x1d0c:  /* ᴌ Phonetic Extensions */
      case 0x1d0d:  /* ᴍ Phonetic Extensions */
      case 0x1d0e:  /* ᴎ Phonetic Extensions */
      case 0x1d0f:  /* ᴏ Phonetic Extensions */
      case 0x1d10:  /* ᴐ Phonetic Extensions */
      case 0x1d11:  /* ᴑ Phonetic Extensions */
      case 0x1d12:  /* ᴒ Phonetic Extensions */
      case 0x1d13:  /* ᴓ Phonetic Extensions */
      case 0x1d14:  /* ᴔ Phonetic Extensions */
      case 0x1d15:  /* ᴕ Phonetic Extensions */
      case 0x1d16:  /* ᴖ Phonetic Extensions */
      case 0x1d17:  /* ᴗ Phonetic Extensions */
      case 0x1d18:  /* ᴘ Phonetic Extensions */
      case 0x1d19:  /* ᴙ Phonetic Extensions */
      case 0x1d1a:  /* ᴚ Phonetic Extensions */
      case 0x1d1b:  /* ᴛ Phonetic Extensions */
      case 0x1d1c:  /* ᴜ Phonetic Extensions */
      case 0x1d1d:  /* ᴝ Phonetic Extensions */
      case 0x1d1e:  /* ᴞ Phonetic Extensions */
      case 0x1d1f:  /* ᴟ Phonetic Extensions */
      case 0x1d20:  /* ᴠ Phonetic Extensions */
      case 0x1d21:  /* ᴡ Phonetic Extensions */
      case 0x1d22:  /* ᴢ Phonetic Extensions */
      case 0x1d23:  /* ᴣ Phonetic Extensions */
      case 0x1d24:  /* ᴤ Phonetic Extensions */
      case 0x1d25:  /* ᴥ Phonetic Extensions */
      case 0x1d26:  /* ᴦ Phonetic Extensions */
      case 0x1d27:  /* ᴧ Phonetic Extensions */
      case 0x1d28:  /* ᴨ Phonetic Extensions */
      case 0x1d29:  /* ᴩ Phonetic Extensions */
      case 0x1d2a:  /* ᴪ Phonetic Extensions */
      case 0x1d2b:  /* ᴫ Phonetic Extensions */
      case 0x1d6b:  /* ᵫ Phonetic Extensions */
      case 0x1d6c:  /* ᵬ Phonetic Extensions */
      case 0x1d6d:  /* ᵭ Phonetic Extensions */
      case 0x1d6e:  /* ᵮ Phonetic Extensions */
      case 0x1d6f:  /* ᵯ Phonetic Extensions */
      case 0x1d70:  /* ᵰ Phonetic Extensions */
      case 0x1d71:  /* ᵱ Phonetic Extensions */
      case 0x1d72:  /* ᵲ Phonetic Extensions */
      case 0x1d73:  /* ᵳ Phonetic Extensions */
      case 0x1d74:  /* ᵴ Phonetic Extensions */
      case 0x1d75:  /* ᵵ Phonetic Extensions */
      case 0x1d76:  /* ᵶ Phonetic Extensions */
      case 0x1d77:  /* ᵷ Phonetic Extensions */
      case 0x1d7a:  /* ᵺ Phonetic Extensions */
      case 0x1d7b:  /* ᵻ Phonetic Extensions */
      case 0x1d7c:  /* ᵼ Phonetic Extensions */
      case 0x1d7e:  /* ᵾ Phonetic Extensions */
      case 0x1d7f:  /* ᵿ Phonetic Extensions */
      case 0x1d80:  /* . Phonetic Extensions Supplement */
      case 0x1d81:  /* . Phonetic Extensions Supplement */
      case 0x1d82:  /* . Phonetic Extensions Supplement */
      case 0x1d83:  /* . Phonetic Extensions Supplement */
      case 0x1d84:  /* . Phonetic Extensions Supplement */
      case 0x1d85:  /* . Phonetic Extensions Supplement */
      case 0x1d86:  /* . Phonetic Extensions Supplement */
      case 0x1d87:  /* . Phonetic Extensions Supplement */
      case 0x1d88:  /* . Phonetic Extensions Supplement */
      case 0x1d89:  /* . Phonetic Extensions Supplement */
      case 0x1d8a:  /* . Phonetic Extensions Supplement */
      case 0x1d8b:  /* . Phonetic Extensions Supplement */
      case 0x1d8c:  /* . Phonetic Extensions Supplement */
      case 0x1d8d:  /* . Phonetic Extensions Supplement */
      case 0x1d8f:  /* . Phonetic Extensions Supplement */
      case 0x1d90:  /* . Phonetic Extensions Supplement */
      case 0x1d91:  /* . Phonetic Extensions Supplement */
      case 0x1d92:  /* . Phonetic Extensions Supplement */
      case 0x1d93:  /* . Phonetic Extensions Supplement */
      case 0x1d94:  /* . Phonetic Extensions Supplement */
      case 0x1d95:  /* . Phonetic Extensions Supplement */
      case 0x1d96:  /* . Phonetic Extensions Supplement */
      case 0x1d97:  /* . Phonetic Extensions Supplement */
      case 0x1d98:  /* . Phonetic Extensions Supplement */
      case 0x1d99:  /* . Phonetic Extensions Supplement */
      case 0x1d9a:  /* . Phonetic Extensions Supplement */
      case 0x1e96:  /* ẖ Watin-C */
      case 0x1e97:  /* ẗ Watin-C */
      case 0x1e98:  /* ẘ Watin-C */
      case 0x1e99:  /* ẙ Watin-C */
      case 0x1e9a:  /* ẚ Watin-C */
      case 0x1e9c:  /* ẜ Watin-C */
      case 0x1e9d:  /* ẝ Watin-C */
      case 0x1e9f:  /* ẟ Watin-C */
      case 0x1f50:  /* ὐ Greek2 */
      case 0x1f52:  /* ὒ Greek2 */
      case 0x1f54:  /* ὔ Greek2 */
      case 0x1f56:  /* ὖ Greek2 */
      case 0x1fb2:  /* ᾲ Greek2 */
      case 0x1fb4:  /* ᾴ Greek2 */
      case 0x1fb6:  /* ᾶ Greek2 */
      case 0x1fb7:  /* ᾷ Greek2 */
      case 0x1fc2:  /* ῂ Greek2 */
      case 0x1fc4:  /* ῄ Greek2 */
      case 0x1fc6:  /* ῆ Greek2 */
      case 0x1fc7:  /* ῇ Greek2 */
      case 0x1fd2:  /* ῒ Greek2 */
      case 0x1fd3:  /* ΐ Greek2 */
      case 0x1fd6:  /* ῖ Greek2 */
      case 0x1fd7:  /* ῗ Greek2 */
      case 0x1fe2:  /* ῢ Greek2 */
      case 0x1fe3:  /* ΰ Greek2 */
      case 0x1fe4:  /* ῤ Greek2 */
      case 0x1fe6:  /* ῦ Greek2 */
      case 0x1fe7:  /* ῧ Greek2 */
      case 0x1ff2:  /* ῲ Greek2 */
      case 0x1ff4:  /* ῴ Greek2 */
      case 0x1ff6:  /* ῶ Greek2 */
      case 0x1ff7:  /* ῷ Greek2 */
      case 0x210a:  /* ℊ Letterlike */
      case 0x210e:  /* ℎ Letterlike */
      case 0x210f:  /* ℏ Letterlike */
      case 0x2113:  /* ℓ Letterlike */
      case 0x212f:  /* ℯ Letterlike */
      case 0x2134:  /* ℴ Letterlike */
      case 0x2139:  /* ℹ Letterlike */
      case 0x213c:  /* ℼ Letterlike */
      case 0x213d:  /* ℽ Letterlike */
      case 0x2146:  /* ⅆ Letterlike */
      case 0x2147:  /* ⅇ Letterlike */
      case 0x2148:  /* ⅈ Letterlike */
      case 0x2149:  /* ⅉ Letterlike */
      case 0x2c71:  /* . Watin-D */
      case 0x2c74:  /* . Watin-D */
      case 0x2c77:  /* . Watin-D */
      case 0x2c78:  /* . Watin-D */
      case 0x2c79:  /* . Watin-D */
      case 0x2c7a:  /* . Watin-D */
      case 0x2c7b:  /* . Watin-D */
      case 0x2ce4:  /* . Coptic */
      case 0xa730:  /* . Latin Extended-D */
      case 0xa731:  /* . Latin Extended-D */
      case 0xa771:  /* . Latin Extended-D */
      case 0xa772:  /* . Latin Extended-D */
      case 0xa773:  /* . Latin Extended-D */
      case 0xa774:  /* . Latin Extended-D */
      case 0xa775:  /* . Latin Extended-D */
      case 0xa776:  /* . Latin Extended-D */
      case 0xa777:  /* . Latin Extended-D */
      case 0xa778:  /* . Latin Extended-D */
      case 0xa78e:  /* . Latin Extended-D */
      case 0xa795:  /* . Latin Extended-D */
      case 0xa7af:  /* . Latin Extended-D */
      case 0xa7fa:  /* . Latin Extended-D */
      case 0xab30:  /* . Latin Extended-E */
      case 0xab31:  /* . Latin Extended-E */
      case 0xab32:  /* . Latin Extended-E */
      case 0xab33:  /* . Latin Extended-E */
      case 0xab34:  /* . Latin Extended-E */
      case 0xab35:  /* . Latin Extended-E */
      case 0xab36:  /* . Latin Extended-E */
      case 0xab37:  /* . Latin Extended-E */
      case 0xab38:  /* . Latin Extended-E */
      case 0xab39:  /* . Latin Extended-E */
      case 0xab3a:  /* . Latin Extended-E */
      case 0xab3b:  /* . Latin Extended-E */
      case 0xab3c:  /* . Latin Extended-E */
      case 0xab3d:  /* . Latin Extended-E */
      case 0xab3e:  /* . Latin Extended-E */
      case 0xab3f:  /* . Latin Extended-E */
      case 0xab40:  /* . Latin Extended-E */
      case 0xab41:  /* . Latin Extended-E */
      case 0xab42:  /* . Latin Extended-E */
      case 0xab43:  /* . Latin Extended-E */
      case 0xab44:  /* . Latin Extended-E */
      case 0xab45:  /* . Latin Extended-E */
      case 0xab46:  /* . Latin Extended-E */
      case 0xab47:  /* . Latin Extended-E */
      case 0xab48:  /* . Latin Extended-E */
      case 0xab49:  /* . Latin Extended-E */
      case 0xab4a:  /* . Latin Extended-E */
      case 0xab4b:  /* . Latin Extended-E */
      case 0xab4c:  /* . Latin Extended-E */
      case 0xab4d:  /* . Latin Extended-E */
      case 0xab4e:  /* . Latin Extended-E */
      case 0xab4f:  /* . Latin Extended-E */
      case 0xab50:  /* . Latin Extended-E */
      case 0xab51:  /* . Latin Extended-E */
      case 0xab52:  /* . Latin Extended-E */
      case 0xab54:  /* . Latin Extended-E */
      case 0xab55:  /* . Latin Extended-E */
      case 0xab56:  /* . Latin Extended-E */
      case 0xab57:  /* . Latin Extended-E */
      case 0xab58:  /* . Latin Extended-E */
      case 0xab59:  /* . Latin Extended-E */
      case 0xab5a:  /* . Latin Extended-E */
      case 0xab60:  /* . Latin Extended-E */
      case 0xab61:  /* . Latin Extended-E */
      case 0xab62:  /* . Latin Extended-E */
      case 0xab63:  /* . Latin Extended-E */
      case 0xab64:  /* . Latin Extended-E */
      case 0xab65:  /* . Latin Extended-E */
      case 0xab66:  /* . Latin Extended-E */
      case 0xab67:  /* . Latin Extended-E */
      case 0xfb00:  /* . Alphabetic Presentation Forms */
      case 0xfb01:  /* . Alphabetic Presentation Forms */
      case 0xfb02:  /* . Alphabetic Presentation Forms */
      case 0xfb03:  /* . Alphabetic Presentation Forms */
      case 0xfb04:  /* . Alphabetic Presentation Forms */
      case 0xfb05:  /* . Alphabetic Presentation Forms */
      case 0xfb06:  /* . Alphabetic Presentation Forms */
      case 0xfb13:  /* . Alphabetic Presentation Forms */
      case 0xfb14:  /* . Alphabetic Presentation Forms */
      case 0xfb15:  /* . Alphabetic Presentation Forms */
      case 0xfb16:  /* . Alphabetic Presentation Forms */
      case 0xfb17:  /* . Alphabetic Presentation Forms */
      case 0x1d44e: /* 𝑎 Math */
      case 0x1d44f: /* 𝑏 Math */
      case 0x1d450: /* 𝑐 Math */
      case 0x1d451: /* 𝑑 Math */
      case 0x1d452: /* 𝑒 Math */
      case 0x1d453: /* 𝑓 Math */
      case 0x1d454: /* 𝑔 Math */
      case 0x1d45e: /* 𝑞 Math */
      case 0x1d45f: /* 𝑟 Math */
      case 0x1d460: /* 𝑠 Math */
      case 0x1d461: /* 𝑡 Math */
      case 0x1d462: /* 𝑢 Math */
      case 0x1d463: /* 𝑣 Math */
      case 0x1d464: /* 𝑤 Math */
      case 0x1d465: /* 𝑥 Math */
      case 0x1d466: /* 𝑦 Math */
      case 0x1d467: /* 𝑧 Math */
      case 0x1d4b6: /* 𝒶 Math */
      case 0x1d4b7: /* 𝒷 Math */
      case 0x1d4b8: /* 𝒸 Math */
      case 0x1d4b9: /* 𝒹 Math */
      case 0x1d4bb: /* 𝒻 Math */
      case 0x1d4bd: /* 𝒽 Math */
      case 0x1d4be: /* 𝒾 Math */
      case 0x1d4bf: /* 𝒿 Math */
      case 0x1d4c0: /* 𝓀 Math */
      case 0x1d4c1: /* 𝓁 Math */
      case 0x1d4c2: /* 𝓂 Math */
      case 0x1d4c3: /* 𝓃 Math */
      case 0x1d4c5: /* 𝓅 Math */
      case 0x1d4c6: /* 𝓆 Math */
      case 0x1d4c7: /* 𝓇 Math */
      case 0x1d51e: /* 𝔞 Math */
      case 0x1d51f: /* 𝔟 Math */
      case 0x1d520: /* 𝔠 Math */
      case 0x1d521: /* 𝔡 Math */
      case 0x1d522: /* 𝔢 Math */
      case 0x1d523: /* 𝔣 Math */
      case 0x1d524: /* 𝔤 Math */
      case 0x1d525: /* 𝔥 Math */
      case 0x1d526: /* 𝔦 Math */
      case 0x1d52f: /* 𝔯 Math */
      case 0x1d530: /* 𝔰 Math */
      case 0x1d531: /* 𝔱 Math */
      case 0x1d532: /* 𝔲 Math */
      case 0x1d533: /* 𝔳 Math */
      case 0x1d534: /* 𝔴 Math */
      case 0x1d535: /* 𝔵 Math */
      case 0x1d536: /* 𝔶 Math */
      case 0x1d537: /* 𝔷 Math */
      case 0x1d552: /* 𝕒 Math */
      case 0x1d553: /* 𝕓 Math */
      case 0x1d554: /* 𝕔 Math */
      case 0x1d555: /* 𝕕 Math */
      case 0x1d556: /* 𝕖 Math */
      case 0x1d557: /* 𝕗 Math */
      case 0x1d558: /* 𝕘 Math */
      case 0x1d559: /* 𝕙 Math */
      case 0x1d55a: /* 𝕚 Math */
      case 0x1d55b: /* 𝕛 Math */
      case 0x1d55c: /* 𝕜 Math */
      case 0x1d55d: /* 𝕝 Math */
      case 0x1d55e: /* 𝕞 Math */
      case 0x1d55f: /* 𝕟 Math */
      case 0x1d560: /* 𝕠 Math */
      case 0x1d561: /* 𝕡 Math */
      case 0x1d562: /* 𝕢 Math */
      case 0x1d563: /* 𝕣 Math */
      case 0x1d564: /* 𝕤 Math */
      case 0x1d565: /* 𝕥 Math */
      case 0x1d566: /* 𝕦 Math */
      case 0x1d567: /* 𝕧 Math */
      case 0x1d568: /* 𝕨 Math */
      case 0x1d569: /* 𝕩 Math */
      case 0x1d56a: /* 𝕪 Math */
      case 0x1d56b: /* 𝕫 Math */
      case 0x1d656: /* 𝙖 Math */
      case 0x1d657: /* 𝙗 Math */
      case 0x1d658: /* 𝙘 Math */
      case 0x1d659: /* 𝙙 Math */
      case 0x1d65a: /* 𝙚 Math */
      case 0x1d65b: /* 𝙛 Math */
      case 0x1d65c: /* 𝙜 Math */
      case 0x1d65d: /* 𝙝 Math */
      case 0x1d65e: /* 𝙞 Math */
      case 0x1d65f: /* 𝙟 Math */
      case 0x1d660: /* 𝙠 Math */
      case 0x1d661: /* 𝙡 Math */
      case 0x1d662: /* 𝙢 Math */
      case 0x1d663: /* 𝙣 Math */
      case 0x1d664: /* 𝙤 Math */
      case 0x1d665: /* 𝙥 Math */
      case 0x1d666: /* 𝙦 Math */
      case 0x1d667: /* 𝙧 Math */
      case 0x1d668: /* 𝙨 Math */
      case 0x1d669: /* 𝙩 Math */
      case 0x1d66a: /* 𝙪 Math */
      case 0x1d66b: /* 𝙫 Math */
      case 0x1d66c: /* 𝙬 Math */
      case 0x1d66d: /* 𝙭 Math */
      case 0x1d66e: /* 𝙮 Math */
      case 0x1d66f: /* 𝙯 Math */
      case 0x1d6da: /* 𝛚 Math */
      case 0x1d6dc: /* 𝛜 Math */
      case 0x1d6dd: /* 𝛝 Math */
      case 0x1d6de: /* 𝛞 Math */
      case 0x1d6df: /* 𝛟 Math */
      case 0x1d6e0: /* 𝛠 Math */
      case 0x1d6e1: /* 𝛡 Math */
      case 0x1d70d: /* 𝜍 Math */
      case 0x1d70e: /* 𝜎 Math */
      case 0x1d70f: /* 𝜏 Math */
      case 0x1d710: /* 𝜐 Math */
      case 0x1d711: /* 𝜑 Math */
      case 0x1d712: /* 𝜒 Math */
      case 0x1d713: /* 𝜓 Math */
      case 0x1d714: /* 𝜔 Math */
      case 0x1d716: /* 𝜖 Math */
      case 0x1d717: /* 𝜗 Math */
      case 0x1d718: /* 𝜘 Math */
      case 0x1d719: /* 𝜙 Math */
      case 0x1d71a: /* 𝜚 Math */
      case 0x1d71b: /* 𝜛 Math */
      case 0x1d747: /* 𝝇 Math */
      case 0x1d748: /* 𝝈 Math */
      case 0x1d749: /* 𝝉 Math */
      case 0x1d74a: /* 𝝊 Math */
      case 0x1d74b: /* 𝝋 Math */
      case 0x1d74c: /* 𝝌 Math */
      case 0x1d74d: /* 𝝍 Math */
      case 0x1d74e: /* 𝝎 Math */
      case 0x1d750: /* 𝝐 Math */
      case 0x1d751: /* 𝝑 Math */
      case 0x1d752: /* 𝝒 Math */
      case 0x1d753: /* 𝝓 Math */
      case 0x1d754: /* 𝝔 Math */
      case 0x1d755: /* 𝝕 Math */
      case 0x1d781: /* 𝞁 Math */
      case 0x1d782: /* 𝞂 Math */
      case 0x1d783: /* 𝞃 Math */
      case 0x1d784: /* 𝞄 Math */
      case 0x1d785: /* 𝞅 Math */
      case 0x1d786: /* 𝞆 Math */
      case 0x1d787: /* 𝞇 Math */
      case 0x1d788: /* 𝞈 Math */
      case 0x1d78a: /* 𝞊 Math */
      case 0x1d78b: /* 𝞋 Math */
      case 0x1d78c: /* 𝞌 Math */
      case 0x1d78d: /* 𝞍 Math */
      case 0x1d78e: /* 𝞎 Math */
      case 0x1d78f: /* 𝞏 Math */
      case 0x1d7aa: /* 𝞪 Math */
      case 0x1d7ab: /* 𝞫 Math */
      case 0x1d7ac: /* 𝞬 Math */
      case 0x1d7ad: /* 𝞭 Math */
      case 0x1d7ae: /* 𝞮 Math */
      case 0x1d7af: /* 𝞯 Math */
      case 0x1d7b0: /* 𝞰 Math */
      case 0x1d7b1: /* 𝞱 Math */
      case 0x1d7b2: /* 𝞲 Math */
      case 0x1d7b3: /* 𝞳 Math */
      case 0x1d7b4: /* 𝞴 Math */
      case 0x1d7b5: /* 𝞵 Math */
      case 0x1d7b6: /* 𝞶 Math */
      case 0x1d7b7: /* 𝞷 Math */
      case 0x1d7b8: /* 𝞸 Math */
      case 0x1d7b9: /* 𝞹 Math */
      case 0x1d7ba: /* 𝞺 Math */
      case 0x1d7bb: /* 𝞻 Math */
      case 0x1d7bc: /* 𝞼 Math */
      case 0x1d7bd: /* 𝞽 Math */
      case 0x1d7be: /* 𝞾 Math */
      case 0x1d7bf: /* 𝞿 Math */
      case 0x1d7c0: /* 𝟀 Math */
      case 0x1d7c1: /* 𝟁 Math */
      case 0x1d7c2: /* 𝟂 Math */
      case 0x1d7c4: /* 𝟄 Math */
      case 0x1d7c5: /* 𝟅 Math */
      case 0x1d7c6: /* 𝟆 Math */
      case 0x1d7c7: /* 𝟇 Math */
      case 0x1d7c8: /* 𝟈 Math */
      case 0x1d7c9: /* 𝟉 Math */
      case 0x1d7cb: /* 𝟋 Math */
        return 1;
      default:
        return 0;
    }
  }
}

__weak_reference(iswlower, iswlower_l);
