#/*─────────────────────────────────────────────────────────────────╗
 │ To the extent possible under law, Justin Tunney has waived       │
 │ all copyright and related or neighboring rights to this file,    │
 │ as it is written in the following disclaimers:                   │
 │   • http://unlicense.org/                                        │
 │   • http://creativecommons.org/publicdomain/zero/1.0/            │
 ╚─────────────────────────────────────────────────────────────────*/
#include "char.h"

bool IsHex(int c) {
  return ((L'0' <= c && c <= L'9') || (L'A' <= c && c <= L'F') ||
          (L'a' <= c && c <= L'f'));
}

int GetDiglet(int c) {
  if (IsDigit(c))
    return c - L'0';
  if (IsUpper(c))
    return c - L'A' + 10;
  if (IsLower(c))
    return c - L'a' + 10;
  return -1;
}

bool IsSpace(int c) {
  switch (c) {
    case L' ':
    case L'\t':
    case L'\n':
    case L'\f':
    case L'\v':
    case L'\r':
      return true;
    default:
      return false;
  }
}

bool IsParen(int c) {
  switch (c) {
    case L'(':
    case L')':
    case L'[':
    case L']':
    case L'{':
    case L'}':
      return true;
    default:
      return false;
  }
}
