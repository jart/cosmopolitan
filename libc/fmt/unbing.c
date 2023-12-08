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
#include "libc/fmt/bing.internal.h"
#include "libc/macros.internal.h"

static const int kCp437i[] = {
    0x000a << 8 | 10,   // \n NEWLINE
    0x000d << 8 | 13,   // \r CARRIAGE RETURN
    0x0022 << 8 | 34,   // " QUOTATION MARK
    0x0027 << 8 | 39,   // ' APOSTROPHE
    0x003f << 8 | 63,   // ? QUESTION MARK
    0x005c << 8 | 92,   // \ REVERSE SOLIDUS
    0x00a0 << 8 | 0,    //   NO-BREAK SPACE
    0x00a1 << 8 | 173,  // ¡ INVERTED EXCLAMATION MARK
    0x00a2 << 8 | 155,  // ¢ CENT SIGN
    0x00a3 << 8 | 156,  // £ POUND SIGN
    0x00a5 << 8 | 157,  // ¥ YEN SIGN
    0x00a7 << 8 | 21,   // § SECTION SIGN
    0x00aa << 8 | 166,  // ª FEMININE ORDINAL INDICATOR
    0x00ab << 8 | 174,  // « LEFT-POINTING DOUBLE ANGLE QUOTATION MARK
    0x00ac << 8 | 170,  // ¬ NOT SIGN
    0x00b0 << 8 | 248,  // ° DEGREE SIGN
    0x00b1 << 8 | 241,  // ± PLUS-MINUS SIGN
    0x00b2 << 8 | 253,  // ² SUPERSCRIPT TWO
    0x00b5 << 8 | 230,  // µ MICRO SIGN
    0x00b6 << 8 | 20,   // ¶ PILCROW SIGN
    0x00b7 << 8 | 250,  // · MIDDLE DOT
    0x00ba << 8 | 167,  // º MASCULINE ORDINAL INDICATOR
    0x00bb << 8 | 175,  // » RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK
    0x00bc << 8 | 172,  // ¼ VULGAR FRACTION ONE QUARTER
    0x00bd << 8 | 171,  // ½ VULGAR FRACTION ONE HALF
    0x00bf << 8 | 168,  // ¿ INVERTED QUESTION MARK
    0x00c4 << 8 | 142,  // Ä LATIN CAPITAL LETTER A WITH DIAERESIS
    0x00c5 << 8 | 143,  // Å LATIN CAPITAL LETTER A WITH RING ABOVE
    0x00c6 << 8 | 146,  // Æ LATIN CAPITAL LETTER AE
    0x00c7 << 8 | 128,  // Ç LATIN CAPITAL LETTER C WITH CEDILLA
    0x00c9 << 8 | 144,  // É LATIN CAPITAL LETTER E WITH ACUTE
    0x00d1 << 8 | 165,  // Ñ LATIN CAPITAL LETTER N WITH TILDE
    0x00d6 << 8 | 153,  // Ö LATIN CAPITAL LETTER O WITH DIAERESIS
    0x00d7 << 8 | 250,  // × MULTIPLICATION SIGN
    0x00dc << 8 | 154,  // Ü LATIN CAPITAL LETTER U WITH DIAERESIS
    0x00df << 8 | 225,  // ß LATIN SMALL LETTER SHARP S
    0x00e0 << 8 | 133,  // à LATIN SMALL LETTER A WITH GRAVE
    0x00e1 << 8 | 160,  // á LATIN SMALL LETTER A WITH ACUTE
    0x00e2 << 8 | 131,  // â LATIN SMALL LETTER A WITH CIRCUMFLEX
    0x00e4 << 8 | 132,  // ä LATIN SMALL LETTER A WITH DIAERESIS
    0x00e5 << 8 | 134,  // å LATIN SMALL LETTER A WITH RING ABOVE
    0x00e6 << 8 | 145,  // æ LATIN SMALL LETTER AE
    0x00e7 << 8 | 135,  // ç LATIN SMALL LETTER C WITH CEDILLA
    0x00e8 << 8 | 138,  // è LATIN SMALL LETTER E WITH GRAVE
    0x00e9 << 8 | 130,  // é LATIN SMALL LETTER E WITH ACUTE
    0x00ea << 8 | 136,  // ê LATIN SMALL LETTER E WITH CIRCUMFLEX
    0x00eb << 8 | 137,  // ë LATIN SMALL LETTER E WITH DIAERESIS
    0x00ec << 8 | 141,  // ì LATIN SMALL LETTER I WITH GRAVE
    0x00ed << 8 | 161,  // í LATIN SMALL LETTER I WITH ACUTE
    0x00ee << 8 | 140,  // î LATIN SMALL LETTER I WITH CIRCUMFLEX
    0x00ef << 8 | 139,  // ï LATIN SMALL LETTER I WITH DIAERESIS
    0x00f1 << 8 | 164,  // ñ LATIN SMALL LETTER N WITH TILDE
    0x00f2 << 8 | 149,  // ò LATIN SMALL LETTER O WITH GRAVE
    0x00f3 << 8 | 162,  // ó LATIN SMALL LETTER O WITH ACUTE
    0x00f4 << 8 | 147,  // ô LATIN SMALL LETTER O WITH CIRCUMFLEX
    0x00f6 << 8 | 148,  // ö LATIN SMALL LETTER O WITH DIAERESIS
    0x00f7 << 8 | 246,  // ÷ DIVISION SIGN
    0x00f9 << 8 | 151,  // ù LATIN SMALL LETTER U WITH GRAVE
    0x00fa << 8 | 163,  // ú LATIN SMALL LETTER U WITH ACUTE
    0x00fb << 8 | 150,  // û LATIN SMALL LETTER U WITH CIRCUMFLEX
    0x00fc << 8 | 129,  // ü LATIN SMALL LETTER U WITH DIAERESIS
    0x00ff << 8 | 152,  // ÿ LATIN SMALL LETTER Y WITH DIAERESIS
    0x017f << 8 | 244,  // ſ LATIN SMALL LETTER LONG S
    0x0192 << 8 | 159,  // ƒ LATIN SMALL LETTER F WITH HOOK
    0x0393 << 8 | 226,  // Γ GREEK CAPITAL LETTER GAMMA
    0x0398 << 8 | 233,  // Θ GREEK CAPITAL LETTER THETA
    0x03a0 << 8 | 227,  // Π GREEK CAPITAL LETTER PI
    0x03a3 << 8 | 228,  // Σ GREEK CAPITAL LETTER SIGMA
    0x03a6 << 8 | 232,  // Φ GREEK CAPITAL LETTER PHI
    0x03a9 << 8 | 234,  // Ω GREEK CAPITAL LETTER OMEGA
    0x03b1 << 8 | 224,  // α GREEK SMALL LETTER ALPHA
    0x03b2 << 8 | 225,  // β GREEK SMALL LETTER BETA
    0x03b4 << 8 | 235,  // δ GREEK SMALL LETTER DELTA
    0x03b5 << 8 | 238,  // ε GREEK SMALL LETTER EPSILON
    0x03bb << 8 | 255,  // λ GREEK SMALL LETTER LAMDA
    0x03bc << 8 | 230,  // μ GREEK SMALL LETTER MU
    0x03c0 << 8 | 227,  // π GREEK SMALL LETTER PI
    0x03c3 << 8 | 229,  // σ GREEK SMALL LETTER SIGMA
    0x03c4 << 8 | 231,  // τ GREEK SMALL LETTER TAU
    0x03c6 << 8 | 237,  // φ GREEK SMALL LETTER PHI
    0x03d5 << 8 | 237,  // ϕ GREEK PHI SYMBOL
    0x2018 << 8 | 39,   // ‘ LEFT SINGLE QUOTATION MARK
    0x201c << 8 | 34,   // “ LEFT DOUBLE QUOTATION MARK
    0x2022 << 8 | 7,    // • BULLET
    0x203c << 8 | 19,   // ‼ DOUBLE EXCLAMATION MARK
    0x2047 << 8 | 63,   // ⁇ DOUBLE QUESTION MARK
    0x207f << 8 | 252,  // ⁿ SUPERSCRIPT LATIN SMALL LETTER N
    0x20a7 << 8 | 158,  // ₧ PESETA SIGN
    0x20ac << 8 | 158,  // € EURO SIGN
    0x2126 << 8 | 234,  // Ω OHM SIGN
    0x2190 << 8 | 27,   // ← LEFTWARDS ARROW
    0x2191 << 8 | 24,   // ↑ UPWARDS ARROW
    0x2192 << 8 | 26,   // → RIGHTWARDS ARROW
    0x2193 << 8 | 25,   // ↓ DOWNWARDS ARROW
    0x2194 << 8 | 29,   // ↔ LEFT RIGHT ARROW
    0x2195 << 8 | 18,   // ↕ UP DOWN ARROW
    0x21a8 << 8 | 23,   // ↨ UP DOWN ARROW WITH BASE
    0x2202 << 8 | 235,  // ∂ PARTIAL DIFFERENTIAL
    0x2205 << 8 | 0,    // ∅ EMPTY SET
    0x2208 << 8 | 238,  // ∈ ELEMENT OF
    0x220a << 8 | 238,  // ∊ SMALL ELEMENT OF
    0x220f << 8 | 227,  // ∏ N-ARY PRODUCT
    0x2211 << 8 | 228,  // ∑ N-ARY SUMMATION
    0x2219 << 8 | 249,  // ∙ BULLET OPERATOR
    0x221a << 8 | 251,  // √ SQUARE ROOT
    0x221e << 8 | 236,  // ∞ INFINITY
    0x221f << 8 | 28,   // ∟ RIGHT ANGLE
    0x2229 << 8 | 239,  // ∩ INTERSECTION
    0x2248 << 8 | 247,  // ≈ ALMOST EQUAL TO
    0x2261 << 8 | 240,  // ≡ IDENTICAL TO
    0x2264 << 8 | 243,  // ≤ LESS-THAN OR EQUAL TO
    0x2265 << 8 | 242,  // ≥ GREATER-THAN OR EQUAL TO
    0x2302 << 8 | 127,  // ⌂ HOUSE
    0x2310 << 8 | 169,  // ⌐ REVERSED NOT SIGN
    0x2320 << 8 | 244,  // ⌠ TOP HALF INTEGRAL
    0x2321 << 8 | 245,  // ⌡ BOTTOM HALF INTEGRAL
    0x2400 << 8 | 0,    // ␀ SYMBOL FOR NULL
    0x2500 << 8 | 196,  // ─ BOX DRAWS LIGHT HORIZONTAL
    0x2502 << 8 | 179,  // │ BOX DRAWS LIGHT VERTICAL
    0x250c << 8 | 218,  // ┌ BOX DRAWS LIGHT DOWN AND RIGHT
    0x2510 << 8 | 191,  // ┐ BOX DRAWS LIGHT DOWN AND LEFT
    0x2514 << 8 | 192,  // └ BOX DRAWS LIGHT UP AND RIGHT
    0x2518 << 8 | 217,  // ┘ BOX DRAWS LIGHT UP AND LEFT
    0x251c << 8 | 195,  // ├ BOX DRAWS LIGHT VERTICAL AND RIGHT
    0x2524 << 8 | 180,  // ┤ BOX DRAWS LIGHT VERTICAL AND LEFT
    0x252c << 8 | 194,  // ┬ BOX DRAWS LIGHT DOWN AND HORIZONTAL
    0x2534 << 8 | 193,  // ┴ BOX DRAWS LIGHT UP AND HORIZONTAL
    0x253c << 8 | 197,  // ┼ BOX DRAWS LIGHT VERTICAL AND HORIZONTAL
    0x2550 << 8 | 205,  // ═ BOX DRAWS DOUBLE HORIZONTAL
    0x2551 << 8 | 186,  // ║ BOX DRAWS DOUBLE VERTICAL
    0x2552 << 8 | 213,  // ╒ BOX DRAWS DOWN SINGLE AND RIGHT DOUBLE
    0x2553 << 8 | 214,  // ╓ BOX DRAWS DOWN DOUBLE AND RIGHT SINGLE
    0x2554 << 8 | 201,  // ╔ BOX DRAWS DOUBLE DOWN AND RIGHT
    0x2555 << 8 | 184,  // ╕ BOX DRAWS DOWN SINGLE AND LEFT DOUBLE
    0x2556 << 8 | 183,  // ╖ BOX DRAWS DOWN DOUBLE AND LEFT SINGLE
    0x2557 << 8 | 187,  // ╗ BOX DRAWS DOUBLE DOWN AND LEFT
    0x2558 << 8 | 212,  // ╘ BOX DRAWS UP SINGLE AND RIGHT DOUBLE
    0x2559 << 8 | 211,  // ╙ BOX DRAWS UP DOUBLE AND RIGHT SINGLE
    0x255a << 8 | 200,  // ╚ BOX DRAWS DOUBLE UP AND RIGHT
    0x255b << 8 | 190,  // ╛ BOX DRAWS UP SINGLE AND LEFT DOUBLE
    0x255c << 8 | 189,  // ╜ BOX DRAWS UP DOUBLE AND LEFT SINGLE
    0x255d << 8 | 188,  // ╝ BOX DRAWS DOUBLE UP AND LEFT
    0x255e << 8 | 198,  // ╞ BOX DRAWS VERTICAL SINGLE AND RIGHT DOUBLE
    0x255f << 8 | 199,  // ╟ BOX DRAWS VERTICAL DOUBLE AND RIGHT SINGLE
    0x2560 << 8 | 204,  // ╠ BOX DRAWS DOUBLE VERTICAL AND RIGHT
    0x2561 << 8 | 181,  // ╡ BOX DRAWS VERTICAL SINGLE AND LEFT DOUBLE
    0x2562 << 8 | 182,  // ╢ BOX DRAWS VERTICAL DOUBLE AND LEFT SINGLE
    0x2563 << 8 | 185,  // ╣ BOX DRAWS DOUBLE VERTICAL AND LEFT
    0x2564 << 8 | 209,  // ╤ BOX DRAWS DOWN SINGLE AND HORIZONTAL DOUBLE
    0x2565 << 8 | 210,  // ╥ BOX DRAWS DOWN DOUBLE AND HORIZONTAL SINGLE
    0x2566 << 8 | 203,  // ╦ BOX DRAWS DOUBLE DOWN AND HORIZONTAL
    0x2567 << 8 | 207,  // ╧ BOX DRAWS UP SINGLE AND HORIZONTAL DOUBLE
    0x2568 << 8 | 208,  // ╨ BOX DRAWS UP DOUBLE AND HORIZONTAL SINGLE
    0x2569 << 8 | 202,  // ╩ BOX DRAWS DOUBLE UP AND HORIZONTAL
    0x256a << 8 | 216,  // ╪ BOX DRAWS VERTICAL SINGLE AND HORIZONTAL DOUBLE
    0x256b << 8 | 215,  // ╫ BOX DRAWS VERTICAL DOUBLE AND HORIZONTAL SINGLE
    0x256c << 8 | 206,  // ╬ BOX DRAWS DOUBLE VERTICAL AND HORIZONTAL
    0x2580 << 8 | 223,  // ▀ UPPER HALF BLOCK
    0x2584 << 8 | 220,  // ▄ LOWER HALF BLOCK
    0x2588 << 8 | 219,  // █ FULL BLOCK
    0x258c << 8 | 221,  // ▌ LEFT HALF BLOCK
    0x2590 << 8 | 222,  // ▐ RIGHT HALF BLOCK
    0x2591 << 8 | 176,  // ░ LIGHT SHADE
    0x2592 << 8 | 177,  // ▒ MEDIUM SHADE
    0x2593 << 8 | 178,  // ▓ DARK SHADE
    0x25a0 << 8 | 254,  // ■ BLACK SQUARE
    0x25ac << 8 | 22,   // ▬ BLACK RECTANGLE
    0x25b2 << 8 | 30,   // ▲ BLACK UP-POINTING TRIANGLE
    0x25ba << 8 | 16,   // ► BLACK RIGHT-POINTING POINTER
    0x25bc << 8 | 31,   // ▼ BLACK DOWN-POINTING TRIANGLE
    0x25c4 << 8 | 17,   // ◄ BLACK LEFT-POINTING POINTER
    0x25cb << 8 | 9,    // ○ WHITE CIRCLE
    0x25d8 << 8 | 8,    // ◘ INVERSE BULLET
    0x25d9 << 8 | 10,   // ◙ INVERSE WHITE CIRCLE
    0x263a << 8 | 1,    // ☺ WHITE SMILING FACE
    0x263b << 8 | 2,    // ☻ BLACK SMILING FACE
    0x263c << 8 | 15,   // ☼ WHITE SUN WITH RAYS
    0x2640 << 8 | 12,   // ♀ FEMALE SIGN
    0x2642 << 8 | 11,   // ♂ MALE SIGN
    0x2660 << 8 | 6,    // ♠ BLACK SPADE SUIT
    0x2663 << 8 | 5,    // ♣ BLACK CLUB SUIT
    0x2665 << 8 | 3,    // ♥ BLACK HEART SUIT
    0x2666 << 8 | 4,    // ♦ BLACK DIAMOND SUIT
    0x266a << 8 | 13,   // ♪ EIGHTH NOTE
    0x266b << 8 | 14,   // ♫ BEAMED EIGHTH NOTES
};

/**
 * Turns CP437 unicode glyph into its binary representation.
 *
 * @param c is a unicode character
 * @return byte representation, or -1 if ch wasn't ibm cp437
 * @see bing()
 */
int unbing(int c) {
  int m, l, r;
  if (32 <= c && c < 127) return c;
  l = 0;
  r = ARRAYLEN(kCp437i) - 1;
  while (l <= r) {
    m = (l & r) + ((l ^ r) >> 1);  // floor((a+b)/2)
    if ((kCp437i[m] >> 8) < c) {
      l = m + 1;
    } else if ((kCp437i[m] >> 8) > c) {
      r = m - 1;
    } else {
      return kCp437i[m] & 255;
    }
  }
  return -1;
}
