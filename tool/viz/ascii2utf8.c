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
#include "libc/errno.h"
#include "libc/stdio/stdio.h"

/**
  @fileoverview Converts ASCII combining marks to UNICODE.

  Getting started:

    nroff -mandoc -rLL=72n -rLT=78n -Tutf8 <manpage.1 |
      o/tool/viz/ascii2utf8.com

  ASCII Bold:

    - CHAR BACKSPACE CHAR                      c ++ "\b" ++ c
      e.g. AABBCCDD                    410841 420842 430843 440844

  ASCII Underlines:

    - CHAR BACKSPACE LOW LINE                  c ++ "\b_"
      e.g. A_B_C_D_                    41085F 42085F 43085F 44085F

  UNICODE Underlines:

    - COMBINING LOW LINE                       U+0332 (\xCC\xB1)
      e.g. A̲B̲C̲D̲ (ugly notches)                 41CCB2 42CCB2 43CCB2 44CCB2

    - COMBINING MACRON BELOW                   U+0331 (\xCC\xB1)
      e.g. A̱ḆC̱Ḏ (too short)                    41CCB1 42CCB1 43CCB1 44CCB1

    - COMBINING DOUBLE MACRON BELOW            U+035F (\xCD\x9F)
      e.g. A͟B͟C͟D͟ (too long)                     41CD9F 42CD9F 43CD9F 44CD9F

    - DOUBLE PLUS COMBINING MACRON BELOW       3×U+035F + 1×U+0331
      e.g. A͟B͟C͟Ḏ (too narrow)                   41CCB1 42CCB1 43CCB1 44CD9F

    - DOUBLE PLUS COMBINING MACRON LOW LINE    [it's complicated]
      e.g. A͟B͟C͟D̲ (𝑓𝑙𝑎𝑤𝑙𝑒𝑠𝑠)                     41CD9F 42CD9F 43CD9F 44CCB2

*/

const wint_t kBackspace = '\b';
const wint_t kCombiningLowLine = L'\u0332';
const wint_t kCombiningDoubleMacronBelow = L'\u035f';

forceinline int PutChar(wint_t (*buf)[3], size_t *i, wint_t *cc, FILE *out) {
  if (fputwc((*buf)[0], out) == -1) return -1;
  if (*cc != -1) {
    if (fputwc(*cc, out) == -1) return -1;
    *cc = -1;
  }
  (*buf)[0] = (*buf)[1];
  (*buf)[1] = (*buf)[2];
  --*i;
  return 0;
}

int CombineAsciiMarks(FILE *in, FILE *out) {
  wint_t buf[3], wc = 0, cc = -1;
  size_t i = 0;
  for (;;) {
    while (i < 3) {
      if ((wc = fgetwc(in)) == -1) goto InputBreak;
      buf[i++] = wc;
    }
    if (buf[1] == '\b' && cc == -1) {
      if (buf[0] == buf[2]) { /* bold */
        if (L'0' <= buf[0] && buf[0] <= L'9') {
          buf[0] = L'𝟬' + (buf[0] - L'0');
          i = 1;
        } else if (L'A' <= buf[0] && buf[0] <= L'Z') {
          buf[0] = L'𝐀' + (buf[0] - L'A');
          i = 1;
        } else if ('a' <= buf[0] && buf[0] <= L'z') {
          buf[0] = L'𝗮' + (buf[0] - L'a');
          i = 1;
        } else {
          i = 1;
        }
      } else if (buf[2] == '_') { /* underline */
        cc = kCombiningLowLine;
        i = 1;
      } else if (buf[0] == '_') {
        cc = kCombiningLowLine;
        buf[0] = buf[2];
        i = 1;
      }
    }
    if (i == 3) {
      if (PutChar(&buf, &i, &cc, out) == -1) goto OutputBreak;
    }
  }
InputBreak:
  while (i) {
    if (PutChar(&buf, &i, &cc, out) == -1) goto OutputBreak;
  }
OutputBreak:
  return (fclose(in) | fclose(out)) != -1 ? 0 : -1;
}

int main(int argc, char *argv[]) {
  return CombineAsciiMarks(stdin, stdout) != -1 ? 0 : errno;
}
