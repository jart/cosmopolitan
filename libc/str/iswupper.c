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
#include "libc/str/str.h"

/**
 * Returns nonzero if c is uppercase letter.
 */
int iswupper(wint_t c) {
  if (isascii(c)) return isupper(c);
  switch (c) {
    case u'À':     // LATIN CAPITAL A W/ GRAVE 00c0
    case u'Á':     // LATIN CAPITAL A W/ ACUTE 00c1
    case u'Â':     // LATIN CAPITAL A W/ CIRCUMFLEX 00c2
    case u'Ã':     // LATIN CAPITAL A W/ TILDE 00c3
    case u'Ä':     // LATIN CAPITAL A W/ DIAERESIS 00c4
    case u'Å':     // LATIN CAPITAL A W/ RING ABOVE 00c5
    case u'Æ':     // LATIN CAPITAL AE 00c6
    case u'Ç':     // LATIN CAPITAL C W/ CEDILLA 00c7
    case u'È':     // LATIN CAPITAL E W/ GRAVE 00c8
    case u'É':     // LATIN CAPITAL E W/ ACUTE 00c9
    case u'Ê':     // LATIN CAPITAL E W/ CIRCUMFLEX 00ca
    case u'Ë':     // LATIN CAPITAL E W/ DIAERESIS 00cb
    case u'Ì':     // LATIN CAPITAL I W/ GRAVE 00cc
    case u'Í':     // LATIN CAPITAL I W/ ACUTE 00cd
    case u'Î':     // LATIN CAPITAL I W/ CIRCUMFLEX 00ce
    case u'Ï':     // LATIN CAPITAL I W/ DIAERESIS 00cf
    case u'Ð':     // LATIN CAPITAL ETH 00d0
    case u'Ñ':     // LATIN CAPITAL N W/ TILDE 00d1
    case u'Ò':     // LATIN CAPITAL O W/ GRAVE 00d2
    case u'Ó':     // LATIN CAPITAL O W/ ACUTE 00d3
    case u'Ô':     // LATIN CAPITAL O W/ CIRCUMFLEX 00d4
    case u'Õ':     // LATIN CAPITAL O W/ TILDE 00d5
    case u'Ö':     // LATIN CAPITAL O W/ DIAERESIS 00d6
    case u'Ø':     // LATIN CAPITAL O W/ STROKE 00d8
    case u'Ù':     // LATIN CAPITAL U W/ GRAVE 00d9
    case u'Ú':     // LATIN CAPITAL U W/ ACUTE 00da
    case u'Û':     // LATIN CAPITAL U W/ CIRCUMFLEX 00db
    case u'Ü':     // LATIN CAPITAL U W/ DIAERESIS 00dc
    case u'Ý':     // LATIN CAPITAL Y W/ ACUTE 00dd
    case u'Þ':     // LATIN CAPITAL THORN 00de
    case u'Ā':     // LATIN CAPITAL A W/ MACRON 0100
    case u'Ă':     // LATIN CAPITAL A W/ BREVE 0102
    case u'Ą':     // LATIN CAPITAL A W/ OGONEK 0104
    case u'Ć':     // LATIN CAPITAL C W/ ACUTE 0106
    case u'Ĉ':     // LATIN CAPITAL C W/ CIRCUMFLEX 0108
    case u'Ċ':     // LATIN CAPITAL C W/ DOT ABOVE 010a
    case u'Č':     // LATIN CAPITAL C W/ CARON 010c
    case u'Ď':     // LATIN CAPITAL D W/ CARON 010e
    case u'Đ':     // LATIN CAPITAL D W/ STROKE 0110
    case u'Ē':     // LATIN CAPITAL E W/ MACRON 0112
    case u'Ĕ':     // LATIN CAPITAL E W/ BREVE 0114
    case u'Ė':     // LATIN CAPITAL E W/ DOT ABOVE 0116
    case u'Ę':     // LATIN CAPITAL E W/ OGONEK 0118
    case u'Ě':     // LATIN CAPITAL E W/ CARON 011a
    case u'Ĝ':     // LATIN CAPITAL G W/ CIRCUMFLEX 011c
    case u'Ğ':     // LATIN CAPITAL G W/ BREVE 011e
    case u'Ġ':     // LATIN CAPITAL G W/ DOT ABOVE 0120
    case u'Ģ':     // LATIN CAPITAL G W/ CEDILLA 0122
    case u'Ĥ':     // LATIN CAPITAL H W/ CIRCUMFLEX 0124
    case u'Ħ':     // LATIN CAPITAL H W/ STROKE 0126
    case u'Ĩ':     // LATIN CAPITAL I W/ TILDE 0128
    case u'Ī':     // LATIN CAPITAL I W/ MACRON 012a
    case u'Ĭ':     // LATIN CAPITAL I W/ BREVE 012c
    case u'Į':     // LATIN CAPITAL I W/ OGONEK 012e
    case u'İ':     // LATIN CAPITAL I W/ DOT ABOVE 0130
    case u'Ĳ':     // LATIN CAPITAL LIGATURE IJ 0132
    case u'Ĵ':     // LATIN CAPITAL J W/ CIRCUMFLEX 0134
    case u'Ķ':     // LATIN CAPITAL K W/ CEDILLA 0136
    case u'Ĺ':     // LATIN CAPITAL L W/ ACUTE 0139
    case u'Ļ':     // LATIN CAPITAL L W/ CEDILLA 013b
    case u'Ľ':     // LATIN CAPITAL L W/ CARON 013d
    case u'Ŀ':     // LATIN CAPITAL L W/ MIDDLE DOT 013f
    case u'Ł':     // LATIN CAPITAL L W/ STROKE 0141
    case u'Ń':     // LATIN CAPITAL N W/ ACUTE 0143
    case u'Ņ':     // LATIN CAPITAL N W/ CEDILLA 0145
    case u'Ň':     // LATIN CAPITAL N W/ CARON 0147
    case u'Ŋ':     // LATIN CAPITAL ENG 014a
    case u'Ō':     // LATIN CAPITAL O W/ MACRON 014c
    case u'Ŏ':     // LATIN CAPITAL O W/ BREVE 014e
    case u'Ő':     // LATIN CAPITAL O W/ DOUBLE ACUTE 0150
    case u'Œ':     // LATIN CAPITAL LIGATURE OE 0152
    case u'Ŕ':     // LATIN CAPITAL R W/ ACUTE 0154
    case u'Ŗ':     // LATIN CAPITAL R W/ CEDILLA 0156
    case u'Ř':     // LATIN CAPITAL R W/ CARON 0158
    case u'Ś':     // LATIN CAPITAL S W/ ACUTE 015a
    case u'Ŝ':     // LATIN CAPITAL S W/ CIRCUMFLEX 015c
    case u'Ş':     // LATIN CAPITAL S W/ CEDILLA 015e
    case u'Š':     // LATIN CAPITAL S W/ CARON 0160
    case u'Ţ':     // LATIN CAPITAL T W/ CEDILLA 0162
    case u'Ť':     // LATIN CAPITAL T W/ CARON 0164
    case u'Ŧ':     // LATIN CAPITAL T W/ STROKE 0166
    case u'Ũ':     // LATIN CAPITAL U W/ TILDE 0168
    case u'Ū':     // LATIN CAPITAL U W/ MACRON 016a
    case u'Ŭ':     // LATIN CAPITAL U W/ BREVE 016c
    case u'Ů':     // LATIN CAPITAL U W/ RING ABOVE 016e
    case u'Ű':     // LATIN CAPITAL U W/ DOUBLE ACUTE 0170
    case u'Ų':     // LATIN CAPITAL U W/ OGONEK 0172
    case u'Ŵ':     // LATIN CAPITAL W W/ CIRCUMFLEX 0174
    case u'Ŷ':     // LATIN CAPITAL Y W/ CIRCUMFLEX 0176
    case u'Ÿ':     // LATIN CAPITAL Y W/ DIAERESIS 0178
    case u'Ź':     // LATIN CAPITAL Z W/ ACUTE 0179
    case u'Ż':     // LATIN CAPITAL Z W/ DOT ABOVE 017b
    case u'Ž':     // LATIN CAPITAL Z W/ CARON 017d
    case u'Ɓ':     // LATIN CAPITAL B W/ HOOK 0181
    case u'Ƃ':     // LATIN CAPITAL B W/ TOPBAR 0182
    case u'Ƅ':     // LATIN CAPITAL TONE SIX 0184
    case u'Ɔ':     // LATIN CAPITAL OPEN O 0186
    case u'Ƈ':     // LATIN CAPITAL C W/ HOOK 0187
    case u'Ɖ':     // LATIN CAPITAL AFRICAN D 0189
    case u'Ɗ':     // LATIN CAPITAL D W/ HOOK 018a
    case u'Ƌ':     // LATIN CAPITAL D W/ TOPBAR 018b
    case u'Ǝ':     // LATIN CAPITAL REVERSED E 018e
    case u'Ə':     // LATIN CAPITAL SCHWA 018f
    case u'Ɛ':     // LATIN CAPITAL OPEN E 0190
    case u'Ƒ':     // LATIN CAPITAL F W/ HOOK 0191
    case u'Ɠ':     // LATIN CAPITAL G W/ HOOK 0193
    case u'Ɣ':     // LATIN CAPITAL GAMMA 0194
    case u'Ɩ':     // LATIN CAPITAL IOTA 0196
    case u'Ɨ':     // LATIN CAPITAL I W/ STROKE 0197
    case u'Ƙ':     // LATIN CAPITAL K W/ HOOK 0198
    case u'Ɯ':     // LATIN CAPITAL TURNED M 019c
    case u'Ɲ':     // LATIN CAPITAL N W/ LEFT HOOK 019d
    case u'Ɵ':     // LATIN CAPITAL O W/ MIDDLE TILDE 019f
    case u'Ơ':     // LATIN CAPITAL O W/ HORN 01a0
    case u'Ƣ':     // LATIN CAPITAL OI 01a2
    case u'Ƥ':     // LATIN CAPITAL P W/ HOOK 01a4
    case u'Ʀ':     // LATIN LETTER YR 01a6
    case u'Ƨ':     // LATIN CAPITAL TONE TWO 01a7
    case u'Ʃ':     // LATIN CAPITAL ESH 01a9
    case u'Ƭ':     // LATIN CAPITAL T W/ HOOK 01ac
    case u'Ʈ':     // LATIN CAPITAL T W/ RETROFLEX HOOK 01ae
    case u'Ư':     // LATIN CAPITAL U W/ HORN 01af
    case u'Ʊ':     // LATIN CAPITAL UPSILON 01b1
    case u'Ʋ':     // LATIN CAPITAL V W/ HOOK 01b2
    case u'Ƴ':     // LATIN CAPITAL Y W/ HOOK 01b3
    case u'Ƶ':     // LATIN CAPITAL Z W/ STROKE 01b5
    case u'Ʒ':     // LATIN CAPITAL EZH 01b7
    case u'Ƹ':     // LATIN CAPITAL EZH REVERSED 01b8
    case u'Ƽ':     // LATIN CAPITAL TONE FIVE 01bc
    case u'Ǆ':     // LATIN CAPITAL DZ W/ CARON 01c4
    case u'Ǉ':     // LATIN CAPITAL LJ 01c7
    case u'Ǌ':     // LATIN CAPITAL NJ 01ca
    case u'Ǎ':     // LATIN CAPITAL A W/ CARON 01cd
    case u'Ǐ':     // LATIN CAPITAL I W/ CARON 01cf
    case u'Ǒ':     // LATIN CAPITAL O W/ CARON 01d1
    case u'Ǔ':     // LATIN CAPITAL U W/ CARON 01d3
    case u'Ǖ':     // LATIN CAPITAL U W/ DIAERESIS AND MACRON 01d5
    case u'Ǘ':     // LATIN CAPITAL U W/ DIAERESIS AND ACUTE 01d7
    case u'Ǚ':     // LATIN CAPITAL U W/ DIAERESIS AND CARON 01d9
    case u'Ǜ':     // LATIN CAPITAL U W/ DIAERESIS AND GRAVE 01db
    case u'Ǟ':     // LATIN CAPITAL A W/ DIAERESIS AND MACRON 01de
    case u'Ǡ':     // LATIN CAPITAL A W/ DOT ABOVE AND MACRON 01e0
    case u'Ǣ':     // LATIN CAPITAL AE W/ MACRON 01e2
    case u'Ǥ':     // LATIN CAPITAL G W/ STROKE 01e4
    case u'Ǧ':     // LATIN CAPITAL G W/ CARON 01e6
    case u'Ǩ':     // LATIN CAPITAL K W/ CARON 01e8
    case u'Ǫ':     // LATIN CAPITAL O W/ OGONEK 01ea
    case u'Ǭ':     // LATIN CAPITAL O W/ OGONEK AND MACRON 01ec
    case u'Ǯ':     // LATIN CAPITAL EZH W/ CARON 01ee
    case u'Ǳ':     // LATIN CAPITAL DZ 01f1
    case u'Ǵ':     // LATIN CAPITAL G W/ ACUTE 01f4
    case u'Ƕ':     // LATIN CAPITAL HWAIR 01f6
    case u'Ƿ':     // LATIN CAPITAL WYNN 01f7
    case u'Ǹ':     // LATIN CAPITAL N W/ GRAVE 01f8
    case u'Ǻ':     // LATIN CAPITAL A W/ RING ABOVE AND ACUTE 01fa
    case u'Ǽ':     // LATIN CAPITAL AE W/ ACUTE 01fc
    case u'Ǿ':     // LATIN CAPITAL O W/ STROKE AND ACUTE 01fe
    case u'Ȁ':     // LATIN CAPITAL A W/ DOUBLE GRAVE 0200
    case u'Ȃ':     // LATIN CAPITAL A W/ INVERTED BREVE 0202
    case u'Ȅ':     // LATIN CAPITAL E W/ DOUBLE GRAVE 0204
    case u'Ȇ':     // LATIN CAPITAL E W/ INVERTED BREVE 0206
    case u'Ȉ':     // LATIN CAPITAL I W/ DOUBLE GRAVE 0208
    case u'Ȋ':     // LATIN CAPITAL I W/ INVERTED BREVE 020a
    case u'Ȍ':     // LATIN CAPITAL O W/ DOUBLE GRAVE 020c
    case u'Ȏ':     // LATIN CAPITAL O W/ INVERTED BREVE 020e
    case u'Ȑ':     // LATIN CAPITAL R W/ DOUBLE GRAVE 0210
    case u'Ȓ':     // LATIN CAPITAL R W/ INVERTED BREVE 0212
    case u'Ȕ':     // LATIN CAPITAL U W/ DOUBLE GRAVE 0214
    case u'Ȗ':     // LATIN CAPITAL U W/ INVERTED BREVE 0216
    case u'Ș':     // LATIN CAPITAL S W/ COMMA BELOW 0218
    case u'Ț':     // LATIN CAPITAL T W/ COMMA BELOW 021a
    case u'Ȝ':     // LATIN CAPITAL YOGH 021c
    case u'Ȟ':     // LATIN CAPITAL H W/ CARON 021e
    case u'Ƞ':     // LATIN CAPITAL N W/ LONG RIGHT LEG 0220
    case u'Ȣ':     // LATIN CAPITAL OU 0222
    case u'Ȥ':     // LATIN CAPITAL Z W/ HOOK 0224
    case u'Ȧ':     // LATIN CAPITAL A W/ DOT ABOVE 0226
    case u'Ȩ':     // LATIN CAPITAL E W/ CEDILLA 0228
    case u'Ȫ':     // LATIN CAPITAL O W/ DIAERESIS AND MACRON 022a
    case u'Ȭ':     // LATIN CAPITAL O W/ TILDE AND MACRON 022c
    case u'Ȯ':     // LATIN CAPITAL O W/ DOT ABOVE 022e
    case u'Ȱ':     // LATIN CAPITAL O W/ DOT ABOVE AND MACRON 0230
    case u'Ȳ':     // LATIN CAPITAL Y W/ MACRON 0232
    case u'Ⱥ':     // LATIN CAPITAL A W/ STROKE 023a
    case u'Ȼ':     // LATIN CAPITAL C W/ STROKE 023b
    case u'Ƚ':     // LATIN CAPITAL L W/ BAR 023d
    case u'Ⱦ':     // LATIN CAPITAL T W/ DIAGONAL STROKE 023e
    case u'Ɂ':     // LATIN CAPITAL GLOTTAL STOP 0241
    case u'Ƀ':     // LATIN CAPITAL B W/ STROKE 0243
    case u'Ʉ':     // LATIN CAPITAL U BAR 0244
    case u'Ʌ':     // LATIN CAPITAL TURNED V 0245
    case u'Ɇ':     // LATIN CAPITAL E W/ STROKE 0246
    case u'Ɉ':     // LATIN CAPITAL J W/ STROKE 0248
    case u'Ɋ':     // LATIN CAPITAL SMALL Q W/ HOOK TAIL 024a
    case u'Ɍ':     // LATIN CAPITAL R W/ STROKE 024c
    case u'Ɏ':     // LATIN CAPITAL Y W/ STROKE 024e
    case u'Ͱ':     // GREEK CAPITAL HETA 0370
    case u'Ͳ':     // GREEK CAPITAL ARCHAIC SAMPI 0372
    case u'Ͷ':     // GREEK CAPITAL PAMPHYLIAN DIGAMMA 0376
    case u'Ϳ':    // GREEK CAPITAL YOT 037f
    case u'Ά':     // GREEK CAPITAL ALPHA W/ TONOS 0386
    case u'Έ':     // GREEK CAPITAL EPSILON W/ TONOS 0388
    case u'Ή':     // GREEK CAPITAL ETA W/ TONOS 0389
    case u'Ί':     // GREEK CAPITAL IOTA W/ TONOS 038a
    case u'Ό':     // GREEK CAPITAL OMICRON W/ TONOS 038c
    case u'Ύ':     // GREEK CAPITAL UPSILON W/ TONOS 038e
    case u'Ώ':     // GREEK CAPITAL OMEGA W/ TONOS 038f
    case u'Α':     // GREEK CAPITAL ALPHA 0391
    case u'Β':     // GREEK CAPITAL BETA 0392
    case u'Γ':     // GREEK CAPITAL GAMMA 0393
    case u'Δ':     // GREEK CAPITAL DELTA 0394
    case u'Ε':     // GREEK CAPITAL EPSILON 0395
    case u'Ζ':     // GREEK CAPITAL ZETA 0396
    case u'Η':     // GREEK CAPITAL ETA 0397
    case u'Θ':     // GREEK CAPITAL THETA 0398
    case u'Ι':     // GREEK CAPITAL IOTA 0399
    case u'Κ':     // GREEK CAPITAL KAPPA 039a
    case u'Λ':     // GREEK CAPITAL LAMDA 039b
    case u'Μ':     // GREEK CAPITAL MU 039c
    case u'Ν':     // GREEK CAPITAL NU 039d
    case u'Ξ':     // GREEK CAPITAL XI 039e
    case u'Ο':     // GREEK CAPITAL OMICRON 039f
    case u'Π':     // GREEK CAPITAL PI 03a0
    case u'Ρ':     // GREEK CAPITAL RHO 03a1
    case u'Σ':     // GREEK CAPITAL SIGMA 03a3
    case u'Τ':     // GREEK CAPITAL TAU 03a4
    case u'Υ':     // GREEK CAPITAL UPSILON 03a5
    case u'Φ':     // GREEK CAPITAL PHI 03a6
    case u'Χ':     // GREEK CAPITAL CHI 03a7
    case u'Ψ':     // GREEK CAPITAL PSI 03a8
    case u'Ω':     // GREEK CAPITAL OMEGA 03a9
    case u'Ϊ':     // GREEK CAPITAL IOTA W/ DIALYTIKA 03aa
    case u'Ϋ':     // GREEK CAPITAL UPSILON W/ DIALYTIKA 03ab
    case u'Ϗ':     // GREEK CAPITAL KAI SYMBOL 03cf
    case u'ϒ':     // GREEK UPSILON W/ HOOK SYMBOL 03d2
    case u'ϓ':     // GREEK UPSILON W/ ACUTE AND HOOK SYMBOL 03d3
    case u'ϔ':     // GREEK UPSILON W/ DIAERESIS AND HOOK SYMBOL 03d4
    case u'Ϙ':     // GREEK LETTER ARCHAIC KOPPA 03d8
    case u'Ϛ':     // GREEK LETTER STIGMA 03da
    case u'Ϝ':     // GREEK LETTER DIGAMMA 03dc
    case u'Ϟ':     // GREEK LETTER KOPPA 03de
    case u'Ϡ':     // GREEK LETTER SAMPI 03e0
    case u'Ϣ':     // COPTIC CAPITAL SHEI 03e2
    case u'Ϥ':     // COPTIC CAPITAL FEI 03e4
    case u'Ϧ':     // COPTIC CAPITAL KHEI 03e6
    case u'Ϩ':     // COPTIC CAPITAL HORI 03e8
    case u'Ϫ':     // COPTIC CAPITAL GANGIA 03ea
    case u'Ϭ':     // COPTIC CAPITAL SHIMA 03ec
    case u'Ϯ':     // COPTIC CAPITAL DEI 03ee
    case u'ϴ':     // GREEK CAPITAL THETA SYMBOL 03f4
    case u'Ϸ':     // GREEK CAPITAL SHO 03f7
    case u'Ϲ':     // GREEK CAPITAL LUNATE SIGMA SYMBOL 03f9
    case u'Ϻ':     // GREEK CAPITAL SAN 03fa
    case u'Ͻ':     // GREEK CAPITAL REVERSED LUNATE SIGMA SYMBOL 03fd
    case u'Ͼ':     // GREEK CAPITAL DOTTED LUNATE SIGMA SYMBOL 03fe
    case u'Ͽ':     // GREEK CAPITAL REVERSED DOTTED LUNATE SIGMA SYMBOL 03ff
    case u'Ѐ':     // CYRILLIC CAPITAL IE W/ GRAVE 0400
    case u'Ё':     // CYRILLIC CAPITAL IO 0401
    case u'Ђ':     // CYRILLIC CAPITAL DJE 0402
    case u'Ѓ':     // CYRILLIC CAPITAL GJE 0403
    case u'Є':     // CYRILLIC CAPITAL UKRAINIAN IE 0404
    case u'Ѕ':     // CYRILLIC CAPITAL DZE 0405
    case u'І':     // CYRILLIC CAPITAL BYELORUSSIAN-UKRAINIAN I 0406
    case u'Ї':     // CYRILLIC CAPITAL YI 0407
    case u'Ј':     // CYRILLIC CAPITAL JE 0408
    case u'Љ':     // CYRILLIC CAPITAL LJE 0409
    case u'Њ':     // CYRILLIC CAPITAL NJE 040a
    case u'Ћ':     // CYRILLIC CAPITAL TSHE 040b
    case u'Ќ':     // CYRILLIC CAPITAL KJE 040c
    case u'Ѝ':     // CYRILLIC CAPITAL I W/ GRAVE 040d
    case u'Ў':     // CYRILLIC CAPITAL SHORT U 040e
    case u'Џ':     // CYRILLIC CAPITAL DZHE 040f
    case u'А':     // CYRILLIC CAPITAL A 0410
    case u'Б':     // CYRILLIC CAPITAL BE 0411
    case u'В':     // CYRILLIC CAPITAL VE 0412
    case u'Г':     // CYRILLIC CAPITAL GHE 0413
    case u'Д':     // CYRILLIC CAPITAL DE 0414
    case u'Е':     // CYRILLIC CAPITAL IE 0415
    case u'Ж':     // CYRILLIC CAPITAL ZHE 0416
    case u'З':     // CYRILLIC CAPITAL ZE 0417
    case u'И':     // CYRILLIC CAPITAL I 0418
    case u'Й':     // CYRILLIC CAPITAL SHORT I 0419
    case u'К':     // CYRILLIC CAPITAL KA 041a
    case u'Л':     // CYRILLIC CAPITAL EL 041b
    case u'М':     // CYRILLIC CAPITAL EM 041c
    case u'Н':     // CYRILLIC CAPITAL EN 041d
    case u'О':     // CYRILLIC CAPITAL O 041e
    case u'П':     // CYRILLIC CAPITAL PE 041f
    case u'Р':     // CYRILLIC CAPITAL ER 0420
    case u'С':     // CYRILLIC CAPITAL ES 0421
    case u'Т':     // CYRILLIC CAPITAL TE 0422
    case u'У':     // CYRILLIC CAPITAL U 0423
    case u'Ф':     // CYRILLIC CAPITAL EF 0424
    case u'Х':     // CYRILLIC CAPITAL HA 0425
    case u'Ц':     // CYRILLIC CAPITAL TSE 0426
    case u'Ч':     // CYRILLIC CAPITAL CHE 0427
    case u'Ш':     // CYRILLIC CAPITAL SHA 0428
    case u'Щ':     // CYRILLIC CAPITAL SHCHA 0429
    case u'Ъ':     // CYRILLIC CAPITAL HARD SIGN 042a
    case u'Ы':     // CYRILLIC CAPITAL YERU 042b
    case u'Ь':     // CYRILLIC CAPITAL SOFT SIGN 042c
    case u'Э':     // CYRILLIC CAPITAL E 042d
    case u'Ю':     // CYRILLIC CAPITAL YU 042e
    case u'Я':     // CYRILLIC CAPITAL YA 042f
    case u'Ѡ':     // CYRILLIC CAPITAL OMEGA 0460
    case u'Ѣ':     // CYRILLIC CAPITAL YAT 0462
    case u'Ѥ':     // CYRILLIC CAPITAL IOTIFIED E 0464
    case u'Ѧ':     // CYRILLIC CAPITAL LITTLE YUS 0466
    case u'Ѩ':     // CYRILLIC CAPITAL IOTIFIED LITTLE YUS 0468
    case u'Ѫ':     // CYRILLIC CAPITAL BIG YUS 046a
    case u'Ѭ':     // CYRILLIC CAPITAL IOTIFIED BIG YUS 046c
    case u'Ѯ':     // CYRILLIC CAPITAL KSI 046e
    case u'Ѱ':     // CYRILLIC CAPITAL PSI 0470
    case u'Ѳ':     // CYRILLIC CAPITAL FITA 0472
    case u'Ѵ':     // CYRILLIC CAPITAL IZHITSA 0474
    case u'Ѷ':     // CYRILLIC CAPITAL IZHITSA W/ DOUBLE GRAVE ACCENT 0476
    case u'Ѹ':     // CYRILLIC CAPITAL UK 0478
    case u'Ѻ':     // CYRILLIC CAPITAL ROUND OMEGA 047a
    case u'Ѽ':     // CYRILLIC CAPITAL OMEGA W/ TITLO 047c
    case u'Ѿ':     // CYRILLIC CAPITAL OT 047e
    case u'Ҁ':     // CYRILLIC CAPITAL KOPPA 0480
    case u'Ҋ':     // CYRILLIC CAPITAL SHORT I W/ TAIL 048a
    case u'Ҍ':     // CYRILLIC CAPITAL SEMISOFT SIGN 048c
    case u'Ҏ':     // CYRILLIC CAPITAL ER W/ TICK 048e
    case u'Ґ':     // CYRILLIC CAPITAL GHE W/ UPTURN 0490
    case u'Ғ':     // CYRILLIC CAPITAL GHE W/ STROKE 0492
    case u'Ҕ':     // CYRILLIC CAPITAL GHE W/ MIDDLE HOOK 0494
    case u'Җ':     // CYRILLIC CAPITAL ZHE W/ DESCENDER 0496
    case u'Ҙ':     // CYRILLIC CAPITAL ZE W/ DESCENDER 0498
    case u'Қ':     // CYRILLIC CAPITAL KA W/ DESCENDER 049a
    case u'Ҝ':     // CYRILLIC CAPITAL KA W/ VERTICAL STROKE 049c
    case u'Ҟ':     // CYRILLIC CAPITAL KA W/ STROKE 049e
    case u'Ҡ':     // CYRILLIC CAPITAL BASHKIR KA 04a0
    case u'Ң':     // CYRILLIC CAPITAL EN W/ DESCENDER 04a2
    case u'Ҥ':     // CYRILLIC CAPITAL LIGATURE EN GHE 04a4
    case u'Ҧ':     // CYRILLIC CAPITAL PE W/ MIDDLE HOOK 04a6
    case u'Ҩ':     // CYRILLIC CAPITAL ABKHASIAN HA 04a8
    case u'Ҫ':     // CYRILLIC CAPITAL ES W/ DESCENDER 04aa
    case u'Ҭ':     // CYRILLIC CAPITAL TE W/ DESCENDER 04ac
    case u'Ү':     // CYRILLIC CAPITAL STRAIGHT U 04ae
    case u'Ұ':     // CYRILLIC CAPITAL STRAIGHT U W/ STROKE 04b0
    case u'Ҳ':     // CYRILLIC CAPITAL HA W/ DESCENDER 04b2
    case u'Ҵ':     // CYRILLIC CAPITAL LIGATURE TE TSE 04b4
    case u'Ҷ':     // CYRILLIC CAPITAL CHE W/ DESCENDER 04b6
    case u'Ҹ':     // CYRILLIC CAPITAL CHE W/ VERTICAL STROKE 04b8
    case u'Һ':     // CYRILLIC CAPITAL SHHA 04ba
    case u'Ҽ':     // CYRILLIC CAPITAL ABKHASIAN CHE 04bc
    case u'Ҿ':     // CYRILLIC CAPITAL ABKHASIAN CHE W/ DESCENDER 04be
    case u'Ӏ':     // CYRILLIC LETTER PALOCHKA 04c0
    case u'Ӂ':     // CYRILLIC CAPITAL ZHE W/ BREVE 04c1
    case u'Ӄ':     // CYRILLIC CAPITAL KA W/ HOOK 04c3
    case u'Ӆ':     // CYRILLIC CAPITAL EL W/ TAIL 04c5
    case u'Ӈ':     // CYRILLIC CAPITAL EN W/ HOOK 04c7
    case u'Ӊ':     // CYRILLIC CAPITAL EN W/ TAIL 04c9
    case u'Ӌ':     // CYRILLIC CAPITAL KHAKASSIAN CHE 04cb
    case u'Ӎ':     // CYRILLIC CAPITAL EM W/ TAIL 04cd
    case u'Ӑ':     // CYRILLIC CAPITAL A W/ BREVE 04d0
    case u'Ӓ':     // CYRILLIC CAPITAL A W/ DIAERESIS 04d2
    case u'Ӕ':     // CYRILLIC CAPITAL LIGATURE A IE 04d4
    case u'Ӗ':     // CYRILLIC CAPITAL IE W/ BREVE 04d6
    case u'Ә':     // CYRILLIC CAPITAL SCHWA 04d8
    case u'Ӛ':     // CYRILLIC CAPITAL SCHWA W/ DIAERESIS 04da
    case u'Ӝ':     // CYRILLIC CAPITAL ZHE W/ DIAERESIS 04dc
    case u'Ӟ':     // CYRILLIC CAPITAL ZE W/ DIAERESIS 04de
    case u'Ӡ':     // CYRILLIC CAPITAL ABKHASIAN DZE 04e0
    case u'Ӣ':     // CYRILLIC CAPITAL I W/ MACRON 04e2
    case u'Ӥ':     // CYRILLIC CAPITAL I W/ DIAERESIS 04e4
    case u'Ӧ':     // CYRILLIC CAPITAL O W/ DIAERESIS 04e6
    case u'Ө':     // CYRILLIC CAPITAL BARRED O 04e8
    case u'Ӫ':     // CYRILLIC CAPITAL BARRED O W/ DIAERESIS 04ea
    case u'Ӭ':     // CYRILLIC CAPITAL E W/ DIAERESIS 04ec
    case u'Ӯ':     // CYRILLIC CAPITAL U W/ MACRON 04ee
    case u'Ӱ':     // CYRILLIC CAPITAL U W/ DIAERESIS 04f0
    case u'Ӳ':     // CYRILLIC CAPITAL U W/ DOUBLE ACUTE 04f2
    case u'Ӵ':     // CYRILLIC CAPITAL CHE W/ DIAERESIS 04f4
    case u'Ӷ':     // CYRILLIC CAPITAL GHE W/ DESCENDER 04f6
    case u'Ӹ':     // CYRILLIC CAPITAL YERU W/ DIAERESIS 04f8
    case u'Ӻ':     // CYRILLIC CAPITAL GHE W/ STROKE AND HOOK 04fa
    case u'Ӽ':     // CYRILLIC CAPITAL HA W/ HOOK 04fc
    case u'Ӿ':     // CYRILLIC CAPITAL HA W/ STROKE 04fe
    case u'Ԁ':     // CYRILLIC CAPITAL KOMI DE 0500
    case u'Ԃ':     // CYRILLIC CAPITAL KOMI DJE 0502
    case u'Ԅ':     // CYRILLIC CAPITAL KOMI ZJE 0504
    case u'Ԇ':     // CYRILLIC CAPITAL KOMI DZJE 0506
    case u'Ԉ':     // CYRILLIC CAPITAL KOMI LJE 0508
    case u'Ԋ':     // CYRILLIC CAPITAL KOMI NJE 050a
    case u'Ԍ':     // CYRILLIC CAPITAL KOMI SJE 050c
    case u'Ԏ':     // CYRILLIC CAPITAL KOMI TJE 050e
    case u'Ԑ':     // CYRILLIC CAPITAL REVERSED ZE 0510
    case u'Ԓ':     // CYRILLIC CAPITAL EL W/ HOOK 0512
    case u'Ԕ':     // CYRILLIC CAPITAL LHA 0514
    case u'Ԗ':     // CYRILLIC CAPITAL RHA 0516
    case u'Ԙ':     // CYRILLIC CAPITAL YAE 0518
    case u'Ԛ':     // CYRILLIC CAPITAL QA 051a
    case u'Ԝ':     // CYRILLIC CAPITAL WE 051c
    case u'Ԟ':     // CYRILLIC CAPITAL ALEUT KA 051e
    case u'Ԡ':     // CYRILLIC CAPITAL EL W/ MIDDLE HOOK 0520
    case u'Ԣ':     // CYRILLIC CAPITAL EN W/ MIDDLE HOOK 0522
    case u'Ԥ':     // CYRILLIC CAPITAL PE W/ DESCENDER 0524
    case u'Ԧ':     // CYRILLIC CAPITAL SHHA W/ DESCENDER 0526
    case u'Ԩ':    // CYRILLIC CAPITAL EN W/ LEFT HOOK 0528
    case u'Ԫ':    // CYRILLIC CAPITAL DZZHE 052a
    case u'Ԭ':    // CYRILLIC CAPITAL DCHE 052c
    case u'Ԯ':    // CYRILLIC CAPITAL EL W/ DESCENDER 052e
    case u'Ա':     // ARMENIAN CAPITAL AYB 0531
    case u'Բ':     // ARMENIAN CAPITAL BEN 0532
    case u'Գ':     // ARMENIAN CAPITAL GIM 0533
    case u'Դ':     // ARMENIAN CAPITAL DA 0534
    case u'Ե':     // ARMENIAN CAPITAL ECH 0535
    case u'Զ':     // ARMENIAN CAPITAL ZA 0536
    case u'Է':     // ARMENIAN CAPITAL EH 0537
    case u'Ը':     // ARMENIAN CAPITAL ET 0538
    case u'Թ':     // ARMENIAN CAPITAL TO 0539
    case u'Ժ':     // ARMENIAN CAPITAL ZHE 053a
    case u'Ի':     // ARMENIAN CAPITAL INI 053b
    case u'Լ':     // ARMENIAN CAPITAL LIWN 053c
    case u'Խ':     // ARMENIAN CAPITAL XEH 053d
    case u'Ծ':     // ARMENIAN CAPITAL CA 053e
    case u'Կ':     // ARMENIAN CAPITAL KEN 053f
    case u'Հ':     // ARMENIAN CAPITAL HO 0540
    case u'Ձ':     // ARMENIAN CAPITAL JA 0541
    case u'Ղ':     // ARMENIAN CAPITAL GHAD 0542
    case u'Ճ':     // ARMENIAN CAPITAL CHEH 0543
    case u'Մ':     // ARMENIAN CAPITAL MEN 0544
    case u'Յ':     // ARMENIAN CAPITAL YI 0545
    case u'Ն':     // ARMENIAN CAPITAL NOW 0546
    case u'Շ':     // ARMENIAN CAPITAL SHA 0547
    case u'Ո':     // ARMENIAN CAPITAL VO 0548
    case u'Չ':     // ARMENIAN CAPITAL CHA 0549
    case u'Պ':     // ARMENIAN CAPITAL PEH 054a
    case u'Ջ':     // ARMENIAN CAPITAL JHEH 054b
    case u'Ռ':     // ARMENIAN CAPITAL RA 054c
    case u'Ս':     // ARMENIAN CAPITAL SEH 054d
    case u'Վ':     // ARMENIAN CAPITAL VEW 054e
    case u'Տ':     // ARMENIAN CAPITAL TIWN 054f
    case u'Ր':     // ARMENIAN CAPITAL REH 0550
    case u'Ց':     // ARMENIAN CAPITAL CO 0551
    case u'Ւ':     // ARMENIAN CAPITAL YIWN 0552
    case u'Փ':     // ARMENIAN CAPITAL PIWR 0553
    case u'Ք':     // ARMENIAN CAPITAL KEH 0554
    case u'Օ':     // ARMENIAN CAPITAL OH 0555
    case u'Ֆ':     // ARMENIAN CAPITAL FEH 0556
    case u'Ⴀ':     // GEORGIAN CAPITAL AN 10a0
    case u'Ⴁ':     // GEORGIAN CAPITAL BAN 10a1
    case u'Ⴂ':     // GEORGIAN CAPITAL GAN 10a2
    case u'Ⴃ':     // GEORGIAN CAPITAL DON 10a3
    case u'Ⴄ':     // GEORGIAN CAPITAL EN 10a4
    case u'Ⴅ':     // GEORGIAN CAPITAL VIN 10a5
    case u'Ⴆ':     // GEORGIAN CAPITAL ZEN 10a6
    case u'Ⴇ':     // GEORGIAN CAPITAL TAN 10a7
    case u'Ⴈ':     // GEORGIAN CAPITAL IN 10a8
    case u'Ⴉ':     // GEORGIAN CAPITAL KAN 10a9
    case u'Ⴊ':     // GEORGIAN CAPITAL LAS 10aa
    case u'Ⴋ':     // GEORGIAN CAPITAL MAN 10ab
    case u'Ⴌ':     // GEORGIAN CAPITAL NAR 10ac
    case u'Ⴍ':     // GEORGIAN CAPITAL ON 10ad
    case u'Ⴎ':     // GEORGIAN CAPITAL PAR 10ae
    case u'Ⴏ':     // GEORGIAN CAPITAL ZHAR 10af
    case u'Ⴐ':     // GEORGIAN CAPITAL RAE 10b0
    case u'Ⴑ':     // GEORGIAN CAPITAL SAN 10b1
    case u'Ⴒ':     // GEORGIAN CAPITAL TAR 10b2
    case u'Ⴓ':     // GEORGIAN CAPITAL UN 10b3
    case u'Ⴔ':     // GEORGIAN CAPITAL PHAR 10b4
    case u'Ⴕ':     // GEORGIAN CAPITAL KHAR 10b5
    case u'Ⴖ':     // GEORGIAN CAPITAL GHAN 10b6
    case u'Ⴗ':     // GEORGIAN CAPITAL QAR 10b7
    case u'Ⴘ':     // GEORGIAN CAPITAL SHIN 10b8
    case u'Ⴙ':     // GEORGIAN CAPITAL CHIN 10b9
    case u'Ⴚ':     // GEORGIAN CAPITAL CAN 10ba
    case u'Ⴛ':     // GEORGIAN CAPITAL JIL 10bb
    case u'Ⴜ':     // GEORGIAN CAPITAL CIL 10bc
    case u'Ⴝ':     // GEORGIAN CAPITAL CHAR 10bd
    case u'Ⴞ':     // GEORGIAN CAPITAL XAN 10be
    case u'Ⴟ':     // GEORGIAN CAPITAL JHAN 10bf
    case u'Ⴠ':     // GEORGIAN CAPITAL HAE 10c0
    case u'Ⴡ':     // GEORGIAN CAPITAL HE 10c1
    case u'Ⴢ':     // GEORGIAN CAPITAL HIE 10c2
    case u'Ⴣ':     // GEORGIAN CAPITAL WE 10c3
    case u'Ⴤ':     // GEORGIAN CAPITAL HAR 10c4
    case u'Ⴥ':     // GEORGIAN CAPITAL HOE 10c5
    case u'Ⴧ':     // GEORGIAN CAPITAL YN 10c7
    case u'Ⴭ':     // GEORGIAN CAPITAL AEN 10cd
    case u'Ꭰ':     // CHEROKEE LETTER A 13a0
    case u'Ꭱ':     // CHEROKEE LETTER E 13a1
    case u'Ꭲ':     // CHEROKEE LETTER I 13a2
    case u'Ꭳ':     // CHEROKEE LETTER O 13a3
    case u'Ꭴ':     // CHEROKEE LETTER U 13a4
    case u'Ꭵ':     // CHEROKEE LETTER V 13a5
    case u'Ꭶ':     // CHEROKEE LETTER GA 13a6
    case u'Ꭷ':     // CHEROKEE LETTER KA 13a7
    case u'Ꭸ':     // CHEROKEE LETTER GE 13a8
    case u'Ꭹ':     // CHEROKEE LETTER GI 13a9
    case u'Ꭺ':     // CHEROKEE LETTER GO 13aa
    case u'Ꭻ':     // CHEROKEE LETTER GU 13ab
    case u'Ꭼ':     // CHEROKEE LETTER GV 13ac
    case u'Ꭽ':     // CHEROKEE LETTER HA 13ad
    case u'Ꭾ':     // CHEROKEE LETTER HE 13ae
    case u'Ꭿ':     // CHEROKEE LETTER HI 13af
    case u'Ꮀ':     // CHEROKEE LETTER HO 13b0
    case u'Ꮁ':     // CHEROKEE LETTER HU 13b1
    case u'Ꮂ':     // CHEROKEE LETTER HV 13b2
    case u'Ꮃ':     // CHEROKEE LETTER LA 13b3
    case u'Ꮄ':     // CHEROKEE LETTER LE 13b4
    case u'Ꮅ':     // CHEROKEE LETTER LI 13b5
    case u'Ꮆ':     // CHEROKEE LETTER LO 13b6
    case u'Ꮇ':     // CHEROKEE LETTER LU 13b7
    case u'Ꮈ':     // CHEROKEE LETTER LV 13b8
    case u'Ꮉ':     // CHEROKEE LETTER MA 13b9
    case u'Ꮊ':     // CHEROKEE LETTER ME 13ba
    case u'Ꮋ':     // CHEROKEE LETTER MI 13bb
    case u'Ꮌ':     // CHEROKEE LETTER MO 13bc
    case u'Ꮍ':     // CHEROKEE LETTER MU 13bd
    case u'Ꮎ':     // CHEROKEE LETTER NA 13be
    case u'Ꮏ':     // CHEROKEE LETTER HNA 13bf
    case u'Ꮐ':     // CHEROKEE LETTER NAH 13c0
    case u'Ꮑ':     // CHEROKEE LETTER NE 13c1
    case u'Ꮒ':     // CHEROKEE LETTER NI 13c2
    case u'Ꮓ':     // CHEROKEE LETTER NO 13c3
    case u'Ꮔ':     // CHEROKEE LETTER NU 13c4
    case u'Ꮕ':     // CHEROKEE LETTER NV 13c5
    case u'Ꮖ':     // CHEROKEE LETTER QUA 13c6
    case u'Ꮗ':     // CHEROKEE LETTER QUE 13c7
    case u'Ꮘ':     // CHEROKEE LETTER QUI 13c8
    case u'Ꮙ':     // CHEROKEE LETTER QUO 13c9
    case u'Ꮚ':     // CHEROKEE LETTER QUU 13ca
    case u'Ꮛ':     // CHEROKEE LETTER QUV 13cb
    case u'Ꮜ':     // CHEROKEE LETTER SA 13cc
    case u'Ꮝ':     // CHEROKEE LETTER S 13cd
    case u'Ꮞ':     // CHEROKEE LETTER SE 13ce
    case u'Ꮟ':     // CHEROKEE LETTER SI 13cf
    case u'Ꮠ':     // CHEROKEE LETTER SO 13d0
    case u'Ꮡ':     // CHEROKEE LETTER SU 13d1
    case u'Ꮢ':     // CHEROKEE LETTER SV 13d2
    case u'Ꮣ':     // CHEROKEE LETTER DA 13d3
    case u'Ꮤ':     // CHEROKEE LETTER TA 13d4
    case u'Ꮥ':     // CHEROKEE LETTER DE 13d5
    case u'Ꮦ':     // CHEROKEE LETTER TE 13d6
    case u'Ꮧ':     // CHEROKEE LETTER DI 13d7
    case u'Ꮨ':     // CHEROKEE LETTER TI 13d8
    case u'Ꮩ':     // CHEROKEE LETTER DO 13d9
    case u'Ꮪ':     // CHEROKEE LETTER DU 13da
    case u'Ꮫ':     // CHEROKEE LETTER DV 13db
    case u'Ꮬ':     // CHEROKEE LETTER DLA 13dc
    case u'Ꮭ':     // CHEROKEE LETTER TLA 13dd
    case u'Ꮮ':     // CHEROKEE LETTER TLE 13de
    case u'Ꮯ':     // CHEROKEE LETTER TLI 13df
    case u'Ꮰ':     // CHEROKEE LETTER TLO 13e0
    case u'Ꮱ':     // CHEROKEE LETTER TLU 13e1
    case u'Ꮲ':     // CHEROKEE LETTER TLV 13e2
    case u'Ꮳ':     // CHEROKEE LETTER TSA 13e3
    case u'Ꮴ':     // CHEROKEE LETTER TSE 13e4
    case u'Ꮵ':     // CHEROKEE LETTER TSI 13e5
    case u'Ꮶ':     // CHEROKEE LETTER TSO 13e6
    case u'Ꮷ':     // CHEROKEE LETTER TSU 13e7
    case u'Ꮸ':     // CHEROKEE LETTER TSV 13e8
    case u'Ꮹ':     // CHEROKEE LETTER WA 13e9
    case u'Ꮺ':     // CHEROKEE LETTER WE 13ea
    case u'Ꮻ':     // CHEROKEE LETTER WI 13eb
    case u'Ꮼ':     // CHEROKEE LETTER WO 13ec
    case u'Ꮽ':     // CHEROKEE LETTER WU 13ed
    case u'Ꮾ':     // CHEROKEE LETTER WV 13ee
    case u'Ꮿ':     // CHEROKEE LETTER YA 13ef
    case u'Ᏸ':     // CHEROKEE LETTER YE 13f0
    case u'Ᏹ':     // CHEROKEE LETTER YI 13f1
    case u'Ᏺ':     // CHEROKEE LETTER YO 13f2
    case u'Ᏻ':     // CHEROKEE LETTER YU 13f3
    case u'Ᏼ':     // CHEROKEE LETTER YV 13f4
    case u'Ᏽ':   // CHEROKEE LETTER MV 13f5
    case u'Ა':   // GEORGIAN MTAVRULI CAPITAL AN 1c90
    case u'Ბ':   // GEORGIAN MTAVRULI CAPITAL BAN 1c91
    case u'Გ':   // GEORGIAN MTAVRULI CAPITAL GAN 1c92
    case u'Დ':   // GEORGIAN MTAVRULI CAPITAL DON 1c93
    case u'Ე':   // GEORGIAN MTAVRULI CAPITAL EN 1c94
    case u'Ვ':   // GEORGIAN MTAVRULI CAPITAL VIN 1c95
    case u'Ზ':   // GEORGIAN MTAVRULI CAPITAL ZEN 1c96
    case u'Თ':   // GEORGIAN MTAVRULI CAPITAL TAN 1c97
    case u'Ი':   // GEORGIAN MTAVRULI CAPITAL IN 1c98
    case u'Კ':   // GEORGIAN MTAVRULI CAPITAL KAN 1c99
    case u'Ლ':   // GEORGIAN MTAVRULI CAPITAL LAS 1c9a
    case u'Მ':   // GEORGIAN MTAVRULI CAPITAL MAN 1c9b
    case u'Ნ':   // GEORGIAN MTAVRULI CAPITAL NAR 1c9c
    case u'Ო':   // GEORGIAN MTAVRULI CAPITAL ON 1c9d
    case u'Პ':   // GEORGIAN MTAVRULI CAPITAL PAR 1c9e
    case u'Ჟ':   // GEORGIAN MTAVRULI CAPITAL ZHAR 1c9f
    case u'Რ':   // GEORGIAN MTAVRULI CAPITAL RAE 1ca0
    case u'Ს':   // GEORGIAN MTAVRULI CAPITAL SAN 1ca1
    case u'Ტ':   // GEORGIAN MTAVRULI CAPITAL TAR 1ca2
    case u'Უ':   // GEORGIAN MTAVRULI CAPITAL UN 1ca3
    case u'Ფ':   // GEORGIAN MTAVRULI CAPITAL PHAR 1ca4
    case u'Ქ':   // GEORGIAN MTAVRULI CAPITAL KHAR 1ca5
    case u'Ღ':   // GEORGIAN MTAVRULI CAPITAL GHAN 1ca6
    case u'Ყ':   // GEORGIAN MTAVRULI CAPITAL QAR 1ca7
    case u'Შ':   // GEORGIAN MTAVRULI CAPITAL SHIN 1ca8
    case u'Ჩ':   // GEORGIAN MTAVRULI CAPITAL CHIN 1ca9
    case u'Ც':   // GEORGIAN MTAVRULI CAPITAL CAN 1caa
    case u'Ძ':   // GEORGIAN MTAVRULI CAPITAL JIL 1cab
    case u'Წ':   // GEORGIAN MTAVRULI CAPITAL CIL 1cac
    case u'Ჭ':   // GEORGIAN MTAVRULI CAPITAL CHAR 1cad
    case u'Ხ':   // GEORGIAN MTAVRULI CAPITAL XAN 1cae
    case u'Ჯ':   // GEORGIAN MTAVRULI CAPITAL JHAN 1caf
    case u'Ჰ':   // GEORGIAN MTAVRULI CAPITAL HAE 1cb0
    case u'Ჱ':   // GEORGIAN MTAVRULI CAPITAL HE 1cb1
    case u'Ჲ':   // GEORGIAN MTAVRULI CAPITAL HIE 1cb2
    case u'Ჳ':   // GEORGIAN MTAVRULI CAPITAL WE 1cb3
    case u'Ჴ':   // GEORGIAN MTAVRULI CAPITAL HAR 1cb4
    case u'Ჵ':   // GEORGIAN MTAVRULI CAPITAL HOE 1cb5
    case u'Ჶ':   // GEORGIAN MTAVRULI CAPITAL FI 1cb6
    case u'Ჷ':   // GEORGIAN MTAVRULI CAPITAL YN 1cb7
    case u'Ჸ':   // GEORGIAN MTAVRULI CAPITAL ELIFI 1cb8
    case u'Ჹ':   // GEORGIAN MTAVRULI CAPITAL TURNED GAN 1cb9
    case u'Ჺ':   // GEORGIAN MTAVRULI CAPITAL AIN 1cba
    case u'Ჽ':   // GEORGIAN MTAVRULI CAPITAL AEN 1cbd
    case u'Ჾ':   // GEORGIAN MTAVRULI CAPITAL HARD SIGN 1cbe
    case u'Ჿ':   // GEORGIAN MTAVRULI CAPITAL LABIAL SIGN 1cbf
    case u'Ḁ':     // LATIN CAPITAL A W/ RING BELOW 1e00
    case u'Ḃ':     // LATIN CAPITAL B W/ DOT ABOVE 1e02
    case u'Ḅ':     // LATIN CAPITAL B W/ DOT BELOW 1e04
    case u'Ḇ':     // LATIN CAPITAL B W/ LINE BELOW 1e06
    case u'Ḉ':     // LATIN CAPITAL C W/ CEDILLA AND ACUTE 1e08
    case u'Ḋ':     // LATIN CAPITAL D W/ DOT ABOVE 1e0a
    case u'Ḍ':     // LATIN CAPITAL D W/ DOT BELOW 1e0c
    case u'Ḏ':     // LATIN CAPITAL D W/ LINE BELOW 1e0e
    case u'Ḑ':     // LATIN CAPITAL D W/ CEDILLA 1e10
    case u'Ḓ':     // LATIN CAPITAL D W/ CIRCUMFLEX BELOW 1e12
    case u'Ḕ':     // LATIN CAPITAL E W/ MACRON AND GRAVE 1e14
    case u'Ḗ':     // LATIN CAPITAL E W/ MACRON AND ACUTE 1e16
    case u'Ḙ':     // LATIN CAPITAL E W/ CIRCUMFLEX BELOW 1e18
    case u'Ḛ':     // LATIN CAPITAL E W/ TILDE BELOW 1e1a
    case u'Ḝ':     // LATIN CAPITAL E W/ CEDILLA AND BREVE 1e1c
    case u'Ḟ':     // LATIN CAPITAL F W/ DOT ABOVE 1e1e
    case u'Ḡ':     // LATIN CAPITAL G W/ MACRON 1e20
    case u'Ḣ':     // LATIN CAPITAL H W/ DOT ABOVE 1e22
    case u'Ḥ':     // LATIN CAPITAL H W/ DOT BELOW 1e24
    case u'Ḧ':     // LATIN CAPITAL H W/ DIAERESIS 1e26
    case u'Ḩ':     // LATIN CAPITAL H W/ CEDILLA 1e28
    case u'Ḫ':     // LATIN CAPITAL H W/ BREVE BELOW 1e2a
    case u'Ḭ':     // LATIN CAPITAL I W/ TILDE BELOW 1e2c
    case u'Ḯ':     // LATIN CAPITAL I W/ DIAERESIS AND ACUTE 1e2e
    case u'Ḱ':     // LATIN CAPITAL K W/ ACUTE 1e30
    case u'Ḳ':     // LATIN CAPITAL K W/ DOT BELOW 1e32
    case u'Ḵ':     // LATIN CAPITAL K W/ LINE BELOW 1e34
    case u'Ḷ':     // LATIN CAPITAL L W/ DOT BELOW 1e36
    case u'Ḹ':     // LATIN CAPITAL L W/ DOT BELOW AND MACRON 1e38
    case u'Ḻ':     // LATIN CAPITAL L W/ LINE BELOW 1e3a
    case u'Ḽ':     // LATIN CAPITAL L W/ CIRCUMFLEX BELOW 1e3c
    case u'Ḿ':     // LATIN CAPITAL M W/ ACUTE 1e3e
    case u'Ṁ':     // LATIN CAPITAL M W/ DOT ABOVE 1e40
    case u'Ṃ':     // LATIN CAPITAL M W/ DOT BELOW 1e42
    case u'Ṅ':     // LATIN CAPITAL N W/ DOT ABOVE 1e44
    case u'Ṇ':     // LATIN CAPITAL N W/ DOT BELOW 1e46
    case u'Ṉ':     // LATIN CAPITAL N W/ LINE BELOW 1e48
    case u'Ṋ':     // LATIN CAPITAL N W/ CIRCUMFLEX BELOW 1e4a
    case u'Ṍ':     // LATIN CAPITAL O W/ TILDE AND ACUTE 1e4c
    case u'Ṏ':     // LATIN CAPITAL O W/ TILDE AND DIAERESIS 1e4e
    case u'Ṑ':     // LATIN CAPITAL O W/ MACRON AND GRAVE 1e50
    case u'Ṓ':     // LATIN CAPITAL O W/ MACRON AND ACUTE 1e52
    case u'Ṕ':     // LATIN CAPITAL P W/ ACUTE 1e54
    case u'Ṗ':     // LATIN CAPITAL P W/ DOT ABOVE 1e56
    case u'Ṙ':     // LATIN CAPITAL R W/ DOT ABOVE 1e58
    case u'Ṛ':     // LATIN CAPITAL R W/ DOT BELOW 1e5a
    case u'Ṝ':     // LATIN CAPITAL R W/ DOT BELOW AND MACRON 1e5c
    case u'Ṟ':     // LATIN CAPITAL R W/ LINE BELOW 1e5e
    case u'Ṡ':     // LATIN CAPITAL S W/ DOT ABOVE 1e60
    case u'Ṣ':     // LATIN CAPITAL S W/ DOT BELOW 1e62
    case u'Ṥ':     // LATIN CAPITAL S W/ ACUTE AND DOT ABOVE 1e64
    case u'Ṧ':     // LATIN CAPITAL S W/ CARON AND DOT ABOVE 1e66
    case u'Ṩ':     // LATIN CAPITAL S W/ DOT BELOW AND DOT ABOVE 1e68
    case u'Ṫ':     // LATIN CAPITAL T W/ DOT ABOVE 1e6a
    case u'Ṭ':     // LATIN CAPITAL T W/ DOT BELOW 1e6c
    case u'Ṯ':     // LATIN CAPITAL T W/ LINE BELOW 1e6e
    case u'Ṱ':     // LATIN CAPITAL T W/ CIRCUMFLEX BELOW 1e70
    case u'Ṳ':     // LATIN CAPITAL U W/ DIAERESIS BELOW 1e72
    case u'Ṵ':     // LATIN CAPITAL U W/ TILDE BELOW 1e74
    case u'Ṷ':     // LATIN CAPITAL U W/ CIRCUMFLEX BELOW 1e76
    case u'Ṹ':     // LATIN CAPITAL U W/ TILDE AND ACUTE 1e78
    case u'Ṻ':     // LATIN CAPITAL U W/ MACRON AND DIAERESIS 1e7a
    case u'Ṽ':     // LATIN CAPITAL V W/ TILDE 1e7c
    case u'Ṿ':     // LATIN CAPITAL V W/ DOT BELOW 1e7e
    case u'Ẁ':     // LATIN CAPITAL W W/ GRAVE 1e80
    case u'Ẃ':     // LATIN CAPITAL W W/ ACUTE 1e82
    case u'Ẅ':     // LATIN CAPITAL W W/ DIAERESIS 1e84
    case u'Ẇ':     // LATIN CAPITAL W W/ DOT ABOVE 1e86
    case u'Ẉ':     // LATIN CAPITAL W W/ DOT BELOW 1e88
    case u'Ẋ':     // LATIN CAPITAL X W/ DOT ABOVE 1e8a
    case u'Ẍ':     // LATIN CAPITAL X W/ DIAERESIS 1e8c
    case u'Ẏ':     // LATIN CAPITAL Y W/ DOT ABOVE 1e8e
    case u'Ẑ':     // LATIN CAPITAL Z W/ CIRCUMFLEX 1e90
    case u'Ẓ':     // LATIN CAPITAL Z W/ DOT BELOW 1e92
    case u'Ẕ':     // LATIN CAPITAL Z W/ LINE BELOW 1e94
    case u'ẞ':     // LATIN CAPITAL SHARP S 1e9e
    case u'Ạ':     // LATIN CAPITAL A W/ DOT BELOW 1ea0
    case u'Ả':     // LATIN CAPITAL A W/ HOOK ABOVE 1ea2
    case u'Ấ':     // LATIN CAPITAL A W/ CIRCUMFLEX AND ACUTE 1ea4
    case u'Ầ':     // LATIN CAPITAL A W/ CIRCUMFLEX AND GRAVE 1ea6
    case u'Ẩ':     // LATIN CAPITAL A W/ CIRCUMFLEX AND HOOK ABOVE 1ea8
    case u'Ẫ':     // LATIN CAPITAL A W/ CIRCUMFLEX AND TILDE 1eaa
    case u'Ậ':     // LATIN CAPITAL A W/ CIRCUMFLEX AND DOT BELOW 1eac
    case u'Ắ':     // LATIN CAPITAL A W/ BREVE AND ACUTE 1eae
    case u'Ằ':     // LATIN CAPITAL A W/ BREVE AND GRAVE 1eb0
    case u'Ẳ':     // LATIN CAPITAL A W/ BREVE AND HOOK ABOVE 1eb2
    case u'Ẵ':     // LATIN CAPITAL A W/ BREVE AND TILDE 1eb4
    case u'Ặ':     // LATIN CAPITAL A W/ BREVE AND DOT BELOW 1eb6
    case u'Ẹ':     // LATIN CAPITAL E W/ DOT BELOW 1eb8
    case u'Ẻ':     // LATIN CAPITAL E W/ HOOK ABOVE 1eba
    case u'Ẽ':     // LATIN CAPITAL E W/ TILDE 1ebc
    case u'Ế':     // LATIN CAPITAL E W/ CIRCUMFLEX AND ACUTE 1ebe
    case u'Ề':     // LATIN CAPITAL E W/ CIRCUMFLEX AND GRAVE 1ec0
    case u'Ể':     // LATIN CAPITAL E W/ CIRCUMFLEX AND HOOK ABOVE 1ec2
    case u'Ễ':     // LATIN CAPITAL E W/ CIRCUMFLEX AND TILDE 1ec4
    case u'Ệ':     // LATIN CAPITAL E W/ CIRCUMFLEX AND DOT BELOW 1ec6
    case u'Ỉ':     // LATIN CAPITAL I W/ HOOK ABOVE 1ec8
    case u'Ị':     // LATIN CAPITAL I W/ DOT BELOW 1eca
    case u'Ọ':     // LATIN CAPITAL O W/ DOT BELOW 1ecc
    case u'Ỏ':     // LATIN CAPITAL O W/ HOOK ABOVE 1ece
    case u'Ố':     // LATIN CAPITAL O W/ CIRCUMFLEX AND ACUTE 1ed0
    case u'Ồ':     // LATIN CAPITAL O W/ CIRCUMFLEX AND GRAVE 1ed2
    case u'Ổ':     // LATIN CAPITAL O W/ CIRCUMFLEX AND HOOK ABOVE 1ed4
    case u'Ỗ':     // LATIN CAPITAL O W/ CIRCUMFLEX AND TILDE 1ed6
    case u'Ộ':     // LATIN CAPITAL O W/ CIRCUMFLEX AND DOT BELOW 1ed8
    case u'Ớ':     // LATIN CAPITAL O W/ HORN AND ACUTE 1eda
    case u'Ờ':     // LATIN CAPITAL O W/ HORN AND GRAVE 1edc
    case u'Ở':     // LATIN CAPITAL O W/ HORN AND HOOK ABOVE 1ede
    case u'Ỡ':     // LATIN CAPITAL O W/ HORN AND TILDE 1ee0
    case u'Ợ':     // LATIN CAPITAL O W/ HORN AND DOT BELOW 1ee2
    case u'Ụ':     // LATIN CAPITAL U W/ DOT BELOW 1ee4
    case u'Ủ':     // LATIN CAPITAL U W/ HOOK ABOVE 1ee6
    case u'Ứ':     // LATIN CAPITAL U W/ HORN AND ACUTE 1ee8
    case u'Ừ':     // LATIN CAPITAL U W/ HORN AND GRAVE 1eea
    case u'Ử':     // LATIN CAPITAL U W/ HORN AND HOOK ABOVE 1eec
    case u'Ữ':     // LATIN CAPITAL U W/ HORN AND TILDE 1eee
    case u'Ự':     // LATIN CAPITAL U W/ HORN AND DOT BELOW 1ef0
    case u'Ỳ':     // LATIN CAPITAL Y W/ GRAVE 1ef2
    case u'Ỵ':     // LATIN CAPITAL Y W/ DOT BELOW 1ef4
    case u'Ỷ':     // LATIN CAPITAL Y W/ HOOK ABOVE 1ef6
    case u'Ỹ':     // LATIN CAPITAL Y W/ TILDE 1ef8
    case u'Ỻ':     // LATIN CAPITAL MIDDLE-WELSH LL 1efa
    case u'Ỽ':     // LATIN CAPITAL MIDDLE-WELSH V 1efc
    case u'Ỿ':     // LATIN CAPITAL Y W/ LOOP 1efe
    case u'Ἀ':     // GREEK CAPITAL ALPHA W/ PSILI 1f08
    case u'Ἁ':     // GREEK CAPITAL ALPHA W/ DASIA 1f09
    case u'Ἂ':     // GREEK CAPITAL ALPHA W/ PSILI AND VARIA 1f0a
    case u'Ἃ':     // GREEK CAPITAL ALPHA W/ DASIA AND VARIA 1f0b
    case u'Ἄ':     // GREEK CAPITAL ALPHA W/ PSILI AND OXIA 1f0c
    case u'Ἅ':     // GREEK CAPITAL ALPHA W/ DASIA AND OXIA 1f0d
    case u'Ἆ':     // GREEK CAPITAL ALPHA W/ PSILI AND PERISPOMENI 1f0e
    case u'Ἇ':     // GREEK CAPITAL ALPHA W/ DASIA AND PERISPOMENI 1f0f
    case u'Ἐ':     // GREEK CAPITAL EPSILON W/ PSILI 1f18
    case u'Ἑ':     // GREEK CAPITAL EPSILON W/ DASIA 1f19
    case u'Ἒ':     // GREEK CAPITAL EPSILON W/ PSILI AND VARIA 1f1a
    case u'Ἓ':     // GREEK CAPITAL EPSILON W/ DASIA AND VARIA 1f1b
    case u'Ἔ':     // GREEK CAPITAL EPSILON W/ PSILI AND OXIA 1f1c
    case u'Ἕ':     // GREEK CAPITAL EPSILON W/ DASIA AND OXIA 1f1d
    case u'Ἠ':     // GREEK CAPITAL ETA W/ PSILI 1f28
    case u'Ἡ':     // GREEK CAPITAL ETA W/ DASIA 1f29
    case u'Ἢ':     // GREEK CAPITAL ETA W/ PSILI AND VARIA 1f2a
    case u'Ἣ':     // GREEK CAPITAL ETA W/ DASIA AND VARIA 1f2b
    case u'Ἤ':     // GREEK CAPITAL ETA W/ PSILI AND OXIA 1f2c
    case u'Ἥ':     // GREEK CAPITAL ETA W/ DASIA AND OXIA 1f2d
    case u'Ἦ':     // GREEK CAPITAL ETA W/ PSILI AND PERISPOMENI 1f2e
    case u'Ἧ':     // GREEK CAPITAL ETA W/ DASIA AND PERISPOMENI 1f2f
    case u'Ἰ':     // GREEK CAPITAL IOTA W/ PSILI 1f38
    case u'Ἱ':     // GREEK CAPITAL IOTA W/ DASIA 1f39
    case u'Ἲ':     // GREEK CAPITAL IOTA W/ PSILI AND VARIA 1f3a
    case u'Ἳ':     // GREEK CAPITAL IOTA W/ DASIA AND VARIA 1f3b
    case u'Ἴ':     // GREEK CAPITAL IOTA W/ PSILI AND OXIA 1f3c
    case u'Ἵ':     // GREEK CAPITAL IOTA W/ DASIA AND OXIA 1f3d
    case u'Ἶ':     // GREEK CAPITAL IOTA W/ PSILI AND PERISPOMENI 1f3e
    case u'Ἷ':     // GREEK CAPITAL IOTA W/ DASIA AND PERISPOMENI 1f3f
    case u'Ὀ':     // GREEK CAPITAL OMICRON W/ PSILI 1f48
    case u'Ὁ':     // GREEK CAPITAL OMICRON W/ DASIA 1f49
    case u'Ὂ':     // GREEK CAPITAL OMICRON W/ PSILI AND VARIA 1f4a
    case u'Ὃ':     // GREEK CAPITAL OMICRON W/ DASIA AND VARIA 1f4b
    case u'Ὄ':     // GREEK CAPITAL OMICRON W/ PSILI AND OXIA 1f4c
    case u'Ὅ':     // GREEK CAPITAL OMICRON W/ DASIA AND OXIA 1f4d
    case u'Ὑ':     // GREEK CAPITAL UPSILON W/ DASIA 1f59
    case u'Ὓ':     // GREEK CAPITAL UPSILON W/ DASIA AND VARIA 1f5b
    case u'Ὕ':     // GREEK CAPITAL UPSILON W/ DASIA AND OXIA 1f5d
    case u'Ὗ':     // GREEK CAPITAL UPSILON W/ DASIA AND PERISPOMENI 1f5f
    case u'Ὠ':     // GREEK CAPITAL OMEGA W/ PSILI 1f68
    case u'Ὡ':     // GREEK CAPITAL OMEGA W/ DASIA 1f69
    case u'Ὢ':     // GREEK CAPITAL OMEGA W/ PSILI AND VARIA 1f6a
    case u'Ὣ':     // GREEK CAPITAL OMEGA W/ DASIA AND VARIA 1f6b
    case u'Ὤ':     // GREEK CAPITAL OMEGA W/ PSILI AND OXIA 1f6c
    case u'Ὥ':     // GREEK CAPITAL OMEGA W/ DASIA AND OXIA 1f6d
    case u'Ὦ':     // GREEK CAPITAL OMEGA W/ PSILI AND PERISPOMENI 1f6e
    case u'Ὧ':     // GREEK CAPITAL OMEGA W/ DASIA AND PERISPOMENI 1f6f
    case u'Ᾰ':     // GREEK CAPITAL ALPHA W/ VRACHY 1fb8
    case u'Ᾱ':     // GREEK CAPITAL ALPHA W/ MACRON 1fb9
    case u'Ὰ':     // GREEK CAPITAL ALPHA W/ VARIA 1fba
    case u'Ά':     // GREEK CAPITAL ALPHA W/ OXIA 1fbb
    case u'Ὲ':     // GREEK CAPITAL EPSILON W/ VARIA 1fc8
    case u'Έ':     // GREEK CAPITAL EPSILON W/ OXIA 1fc9
    case u'Ὴ':     // GREEK CAPITAL ETA W/ VARIA 1fca
    case u'Ή':     // GREEK CAPITAL ETA W/ OXIA 1fcb
    case u'Ῐ':     // GREEK CAPITAL IOTA W/ VRACHY 1fd8
    case u'Ῑ':     // GREEK CAPITAL IOTA W/ MACRON 1fd9
    case u'Ὶ':     // GREEK CAPITAL IOTA W/ VARIA 1fda
    case u'Ί':     // GREEK CAPITAL IOTA W/ OXIA 1fdb
    case u'Ῠ':     // GREEK CAPITAL UPSILON W/ VRACHY 1fe8
    case u'Ῡ':     // GREEK CAPITAL UPSILON W/ MACRON 1fe9
    case u'Ὺ':     // GREEK CAPITAL UPSILON W/ VARIA 1fea
    case u'Ύ':     // GREEK CAPITAL UPSILON W/ OXIA 1feb
    case u'Ῥ':     // GREEK CAPITAL RHO W/ DASIA 1fec
    case u'Ὸ':     // GREEK CAPITAL OMICRON W/ VARIA 1ff8
    case u'Ό':     // GREEK CAPITAL OMICRON W/ OXIA 1ff9
    case u'Ὼ':     // GREEK CAPITAL OMEGA W/ VARIA 1ffa
    case u'Ώ':     // GREEK CAPITAL OMEGA W/ OXIA 1ffb
    case u'ℂ':     // DOUBLE-STRUCK CAPITAL C 2102
    case u'ℇ':     // EULER CONSTANT 2107
    case u'ℋ':     // SCRIPT CAPITAL H 210b
    case u'ℌ':     // BLACK-LETTER CAPITAL H 210c
    case u'ℍ':     // DOUBLE-STRUCK CAPITAL H 210d
    case u'ℐ':     // SCRIPT CAPITAL I 2110
    case u'ℑ':     // BLACK-LETTER CAPITAL I 2111
    case u'ℒ':     // SCRIPT CAPITAL L 2112
    case u'ℕ':     // DOUBLE-STRUCK CAPITAL N 2115
    case u'ℙ':     // DOUBLE-STRUCK CAPITAL P 2119
    case u'ℚ':     // DOUBLE-STRUCK CAPITAL Q 211a
    case u'ℛ':     // SCRIPT CAPITAL R 211b
    case u'ℜ':     // BLACK-LETTER CAPITAL R 211c
    case u'ℝ':     // DOUBLE-STRUCK CAPITAL R 211d
    case u'ℤ':     // DOUBLE-STRUCK CAPITAL Z 2124
    case u'Ω':     // OHM SIGN 2126
    case u'ℨ':     // BLACK-LETTER CAPITAL Z 2128
    case u'K':     // KELVIN SIGN 212a
    case u'Å':     // ANGSTROM SIGN 212b
    case u'ℬ':     // SCRIPT CAPITAL B 212c
    case u'ℭ':     // BLACK-LETTER CAPITAL C 212d
    case u'ℰ':     // SCRIPT CAPITAL E 2130
    case u'ℱ':     // SCRIPT CAPITAL F 2131
    case u'Ⅎ':     // TURNED CAPITAL F 2132
    case u'ℳ':     // SCRIPT CAPITAL M 2133
    case u'ℾ':     // DOUBLE-STRUCK CAPITAL GAMMA 213e
    case u'ℿ':     // DOUBLE-STRUCK CAPITAL PI 213f
    case u'ⅅ':     // DOUBLE-STRUCK ITALIC CAPITAL D 2145
    case u'Ↄ':     // ROMAN NUMERAL REVERSED ONE HUNDRED 2183
    case u'Ⰰ':     // GLAGOLITIC CAPITAL AZU 2c00
    case u'Ⰱ':     // GLAGOLITIC CAPITAL BUKY 2c01
    case u'Ⰲ':     // GLAGOLITIC CAPITAL VEDE 2c02
    case u'Ⰳ':     // GLAGOLITIC CAPITAL GLAGOLI 2c03
    case u'Ⰴ':     // GLAGOLITIC CAPITAL DOBRO 2c04
    case u'Ⰵ':     // GLAGOLITIC CAPITAL YESTU 2c05
    case u'Ⰶ':     // GLAGOLITIC CAPITAL ZHIVETE 2c06
    case u'Ⰷ':     // GLAGOLITIC CAPITAL DZELO 2c07
    case u'Ⰸ':     // GLAGOLITIC CAPITAL ZEMLJA 2c08
    case u'Ⰹ':     // GLAGOLITIC CAPITAL IZHE 2c09
    case u'Ⰺ':     // GLAGOLITIC CAPITAL INITIAL IZHE 2c0a
    case u'Ⰻ':     // GLAGOLITIC CAPITAL I 2c0b
    case u'Ⰼ':     // GLAGOLITIC CAPITAL DJERVI 2c0c
    case u'Ⰽ':     // GLAGOLITIC CAPITAL KAKO 2c0d
    case u'Ⰾ':     // GLAGOLITIC CAPITAL LJUDIJE 2c0e
    case u'Ⰿ':     // GLAGOLITIC CAPITAL MYSLITE 2c0f
    case u'Ⱀ':     // GLAGOLITIC CAPITAL NASHI 2c10
    case u'Ⱁ':     // GLAGOLITIC CAPITAL ONU 2c11
    case u'Ⱂ':     // GLAGOLITIC CAPITAL POKOJI 2c12
    case u'Ⱃ':     // GLAGOLITIC CAPITAL RITSI 2c13
    case u'Ⱄ':     // GLAGOLITIC CAPITAL SLOVO 2c14
    case u'Ⱅ':     // GLAGOLITIC CAPITAL TVRIDO 2c15
    case u'Ⱆ':     // GLAGOLITIC CAPITAL UKU 2c16
    case u'Ⱇ':     // GLAGOLITIC CAPITAL FRITU 2c17
    case u'Ⱈ':     // GLAGOLITIC CAPITAL HERU 2c18
    case u'Ⱉ':     // GLAGOLITIC CAPITAL OTU 2c19
    case u'Ⱊ':     // GLAGOLITIC CAPITAL PE 2c1a
    case u'Ⱋ':     // GLAGOLITIC CAPITAL SHTA 2c1b
    case u'Ⱌ':     // GLAGOLITIC CAPITAL TSI 2c1c
    case u'Ⱍ':     // GLAGOLITIC CAPITAL CHRIVI 2c1d
    case u'Ⱎ':     // GLAGOLITIC CAPITAL SHA 2c1e
    case u'Ⱏ':     // GLAGOLITIC CAPITAL YERU 2c1f
    case u'Ⱐ':     // GLAGOLITIC CAPITAL YERI 2c20
    case u'Ⱑ':     // GLAGOLITIC CAPITAL YATI 2c21
    case u'Ⱒ':     // GLAGOLITIC CAPITAL SPIDERY HA 2c22
    case u'Ⱓ':     // GLAGOLITIC CAPITAL YU 2c23
    case u'Ⱔ':     // GLAGOLITIC CAPITAL SMALL YUS 2c24
    case u'Ⱕ':     // GLAGOLITIC CAPITAL SMALL YUS W/ TAIL 2c25
    case u'Ⱖ':     // GLAGOLITIC CAPITAL YO 2c26
    case u'Ⱗ':     // GLAGOLITIC CAPITAL IOTATED SMALL YUS 2c27
    case u'Ⱘ':     // GLAGOLITIC CAPITAL BIG YUS 2c28
    case u'Ⱙ':     // GLAGOLITIC CAPITAL IOTATED BIG YUS 2c29
    case u'Ⱚ':     // GLAGOLITIC CAPITAL FITA 2c2a
    case u'Ⱛ':     // GLAGOLITIC CAPITAL IZHITSA 2c2b
    case u'Ⱜ':     // GLAGOLITIC CAPITAL SHTAPIC 2c2c
    case u'Ⱝ':     // GLAGOLITIC CAPITAL TROKUTASTI A 2c2d
    case u'Ⱞ':     // GLAGOLITIC CAPITAL LATINATE MYSLITE 2c2e
    case u'Ⱡ':     // LATIN CAPITAL L W/ DOUBLE BAR 2c60
    case u'Ɫ':     // LATIN CAPITAL L W/ MIDDLE TILDE 2c62
    case u'Ᵽ':     // LATIN CAPITAL P W/ STROKE 2c63
    case u'Ɽ':     // LATIN CAPITAL R W/ TAIL 2c64
    case u'Ⱨ':     // LATIN CAPITAL H W/ DESCENDER 2c67
    case u'Ⱪ':     // LATIN CAPITAL K W/ DESCENDER 2c69
    case u'Ⱬ':     // LATIN CAPITAL Z W/ DESCENDER 2c6b
    case u'Ɑ':     // LATIN CAPITAL ALPHA 2c6d
    case u'Ɱ':     // LATIN CAPITAL M W/ HOOK 2c6e
    case u'Ɐ':     // LATIN CAPITAL TURNED A 2c6f
    case u'Ɒ':     // LATIN CAPITAL TURNED ALPHA 2c70
    case u'Ⱳ':     // LATIN CAPITAL W W/ HOOK 2c72
    case u'Ⱶ':     // LATIN CAPITAL HALF H 2c75
    case u'Ȿ':     // LATIN CAPITAL S W/ SWASH TAIL 2c7e
    case u'Ɀ':     // LATIN CAPITAL Z W/ SWASH TAIL 2c7f
    case u'Ⲁ':     // COPTIC CAPITAL ALFA 2c80
    case u'Ⲃ':     // COPTIC CAPITAL VIDA 2c82
    case u'Ⲅ':     // COPTIC CAPITAL GAMMA 2c84
    case u'Ⲇ':     // COPTIC CAPITAL DALDA 2c86
    case u'Ⲉ':     // COPTIC CAPITAL EIE 2c88
    case u'Ⲋ':     // COPTIC CAPITAL SOU 2c8a
    case u'Ⲍ':     // COPTIC CAPITAL ZATA 2c8c
    case u'Ⲏ':     // COPTIC CAPITAL HATE 2c8e
    case u'Ⲑ':     // COPTIC CAPITAL THETHE 2c90
    case u'Ⲓ':     // COPTIC CAPITAL IAUDA 2c92
    case u'Ⲕ':     // COPTIC CAPITAL KAPA 2c94
    case u'Ⲗ':     // COPTIC CAPITAL LAULA 2c96
    case u'Ⲙ':     // COPTIC CAPITAL MI 2c98
    case u'Ⲛ':     // COPTIC CAPITAL NI 2c9a
    case u'Ⲝ':     // COPTIC CAPITAL KSI 2c9c
    case u'Ⲟ':     // COPTIC CAPITAL O 2c9e
    case u'Ⲡ':     // COPTIC CAPITAL PI 2ca0
    case u'Ⲣ':     // COPTIC CAPITAL RO 2ca2
    case u'Ⲥ':     // COPTIC CAPITAL SIMA 2ca4
    case u'Ⲧ':     // COPTIC CAPITAL TAU 2ca6
    case u'Ⲩ':     // COPTIC CAPITAL UA 2ca8
    case u'Ⲫ':     // COPTIC CAPITAL FI 2caa
    case u'Ⲭ':     // COPTIC CAPITAL KHI 2cac
    case u'Ⲯ':     // COPTIC CAPITAL PSI 2cae
    case u'Ⲱ':     // COPTIC CAPITAL OOU 2cb0
    case u'Ⲳ':     // COPTIC CAPITAL DIALECT-P ALEF 2cb2
    case u'Ⲵ':     // COPTIC CAPITAL OLD COPTIC AIN 2cb4
    case u'Ⲷ':     // COPTIC CAPITAL CRYPTOGRAMMIC EIE 2cb6
    case u'Ⲹ':     // COPTIC CAPITAL DIALECT-P KAPA 2cb8
    case u'Ⲻ':     // COPTIC CAPITAL DIALECT-P NI 2cba
    case u'Ⲽ':     // COPTIC CAPITAL CRYPTOGRAMMIC NI 2cbc
    case u'Ⲿ':     // COPTIC CAPITAL OLD COPTIC OOU 2cbe
    case u'Ⳁ':     // COPTIC CAPITAL SAMPI 2cc0
    case u'Ⳃ':     // COPTIC CAPITAL CROSSED SHEI 2cc2
    case u'Ⳅ':     // COPTIC CAPITAL OLD COPTIC SHEI 2cc4
    case u'Ⳇ':     // COPTIC CAPITAL OLD COPTIC ESH 2cc6
    case u'Ⳉ':     // COPTIC CAPITAL AKHMIMIC KHEI 2cc8
    case u'Ⳋ':     // COPTIC CAPITAL DIALECT-P HORI 2cca
    case u'Ⳍ':     // COPTIC CAPITAL OLD COPTIC HORI 2ccc
    case u'Ⳏ':     // COPTIC CAPITAL OLD COPTIC HA 2cce
    case u'Ⳑ':     // COPTIC CAPITAL L-SHAPED HA 2cd0
    case u'Ⳓ':     // COPTIC CAPITAL OLD COPTIC HEI 2cd2
    case u'Ⳕ':     // COPTIC CAPITAL OLD COPTIC HAT 2cd4
    case u'Ⳗ':     // COPTIC CAPITAL OLD COPTIC GANGIA 2cd6
    case u'Ⳙ':     // COPTIC CAPITAL OLD COPTIC DJA 2cd8
    case u'Ⳛ':     // COPTIC CAPITAL OLD COPTIC SHIMA 2cda
    case u'Ⳝ':     // COPTIC CAPITAL OLD NUBIAN SHIMA 2cdc
    case u'Ⳟ':     // COPTIC CAPITAL OLD NUBIAN NGI 2cde
    case u'Ⳡ':     // COPTIC CAPITAL OLD NUBIAN NYI 2ce0
    case u'Ⳣ':     // COPTIC CAPITAL OLD NUBIAN WAU 2ce2
    case u'Ⳬ':     // COPTIC CAPITAL CRYPTOGRAMMIC SHEI 2ceb
    case u'Ⳮ':     // COPTIC CAPITAL CRYPTOGRAMMIC GANGIA 2ced
    case u'Ⳳ':     // COPTIC CAPITAL BOHAIRIC KHEI 2cf2
    case u'Ꙁ':     // CYRILLIC CAPITAL ZEMLYA a640
    case u'Ꙃ':     // CYRILLIC CAPITAL DZELO a642
    case u'Ꙅ':     // CYRILLIC CAPITAL REVERSED DZE a644
    case u'Ꙇ':     // CYRILLIC CAPITAL IOTA a646
    case u'Ꙉ':     // CYRILLIC CAPITAL DJERV a648
    case u'Ꙋ':     // CYRILLIC CAPITAL MONOGRAPH UK a64a
    case u'Ꙍ':     // CYRILLIC CAPITAL BROAD OMEGA a64c
    case u'Ꙏ':     // CYRILLIC CAPITAL NEUTRAL YER a64e
    case u'Ꙑ':     // CYRILLIC CAPITAL YERU W/ BACK YER a650
    case u'Ꙓ':     // CYRILLIC CAPITAL IOTIFIED YAT a652
    case u'Ꙕ':     // CYRILLIC CAPITAL REVERSED YU a654
    case u'Ꙗ':     // CYRILLIC CAPITAL IOTIFIED A a656
    case u'Ꙙ':     // CYRILLIC CAPITAL CLOSED LITTLE YUS a658
    case u'Ꙛ':     // CYRILLIC CAPITAL BLENDED YUS a65a
    case u'Ꙝ':     // CYRILLIC CAPITAL IOTIFIED CLOSED LITTLE YUS a65c
    case u'Ꙟ':     // CYRILLIC CAPITAL YN a65e
    case u'Ꙡ':     // CYRILLIC CAPITAL REVERSED TSE a660
    case u'Ꙣ':     // CYRILLIC CAPITAL SOFT DE a662
    case u'Ꙥ':     // CYRILLIC CAPITAL SOFT EL a664
    case u'Ꙧ':     // CYRILLIC CAPITAL SOFT EM a666
    case u'Ꙩ':     // CYRILLIC CAPITAL MONOCULAR O a668
    case u'Ꙫ':     // CYRILLIC CAPITAL BINOCULAR O a66a
    case u'Ꙭ':     // CYRILLIC CAPITAL DOUBLE MONOCULAR O a66c
    case 0xa680:   // CYRILLIC CAPITAL DWE a680
    case 0xa682:   // CYRILLIC CAPITAL DZWE a682
    case 0xa684:   // CYRILLIC CAPITAL ZHWE a684
    case 0xa686:   // CYRILLIC CAPITAL CCHE a686
    case 0xa688:   // CYRILLIC CAPITAL DZZE a688
    case 0xa68a:   // CYRILLIC CAPITAL TE W/ MIDDLE HOOK a68a
    case 0xa68c:   // CYRILLIC CAPITAL TWE a68c
    case 0xa68e:   // CYRILLIC CAPITAL TSWE a68e
    case 0xa690:   // CYRILLIC CAPITAL TSSE a690
    case 0xa692:   // CYRILLIC CAPITAL TCHE a692
    case 0xa694:   // CYRILLIC CAPITAL HWE a694
    case 0xa696:   // CYRILLIC CAPITAL SHWE a696
    case 0xa698:   // CYRILLIC CAPITAL DOUBLE O a698
    case 0xa69a:   // CYRILLIC CAPITAL CROSSED O a69a
    case u'Ꜣ':     // LATIN CAPITAL EGYPTOLOGICAL ALEF a722
    case u'Ꜥ':     // LATIN CAPITAL EGYPTOLOGICAL AIN a724
    case u'Ꜧ':     // LATIN CAPITAL HENG a726
    case u'Ꜩ':     // LATIN CAPITAL TZ a728
    case u'Ꜫ':     // LATIN CAPITAL TRESILLO a72a
    case u'Ꜭ':     // LATIN CAPITAL CUATRILLO a72c
    case u'Ꜯ':     // LATIN CAPITAL CUATRILLO W/ COMMA a72e
    case u'Ꜳ':     // LATIN CAPITAL AA a732
    case u'Ꜵ':     // LATIN CAPITAL AO a734
    case u'Ꜷ':     // LATIN CAPITAL AU a736
    case u'Ꜹ':     // LATIN CAPITAL AV a738
    case u'Ꜻ':     // LATIN CAPITAL AV W/ HORIZONTAL BAR a73a
    case u'Ꜽ':     // LATIN CAPITAL AY a73c
    case u'Ꜿ':     // LATIN CAPITAL REVERSED C W/ DOT a73e
    case u'Ꝁ':     // LATIN CAPITAL K W/ STROKE a740
    case u'Ꝃ':     // LATIN CAPITAL K W/ DIAGONAL STROKE a742
    case u'Ꝅ':     // LATIN CAPITAL K W/ STROKE AND DIAGONAL STROKE a744
    case u'Ꝇ':     // LATIN CAPITAL BROKEN L a746
    case u'Ꝉ':     // LATIN CAPITAL L W/ HIGH STROKE a748
    case u'Ꝋ':     // LATIN CAPITAL O W/ LONG STROKE OVERLAY a74a
    case u'Ꝍ':     // LATIN CAPITAL O W/ LOOP a74c
    case u'Ꝏ':     // LATIN CAPITAL OO a74e
    case u'Ꝑ':     // LATIN CAPITAL P W/ STROKE THROUGH DESCENDER a750
    case u'Ꝓ':     // LATIN CAPITAL P W/ FLOURISH a752
    case u'Ꝕ':     // LATIN CAPITAL P W/ SQUIRREL TAIL a754
    case u'Ꝗ':     // LATIN CAPITAL Q W/ STROKE THROUGH DESCENDER a756
    case u'Ꝙ':     // LATIN CAPITAL Q W/ DIAGONAL STROKE a758
    case u'Ꝛ':     // LATIN CAPITAL R ROTUNDA a75a
    case u'Ꝝ':     // LATIN CAPITAL RUM ROTUNDA a75c
    case u'Ꝟ':     // LATIN CAPITAL V W/ DIAGONAL STROKE a75e
    case u'Ꝡ':     // LATIN CAPITAL VY a760
    case u'Ꝣ':     // LATIN CAPITAL VISIGOTHIC Z a762
    case u'Ꝥ':     // LATIN CAPITAL THORN W/ STROKE a764
    case u'Ꝧ':     // LATIN CAPITAL THORN W/ STROKE THROUGH DESCENDER a766
    case u'Ꝩ':     // LATIN CAPITAL VEND a768
    case u'Ꝫ':     // LATIN CAPITAL ET a76a
    case u'Ꝭ':     // LATIN CAPITAL IS a76c
    case u'Ꝯ':     // LATIN CAPITAL CON a76e
    case u'Ꝺ':     // LATIN CAPITAL INSULAR D a779
    case u'Ꝼ':     // LATIN CAPITAL INSULAR F a77b
    case u'Ᵹ':     // LATIN CAPITAL INSULAR G a77d
    case u'Ꝿ':     // LATIN CAPITAL TURNED INSULAR G a77e
    case u'Ꞁ':     // LATIN CAPITAL TURNED L a780
    case u'Ꞃ':     // LATIN CAPITAL INSULAR R a782
    case u'Ꞅ':     // LATIN CAPITAL INSULAR S a784
    case u'Ꞇ':     // LATIN CAPITAL INSULAR T a786
    case u'Ꞌ':     // LATIN CAPITAL SALTILLO a78b
    case u'Ɥ':     // LATIN CAPITAL TURNED H a78d
    case u'Ꞑ':     // LATIN CAPITAL N W/ DESCENDER a790
    case u'Ꞓ':     // LATIN CAPITAL C W/ BAR a792
    case u'Ꞗ':   // LATIN CAPITAL B W/ FLOURISH a796
    case u'Ꞙ':   // LATIN CAPITAL F W/ STROKE a798
    case u'Ꞛ':   // LATIN CAPITAL VOLAPUK AE a79a
    case u'Ꞝ':   // LATIN CAPITAL VOLAPUK OE a79c
    case u'Ꞟ':   // LATIN CAPITAL VOLAPUK UE a79e
    case u'Ꞡ':     // LATIN CAPITAL G W/ OBLIQUE STROKE a7a0
    case u'Ꞣ':     // LATIN CAPITAL K W/ OBLIQUE STROKE a7a2
    case u'Ꞥ':     // LATIN CAPITAL N W/ OBLIQUE STROKE a7a4
    case u'Ꞧ':     // LATIN CAPITAL R W/ OBLIQUE STROKE a7a6
    case u'Ꞩ':     // LATIN CAPITAL S W/ OBLIQUE STROKE a7a8
    case u'Ɦ':     // LATIN CAPITAL H W/ HOOK a7aa
    case u'Ɜ':   // LATIN CAPITAL REVERSED OPEN E a7ab
    case u'Ɡ':   // LATIN CAPITAL SCRIPT G a7ac
    case u'Ɬ':   // LATIN CAPITAL L W/ BELT a7ad
    case u'Ɪ':   // LATIN CAPITAL SMALL CAPITAL I a7ae
    case u'Ʞ':   // LATIN CAPITAL TURNED K a7b0
    case u'Ʇ':   // LATIN CAPITAL TURNED T a7b1
    case u'Ʝ':   // LATIN CAPITAL J W/ CROSSED-TAIL a7b2
    case u'Ꭓ':   // LATIN CAPITAL CHI a7b3
    case u'Ꞵ':   // LATIN CAPITAL BETA a7b4
    case u'Ꞷ':   // LATIN CAPITAL OMEGA a7b6
    case u'Ꞹ':   // LATIN CAPITAL U W/ STROKE a7b8
    case u'Ꞻ':   // LATIN CAPITAL GLOTTAL A a7ba
    case u'Ꞽ':   // LATIN CAPITAL GLOTTAL I a7bc
    case u'Ꞿ':   // LATIN CAPITAL GLOTTAL U a7be
    case u'Ꟃ':   // LATIN CAPITAL ANGLICANA W a7c2
    case u'Ꞔ':   // LATIN CAPITAL C W/ PALATAL HOOK a7c4
    case u'Ʂ':   // LATIN CAPITAL S W/ HOOK a7c5
    case u'Ᶎ':   // LATIN CAPITAL Z W/ PALATAL HOOK a7c6
    case u'Ａ':    // FULLWIDTH LATIN CAPITAL A ff21
    case u'Ｂ':    // FULLWIDTH LATIN CAPITAL B ff22
    case u'Ｃ':    // FULLWIDTH LATIN CAPITAL C ff23
    case u'Ｄ':    // FULLWIDTH LATIN CAPITAL D ff24
    case u'Ｅ':    // FULLWIDTH LATIN CAPITAL E ff25
    case u'Ｆ':    // FULLWIDTH LATIN CAPITAL F ff26
    case u'Ｇ':    // FULLWIDTH LATIN CAPITAL G ff27
    case u'Ｈ':    // FULLWIDTH LATIN CAPITAL H ff28
    case u'Ｉ':    // FULLWIDTH LATIN CAPITAL I ff29
    case u'Ｊ':    // FULLWIDTH LATIN CAPITAL J ff2a
    case u'Ｋ':    // FULLWIDTH LATIN CAPITAL K ff2b
    case u'Ｌ':    // FULLWIDTH LATIN CAPITAL L ff2c
    case u'Ｍ':    // FULLWIDTH LATIN CAPITAL M ff2d
    case u'Ｎ':    // FULLWIDTH LATIN CAPITAL N ff2e
    case u'Ｏ':    // FULLWIDTH LATIN CAPITAL O ff2f
    case u'Ｐ':    // FULLWIDTH LATIN CAPITAL P ff30
    case u'Ｑ':    // FULLWIDTH LATIN CAPITAL Q ff31
    case u'Ｒ':    // FULLWIDTH LATIN CAPITAL R ff32
    case u'Ｓ':    // FULLWIDTH LATIN CAPITAL S ff33
    case u'Ｔ':    // FULLWIDTH LATIN CAPITAL T ff34
    case u'Ｕ':    // FULLWIDTH LATIN CAPITAL U ff35
    case u'Ｖ':    // FULLWIDTH LATIN CAPITAL V ff36
    case u'Ｗ':    // FULLWIDTH LATIN CAPITAL W ff37
    case u'Ｘ':    // FULLWIDTH LATIN CAPITAL X ff38
    case u'Ｙ':    // FULLWIDTH LATIN CAPITAL Y ff39
    case u'Ｚ':    // FULLWIDTH LATIN CAPITAL Z ff3a
    case L'𐐀':     // DESERET CAPITAL LONG I 10400
    case L'𐐁':     // DESERET CAPITAL LONG E 10401
    case L'𐐂':     // DESERET CAPITAL LONG A 10402
    case L'𐐃':     // DESERET CAPITAL LONG AH 10403
    case L'𐐄':     // DESERET CAPITAL LONG O 10404
    case L'𐐅':     // DESERET CAPITAL LONG OO 10405
    case L'𐐆':     // DESERET CAPITAL SHORT I 10406
    case L'𐐇':     // DESERET CAPITAL SHORT E 10407
    case L'𐐈':     // DESERET CAPITAL SHORT A 10408
    case L'𐐉':     // DESERET CAPITAL SHORT AH 10409
    case L'𐐊':     // DESERET CAPITAL SHORT O 1040a
    case L'𐐋':     // DESERET CAPITAL SHORT OO 1040b
    case L'𐐌':     // DESERET CAPITAL AY 1040c
    case L'𐐍':     // DESERET CAPITAL OW 1040d
    case L'𐐎':     // DESERET CAPITAL WU 1040e
    case L'𐐏':     // DESERET CAPITAL YEE 1040f
    case L'𐐐':     // DESERET CAPITAL H 10410
    case L'𐐑':     // DESERET CAPITAL PEE 10411
    case L'𐐒':     // DESERET CAPITAL BEE 10412
    case L'𐐓':     // DESERET CAPITAL TEE 10413
    case L'𐐔':     // DESERET CAPITAL DEE 10414
    case L'𐐕':     // DESERET CAPITAL CHEE 10415
    case L'𐐖':     // DESERET CAPITAL JEE 10416
    case L'𐐗':     // DESERET CAPITAL KAY 10417
    case L'𐐘':     // DESERET CAPITAL GAY 10418
    case L'𐐙':     // DESERET CAPITAL EF 10419
    case L'𐐚':     // DESERET CAPITAL VEE 1041a
    case L'𐐛':     // DESERET CAPITAL ETH 1041b
    case L'𐐜':     // DESERET CAPITAL THEE 1041c
    case L'𐐝':     // DESERET CAPITAL ES 1041d
    case L'𐐞':     // DESERET CAPITAL ZEE 1041e
    case L'𐐟':     // DESERET CAPITAL ESH 1041f
    case L'𐐠':     // DESERET CAPITAL ZHEE 10420
    case L'𐐡':     // DESERET CAPITAL ER 10421
    case L'𐐢':     // DESERET CAPITAL EL 10422
    case L'𐐣':     // DESERET CAPITAL EM 10423
    case L'𐐤':     // DESERET CAPITAL EN 10424
    case L'𐐥':     // DESERET CAPITAL ENG 10425
    case L'𐐦':     // DESERET CAPITAL OI 10426
    case L'𐐧':     // DESERET CAPITAL EW 10427
    case L'𐒰':  // OSAGE CAPITAL A 104b0
    case L'𐒱':  // OSAGE CAPITAL AI 104b1
    case L'𐒲':  // OSAGE CAPITAL AIN 104b2
    case L'𐒳':  // OSAGE CAPITAL AH 104b3
    case L'𐒴':  // OSAGE CAPITAL BRA 104b4
    case L'𐒵':  // OSAGE CAPITAL CHA 104b5
    case L'𐒶':  // OSAGE CAPITAL EHCHA 104b6
    case L'𐒷':  // OSAGE CAPITAL E 104b7
    case L'𐒸':  // OSAGE CAPITAL EIN 104b8
    case L'𐒹':  // OSAGE CAPITAL HA 104b9
    case L'𐒺':  // OSAGE CAPITAL HYA 104ba
    case L'𐒻':  // OSAGE CAPITAL I 104bb
    case L'𐒼':  // OSAGE CAPITAL KA 104bc
    case L'𐒽':  // OSAGE CAPITAL EHKA 104bd
    case L'𐒾':  // OSAGE CAPITAL KYA 104be
    case L'𐒿':  // OSAGE CAPITAL LA 104bf
    case L'𐓀':  // OSAGE CAPITAL MA 104c0
    case L'𐓁':  // OSAGE CAPITAL NA 104c1
    case L'𐓂':  // OSAGE CAPITAL O 104c2
    case L'𐓃':  // OSAGE CAPITAL OIN 104c3
    case L'𐓄':  // OSAGE CAPITAL PA 104c4
    case L'𐓅':  // OSAGE CAPITAL EHPA 104c5
    case L'𐓆':  // OSAGE CAPITAL SA 104c6
    case L'𐓇':  // OSAGE CAPITAL SHA 104c7
    case L'𐓈':  // OSAGE CAPITAL TA 104c8
    case L'𐓉':  // OSAGE CAPITAL EHTA 104c9
    case L'𐓊':  // OSAGE CAPITAL TSA 104ca
    case L'𐓋':  // OSAGE CAPITAL EHTSA 104cb
    case L'𐓌':  // OSAGE CAPITAL TSHA 104cc
    case L'𐓍':  // OSAGE CAPITAL DHA 104cd
    case L'𐓎':  // OSAGE CAPITAL U 104ce
    case L'𐓏':  // OSAGE CAPITAL WA 104cf
    case L'𐓐':  // OSAGE CAPITAL KHA 104d0
    case L'𐓑':  // OSAGE CAPITAL GHA 104d1
    case L'𐓒':  // OSAGE CAPITAL ZA 104d2
    case L'𐓓':  // OSAGE CAPITAL ZHA 104d3
    case L'𐲀':  // OLD HUNGARIAN CAPITAL A 10c80
    case L'𐲁':  // OLD HUNGARIAN CAPITAL AA 10c81
    case L'𐲂':  // OLD HUNGARIAN CAPITAL EB 10c82
    case L'𐲃':  // OLD HUNGARIAN CAPITAL AMB 10c83
    case L'𐲄':  // OLD HUNGARIAN CAPITAL EC 10c84
    case L'𐲅':  // OLD HUNGARIAN CAPITAL ENC 10c85
    case L'𐲆':  // OLD HUNGARIAN CAPITAL ECS 10c86
    case L'𐲇':  // OLD HUNGARIAN CAPITAL ED 10c87
    case L'𐲈':  // OLD HUNGARIAN CAPITAL AND 10c88
    case L'𐲉':  // OLD HUNGARIAN CAPITAL E 10c89
    case L'𐲊':  // OLD HUNGARIAN CAPITAL CLOSE E 10c8a
    case L'𐲋':  // OLD HUNGARIAN CAPITAL EE 10c8b
    case L'𐲌':  // OLD HUNGARIAN CAPITAL EF 10c8c
    case L'𐲍':  // OLD HUNGARIAN CAPITAL EG 10c8d
    case L'𐲎':  // OLD HUNGARIAN CAPITAL EGY 10c8e
    case L'𐲏':  // OLD HUNGARIAN CAPITAL EH 10c8f
    case L'𐲐':  // OLD HUNGARIAN CAPITAL I 10c90
    case L'𐲑':  // OLD HUNGARIAN CAPITAL II 10c91
    case L'𐲒':  // OLD HUNGARIAN CAPITAL EJ 10c92
    case L'𐲓':  // OLD HUNGARIAN CAPITAL EK 10c93
    case L'𐲔':  // OLD HUNGARIAN CAPITAL AK 10c94
    case L'𐲕':  // OLD HUNGARIAN CAPITAL UNK 10c95
    case L'𐲖':  // OLD HUNGARIAN CAPITAL EL 10c96
    case L'𐲗':  // OLD HUNGARIAN CAPITAL ELY 10c97
    case L'𐲘':  // OLD HUNGARIAN CAPITAL EM 10c98
    case L'𐲙':  // OLD HUNGARIAN CAPITAL EN 10c99
    case L'𐲚':  // OLD HUNGARIAN CAPITAL ENY 10c9a
    case L'𐲛':  // OLD HUNGARIAN CAPITAL O 10c9b
    case L'𐲜':  // OLD HUNGARIAN CAPITAL OO 10c9c
    case L'𐲝':  // OLD HUNGARIAN CAPITAL NIKOLSBURG OE 10c9d
    case L'𐲞':  // OLD HUNGARIAN CAPITAL RUDIMENTA OE 10c9e
    case L'𐲟':  // OLD HUNGARIAN CAPITAL OEE 10c9f
    case L'𐲠':  // OLD HUNGARIAN CAPITAL EP 10ca0
    case L'𐲡':  // OLD HUNGARIAN CAPITAL EMP 10ca1
    case L'𐲢':  // OLD HUNGARIAN CAPITAL ER 10ca2
    case L'𐲣':  // OLD HUNGARIAN CAPITAL SHORT ER 10ca3
    case L'𐲤':  // OLD HUNGARIAN CAPITAL ES 10ca4
    case L'𐲥':  // OLD HUNGARIAN CAPITAL ESZ 10ca5
    case L'𐲦':  // OLD HUNGARIAN CAPITAL ET 10ca6
    case L'𐲧':  // OLD HUNGARIAN CAPITAL ENT 10ca7
    case L'𐲨':  // OLD HUNGARIAN CAPITAL ETY 10ca8
    case L'𐲩':  // OLD HUNGARIAN CAPITAL ECH 10ca9
    case L'𐲪':  // OLD HUNGARIAN CAPITAL U 10caa
    case L'𐲫':  // OLD HUNGARIAN CAPITAL UU 10cab
    case L'𐲬':  // OLD HUNGARIAN CAPITAL NIKOLSBURG UE 10cac
    case L'𐲭':  // OLD HUNGARIAN CAPITAL RUDIMENTA UE 10cad
    case L'𐲮':  // OLD HUNGARIAN CAPITAL EV 10cae
    case L'𐲯':  // OLD HUNGARIAN CAPITAL EZ 10caf
    case L'𐲰':  // OLD HUNGARIAN CAPITAL EZS 10cb0
    case L'𐲱':  // OLD HUNGARIAN CAPITAL ENT-SHAPED SIGN 10cb1
    case L'𐲲':  // OLD HUNGARIAN CAPITAL US 10cb2
    case L'𑢠':  // WARANG CITI CAPITAL NGAA 118a0
    case L'𑢡':  // WARANG CITI CAPITAL A 118a1
    case L'𑢢':  // WARANG CITI CAPITAL WI 118a2
    case L'𑢣':  // WARANG CITI CAPITAL YU 118a3
    case L'𑢤':  // WARANG CITI CAPITAL YA 118a4
    case L'𑢥':  // WARANG CITI CAPITAL YO 118a5
    case L'𑢦':  // WARANG CITI CAPITAL II 118a6
    case L'𑢧':  // WARANG CITI CAPITAL UU 118a7
    case L'𑢨':  // WARANG CITI CAPITAL E 118a8
    case L'𑢩':  // WARANG CITI CAPITAL O 118a9
    case L'𑢪':  // WARANG CITI CAPITAL ANG 118aa
    case L'𑢫':  // WARANG CITI CAPITAL GA 118ab
    case L'𑢬':  // WARANG CITI CAPITAL KO 118ac
    case L'𑢭':  // WARANG CITI CAPITAL ENY 118ad
    case L'𑢮':  // WARANG CITI CAPITAL YUJ 118ae
    case L'𑢯':  // WARANG CITI CAPITAL UC 118af
    case L'𑢰':  // WARANG CITI CAPITAL ENN 118b0
    case L'𑢱':  // WARANG CITI CAPITAL ODD 118b1
    case L'𑢲':  // WARANG CITI CAPITAL TTE 118b2
    case L'𑢳':  // WARANG CITI CAPITAL NUNG 118b3
    case L'𑢴':  // WARANG CITI CAPITAL DA 118b4
    case L'𑢵':  // WARANG CITI CAPITAL AT 118b5
    case L'𑢶':  // WARANG CITI CAPITAL AM 118b6
    case L'𑢷':  // WARANG CITI CAPITAL BU 118b7
    case L'𑢸':  // WARANG CITI CAPITAL PU 118b8
    case L'𑢹':  // WARANG CITI CAPITAL HIYO 118b9
    case L'𑢺':  // WARANG CITI CAPITAL HOLO 118ba
    case L'𑢻':  // WARANG CITI CAPITAL HORR 118bb
    case L'𑢼':  // WARANG CITI CAPITAL HAR 118bc
    case L'𑢽':  // WARANG CITI CAPITAL SSUU 118bd
    case L'𑢾':  // WARANG CITI CAPITAL SII 118be
    case L'𑢿':  // WARANG CITI CAPITAL VIYO 118bf
    case L'𖹀':  // MEDEFAIDRIN CAPITAL M 16e40
    case L'𖹁':  // MEDEFAIDRIN CAPITAL S 16e41
    case L'𖹂':  // MEDEFAIDRIN CAPITAL V 16e42
    case L'𖹃':  // MEDEFAIDRIN CAPITAL W 16e43
    case L'𖹄':  // MEDEFAIDRIN CAPITAL ATIU 16e44
    case L'𖹅':  // MEDEFAIDRIN CAPITAL Z 16e45
    case L'𖹆':  // MEDEFAIDRIN CAPITAL KP 16e46
    case L'𖹇':  // MEDEFAIDRIN CAPITAL P 16e47
    case L'𖹈':  // MEDEFAIDRIN CAPITAL T 16e48
    case L'𖹉':  // MEDEFAIDRIN CAPITAL G 16e49
    case L'𖹊':  // MEDEFAIDRIN CAPITAL F 16e4a
    case L'𖹋':  // MEDEFAIDRIN CAPITAL I 16e4b
    case L'𖹌':  // MEDEFAIDRIN CAPITAL K 16e4c
    case L'𖹍':  // MEDEFAIDRIN CAPITAL A 16e4d
    case L'𖹎':  // MEDEFAIDRIN CAPITAL J 16e4e
    case L'𖹏':  // MEDEFAIDRIN CAPITAL E 16e4f
    case L'𖹐':  // MEDEFAIDRIN CAPITAL B 16e50
    case L'𖹑':  // MEDEFAIDRIN CAPITAL C 16e51
    case L'𖹒':  // MEDEFAIDRIN CAPITAL U 16e52
    case L'𖹓':  // MEDEFAIDRIN CAPITAL YU 16e53
    case L'𖹔':  // MEDEFAIDRIN CAPITAL L 16e54
    case L'𖹕':  // MEDEFAIDRIN CAPITAL Q 16e55
    case L'𖹖':  // MEDEFAIDRIN CAPITAL HP 16e56
    case L'𖹗':  // MEDEFAIDRIN CAPITAL NY 16e57
    case L'𖹘':  // MEDEFAIDRIN CAPITAL X 16e58
    case L'𖹙':  // MEDEFAIDRIN CAPITAL D 16e59
    case L'𖹚':  // MEDEFAIDRIN CAPITAL OE 16e5a
    case L'𖹛':  // MEDEFAIDRIN CAPITAL N 16e5b
    case L'𖹜':  // MEDEFAIDRIN CAPITAL R 16e5c
    case L'𖹝':  // MEDEFAIDRIN CAPITAL O 16e5d
    case L'𖹞':  // MEDEFAIDRIN CAPITAL AI 16e5e
    case L'𖹟':  // MEDEFAIDRIN CAPITAL Y 16e5f
    case L'𝐀':     // MATHEMATICAL BOLD CAPITAL A 1d400
    case L'𝐁':     // MATHEMATICAL BOLD CAPITAL B 1d401
    case L'𝐂':     // MATHEMATICAL BOLD CAPITAL C 1d402
    case L'𝐃':     // MATHEMATICAL BOLD CAPITAL D 1d403
    case L'𝐄':     // MATHEMATICAL BOLD CAPITAL E 1d404
    case L'𝐅':     // MATHEMATICAL BOLD CAPITAL F 1d405
    case L'𝐆':     // MATHEMATICAL BOLD CAPITAL G 1d406
    case L'𝐇':     // MATHEMATICAL BOLD CAPITAL H 1d407
    case L'𝐈':     // MATHEMATICAL BOLD CAPITAL I 1d408
    case L'𝐉':     // MATHEMATICAL BOLD CAPITAL J 1d409
    case L'𝐊':     // MATHEMATICAL BOLD CAPITAL K 1d40a
    case L'𝐋':     // MATHEMATICAL BOLD CAPITAL L 1d40b
    case L'𝐌':     // MATHEMATICAL BOLD CAPITAL M 1d40c
    case L'𝐍':     // MATHEMATICAL BOLD CAPITAL N 1d40d
    case L'𝐎':     // MATHEMATICAL BOLD CAPITAL O 1d40e
    case L'𝐏':     // MATHEMATICAL BOLD CAPITAL P 1d40f
    case L'𝐐':     // MATHEMATICAL BOLD CAPITAL Q 1d410
    case L'𝐑':     // MATHEMATICAL BOLD CAPITAL R 1d411
    case L'𝐒':     // MATHEMATICAL BOLD CAPITAL S 1d412
    case L'𝐓':     // MATHEMATICAL BOLD CAPITAL T 1d413
    case L'𝐔':     // MATHEMATICAL BOLD CAPITAL U 1d414
    case L'𝐕':     // MATHEMATICAL BOLD CAPITAL V 1d415
    case L'𝐖':     // MATHEMATICAL BOLD CAPITAL W 1d416
    case L'𝐗':     // MATHEMATICAL BOLD CAPITAL X 1d417
    case L'𝐘':     // MATHEMATICAL BOLD CAPITAL Y 1d418
    case L'𝐙':     // MATHEMATICAL BOLD CAPITAL Z 1d419
    case L'𝐴':     // MATHEMATICAL ITALIC CAPITAL A 1d434
    case L'𝐵':     // MATHEMATICAL ITALIC CAPITAL B 1d435
    case L'𝐶':     // MATHEMATICAL ITALIC CAPITAL C 1d436
    case L'𝐷':     // MATHEMATICAL ITALIC CAPITAL D 1d437
    case L'𝐸':     // MATHEMATICAL ITALIC CAPITAL E 1d438
    case L'𝐹':     // MATHEMATICAL ITALIC CAPITAL F 1d439
    case L'𝐺':     // MATHEMATICAL ITALIC CAPITAL G 1d43a
    case L'𝐻':     // MATHEMATICAL ITALIC CAPITAL H 1d43b
    case L'𝐼':     // MATHEMATICAL ITALIC CAPITAL I 1d43c
    case L'𝐽':     // MATHEMATICAL ITALIC CAPITAL J 1d43d
    case L'𝐾':     // MATHEMATICAL ITALIC CAPITAL K 1d43e
    case L'𝐿':     // MATHEMATICAL ITALIC CAPITAL L 1d43f
    case L'𝑀':     // MATHEMATICAL ITALIC CAPITAL M 1d440
    case L'𝑁':     // MATHEMATICAL ITALIC CAPITAL N 1d441
    case L'𝑂':     // MATHEMATICAL ITALIC CAPITAL O 1d442
    case L'𝑃':     // MATHEMATICAL ITALIC CAPITAL P 1d443
    case L'𝑄':     // MATHEMATICAL ITALIC CAPITAL Q 1d444
    case L'𝑅':     // MATHEMATICAL ITALIC CAPITAL R 1d445
    case L'𝑆':     // MATHEMATICAL ITALIC CAPITAL S 1d446
    case L'𝑇':     // MATHEMATICAL ITALIC CAPITAL T 1d447
    case L'𝑈':     // MATHEMATICAL ITALIC CAPITAL U 1d448
    case L'𝑉':     // MATHEMATICAL ITALIC CAPITAL V 1d449
    case L'𝑊':     // MATHEMATICAL ITALIC CAPITAL W 1d44a
    case L'𝑋':     // MATHEMATICAL ITALIC CAPITAL X 1d44b
    case L'𝑌':     // MATHEMATICAL ITALIC CAPITAL Y 1d44c
    case L'𝑍':     // MATHEMATICAL ITALIC CAPITAL Z 1d44d
    case L'𝑨':     // MATHEMATICAL BOLD ITALIC CAPITAL A 1d468
    case L'𝑩':     // MATHEMATICAL BOLD ITALIC CAPITAL B 1d469
    case L'𝑪':     // MATHEMATICAL BOLD ITALIC CAPITAL C 1d46a
    case L'𝑫':     // MATHEMATICAL BOLD ITALIC CAPITAL D 1d46b
    case L'𝑬':     // MATHEMATICAL BOLD ITALIC CAPITAL E 1d46c
    case L'𝑭':     // MATHEMATICAL BOLD ITALIC CAPITAL F 1d46d
    case L'𝑮':     // MATHEMATICAL BOLD ITALIC CAPITAL G 1d46e
    case L'𝑯':     // MATHEMATICAL BOLD ITALIC CAPITAL H 1d46f
    case L'𝑰':     // MATHEMATICAL BOLD ITALIC CAPITAL I 1d470
    case L'𝑱':     // MATHEMATICAL BOLD ITALIC CAPITAL J 1d471
    case L'𝑲':     // MATHEMATICAL BOLD ITALIC CAPITAL K 1d472
    case L'𝑳':     // MATHEMATICAL BOLD ITALIC CAPITAL L 1d473
    case L'𝑴':     // MATHEMATICAL BOLD ITALIC CAPITAL M 1d474
    case L'𝑵':     // MATHEMATICAL BOLD ITALIC CAPITAL N 1d475
    case L'𝑶':     // MATHEMATICAL BOLD ITALIC CAPITAL O 1d476
    case L'𝑷':     // MATHEMATICAL BOLD ITALIC CAPITAL P 1d477
    case L'𝑸':     // MATHEMATICAL BOLD ITALIC CAPITAL Q 1d478
    case L'𝑹':     // MATHEMATICAL BOLD ITALIC CAPITAL R 1d479
    case L'𝑺':     // MATHEMATICAL BOLD ITALIC CAPITAL S 1d47a
    case L'𝑻':     // MATHEMATICAL BOLD ITALIC CAPITAL T 1d47b
    case L'𝑼':     // MATHEMATICAL BOLD ITALIC CAPITAL U 1d47c
    case L'𝑽':     // MATHEMATICAL BOLD ITALIC CAPITAL V 1d47d
    case L'𝑾':     // MATHEMATICAL BOLD ITALIC CAPITAL W 1d47e
    case L'𝑿':     // MATHEMATICAL BOLD ITALIC CAPITAL X 1d47f
    case L'𝒀':     // MATHEMATICAL BOLD ITALIC CAPITAL Y 1d480
    case L'𝒁':     // MATHEMATICAL BOLD ITALIC CAPITAL Z 1d481
    case L'𝒜':     // MATHEMATICAL SCRIPT CAPITAL A 1d49c
    case L'𝒞':     // MATHEMATICAL SCRIPT CAPITAL C 1d49e
    case L'𝒟':     // MATHEMATICAL SCRIPT CAPITAL D 1d49f
    case L'𝒢':     // MATHEMATICAL SCRIPT CAPITAL G 1d4a2
    case L'𝒥':     // MATHEMATICAL SCRIPT CAPITAL J 1d4a5
    case L'𝒦':     // MATHEMATICAL SCRIPT CAPITAL K 1d4a6
    case L'𝒩':     // MATHEMATICAL SCRIPT CAPITAL N 1d4a9
    case L'𝒪':     // MATHEMATICAL SCRIPT CAPITAL O 1d4aa
    case L'𝒫':     // MATHEMATICAL SCRIPT CAPITAL P 1d4ab
    case L'𝒬':     // MATHEMATICAL SCRIPT CAPITAL Q 1d4ac
    case L'𝒮':     // MATHEMATICAL SCRIPT CAPITAL S 1d4ae
    case L'𝒯':     // MATHEMATICAL SCRIPT CAPITAL T 1d4af
    case L'𝒰':     // MATHEMATICAL SCRIPT CAPITAL U 1d4b0
    case L'𝒱':     // MATHEMATICAL SCRIPT CAPITAL V 1d4b1
    case L'𝒲':     // MATHEMATICAL SCRIPT CAPITAL W 1d4b2
    case L'𝒳':     // MATHEMATICAL SCRIPT CAPITAL X 1d4b3
    case L'𝒴':     // MATHEMATICAL SCRIPT CAPITAL Y 1d4b4
    case L'𝒵':     // MATHEMATICAL SCRIPT CAPITAL Z 1d4b5
    case L'𝓐':     // MATHEMATICAL BOLD SCRIPT CAPITAL A 1d4d0
    case L'𝓑':     // MATHEMATICAL BOLD SCRIPT CAPITAL B 1d4d1
    case L'𝓒':     // MATHEMATICAL BOLD SCRIPT CAPITAL C 1d4d2
    case L'𝓓':     // MATHEMATICAL BOLD SCRIPT CAPITAL D 1d4d3
    case L'𝓔':     // MATHEMATICAL BOLD SCRIPT CAPITAL E 1d4d4
    case L'𝓕':     // MATHEMATICAL BOLD SCRIPT CAPITAL F 1d4d5
    case L'𝓖':     // MATHEMATICAL BOLD SCRIPT CAPITAL G 1d4d6
    case L'𝓗':     // MATHEMATICAL BOLD SCRIPT CAPITAL H 1d4d7
    case L'𝓘':     // MATHEMATICAL BOLD SCRIPT CAPITAL I 1d4d8
    case L'𝓙':     // MATHEMATICAL BOLD SCRIPT CAPITAL J 1d4d9
    case L'𝓚':     // MATHEMATICAL BOLD SCRIPT CAPITAL K 1d4da
    case L'𝓛':     // MATHEMATICAL BOLD SCRIPT CAPITAL L 1d4db
    case L'𝓜':     // MATHEMATICAL BOLD SCRIPT CAPITAL M 1d4dc
    case L'𝓝':     // MATHEMATICAL BOLD SCRIPT CAPITAL N 1d4dd
    case L'𝓞':     // MATHEMATICAL BOLD SCRIPT CAPITAL O 1d4de
    case L'𝓟':     // MATHEMATICAL BOLD SCRIPT CAPITAL P 1d4df
    case L'𝓠':     // MATHEMATICAL BOLD SCRIPT CAPITAL Q 1d4e0
    case L'𝓡':     // MATHEMATICAL BOLD SCRIPT CAPITAL R 1d4e1
    case L'𝓢':     // MATHEMATICAL BOLD SCRIPT CAPITAL S 1d4e2
    case L'𝓣':     // MATHEMATICAL BOLD SCRIPT CAPITAL T 1d4e3
    case L'𝓤':     // MATHEMATICAL BOLD SCRIPT CAPITAL U 1d4e4
    case L'𝓥':     // MATHEMATICAL BOLD SCRIPT CAPITAL V 1d4e5
    case L'𝓦':     // MATHEMATICAL BOLD SCRIPT CAPITAL W 1d4e6
    case L'𝓧':     // MATHEMATICAL BOLD SCRIPT CAPITAL X 1d4e7
    case L'𝓨':     // MATHEMATICAL BOLD SCRIPT CAPITAL Y 1d4e8
    case L'𝓩':     // MATHEMATICAL BOLD SCRIPT CAPITAL Z 1d4e9
    case L'𝔄':     // MATHEMATICAL FRAKTUR CAPITAL A 1d504
    case L'𝔅':     // MATHEMATICAL FRAKTUR CAPITAL B 1d505
    case L'𝔇':     // MATHEMATICAL FRAKTUR CAPITAL D 1d507
    case L'𝔈':     // MATHEMATICAL FRAKTUR CAPITAL E 1d508
    case L'𝔉':     // MATHEMATICAL FRAKTUR CAPITAL F 1d509
    case L'𝔊':     // MATHEMATICAL FRAKTUR CAPITAL G 1d50a
    case L'𝔍':     // MATHEMATICAL FRAKTUR CAPITAL J 1d50d
    case L'𝔎':     // MATHEMATICAL FRAKTUR CAPITAL K 1d50e
    case L'𝔏':     // MATHEMATICAL FRAKTUR CAPITAL L 1d50f
    case L'𝔐':     // MATHEMATICAL FRAKTUR CAPITAL M 1d510
    case L'𝔑':     // MATHEMATICAL FRAKTUR CAPITAL N 1d511
    case L'𝔒':     // MATHEMATICAL FRAKTUR CAPITAL O 1d512
    case L'𝔓':     // MATHEMATICAL FRAKTUR CAPITAL P 1d513
    case L'𝔔':     // MATHEMATICAL FRAKTUR CAPITAL Q 1d514
    case L'𝔖':     // MATHEMATICAL FRAKTUR CAPITAL S 1d516
    case L'𝔗':     // MATHEMATICAL FRAKTUR CAPITAL T 1d517
    case L'𝔘':     // MATHEMATICAL FRAKTUR CAPITAL U 1d518
    case L'𝔙':     // MATHEMATICAL FRAKTUR CAPITAL V 1d519
    case L'𝔚':     // MATHEMATICAL FRAKTUR CAPITAL W 1d51a
    case L'𝔛':     // MATHEMATICAL FRAKTUR CAPITAL X 1d51b
    case L'𝔜':     // MATHEMATICAL FRAKTUR CAPITAL Y 1d51c
    case L'𝔸':     // MATHEMATICAL DOUBLE-STRUCK CAPITAL A 1d538
    case L'𝔹':     // MATHEMATICAL DOUBLE-STRUCK CAPITAL B 1d539
    case L'𝔻':     // MATHEMATICAL DOUBLE-STRUCK CAPITAL D 1d53b
    case L'𝔼':     // MATHEMATICAL DOUBLE-STRUCK CAPITAL E 1d53c
    case L'𝔽':     // MATHEMATICAL DOUBLE-STRUCK CAPITAL F 1d53d
    case L'𝔾':     // MATHEMATICAL DOUBLE-STRUCK CAPITAL G 1d53e
    case L'𝕀':     // MATHEMATICAL DOUBLE-STRUCK CAPITAL I 1d540
    case L'𝕁':     // MATHEMATICAL DOUBLE-STRUCK CAPITAL J 1d541
    case L'𝕂':     // MATHEMATICAL DOUBLE-STRUCK CAPITAL K 1d542
    case L'𝕃':     // MATHEMATICAL DOUBLE-STRUCK CAPITAL L 1d543
    case L'𝕄':     // MATHEMATICAL DOUBLE-STRUCK CAPITAL M 1d544
    case L'𝕆':     // MATHEMATICAL DOUBLE-STRUCK CAPITAL O 1d546
    case L'𝕊':     // MATHEMATICAL DOUBLE-STRUCK CAPITAL S 1d54a
    case L'𝕋':     // MATHEMATICAL DOUBLE-STRUCK CAPITAL T 1d54b
    case L'𝕌':     // MATHEMATICAL DOUBLE-STRUCK CAPITAL U 1d54c
    case L'𝕍':     // MATHEMATICAL DOUBLE-STRUCK CAPITAL V 1d54d
    case L'𝕎':     // MATHEMATICAL DOUBLE-STRUCK CAPITAL W 1d54e
    case L'𝕏':     // MATHEMATICAL DOUBLE-STRUCK CAPITAL X 1d54f
    case L'𝕐':     // MATHEMATICAL DOUBLE-STRUCK CAPITAL Y 1d550
    case L'𝕬':     // MATHEMATICAL BOLD FRAKTUR CAPITAL A 1d56c
    case L'𝕭':     // MATHEMATICAL BOLD FRAKTUR CAPITAL B 1d56d
    case L'𝕮':     // MATHEMATICAL BOLD FRAKTUR CAPITAL C 1d56e
    case L'𝕯':     // MATHEMATICAL BOLD FRAKTUR CAPITAL D 1d56f
    case L'𝕰':     // MATHEMATICAL BOLD FRAKTUR CAPITAL E 1d570
    case L'𝕱':     // MATHEMATICAL BOLD FRAKTUR CAPITAL F 1d571
    case L'𝕲':     // MATHEMATICAL BOLD FRAKTUR CAPITAL G 1d572
    case L'𝕳':     // MATHEMATICAL BOLD FRAKTUR CAPITAL H 1d573
    case L'𝕴':     // MATHEMATICAL BOLD FRAKTUR CAPITAL I 1d574
    case L'𝕵':     // MATHEMATICAL BOLD FRAKTUR CAPITAL J 1d575
    case L'𝕶':     // MATHEMATICAL BOLD FRAKTUR CAPITAL K 1d576
    case L'𝕷':     // MATHEMATICAL BOLD FRAKTUR CAPITAL L 1d577
    case L'𝕸':     // MATHEMATICAL BOLD FRAKTUR CAPITAL M 1d578
    case L'𝕹':     // MATHEMATICAL BOLD FRAKTUR CAPITAL N 1d579
    case L'𝕺':     // MATHEMATICAL BOLD FRAKTUR CAPITAL O 1d57a
    case L'𝕻':     // MATHEMATICAL BOLD FRAKTUR CAPITAL P 1d57b
    case L'𝕼':     // MATHEMATICAL BOLD FRAKTUR CAPITAL Q 1d57c
    case L'𝕽':     // MATHEMATICAL BOLD FRAKTUR CAPITAL R 1d57d
    case L'𝕾':     // MATHEMATICAL BOLD FRAKTUR CAPITAL S 1d57e
    case L'𝕿':     // MATHEMATICAL BOLD FRAKTUR CAPITAL T 1d57f
    case L'𝖀':     // MATHEMATICAL BOLD FRAKTUR CAPITAL U 1d580
    case L'𝖁':     // MATHEMATICAL BOLD FRAKTUR CAPITAL V 1d581
    case L'𝖂':     // MATHEMATICAL BOLD FRAKTUR CAPITAL W 1d582
    case L'𝖃':     // MATHEMATICAL BOLD FRAKTUR CAPITAL X 1d583
    case L'𝖄':     // MATHEMATICAL BOLD FRAKTUR CAPITAL Y 1d584
    case L'𝖅':     // MATHEMATICAL BOLD FRAKTUR CAPITAL Z 1d585
    case L'𝖠':     // MATHEMATICAL SANS CAPITAL A 1d5a0
    case L'𝖡':     // MATHEMATICAL SANS CAPITAL B 1d5a1
    case L'𝖢':     // MATHEMATICAL SANS CAPITAL C 1d5a2
    case L'𝖣':     // MATHEMATICAL SANS CAPITAL D 1d5a3
    case L'𝖤':     // MATHEMATICAL SANS CAPITAL E 1d5a4
    case L'𝖥':     // MATHEMATICAL SANS CAPITAL F 1d5a5
    case L'𝖦':     // MATHEMATICAL SANS CAPITAL G 1d5a6
    case L'𝖧':     // MATHEMATICAL SANS CAPITAL H 1d5a7
    case L'𝖨':     // MATHEMATICAL SANS CAPITAL I 1d5a8
    case L'𝖩':     // MATHEMATICAL SANS CAPITAL J 1d5a9
    case L'𝖪':     // MATHEMATICAL SANS CAPITAL K 1d5aa
    case L'𝖫':     // MATHEMATICAL SANS CAPITAL L 1d5ab
    case L'𝖬':     // MATHEMATICAL SANS CAPITAL M 1d5ac
    case L'𝖭':     // MATHEMATICAL SANS CAPITAL N 1d5ad
    case L'𝖮':     // MATHEMATICAL SANS CAPITAL O 1d5ae
    case L'𝖯':     // MATHEMATICAL SANS CAPITAL P 1d5af
    case L'𝖰':     // MATHEMATICAL SANS CAPITAL Q 1d5b0
    case L'𝖱':     // MATHEMATICAL SANS CAPITAL R 1d5b1
    case L'𝖲':     // MATHEMATICAL SANS CAPITAL S 1d5b2
    case L'𝖳':     // MATHEMATICAL SANS CAPITAL T 1d5b3
    case L'𝖴':     // MATHEMATICAL SANS CAPITAL U 1d5b4
    case L'𝖵':     // MATHEMATICAL SANS CAPITAL V 1d5b5
    case L'𝖶':     // MATHEMATICAL SANS CAPITAL W 1d5b6
    case L'𝖷':     // MATHEMATICAL SANS CAPITAL X 1d5b7
    case L'𝖸':     // MATHEMATICAL SANS CAPITAL Y 1d5b8
    case L'𝖹':     // MATHEMATICAL SANS CAPITAL Z 1d5b9
    case L'𝗔':     // MATHEMATICAL SANS BOLD CAPITAL A 1d5d4
    case L'𝗕':     // MATHEMATICAL SANS BOLD CAPITAL B 1d5d5
    case L'𝗖':     // MATHEMATICAL SANS BOLD CAPITAL C 1d5d6
    case L'𝗗':     // MATHEMATICAL SANS BOLD CAPITAL D 1d5d7
    case L'𝗘':     // MATHEMATICAL SANS BOLD CAPITAL E 1d5d8
    case L'𝗙':     // MATHEMATICAL SANS BOLD CAPITAL F 1d5d9
    case L'𝗚':     // MATHEMATICAL SANS BOLD CAPITAL G 1d5da
    case L'𝗛':     // MATHEMATICAL SANS BOLD CAPITAL H 1d5db
    case L'𝗜':     // MATHEMATICAL SANS BOLD CAPITAL I 1d5dc
    case L'𝗝':     // MATHEMATICAL SANS BOLD CAPITAL J 1d5dd
    case L'𝗞':     // MATHEMATICAL SANS BOLD CAPITAL K 1d5de
    case L'𝗟':     // MATHEMATICAL SANS BOLD CAPITAL L 1d5df
    case L'𝗠':     // MATHEMATICAL SANS BOLD CAPITAL M 1d5e0
    case L'𝗡':     // MATHEMATICAL SANS BOLD CAPITAL N 1d5e1
    case L'𝗢':     // MATHEMATICAL SANS BOLD CAPITAL O 1d5e2
    case L'𝗣':     // MATHEMATICAL SANS BOLD CAPITAL P 1d5e3
    case L'𝗤':     // MATHEMATICAL SANS BOLD CAPITAL Q 1d5e4
    case L'𝗥':     // MATHEMATICAL SANS BOLD CAPITAL R 1d5e5
    case L'𝗦':     // MATHEMATICAL SANS BOLD CAPITAL S 1d5e6
    case L'𝗧':     // MATHEMATICAL SANS BOLD CAPITAL T 1d5e7
    case L'𝗨':     // MATHEMATICAL SANS BOLD CAPITAL U 1d5e8
    case L'𝗩':     // MATHEMATICAL SANS BOLD CAPITAL V 1d5e9
    case L'𝗪':     // MATHEMATICAL SANS BOLD CAPITAL W 1d5ea
    case L'𝗫':     // MATHEMATICAL SANS BOLD CAPITAL X 1d5eb
    case L'𝗬':     // MATHEMATICAL SANS BOLD CAPITAL Y 1d5ec
    case L'𝗭':     // MATHEMATICAL SANS BOLD CAPITAL Z 1d5ed
    case L'𝘈':     // MATHEMATICAL SANS ITALIC CAPITAL A 1d608
    case L'𝘉':     // MATHEMATICAL SANS ITALIC CAPITAL B 1d609
    case L'𝘊':     // MATHEMATICAL SANS ITALIC CAPITAL C 1d60a
    case L'𝘋':     // MATHEMATICAL SANS ITALIC CAPITAL D 1d60b
    case L'𝘌':     // MATHEMATICAL SANS ITALIC CAPITAL E 1d60c
    case L'𝘍':     // MATHEMATICAL SANS ITALIC CAPITAL F 1d60d
    case L'𝘎':     // MATHEMATICAL SANS ITALIC CAPITAL G 1d60e
    case L'𝘏':     // MATHEMATICAL SANS ITALIC CAPITAL H 1d60f
    case L'𝘐':     // MATHEMATICAL SANS ITALIC CAPITAL I 1d610
    case L'𝘑':     // MATHEMATICAL SANS ITALIC CAPITAL J 1d611
    case L'𝘒':     // MATHEMATICAL SANS ITALIC CAPITAL K 1d612
    case L'𝘓':     // MATHEMATICAL SANS ITALIC CAPITAL L 1d613
    case L'𝘔':     // MATHEMATICAL SANS ITALIC CAPITAL M 1d614
    case L'𝘕':     // MATHEMATICAL SANS ITALIC CAPITAL N 1d615
    case L'𝘖':     // MATHEMATICAL SANS ITALIC CAPITAL O 1d616
    case L'𝘗':     // MATHEMATICAL SANS ITALIC CAPITAL P 1d617
    case L'𝘘':     // MATHEMATICAL SANS ITALIC CAPITAL Q 1d618
    case L'𝘙':     // MATHEMATICAL SANS ITALIC CAPITAL R 1d619
    case L'𝘚':     // MATHEMATICAL SANS ITALIC CAPITAL S 1d61a
    case L'𝘛':     // MATHEMATICAL SANS ITALIC CAPITAL T 1d61b
    case L'𝘜':     // MATHEMATICAL SANS ITALIC CAPITAL U 1d61c
    case L'𝘝':     // MATHEMATICAL SANS ITALIC CAPITAL V 1d61d
    case L'𝘞':     // MATHEMATICAL SANS ITALIC CAPITAL W 1d61e
    case L'𝘟':     // MATHEMATICAL SANS ITALIC CAPITAL X 1d61f
    case L'𝘠':     // MATHEMATICAL SANS ITALIC CAPITAL Y 1d620
    case L'𝘡':     // MATHEMATICAL SANS ITALIC CAPITAL Z 1d621
    case L'𝘼':     // MATHEMATICAL SANS BOLD ITALIC CAPITAL A 1d63c
    case L'𝘽':     // MATHEMATICAL SANS BOLD ITALIC CAPITAL B 1d63d
    case L'𝘾':     // MATHEMATICAL SANS BOLD ITALIC CAPITAL C 1d63e
    case L'𝘿':     // MATHEMATICAL SANS BOLD ITALIC CAPITAL D 1d63f
    case L'𝙀':     // MATHEMATICAL SANS BOLD ITALIC CAPITAL E 1d640
    case L'𝙁':     // MATHEMATICAL SANS BOLD ITALIC CAPITAL F 1d641
    case L'𝙂':     // MATHEMATICAL SANS BOLD ITALIC CAPITAL G 1d642
    case L'𝙃':     // MATHEMATICAL SANS BOLD ITALIC CAPITAL H 1d643
    case L'𝙄':     // MATHEMATICAL SANS BOLD ITALIC CAPITAL I 1d644
    case L'𝙅':     // MATHEMATICAL SANS BOLD ITALIC CAPITAL J 1d645
    case L'𝙆':     // MATHEMATICAL SANS BOLD ITALIC CAPITAL K 1d646
    case L'𝙇':     // MATHEMATICAL SANS BOLD ITALIC CAPITAL L 1d647
    case L'𝙈':     // MATHEMATICAL SANS BOLD ITALIC CAPITAL M 1d648
    case L'𝙉':     // MATHEMATICAL SANS BOLD ITALIC CAPITAL N 1d649
    case L'𝙊':     // MATHEMATICAL SANS BOLD ITALIC CAPITAL O 1d64a
    case L'𝙋':     // MATHEMATICAL SANS BOLD ITALIC CAPITAL P 1d64b
    case L'𝙌':     // MATHEMATICAL SANS BOLD ITALIC CAPITAL Q 1d64c
    case L'𝙍':     // MATHEMATICAL SANS BOLD ITALIC CAPITAL R 1d64d
    case L'𝙎':     // MATHEMATICAL SANS BOLD ITALIC CAPITAL S 1d64e
    case L'𝙏':     // MATHEMATICAL SANS BOLD ITALIC CAPITAL T 1d64f
    case L'𝙐':     // MATHEMATICAL SANS BOLD ITALIC CAPITAL U 1d650
    case L'𝙑':     // MATHEMATICAL SANS BOLD ITALIC CAPITAL V 1d651
    case L'𝙒':     // MATHEMATICAL SANS BOLD ITALIC CAPITAL W 1d652
    case L'𝙓':     // MATHEMATICAL SANS BOLD ITALIC CAPITAL X 1d653
    case L'𝙔':     // MATHEMATICAL SANS BOLD ITALIC CAPITAL Y 1d654
    case L'𝙕':     // MATHEMATICAL SANS BOLD ITALIC CAPITAL Z 1d655
    case L'𝙰':     // MATHEMATICAL MONOSPACE CAPITAL A 1d670
    case L'𝙱':     // MATHEMATICAL MONOSPACE CAPITAL B 1d671
    case L'𝙲':     // MATHEMATICAL MONOSPACE CAPITAL C 1d672
    case L'𝙳':     // MATHEMATICAL MONOSPACE CAPITAL D 1d673
    case L'𝙴':     // MATHEMATICAL MONOSPACE CAPITAL E 1d674
    case L'𝙵':     // MATHEMATICAL MONOSPACE CAPITAL F 1d675
    case L'𝙶':     // MATHEMATICAL MONOSPACE CAPITAL G 1d676
    case L'𝙷':     // MATHEMATICAL MONOSPACE CAPITAL H 1d677
    case L'𝙸':     // MATHEMATICAL MONOSPACE CAPITAL I 1d678
    case L'𝙹':     // MATHEMATICAL MONOSPACE CAPITAL J 1d679
    case L'𝙺':     // MATHEMATICAL MONOSPACE CAPITAL K 1d67a
    case L'𝙻':     // MATHEMATICAL MONOSPACE CAPITAL L 1d67b
    case L'𝙼':     // MATHEMATICAL MONOSPACE CAPITAL M 1d67c
    case L'𝙽':     // MATHEMATICAL MONOSPACE CAPITAL N 1d67d
    case L'𝙾':     // MATHEMATICAL MONOSPACE CAPITAL O 1d67e
    case L'𝙿':     // MATHEMATICAL MONOSPACE CAPITAL P 1d67f
    case L'𝚀':     // MATHEMATICAL MONOSPACE CAPITAL Q 1d680
    case L'𝚁':     // MATHEMATICAL MONOSPACE CAPITAL R 1d681
    case L'𝚂':     // MATHEMATICAL MONOSPACE CAPITAL S 1d682
    case L'𝚃':     // MATHEMATICAL MONOSPACE CAPITAL T 1d683
    case L'𝚄':     // MATHEMATICAL MONOSPACE CAPITAL U 1d684
    case L'𝚅':     // MATHEMATICAL MONOSPACE CAPITAL V 1d685
    case L'𝚆':     // MATHEMATICAL MONOSPACE CAPITAL W 1d686
    case L'𝚇':     // MATHEMATICAL MONOSPACE CAPITAL X 1d687
    case L'𝚈':     // MATHEMATICAL MONOSPACE CAPITAL Y 1d688
    case L'𝚉':     // MATHEMATICAL MONOSPACE CAPITAL Z 1d689
    case L'𝚨':     // MATHEMATICAL BOLD CAPITAL ALPHA 1d6a8
    case L'𝚩':     // MATHEMATICAL BOLD CAPITAL BETA 1d6a9
    case L'𝚪':     // MATHEMATICAL BOLD CAPITAL GAMMA 1d6aa
    case L'𝚫':     // MATHEMATICAL BOLD CAPITAL DELTA 1d6ab
    case L'𝚬':     // MATHEMATICAL BOLD CAPITAL EPSILON 1d6ac
    case L'𝚭':     // MATHEMATICAL BOLD CAPITAL ZETA 1d6ad
    case L'𝚮':     // MATHEMATICAL BOLD CAPITAL ETA 1d6ae
    case L'𝚯':     // MATHEMATICAL BOLD CAPITAL THETA 1d6af
    case L'𝚰':     // MATHEMATICAL BOLD CAPITAL IOTA 1d6b0
    case L'𝚱':     // MATHEMATICAL BOLD CAPITAL KAPPA 1d6b1
    case L'𝚲':     // MATHEMATICAL BOLD CAPITAL LAMDA 1d6b2
    case L'𝚳':     // MATHEMATICAL BOLD CAPITAL MU 1d6b3
    case L'𝚴':     // MATHEMATICAL BOLD CAPITAL NU 1d6b4
    case L'𝚵':     // MATHEMATICAL BOLD CAPITAL XI 1d6b5
    case L'𝚶':     // MATHEMATICAL BOLD CAPITAL OMICRON 1d6b6
    case L'𝚷':     // MATHEMATICAL BOLD CAPITAL PI 1d6b7
    case L'𝚸':     // MATHEMATICAL BOLD CAPITAL RHO 1d6b8
    case L'𝚹':     // MATHEMATICAL BOLD CAPITAL THETA SYMBOL 1d6b9
    case L'𝚺':     // MATHEMATICAL BOLD CAPITAL SIGMA 1d6ba
    case L'𝚻':     // MATHEMATICAL BOLD CAPITAL TAU 1d6bb
    case L'𝚼':     // MATHEMATICAL BOLD CAPITAL UPSILON 1d6bc
    case L'𝚽':     // MATHEMATICAL BOLD CAPITAL PHI 1d6bd
    case L'𝚾':     // MATHEMATICAL BOLD CAPITAL CHI 1d6be
    case L'𝚿':     // MATHEMATICAL BOLD CAPITAL PSI 1d6bf
    case L'𝛀':     // MATHEMATICAL BOLD CAPITAL OMEGA 1d6c0
    case L'𝛢':     // MATHEMATICAL ITALIC CAPITAL ALPHA 1d6e2
    case L'𝛣':     // MATHEMATICAL ITALIC CAPITAL BETA 1d6e3
    case L'𝛤':     // MATHEMATICAL ITALIC CAPITAL GAMMA 1d6e4
    case L'𝛥':     // MATHEMATICAL ITALIC CAPITAL DELTA 1d6e5
    case L'𝛦':     // MATHEMATICAL ITALIC CAPITAL EPSILON 1d6e6
    case L'𝛧':     // MATHEMATICAL ITALIC CAPITAL ZETA 1d6e7
    case L'𝛨':     // MATHEMATICAL ITALIC CAPITAL ETA 1d6e8
    case L'𝛩':     // MATHEMATICAL ITALIC CAPITAL THETA 1d6e9
    case L'𝛪':     // MATHEMATICAL ITALIC CAPITAL IOTA 1d6ea
    case L'𝛫':     // MATHEMATICAL ITALIC CAPITAL KAPPA 1d6eb
    case L'𝛬':     // MATHEMATICAL ITALIC CAPITAL LAMDA 1d6ec
    case L'𝛭':     // MATHEMATICAL ITALIC CAPITAL MU 1d6ed
    case L'𝛮':     // MATHEMATICAL ITALIC CAPITAL NU 1d6ee
    case L'𝛯':     // MATHEMATICAL ITALIC CAPITAL XI 1d6ef
    case L'𝛰':     // MATHEMATICAL ITALIC CAPITAL OMICRON 1d6f0
    case L'𝛱':     // MATHEMATICAL ITALIC CAPITAL PI 1d6f1
    case L'𝛲':     // MATHEMATICAL ITALIC CAPITAL RHO 1d6f2
    case L'𝛳':     // MATHEMATICAL ITALIC CAPITAL THETA SYMBOL 1d6f3
    case L'𝛴':     // MATHEMATICAL ITALIC CAPITAL SIGMA 1d6f4
    case L'𝛵':     // MATHEMATICAL ITALIC CAPITAL TAU 1d6f5
    case L'𝛶':     // MATHEMATICAL ITALIC CAPITAL UPSILON 1d6f6
    case L'𝛷':     // MATHEMATICAL ITALIC CAPITAL PHI 1d6f7
    case L'𝛸':     // MATHEMATICAL ITALIC CAPITAL CHI 1d6f8
    case L'𝛹':     // MATHEMATICAL ITALIC CAPITAL PSI 1d6f9
    case L'𝛺':     // MATHEMATICAL ITALIC CAPITAL OMEGA 1d6fa
    case L'𝜜':     // MATHEMATICAL BOLD ITALIC CAPITAL ALPHA 1d71c
    case L'𝜝':     // MATHEMATICAL BOLD ITALIC CAPITAL BETA 1d71d
    case L'𝜞':     // MATHEMATICAL BOLD ITALIC CAPITAL GAMMA 1d71e
    case L'𝜟':     // MATHEMATICAL BOLD ITALIC CAPITAL DELTA 1d71f
    case L'𝜠':     // MATHEMATICAL BOLD ITALIC CAPITAL EPSILON 1d720
    case L'𝜡':     // MATHEMATICAL BOLD ITALIC CAPITAL ZETA 1d721
    case L'𝜢':     // MATHEMATICAL BOLD ITALIC CAPITAL ETA 1d722
    case L'𝜣':     // MATHEMATICAL BOLD ITALIC CAPITAL THETA 1d723
    case L'𝜤':     // MATHEMATICAL BOLD ITALIC CAPITAL IOTA 1d724
    case L'𝜥':     // MATHEMATICAL BOLD ITALIC CAPITAL KAPPA 1d725
    case L'𝜦':     // MATHEMATICAL BOLD ITALIC CAPITAL LAMDA 1d726
    case L'𝜧':     // MATHEMATICAL BOLD ITALIC CAPITAL MU 1d727
    case L'𝜨':     // MATHEMATICAL BOLD ITALIC CAPITAL NU 1d728
    case L'𝜩':     // MATHEMATICAL BOLD ITALIC CAPITAL XI 1d729
    case L'𝜪':     // MATHEMATICAL BOLD ITALIC CAPITAL OMICRON 1d72a
    case L'𝜫':     // MATHEMATICAL BOLD ITALIC CAPITAL PI 1d72b
    case L'𝜬':     // MATHEMATICAL BOLD ITALIC CAPITAL RHO 1d72c
    case L'𝜭':     // MATHEMATICAL BOLD ITALIC CAPITAL THETA SYMBOL 1d72d
    case L'𝜮':     // MATHEMATICAL BOLD ITALIC CAPITAL SIGMA 1d72e
    case L'𝜯':     // MATHEMATICAL BOLD ITALIC CAPITAL TAU 1d72f
    case L'𝜰':     // MATHEMATICAL BOLD ITALIC CAPITAL UPSILON 1d730
    case L'𝜱':     // MATHEMATICAL BOLD ITALIC CAPITAL PHI 1d731
    case L'𝜲':     // MATHEMATICAL BOLD ITALIC CAPITAL CHI 1d732
    case L'𝜳':     // MATHEMATICAL BOLD ITALIC CAPITAL PSI 1d733
    case L'𝜴':     // MATHEMATICAL BOLD ITALIC CAPITAL OMEGA 1d734
    case L'𝝖':     // MATHEMATICAL SANS BOLD CAPITAL ALPHA 1d756
    case L'𝝗':     // MATHEMATICAL SANS BOLD CAPITAL BETA 1d757
    case L'𝝘':     // MATHEMATICAL SANS BOLD CAPITAL GAMMA 1d758
    case L'𝝙':     // MATHEMATICAL SANS BOLD CAPITAL DELTA 1d759
    case L'𝝚':     // MATHEMATICAL SANS BOLD CAPITAL EPSILON 1d75a
    case L'𝝛':     // MATHEMATICAL SANS BOLD CAPITAL ZETA 1d75b
    case L'𝝜':     // MATHEMATICAL SANS BOLD CAPITAL ETA 1d75c
    case L'𝝝':     // MATHEMATICAL SANS BOLD CAPITAL THETA 1d75d
    case L'𝝞':     // MATHEMATICAL SANS BOLD CAPITAL IOTA 1d75e
    case L'𝝟':     // MATHEMATICAL SANS BOLD CAPITAL KAPPA 1d75f
    case L'𝝠':     // MATHEMATICAL SANS BOLD CAPITAL LAMDA 1d760
    case L'𝝡':     // MATHEMATICAL SANS BOLD CAPITAL MU 1d761
    case L'𝝢':     // MATHEMATICAL SANS BOLD CAPITAL NU 1d762
    case L'𝝣':     // MATHEMATICAL SANS BOLD CAPITAL XI 1d763
    case L'𝝤':     // MATHEMATICAL SANS BOLD CAPITAL OMICRON 1d764
    case L'𝝥':     // MATHEMATICAL SANS BOLD CAPITAL PI 1d765
    case L'𝝦':     // MATHEMATICAL SANS BOLD CAPITAL RHO 1d766
    case L'𝝧':     // MATHEMATICAL SANS BOLD CAPITAL THETA SYMBOL 1d767
    case L'𝝨':     // MATHEMATICAL SANS BOLD CAPITAL SIGMA 1d768
    case L'𝝩':     // MATHEMATICAL SANS BOLD CAPITAL TAU 1d769
    case L'𝝪':     // MATHEMATICAL SANS BOLD CAPITAL UPSILON 1d76a
    case L'𝝫':     // MATHEMATICAL SANS BOLD CAPITAL PHI 1d76b
    case L'𝝬':     // MATHEMATICAL SANS BOLD CAPITAL CHI 1d76c
    case L'𝝭':     // MATHEMATICAL SANS BOLD CAPITAL PSI 1d76d
    case L'𝝮':     // MATHEMATICAL SANS BOLD CAPITAL OMEGA 1d76e
    case L'𝞐':     // MATHEMATICAL SANS BOLD ITALIC CAPITAL ALPHA 1d790
    case L'𝞑':     // MATHEMATICAL SANS BOLD ITALIC CAPITAL BETA 1d791
    case L'𝞒':     // MATHEMATICAL SANS BOLD ITALIC CAPITAL GAMMA 1d792
    case L'𝞓':     // MATHEMATICAL SANS BOLD ITALIC CAPITAL DELTA 1d793
    case L'𝞔':     // MATHEMATICAL SANS BOLD ITALIC CAPITAL EPSILON 1d794
    case L'𝞕':     // MATHEMATICAL SANS BOLD ITALIC CAPITAL ZETA 1d795
    case L'𝞖':     // MATHEMATICAL SANS BOLD ITALIC CAPITAL ETA 1d796
    case L'𝞗':     // MATHEMATICAL SANS BOLD ITALIC CAPITAL THETA 1d797
    case L'𝞘':     // MATHEMATICAL SANS BOLD ITALIC CAPITAL IOTA 1d798
    case L'𝞙':     // MATHEMATICAL SANS BOLD ITALIC CAPITAL KAPPA 1d799
    case L'𝞚':     // MATHEMATICAL SANS BOLD ITALIC CAPITAL LAMDA 1d79a
    case L'𝞛':     // MATHEMATICAL SANS BOLD ITALIC CAPITAL MU 1d79b
    case L'𝞜':     // MATHEMATICAL SANS BOLD ITALIC CAPITAL NU 1d79c
    case L'𝞝':     // MATHEMATICAL SANS BOLD ITALIC CAPITAL XI 1d79d
    case L'𝞞':     // MATHEMATICAL SANS BOLD ITALIC CAPITAL OMICRON 1d79e
    case L'𝞟':     // MATHEMATICAL SANS BOLD ITALIC CAPITAL PI 1d79f
    case L'𝞠':     // MATHEMATICAL SANS BOLD ITALIC CAPITAL RHO 1d7a0
    case L'𝞡':     // MATHEMATICAL SANS BOLD ITALIC CAPITAL THETA SYMBOL 1d7a1
    case L'𝞢':     // MATHEMATICAL SANS BOLD ITALIC CAPITAL SIGMA 1d7a2
    case L'𝞣':     // MATHEMATICAL SANS BOLD ITALIC CAPITAL TAU 1d7a3
    case L'𝞤':     // MATHEMATICAL SANS BOLD ITALIC CAPITAL UPSILON 1d7a4
    case L'𝞥':     // MATHEMATICAL SANS BOLD ITALIC CAPITAL PHI 1d7a5
    case L'𝞦':     // MATHEMATICAL SANS BOLD ITALIC CAPITAL CHI 1d7a6
    case L'𝞧':     // MATHEMATICAL SANS BOLD ITALIC CAPITAL PSI 1d7a7
    case L'𝞨':     // MATHEMATICAL SANS BOLD ITALIC CAPITAL OMEGA 1d7a8
    case L'𝟊':     // MATHEMATICAL BOLD CAPITAL DIGAMMA 1d7ca
    case L'𞤀':  // ADLAM CAPITAL ALIF 1e900
    case L'𞤁':  // ADLAM CAPITAL DAALI 1e901
    case L'𞤂':  // ADLAM CAPITAL LAAM 1e902
    case L'𞤃':  // ADLAM CAPITAL MIIM 1e903
    case L'𞤄':  // ADLAM CAPITAL BA 1e904
    case L'𞤅':  // ADLAM CAPITAL SINNYIIYHE 1e905
    case L'𞤆':  // ADLAM CAPITAL PE 1e906
    case L'𞤇':  // ADLAM CAPITAL BHE 1e907
    case L'𞤈':  // ADLAM CAPITAL RA 1e908
    case L'𞤉':  // ADLAM CAPITAL E 1e909
    case L'𞤊':  // ADLAM CAPITAL FA 1e90a
    case L'𞤋':  // ADLAM CAPITAL I 1e90b
    case L'𞤌':  // ADLAM CAPITAL O 1e90c
    case L'𞤍':  // ADLAM CAPITAL DHA 1e90d
    case L'𞤎':  // ADLAM CAPITAL YHE 1e90e
    case L'𞤏':  // ADLAM CAPITAL WAW 1e90f
    case L'𞤐':  // ADLAM CAPITAL NUN 1e910
    case L'𞤑':  // ADLAM CAPITAL KAF 1e911
    case L'𞤒':  // ADLAM CAPITAL YA 1e912
    case L'𞤓':  // ADLAM CAPITAL U 1e913
    case L'𞤔':  // ADLAM CAPITAL JIIM 1e914
    case L'𞤕':  // ADLAM CAPITAL CHI 1e915
    case L'𞤖':  // ADLAM CAPITAL HA 1e916
    case L'𞤗':  // ADLAM CAPITAL QAAF 1e917
    case L'𞤘':  // ADLAM CAPITAL GA 1e918
    case L'𞤙':  // ADLAM CAPITAL NYA 1e919
    case L'𞤚':  // ADLAM CAPITAL TU 1e91a
    case L'𞤛':  // ADLAM CAPITAL NHA 1e91b
    case L'𞤜':  // ADLAM CAPITAL VA 1e91c
    case L'𞤝':  // ADLAM CAPITAL KHA 1e91d
    case L'𞤞':  // ADLAM CAPITAL GBE 1e91e
    case L'𞤟':  // ADLAM CAPITAL ZAL 1e91f
    case L'𞤠':  // ADLAM CAPITAL KPO 1e920
    case L'𞤡':  // ADLAM CAPITAL SHA 1e921
      return 1;
    default:
      return 0;
  }
}
