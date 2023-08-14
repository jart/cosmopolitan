#ifndef COSMOPOLITAN_LIBC_FMT_ITOA_H_
#define COSMOPOLITAN_LIBC_FMT_ITOA_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
#ifdef _COSMO_SOURCE
COSMOPOLITAN_C_START_

#define LengthInt64           __LengthInt64
#define LengthUint64          __LengthUint64
#define LengthInt64Thousands  __LengthInt64Thousands
#define LengthUint64Thousands __LengthUint64Thousands
#define FormatInt32           __FormatInt32
#define FormatUint32          __FormatUint32
#define FormatInt64           __FormatInt64
#define FormatUint64          __FormatUint64
#define FormatInt64Thousands  __FormatInt64Thousands
#define FormatUint64Thousands __FormatUint64Thousands
#define FormatOctal32         __FormatOctal32
#define FormatOctal64         __FormatOctal64
#define FormatBinary64        __FormatBinary64
#define FormatHex64           __FormatHex64
#define FormatFlex64          __FormatFlex64
#define uint64toarray_radix16 __uint64toarray_radix16
#define uint64toarray_fixed16 __uint64toarray_fixed16
#define uint64toarray_radix8  __uint64toarray_radix8

unsigned LengthInt64(int64_t) pureconst;
unsigned LengthUint64(uint64_t) pureconst;
unsigned LengthInt64Thousands(int64_t) pureconst;
unsigned LengthUint64Thousands(uint64_t) pureconst;
char *FormatInt32(char[hasatleast 12], int32_t);
char *FormatUint32(char[hasatleast 12], uint32_t);
char *FormatInt64(char[hasatleast 21], int64_t);
char *FormatUint64(char[hasatleast 21], uint64_t);
char *FormatInt64Thousands(char[hasatleast 27], int64_t);
char *FormatUint64Thousands(char[hasatleast 27], uint64_t);
char *FormatOctal32(char[hasatleast 13], uint32_t, bool);
char *FormatOctal64(char[hasatleast 24], uint64_t, bool);
char *FormatBinary64(char[hasatleast 67], uint64_t, char);
char *FormatHex64(char[hasatleast 19], uint64_t, char);
char *FormatFlex64(char[hasatleast 24], int64_t, char);
size_t uint64toarray_radix16(uint64_t, char[hasatleast 17]);
size_t uint64toarray_fixed16(uint64_t, char[hasatleast 17], uint8_t);
size_t uint64toarray_radix8(uint64_t, char[hasatleast 24]);

#ifndef __STRICT_ANSI__
size_t int128toarray_radix10(int128_t, char *);
size_t uint128toarray_radix10(uint128_t, char *);
#endif

COSMOPOLITAN_C_END_
#endif /* _COSMO_SOURCE */
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_FMT_ITOA_H_ */
