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
#include "libc/str/str.h"

static const unsigned short kCodes[][2] = {
    {0x00aa, 0x00aa}, /*     1x English */
    {0x00b2, 0x00b3}, /*     2x English Arabic */
    {0x00b5, 0x00b5}, /*     1x Greek */
    {0x00b9, 0x00ba}, /*     2x English Arabic */
    {0x00bc, 0x00be}, /*     3x Vulgar English Arabic */
    {0x00c0, 0x00d6}, /*    23x Watin */
    {0x00d8, 0x00f6}, /*    31x Watin */
    {0x0100, 0x02c1}, /*   450x Watin-AB,IPA,Spacemod */
    {0x02c6, 0x02d1}, /*    12x Spacemod */
    {0x02e0, 0x02e4}, /*     5x Spacemod */
    {0x02ec, 0x02ec}, /*     1x Spacemod */
    {0x02ee, 0x02ee}, /*     1x Spacemod */
    {0x0370, 0x0374}, /*     5x Greek */
    {0x0376, 0x0377}, /*     2x Greek */
    {0x037a, 0x037d}, /*     4x Greek */
    {0x037f, 0x037f}, /*     1x Greek */
    {0x0386, 0x0386}, /*     1x Greek */
    {0x0388, 0x038a}, /*     3x Greek */
    {0x038c, 0x038c}, /*     1x Greek */
    {0x038e, 0x03a1}, /*    20x Greek */
    {0x03a3, 0x03f5}, /*    83x Greek */
    {0x03f7, 0x0481}, /*   139x Greek */
    {0x048a, 0x052f}, /*   166x Cyrillic */
    {0x0531, 0x0556}, /*    38x Armenian */
    {0x0560, 0x0588}, /*    41x Armenian */
    {0x05d0, 0x05ea}, /*    27x Hebrew */
    {0x0620, 0x064a}, /*    43x Arabic */
    {0x0660, 0x0669}, /*    10x Arabic */
    {0x0671, 0x06d3}, /*    99x Arabic */
    {0x06ee, 0x06fc}, /*    15x Arabic */
    {0x0712, 0x072f}, /*    30x Syriac */
    {0x074d, 0x07a5}, /*    89x Syriac,Arabic2,Thaana */
    {0x07c0, 0x07ea}, /*    43x NKo */
    {0x0800, 0x0815}, /*    22x Samaritan */
    {0x0840, 0x0858}, /*    25x Mandaic */
    {0x0904, 0x0939}, /*    54x Devanagari */
    {0x0993, 0x09a8}, /*    22x Bengali */
    {0x09e6, 0x09f1}, /*    12x Bengali */
    {0x0a13, 0x0a28}, /*    22x Gurmukhi */
    {0x0a66, 0x0a6f}, /*    10x Gurmukhi */
    {0x0a93, 0x0aa8}, /*    22x Gujarati */
    {0x0b13, 0x0b28}, /*    22x Oriya */
    {0x0c92, 0x0ca8}, /*    23x Kannada */
    {0x0caa, 0x0cb3}, /*    10x Kannada */
    {0x0ce6, 0x0cef}, /*    10x Kannada */
    {0x0d12, 0x0d3a}, /*    41x Malayalam */
    {0x0d85, 0x0d96}, /*    18x Sinhala */
    {0x0d9a, 0x0db1}, /*    24x Sinhala */
    {0x0de6, 0x0def}, /*    10x Sinhala */
    {0x0e01, 0x0e30}, /*    48x Thai */
    {0x0e8c, 0x0ea3}, /*    24x Lao */
    {0x0f20, 0x0f33}, /*    20x Tibetan */
    {0x0f49, 0x0f6c}, /*    36x Tibetan */
    {0x109e, 0x10c5}, /*    40x Myanmar,Georgian */
    {0x10d0, 0x10fa}, /*    43x Georgian */
    {0x10fc, 0x1248}, /*   333x Georgian,Hangul,Ethiopic */
    {0x13a0, 0x13f5}, /*    86x Cherokee */
    {0x1401, 0x166d}, /*   621x Aboriginal */
    {0x16a0, 0x16ea}, /*    75x Runic */
    {0x1700, 0x170c}, /*    13x Tagalog */
    {0x1780, 0x17b3}, /*    52x Khmer */
    {0x1820, 0x1878}, /*    89x Mongolian */
    {0x1a00, 0x1a16}, /*    23x Buginese */
    {0x1a20, 0x1a54}, /*    53x Tai Tham */
    {0x1a80, 0x1a89}, /*    10x Tai Tham */
    {0x1a90, 0x1a99}, /*    10x Tai Tham */
    {0x1b05, 0x1b33}, /*    47x Balinese */
    {0x1b50, 0x1b59}, /*    10x Balinese */
    {0x1b83, 0x1ba0}, /*    30x Sundanese */
    {0x1bae, 0x1be5}, /*    56x Sundanese */
    {0x1c90, 0x1cba}, /*    43x Georgian2 */
    {0x1cbd, 0x1cbf}, /*     3x Georgian2 */
    {0x1e00, 0x1f15}, /*   278x Watin-C,Greek2 */
    {0x2070, 0x2071}, /*     2x Supersub */
    {0x2074, 0x2079}, /*     6x Supersub */
    {0x207f, 0x2089}, /*    11x Supersub */
    {0x2090, 0x209c}, /*    13x Supersub */
    {0x2100, 0x2117}, /*    24x Letterlike */
    {0x2119, 0x213f}, /*    39x Letterlike */
    {0x2145, 0x214a}, /*     6x Letterlike */
    {0x214c, 0x218b}, /*    64x Letterlike,Numbery */
    {0x21af, 0x21cd}, /*    31x Arrows */
    {0x21d5, 0x21f3}, /*    31x Arrows */
    {0x230c, 0x231f}, /*    20x Technical */
    {0x232b, 0x237b}, /*    81x Technical */
    {0x237d, 0x239a}, /*    30x Technical */
    {0x23b4, 0x23db}, /*    40x Technical */
    {0x23e2, 0x2426}, /*    69x Technical,ControlPictures */
    {0x2460, 0x25b6}, /*   343x Enclosed,Boxes,Blocks,Shapes */
    {0x25c2, 0x25f7}, /*    54x Shapes */
    {0x2600, 0x266e}, /*   111x Symbols */
    {0x2670, 0x2767}, /*   248x Symbols,Dingbats */
    {0x2776, 0x27bf}, /*    74x Dingbats */
    {0x2800, 0x28ff}, /*   256x Braille */
    {0x2c00, 0x2c2e}, /*    47x Glagolitic */
    {0x2c30, 0x2c5e}, /*    47x Glagolitic */
    {0x2c60, 0x2ce4}, /*   133x Watin-D */
    {0x2d00, 0x2d25}, /*    38x Georgian2 */
    {0x2d30, 0x2d67}, /*    56x Tifinagh */
    {0x2d80, 0x2d96}, /*    23x Ethiopic2 */
    {0x2e2f, 0x2e2f}, /*     1x Punctuation2 */
    {0x3005, 0x3007}, /*     3x CJK Symbols & Punctuation */
    {0x3021, 0x3029}, /*     9x CJK Symbols & Punctuation */
    {0x3031, 0x3035}, /*     5x CJK Symbols & Punctuation */
    {0x3038, 0x303c}, /*     5x CJK Symbols & Punctuation */
    {0x3041, 0x3096}, /*    86x Hiragana */
    {0x30a1, 0x30fa}, /*    90x Katakana */
    {0x3105, 0x312f}, /*    43x Bopomofo */
    {0x3131, 0x318e}, /*    94x Hangul Compatibility Jamo */
    {0x31a0, 0x31ba}, /*    27x Bopomofo Extended */
    {0x31f0, 0x31ff}, /*    16x Katakana Phonetic Extensions */
    {0x3220, 0x3229}, /*    10x Enclosed CJK Letters & Months */
    {0x3248, 0x324f}, /*     8x Enclosed CJK Letters & Months */
    {0x3251, 0x325f}, /*    15x Enclosed CJK Letters & Months */
    {0x3280, 0x3289}, /*    10x Enclosed CJK Letters & Months */
    {0x32b1, 0x32bf}, /*    15x Enclosed CJK Letters & Months */
    {0x3400, 0x4db5}, /*  6582x CJK Unified Ideographs Extension A */
    {0x4dc0, 0x9fef}, /* 21040x Yijing Hexagram, CJK Unified Ideographs */
    {0xa000, 0xa48c}, /*  1165x Yi Syllables */
    {0xa4d0, 0xa4fd}, /*    46x Lisu */
    {0xa500, 0xa60c}, /*   269x Vai */
    {0xa610, 0xa62b}, /*    28x Vai */
    {0xa6a0, 0xa6ef}, /*    80x Bamum */
    {0xa80c, 0xa822}, /*    23x Syloti Nagri */
    {0xa840, 0xa873}, /*    52x Phags-pa */
    {0xa882, 0xa8b3}, /*    50x Saurashtra */
    {0xa8d0, 0xa8d9}, /*    10x Saurashtra */
    {0xa900, 0xa925}, /*    38x Kayah Li */
    {0xa930, 0xa946}, /*    23x Rejang */
    {0xa960, 0xa97c}, /*    29x Hangul Jamo Extended-A */
    {0xa984, 0xa9b2}, /*    47x Javanese */
    {0xa9cf, 0xa9d9}, /*    11x Javanese */
    {0xaa00, 0xaa28}, /*    41x Cham */
    {0xaa50, 0xaa59}, /*    10x Cham */
    {0xabf0, 0xabf9}, /*    10x Meetei Mayek */
    {0xac00, 0xd7a3}, /* 11172x Hangul Syllables */
    {0xf900, 0xfa6d}, /*   366x CJK Compatibility Ideographs */
    {0xfa70, 0xfad9}, /*   106x CJK Compatibility Ideographs */
    {0xfb1f, 0xfb28}, /*    10x Alphabetic Presentation Forms */
    {0xfb2a, 0xfb36}, /*    13x Alphabetic Presentation Forms */
    {0xfb46, 0xfbb1}, /*   108x Alphabetic Presentation Forms */
    {0xfbd3, 0xfd3d}, /*   363x Arabic Presentation Forms-A */
    {0xfe76, 0xfefc}, /*   135x Arabic Presentation Forms-B */
    {0xff10, 0xff19}, /*    10x Dubs */
    {0xff21, 0xff3a}, /*    26x Dubs */
    {0xff41, 0xff5a}, /*    26x Dubs */
    {0xff66, 0xffbe}, /*    89x Dubs */
    {0xffc2, 0xffc7}, /*     6x Dubs */
    {0xffca, 0xffcf}, /*     6x Dubs */
    {0xffd2, 0xffd7}, /*     6x Dubs */
    {0xffda, 0xffdc}, /*     3x Dubs */
};

