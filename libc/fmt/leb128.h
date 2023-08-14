#ifndef COSMOPOLITAN_LIBC_FMT_LEB128_H_
#define COSMOPOLITAN_LIBC_FMT_LEB128_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
#ifdef _COSMO_SOURCE
COSMOPOLITAN_C_START_

#define sleb64    __sleb64
#define zleb64    __zleb64
#define uleb64    __uleb64
#define unzleb64  __unzleb64
#define unuleb64  __unuleb64
#define sleb128   __sleb128
#define zleb128   __zleb128
#define uleb128   __uleb128
#define unsleb128 __unsleb128

char *sleb64(char *, int64_t);
char *zleb64(char[hasatleast 10], int64_t);
char *uleb64(char[hasatleast 10], uint64_t);
int unzleb64(const char *, size_t, int64_t *);
int unuleb64(char *, size_t, uint64_t *);

#ifndef __STRICT_ANSI__
char *sleb128(char *, int128_t);
char *zleb128(char *, int128_t);
char *uleb128(char *, uint128_t);
int unsleb128(const void *, size_t, int128_t *);
#endif /* __STRICT_ANSI__ */

COSMOPOLITAN_C_END_
#endif /* _COSMO_SOURCE */
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_FMT_LEB128_H_ */
