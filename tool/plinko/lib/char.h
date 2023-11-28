#ifndef COSMOPOLITAN_TOOL_PLINKO_LIB_CHAR_H_
#define COSMOPOLITAN_TOOL_PLINKO_LIB_CHAR_H_
COSMOPOLITAN_C_START_

static inline pureconst bool IsC0(int c) {
  return (0 <= c && c < 32) || c == 0177;
}

static inline pureconst bool IsDigit(int c) {
  return L'0' <= c && c <= L'9';
}

static inline pureconst bool IsUpper(int c) {
  return L'A' <= c && c <= L'Z';
}

static inline pureconst bool IsLower(int c) {
  return L'a' <= c && c <= L'z';
}

static inline pureconst bool IsMathAlnum(int c) {
  return 0x1d400 <= c && c <= 0x1d7ff;
}

static inline pureconst bool IsControl(int c) {
  return (0 <= c && c <= 0x1F) || (0x7F <= c && c <= 0x9F);
}

static dontinstrument pureconst inline int ToUpper(int c) {
  return 'a' <= c && c <= 'z' ? 'A' - 'a' + c : c;
}

int GetDiglet(int) pureconst;
bool IsHex(int) pureconst;
bool IsParen(int) pureconst;
bool IsSpace(int) pureconst;
int GetMonospaceCharacterWidth(int) pureconst;
bool IsWide(int) pureconst;

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_TOOL_PLINKO_LIB_CHAR_H_ */
