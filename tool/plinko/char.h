#ifndef COSMOPOLITAN_TOOL_PLINKO_LIB_CHAR_H_
#define COSMOPOLITAN_TOOL_PLINKO_LIB_CHAR_H_
#include <stdbool.h>

static inline bool IsC0(int c) {
  return (0 <= c && c < 32) || c == 0177;
}

static inline bool IsDigit(int c) {
  return L'0' <= c && c <= L'9';
}

static inline bool IsUpper(int c) {
  return L'A' <= c && c <= L'Z';
}

static inline bool IsLower(int c) {
  return L'a' <= c && c <= L'z';
}

static inline bool IsMathAlnum(int c) {
  return 0x1d400 <= c && c <= 0x1d7ff;
}

static inline bool IsControl(int c) {
  return (0 <= c && c <= 0x1F) || (0x7F <= c && c <= 0x9F);
}

static inline int ToUpper(int c) {
  return 'a' <= c && c <= 'z' ? 'A' - 'a' + c : c;
}

int GetDiglet(int);
bool IsHex(int);
bool IsParen(int);
bool IsSpace(int);
int GetMonospaceCharacterWidth(int);
bool IsWide(int);

#endif /* COSMOPOLITAN_TOOL_PLINKO_LIB_CHAR_H_ */
