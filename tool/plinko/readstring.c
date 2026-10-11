#/*─────────────────────────────────────────────────────────────────╗
 │ To the extent possible under law, Justin Tunney has waived       │
 │ all copyright and related or neighboring rights to this file,    │
 │ as it is written in the following disclaimers:                   │
 │   • http://unlicense.org/                                        │
 │   • http://creativecommons.org/publicdomain/zero/1.0/            │
 ╚─────────────────────────────────────────────────────────────────*/
#include "char.h"
#include "plinko.h"

#define CASE(OP, CODE) \
  case OP:             \
    CODE;              \
    break

int ReadString(unsigned x) {
  int i, n, y, z;
  ax = y = TERM;
  if (x == L'"') {
    dx = ReadRune();
    return ax;
  } else {
    z = ReadRune();
    if (x == L'\\') {
      x = z;
      z = ReadRune();
      switch (x) {
        CASE(L'a', x = L'\a');
        CASE(L'b', x = L'\b');
        CASE(L'e', x = 00033);
        CASE(L'f', x = L'\f');
        CASE(L'n', x = L'\n');
        CASE(L'r', x = L'\r');
        CASE(L't', x = L'\t');
        CASE(L'v', x = L'\v');
        case L'x':
          n = 2;
          goto ReadHexEscape;
        case L'u':
          n = 4;
          goto ReadHexEscape;
        case L'U':
          n = 8;
          goto ReadHexEscape;
        default:
          if (IsDigit(x)) {
            x = GetDiglet(x);
            for (i = 0; IsDigit(z) && i < 2; ++i) {
              x *= 8;
              x += GetDiglet(z);
              z = ReadRune();
            }
          }
          break;
        ReadHexEscape:
          for (x = i = 0; IsHex(z) && i < n; ++i) {
            x *= 16;
            x += GetDiglet(z);
            z = ReadRune();
          }
          break;
      }
    }
    y = ReadString(z);
  }
  return Intern(x, y);
}
