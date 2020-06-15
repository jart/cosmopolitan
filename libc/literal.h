#ifndef COSMOPOLITAN_LIBC_LITERAL_H_
#define COSMOPOLITAN_LIBC_LITERAL_H_

#ifdef __INT8_C
#define INT8_C(c)   __INT8_C(c)
#define UINT8_C(c)  __UINT8_C(c)
#define INT16_C(c)  __INT16_C(c)
#define UINT16_C(c) __UINT16_C(c)
#define INT32_C(c)  __INT32_C(c)
#define UINT32_C(c) __UINT32_C(c)
#define INT64_C(c)  __INT64_C(c)
#define UINT64_C(c) __UINT64_C(c)
#else
#define INT8_C(c)   c
#define UINT8_C(c)  c
#define INT16_C(c)  c
#define UINT16_C(c) c
#define INT32_C(c)  c
#define UINT32_C(c) c##U
#define INT64_C(c)  c##L
#define UINT64_C(c) c##UL
#endif

#if __SIZEOF_INTMAX__ == 16
#define INT128_C(c)  ((intmax_t)(c))
#define UINT128_C(c) ((uintmax_t)(c))
#elif __SIZEOF_INTMAX__ == 8
#define INT128_C(c)  __INT64_C(c)
#define UINT128_C(c) __UINT64_C(c)
#endif

#endif /* COSMOPOLITAN_LIBC_LITERAL_H_ */
