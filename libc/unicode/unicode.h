#ifndef COSMOPOLITAN_LIBC_UNICODE_UNICODE_H_
#define COSMOPOLITAN_LIBC_UNICODE_UNICODE_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § characters » unicode                                      ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

extern const uint8_t kEastAsianWidth[];
extern const uint32_t kEastAsianWidthBits;
extern const uint8_t kCombiningChars[];
extern const uint32_t kCombiningCharsBits;

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § strings » multibyte » unicode                             ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

int wcwidth(wchar_t) pureconst;
int wcswidth(const wchar_t *) strlenesque;
int wcsnwidth(const wchar_t *, size_t) strlenesque;
int strwidth(const char *) strlenesque;
int strnwidth(const char *, size_t) strlenesque;
int strwidth16(const char16_t *) strlenesque;
int strnwidth16(const char16_t *, size_t) strlenesque;

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § unicode » generic typing                                  ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#if __STDC_VERSION__ + 0 >= 201112

#define strwidth(s)              \
  _Generic(*(s), wchar_t         \
           : wcswidth, char16_t  \
           : strwidth16, default \
           : strwidth)(s)

#define strnwidth(s, n)           \
  _Generic(*(s), wchar_t          \
           : wcswidth, char16_t   \
           : strnwidth16, default \
           : strnwidth)(s, n)

#endif /* C11 */

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_UNICODE_UNICODE_H_ */
