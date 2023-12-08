/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"

static bool isoctal(char c) {
  return '0' <= c && c <= '7';
}

char *U(char *p) {
  int c, i = 0, j = 0;
  for (;;) {
    switch ((c = p[i++] & 255)) {
      case '\\':
        switch ((c = p[i++] & 255)) {
          case '0':
          case '1':
          case '2':
          case '3':
          case '4':
          case '5':
          case '6':
          case '7':
            if (isoctal(p[i])) {
              if (isoctal(p[i + 1])) {
                p[j++] = (c - '0') << 6 | (p[i] - '0') << 3 | (p[i + 1] - '0');
                i += 2;
                continue;
              } else {
                p[j++] = (c - '0') << 3 | (p[i++] - '0');
                continue;
              }
            } else {
              p[j++] = (c - '0');
              continue;
            }
          case '\\':
            p[j++] = '\\';
            continue;
          case 'a':
            p[j++] = '\a';
            continue;
          case 'b':
            p[j++] = '\b';
            continue;
          case 't':
            p[j++] = '\t';
            continue;
          case 'f':
            p[j++] = '\f';
            continue;
          case 'v':
            p[j++] = '\v';
            continue;
          case 'e':
            p[j++] = '\e';
            continue;
          case 'r':
            p[j++] = '\r';
            continue;
          case 'n':
            p[j++] = '\n';
            continue;
          case '"':
            p[j++] = '"';
            continue;
          case '\'':
            p[j++] = '\'';
            continue;
        }
        // fallthrough
      default:
        p[j++] = c;
        break;
      case '\0':
        p[j] = 0;
        return p;
    }
  }
}

int main(int argc, char *argv[]) {
  switch (argc) {
    case 2:
      printf(U(argv[1]));
      return 0;
    case 3:
      printf(U(argv[1]), argv[2]);
      return 0;
    case 4:
      printf(U(argv[1]), argv[2], argv[3]);
      return 0;
    case 5:
      printf(U(argv[1]), argv[2], argv[3], argv[4]);
      return 0;
    case 6:
      printf(U(argv[1]), argv[2], argv[3], argv[4], argv[5]);
      return 0;
    case 7:
      printf(U(argv[1]), argv[2], argv[3], argv[4], argv[5], argv[6]);
      return 0;
    case 8:
      printf(U(argv[1]), argv[2], argv[3], argv[4], argv[5], argv[6], argv[7]);
      return 0;
    default:
      fprintf(stderr, "%s: %s format [arguments]\n", argv[0], argv[0]);
      return 1;
  }
}
