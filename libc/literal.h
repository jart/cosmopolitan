#ifndef COSMOPOLITAN_LIBC_LITERAL_H_
#define COSMOPOLITAN_LIBC_LITERAL_H_
#define __STDC_CONSTANT_MACROS

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

#if UINTPTR_MAX == UINT64_MAX
#define INTMAX_C(c)  c##L
#define UINTMAX_C(c) c##UL
#else
#define INTMAX_C(c)  c##LL
#define UINTMAX_C(c) c##ULL
#endif

#endif /* COSMOPOLITAN_LIBC_LITERAL_H_ */
