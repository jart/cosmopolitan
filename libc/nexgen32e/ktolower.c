/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/str/tab.internal.h"

_Alignas(uint8_t) const uint8_t kToLower[256] = {
    0,   1,   2,   3,   4,   5,   6,    7,   8,    9,   10,  11,   12,  13,
    14,  15,  16,  17,  18,  19,  20,   21,  22,   23,  24,  25,   26,  27,
    28,  29,  30,  31,  ' ', '!', '\"', '#', '$',  '%', '&', '\'', '(', ')',
    '*', '+', ',', '-', '.', '/', '0',  '1', '2',  '3', '4', '5',  '6', '7',
    '8', '9', ':', ';', '<', '=', '>',  '?', '@',  'a', 'b', 'c',  'd', 'e',
    'f', 'g', 'h', 'i', 'j', 'k', 'l',  'm', 'n',  'o', 'p', 'q',  'r', 's',
    't', 'u', 'v', 'w', 'x', 'y', 'z',  '[', '\\', ']', '^', '_',  '`', 'a',
    'b', 'c', 'd', 'e', 'f', 'g', 'h',  'i', 'j',  'k', 'l', 'm',  'n', 'o',
    'p', 'q', 'r', 's', 't', 'u', 'v',  'w', 'x',  'y', 'z', '{',  '|', '}',
    '~', 127, 128, 129, 130, 131, 132,  133, 134,  135, 136, 137,  138, 139,
    140, 141, 142, 143, 144, 145, 146,  147, 148,  149, 150, 151,  152, 153,
    154, 155, 156, 157, 158, 159, 160,  161, 162,  163, 164, 165,  166, 167,
    168, 169, 170, 171, 172, 173, 174,  175, 176,  177, 178, 179,  180, 181,
    182, 183, 184, 185, 186, 187, 188,  189, 190,  191, 192, 193,  194, 195,
    196, 197, 198, 199, 200, 201, 202,  203, 204,  205, 206, 207,  208, 209,
    210, 211, 212, 213, 214, 215, 216,  217, 218,  219, 220, 221,  222, 223,
    224, 225, 226, 227, 228, 229, 230,  231, 232,  233, 234, 235,  236, 237,
    238, 239, 240, 241, 242, 243, 244,  245, 246,  247, 248, 249,  250, 251,
    252, 253, 254, 255,
};