static const unsigned kAstralCodes[][2] = {
    {0x10107, 0x10133}, /*    45x Aegean */
    {0x10140, 0x10178}, /*    57x Ancient Greek Numbers */
    {0x1018a, 0x1018b}, /*     2x Ancient Greek Numbers */
    {0x10280, 0x1029c}, /*    29x Lycian */
    {0x102a0, 0x102d0}, /*    49x Carian */
    {0x102e1, 0x102fb}, /*    27x Coptic Epact Numbers */
    {0x10300, 0x10323}, /*    36x Old Italic */
    {0x1032d, 0x1034a}, /*    30x Old Italic, Gothic */
    {0x10350, 0x10375}, /*    38x Old Permic */
    {0x10380, 0x1039d}, /*    30x Ugaritic */
    {0x103a0, 0x103c3}, /*    36x Old Persian */
    {0x103c8, 0x103cf}, /*     8x Old Persian */
    {0x103d1, 0x103d5}, /*     5x Old Persian */
    {0x10400, 0x1049d}, /*    158x Deseret, Shavian, Osmanya */
    {0x104b0, 0x104d3}, /*    36x Osage */
    {0x104d8, 0x104fb}, /*    36x Osage */
    {0x10500, 0x10527}, /*    40x Elbasan */
    {0x10530, 0x10563}, /*    52x Caucasian Albanian */
    {0x10600, 0x10736}, /*   311x Linear A */
    {0x10800, 0x10805}, /*     6x Cypriot Syllabary */
    {0x1080a, 0x10835}, /*    44x Cypriot Syllabary */
    {0x10837, 0x10838}, /*     2x Cypriot Syllabary */
    {0x1083f, 0x1089e}, /*    86x Cypriot,ImperialAramaic,Palmyrene,Nabataean */
    {0x108e0, 0x108f2}, /*    19x Hatran */
    {0x108f4, 0x108f5}, /*     2x Hatran */
    {0x108fb, 0x1091b}, /*    33x Hatran */
    {0x10920, 0x10939}, /*    26x Lydian */
    {0x10980, 0x109b7}, /*    56x Meroitic Hieromarks */
    {0x109bc, 0x109cf}, /*    20x Meroitic Cursive */
    {0x109d2, 0x10a00}, /*    47x Meroitic Cursive */
    {0x10a10, 0x10a13}, /*     4x Kharoshthi */
    {0x10a15, 0x10a17}, /*     3x Kharoshthi */
    {0x10a19, 0x10a35}, /*    29x Kharoshthi */
    {0x10a40, 0x10a48}, /*     9x Kharoshthi */
    {0x10a60, 0x10a7e}, /*    31x Old South Arabian */
    {0x10a80, 0x10a9f}, /*    32x Old North Arabian */
    {0x10ac0, 0x10ac7}, /*     8x Manichaean */
    {0x10ac9, 0x10ae4}, /*    28x Manichaean */
    {0x10aeb, 0x10aef}, /*     5x Manichaean */
    {0x10b00, 0x10b35}, /*    54x Avestan */
    {0x10b40, 0x10b55}, /*    22x Inscriptional Parthian */
    {0x10b58, 0x10b72}, /*    27x Inscriptional Parthian and Pahlavi */
    {0x10b78, 0x10b91}, /*    26x Inscriptional Pahlavi, Psalter Pahlavi */
    {0x10c00, 0x10c48}, /*    73x Old Turkic */
    {0x10c80, 0x10cb2}, /*    51x Old Hungarian */
    {0x10cc0, 0x10cf2}, /*    51x Old Hungarian */
    {0x10cfa, 0x10d23}, /*    42x Old Hungarian, Hanifi Rohingya */
    {0x10d30, 0x10d39}, /*    10x Hanifi Rohingya */
    {0x10e60, 0x10e7e}, /*    31x Rumi Numeral Symbols */
    {0x10f00, 0x10f27}, /*    40x Old Sogdian */
    {0x10f30, 0x10f45}, /*    22x Sogdian */
    {0x10f51, 0x10f54}, /*     4x Sogdian */
    {0x10fe0, 0x10ff6}, /*    23x Elymaic */
    {0x11003, 0x11037}, /*    53x Brahmi */
    {0x11052, 0x1106f}, /*    30x Brahmi */
    {0x11083, 0x110af}, /*    45x Kaithi */
    {0x110d0, 0x110e8}, /*    25x Sora Sompeng */
    {0x110f0, 0x110f9}, /*    10x Sora Sompeng */
    {0x11103, 0x11126}, /*    36x Chakma */
    {0x11136, 0x1113f}, /*    10x Chakma */
    {0x11144, 0x11144}, /*     1x Chakma */
    {0x11150, 0x11172}, /*    35x Mahajani */
    {0x11176, 0x11176}, /*     1x Mahajani */
    {0x11183, 0x111b2}, /*    48x Sharada */
    {0x111c1, 0x111c4}, /*     4x Sharada */
    {0x111d0, 0x111da}, /*    11x Sharada */
    {0x111dc, 0x111dc}, /*     1x Sharada */
    {0x111e1, 0x111f4}, /*    20x Sinhala Archaic Numbers */
    {0x11200, 0x11211}, /*    18x Khojki */
    {0x11213, 0x1122b}, /*    25x Khojki */
    {0x11280, 0x11286}, /*     7x Multani */
    {0x11288, 0x11288}, /*     1x Multani */
    {0x1128a, 0x1128d}, /*     4x Multani */
    {0x1128f, 0x1129d}, /*    15x Multani */
    {0x1129f, 0x112a8}, /*    10x Multani */
    {0x112b0, 0x112de}, /*    47x Khudawadi */
    {0x112f0, 0x112f9}, /*    10x Khudawadi */
    {0x11305, 0x1130c}, /*     8x Grantha */
    {0x1130f, 0x11310}, /*     2x Grantha */
    {0x11313, 0x11328}, /*    22x Grantha */
    {0x1132a, 0x11330}, /*     7x Grantha */
    {0x11332, 0x11333}, /*     2x Grantha */
    {0x11335, 0x11339}, /*     5x Grantha */
    {0x1133d, 0x1133d}, /*     1x Grantha */
    {0x11350, 0x11350}, /*     1x Grantha */
    {0x1135d, 0x11361}, /*     5x Grantha */
    {0x11400, 0x11434}, /*    53x Newa */
    {0x11447, 0x1144a}, /*     4x Newa */
    {0x11450, 0x11459}, /*    10x Newa */
    {0x1145f, 0x1145f}, /*     1x Newa */
    {0x11480, 0x114af}, /*    48x Tirhuta */
    {0x114c4, 0x114c5}, /*     2x Tirhuta */
    {0x114c7, 0x114c7}, /*     1x Tirhuta */
    {0x114d0, 0x114d9}, /*    10x Tirhuta */
    {0x11580, 0x115ae}, /*    47x Siddham */
    {0x115d8, 0x115db}, /*     4x Siddham */
    {0x11600, 0x1162f}, /*    48x Modi */
    {0x11644, 0x11644}, /*     1x Modi */
    {0x11650, 0x11659}, /*    10x Modi */
    {0x11680, 0x116aa}, /*    43x Takri */
    {0x116b8, 0x116b8}, /*     1x Takri */
    {0x116c0, 0x116c9}, /*    10x Takri */
    {0x11700, 0x1171a}, /*    27x Ahom */
    {0x11730, 0x1173b}, /*    12x Ahom */
    {0x11800, 0x1182b}, /*    44x Dogra */
    {0x118a0, 0x118f2}, /*    83x Warang Citi */
    {0x118ff, 0x118ff}, /*     1x Warang Citi */
    {0x119a0, 0x119a7}, /*     8x Nandinagari */
    {0x119aa, 0x119d0}, /*    39x Nandinagari */
    {0x119e1, 0x119e1}, /*     1x Nandinagari */
    {0x119e3, 0x119e3}, /*     1x Nandinagari */
    {0x11a00, 0x11a00}, /*     1x Zanabazar Square */
    {0x11a0b, 0x11a32}, /*    40x Zanabazar Square */
    {0x11a3a, 0x11a3a}, /*     1x Zanabazar Square */
    {0x11a50, 0x11a50}, /*     1x Soyombo */
    {0x11a5c, 0x11a89}, /*    46x Soyombo */
    {0x11a9d, 0x11a9d}, /*     1x Soyombo */
    {0x11ac0, 0x11af8}, /*    57x Pau Cin Hau */
    {0x11c00, 0x11c08}, /*     9x Bhaiksuki */
    {0x11c0a, 0x11c2e}, /*    37x Bhaiksuki */
    {0x11c40, 0x11c40}, /*     1x Bhaiksuki */
    {0x11c50, 0x11c6c}, /*    29x Bhaiksuki */
    {0x11c72, 0x11c8f}, /*    30x Marchen */
    {0x11d00, 0x11d06}, /*     7x Masaram Gondi */
    {0x11d08, 0x11d09}, /*     2x Masaram Gondi */
    {0x11d0b, 0x11d30}, /*    38x Masaram Gondi */
    {0x11d46, 0x11d46}, /*     1x Masaram Gondi */
    {0x11d50, 0x11d59}, /*    10x Masaram Gondi */
    {0x11d60, 0x11d65}, /*     6x Gunjala Gondi */
    {0x11d67, 0x11d68}, /*     2x Gunjala Gondi */
    {0x11d6a, 0x11d89}, /*    32x Gunjala Gondi */
    {0x11d98, 0x11d98}, /*     1x Gunjala Gondi */
    {0x11da0, 0x11da9}, /*    10x Gunjala Gondi */
    {0x11ee0, 0x11ef2}, /*    19x Makasar */
    {0x11fc0, 0x11fd4}, /*    21x Tamil Supplement */
    {0x12000, 0x12399}, /*   922x Cuneiform */
    {0x12400, 0x1246e}, /*   111x Cuneiform Numbers & Punctuation */
    {0x12480, 0x12543}, /*   196x Early Dynastic Cuneiform */
    {0x13000, 0x1342e}, /*  1071x Egyptian Hieromarks */
    {0x14400, 0x14646}, /*   583x Anatolian Hieromarks */
    {0x16800, 0x16a38}, /*   569x Bamum Supplement */
    {0x16a40, 0x16a5e}, /*    31x Mro */
    {0x16a60, 0x16a69}, /*    10x Mro */
    {0x16ad0, 0x16aed}, /*    30x Bassa Vah */
    {0x16b00, 0x16b2f}, /*    48x Pahawh Hmong */
    {0x16b40, 0x16b43}, /*     4x Pahawh Hmong */
    {0x16b50, 0x16b59}, /*    10x Pahawh Hmong */
    {0x16b5b, 0x16b61}, /*     7x Pahawh Hmong */
    {0x16b63, 0x16b77}, /*    21x Pahawh Hmong */
    {0x16b7d, 0x16b8f}, /*    19x Pahawh Hmong */
    {0x16e40, 0x16e96}, /*    87x Medefaidrin */
    {0x16f00, 0x16f4a}, /*    75x Miao */
    {0x16f50, 0x16f50}, /*     1x Miao */
    {0x16f93, 0x16f9f}, /*    13x Miao */
    {0x16fe0, 0x16fe1}, /*     2x Ideographic Symbols & Punctuation */
    {0x16fe3, 0x16fe3}, /*     1x Ideographic Symbols & Punctuation */
    {0x17000, 0x187f7}, /*  6136x Tangut */
    {0x18800, 0x18af2}, /*   755x Tangut Components */
    {0x1b000, 0x1b11e}, /*   287x Kana Supplement */
    {0x1b150, 0x1b152}, /*     3x Small Kana Extension */
    {0x1b164, 0x1b167}, /*     4x Small Kana Extension */
    {0x1b170, 0x1b2fb}, /*   396x Nushu */
    {0x1bc00, 0x1bc6a}, /*   107x Duployan */
    {0x1bc70, 0x1bc7c}, /*    13x Duployan */
    {0x1bc80, 0x1bc88}, /*     9x Duployan */
    {0x1bc90, 0x1bc99}, /*    10x Duployan */
    {0x1d2e0, 0x1d2f3}, /*    20x Mayan Numerals */
    {0x1d360, 0x1d378}, /*    25x Counting Rod Numerals */
    {0x1d400, 0x1d454}, /*    85x 𝐀..𝑔 Math */
    {0x1d456, 0x1d49c}, /*    71x 𝑖..𝒜 Math */
    {0x1d49e, 0x1d49f}, /*     2x 𝒞..𝒟 Math */
    {0x1d4a2, 0x1d4a2}, /*     1x 𝒢..𝒢 Math */
    {0x1d4a5, 0x1d4a6}, /*     2x 𝒥..𝒦 Math */
    {0x1d4a9, 0x1d4ac}, /*     4x 𝒩..𝒬 Math */
    {0x1d4ae, 0x1d4b9}, /*    12x 𝒮..𝒹 Math */
    {0x1d4bb, 0x1d4bb}, /*     1x 𝒻..𝒻 Math */
    {0x1d4bd, 0x1d4c3}, /*     7x 𝒽..𝓃 Math */
    {0x1d4c5, 0x1d505}, /*    65x 𝓅..𝔅 Math */
    {0x1d507, 0x1d50a}, /*     4x 𝔇..𝔊 Math */
    {0x1d50d, 0x1d514}, /*     8x 𝔍..𝔔 Math */
    {0x1d516, 0x1d51c}, /*     7x 𝔖..𝔜 Math */
    {0x1d51e, 0x1d539}, /*    28x 𝔞..𝔹 Math */
    {0x1d53b, 0x1d53e}, /*     4x 𝔻..𝔾 Math */
    {0x1d540, 0x1d544}, /*     5x 𝕀..𝕄 Math */
    {0x1d546, 0x1d546}, /*     1x 𝕆..𝕆 Math */
    {0x1d54a, 0x1d550}, /*     7x 𝕊..𝕐 Math */
    {0x1d552, 0x1d6a5}, /*   340x 𝕒..𝚥 Math */
    {0x1d6a8, 0x1d6c0}, /*    25x 𝚨..𝛀 Math */
    {0x1d6c2, 0x1d6da}, /*    25x 𝛂..𝛚 Math */
    {0x1d6dc, 0x1d6fa}, /*    31x 𝛜..𝛺 Math */
    {0x1d6fc, 0x1d714}, /*    25x 𝛼..𝜔 Math */
    {0x1d716, 0x1d734}, /*    31x 𝜖..𝜴 Math */
    {0x1d736, 0x1d74e}, /*    25x 𝜶..𝝎 Math */
    {0x1d750, 0x1d76e}, /*    31x 𝝐..𝝮 Math */
    {0x1d770, 0x1d788}, /*    25x 𝝰..𝞈 Math */
    {0x1d78a, 0x1d7a8}, /*    31x 𝞊..𝞨 Math */
    {0x1d7aa, 0x1d7c2}, /*    25x 𝞪..𝟂 Math */
    {0x1d7c4, 0x1d7cb}, /*     8x 𝟄..𝟋 Math */
    {0x1d7ce, 0x1d9ff}, /*   562x Math, Sutton SignWriting */
    {0x1f100, 0x1f10c}, /*    13x Enclosed Alphanumeric Supplement */
    {0x20000, 0x2a6d6}, /* 42711x CJK Unified Ideographs Extension B */
    {0x2a700, 0x2b734}, /*  4149x CJK Unified Ideographs Extension C */
    {0x2b740, 0x2b81d}, /*   222x CJK Unified Ideographs Extension D */
    {0x2b820, 0x2cea1}, /*  5762x CJK Unified Ideographs Extension E */
    {0x2ceb0, 0x2ebe0}, /*  7473x CJK Unified Ideographs Extension F */
    {0x2f800, 0x2fa1d}, /*   542x CJK Compatibility Ideographs Supplement */
};

