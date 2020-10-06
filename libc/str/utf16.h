#ifndef COSMOPOLITAN_LIBC_STR_UTF16_H_
#define COSMOPOLITAN_LIBC_STR_UTF16_H_

#define UTF16_MASK 0xfc00
#define UTF16_MOAR 0xd800 /* 0xD800..0xDBFF */
#define UTF16_CONT 0xdc00 /* 0xDC00..0xDBFF */

#define IsUcs2(wc)         (((wc)&UTF16_MASK) != UTF16_MOAR)
#define IsUtf16Cont(wc)    (((wc)&UTF16_MASK) == UTF16_CONT)
#define MergeUtf16(lo, hi) ((((lo)-0xD800) << 10) + ((hi)-0xDC00) + 0x10000)

#endif /* COSMOPOLITAN_LIBC_STR_UTF16_H_ */
