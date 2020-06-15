/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/assert.h"
#include "libc/fmt/bing.h"
#include "libc/str/str.h"

/**
 * Turns binary octet into unicode glyph representation.
 *
 * Cosmopolitan displays RADIX-256 numbers using these digits:
 *
 *    0123456789abcdef
 *   0 ☺☻♥♦♣♠•◘○◙♂♀♪♫☼
 *   1►◄↕‼¶§▬↨↑↓→←∟↔▲▼
 *   2 !"#$%&'()*+,-./
 *   30123456789:;<=>?
 *   4@ABCDEFGHIJKLMNO
 *   5PQRSTUVWXYZ[\]^_
 *   6`abcdefghijklmno
 *   7pqrstuvwxyz{|}~⌂
 *   8ÇüéâäàåçêëèïîìÄÅ
 *   9ÉæÆôöòûùÿÖÜ¢£¥€ƒ
 *   aáíóúñÑªº¿⌐¬½¼¡«»
 *   b░▒▓│┤╡╢╖╕╣║╗╝╜╛┐
 *   c└┴┬├─┼╞╟╚╔╩╦╠═╬╧
 *   d╨╤╥╙╘╒╓╫╪┘┌█▄▌▐▀
 *   eαßΓπΣσμτΦΘΩδ∞φε∩
 *   f≡±≥≤⌠⌡÷≈°∙·√ⁿ²■λ
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
  assert(0 <= b && b < 256);
  return kCp437[b & 0xff]; /* TODO(jart): multimappings */
}