/**
 * Returns nonzero if 𝑐 isn't alphanumeric.
 *
 * Line reading interfaces generally define this operation as UNICODE
 * characters that aren't in the letter category (Lu, Ll, Lt, Lm, Lo)
 * and aren't in the number categorie (Nd, Nl, No). We also add a few
 * other things like blocks and emoji (So).
 */
int iswseparator(wint_t c) {
  int m, l, r, n;
  if (c < 0200) {
    return !(('0' <= c && c <= '9') || ('A' <= c && c <= 'Z') ||
             ('a' <= c && c <= 'z'));
  }
  if (c <= 0xffff) {
    l = 0;
    r = n = sizeof(kCodes) / sizeof(kCodes[0]);
    while (l < r) {
      m = (l & r) + ((l ^ r) >> 1);  // floor((a+b)/2)
      if (kCodes[m][1] < c) {
        l = m + 1;
      } else {
        r = m;
      }
    }
    return !(l < n && kCodes[l][0] <= c && c <= kCodes[l][1]);
  } else {
    l = 0;
    r = n = sizeof(kAstralCodes) / sizeof(kAstralCodes[0]);
    while (l < r) {
      m = (l & r) + ((l ^ r) >> 1);  // floor((a+b)/2)
      if (kAstralCodes[m][1] < c) {
        l = m + 1;
      } else {
        r = m;
      }
    }
    return !(l < n && kAstralCodes[l][0] <= c && c <= kAstralCodes[l][1]);
  }
}
