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
#include "libc/fmt/bing.internal.h"
#include "libc/str/tab.internal.h"

/**
 * Turns binary octet into unicode glyph representation.
 *
 * Cosmopolitan displays RADIX-256 numbers using these digits:
 *
 *      0123456789abcdef
 *     0 ☺☻♥♦♣♠•◘○◙♂♀♪♫☼
 *     1►◄↕‼¶§▬↨↑↓→←∟↔▲▼
 *     2 !"#$%&'()*+,-./
 *     30123456789:;<=>?
 *     4@ABCDEFGHIJKLMNO
 *     5PQRSTUVWXYZ[\]^_
 *     6`abcdefghijklmno
 *     7pqrstuvwxyz{|}~⌂
 *     8ÇüéâäàåçêëèïîìÄÅ
 *     9ÉæÆôöòûùÿÖÜ¢£¥€ƒ
 *     aáíóúñÑªº¿⌐¬½¼¡«»
 *     b░▒▓│┤╡╢╖╕╣║╗╝╜╛┐
 *     c└┴┬├─┼╞╟╚╔╩╦╠═╬╧
 *     d╨╤╥╙╘╒╓╫╪┘┌█▄▌▐▀
 *     eαßΓπΣσμτΦΘΩδ∞φε∩
 *     f≡±≥≤⌠⌡÷≈°∙·√ⁿ²■λ
 *
 * IBM designed these glyphs for the PC to map onto the display bytes at
 * (char *)0xb8000. Because IBM authorized buyers of its PCs to inspect
 * and/or modify this region of memory, it became widely understood by
 * many developers as a quick way to visualize arbitrary data that's so
 * superior to hexdump -- a use-case that's lived on longer than the CGA
 * graphics card for which it was designed.
 *
 * @param b is binary octet to pictorally represent
 * @param intent controls canonical multimappings: ∅␀ ,\n◙,ε∊∈€, λ,etc.
 * @see unbing() for inverse
 */
int bing(int b, int intent) {
  unassert(0 <= b && b < 256);
  return kCp437[b & 0xff];
}
