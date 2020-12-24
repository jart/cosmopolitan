#ifndef COSMOPOLITAN_LIBC_STR_UTF16_H_
#define COSMOPOLITAN_LIBC_STR_UTF16_H_

#define UTF16_MASK 0xfc00
#define UTF16_MOAR 0xd800 /* 0xD800..0xDBFF */
#define UTF16_CONT 0xdc00 /* 0xDC00..0xDBFF */

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define IsUcs2(wc)         (((wc)&UTF16_MASK) != UTF16_MOAR)
#define IsUtf16Cont(wc)    (((wc)&UTF16_MASK) == UTF16_CONT)
#define MergeUtf16(lo, hi) ((((lo)-0xD800) << 10) + ((hi)-0xDC00) + 0x10000)
#define EncodeUtf16(wc)                                    \
  (__builtin_expect(((0x0000 <= (wc) && (wc) <= 0xFFFF) || \
                     (0xE000 <= (wc) && (wc) <= 0xFFFF)),  \
                    1)                                     \
       ? (wc)                                              \
       : 0x10000 <= (wc) && (wc) <= 0x10FFFF               \
             ? (((((wc)-0x10000) >> 10) + 0xD800) |        \
                ((((wc)-0x10000) & 1023) + 0xDC00) << 16)  \
             : 0xFFFD)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_STR_UTF16_H_ */
