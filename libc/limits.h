#ifndef COSMOPOLITAN_LIBC_LIMITS_H_
#define COSMOPOLITAN_LIBC_LIMITS_H_

#define __MAX2MIN(I) (-I - 1)

#define SIZEOF_SHORT       __SIZEOF_SHORT__
#define SIZEOF_INT         __SIZEOF_INT__
#define SIZEOF_LONG        __SIZEOF_LONG__
#define SIZEOF_LONG_LONG   __SIZEOF_LONG_LONG__
#define SIZEOF_POINTER     __SIZEOF_POINTER__
#define SIZEOF_PTRDIFF_T   __SIZEOF_PTRDIFF_T__
#define SIZEOF_SIZE_T      __SIZEOF_SIZE_T__
#define SIZEOF_WCHAR_T     __SIZEOF_WCHAR_T__
#define SIZEOF_WINT_T      __SIZEOF_WINT_T__
#define SIZEOF_FLOAT       __SIZEOF_FLOAT__
#define SIZEOF_FLOAT128    __SIZEOF_FLOAT128__
#define SIZEOF_DOUBLE      __SIZEOF_DOUBLE__
#define SIZEOF_FLOAT80     __SIZEOF_FLOAT80__
#define SIZEOF_LONG_DOUBLE __SIZEOF_LONG_DOUBLE__
#define SIZEOF_INTMAX      __SIZEOF_INTMAX__

#define SCHAR_MAX     __SCHAR_MAX__
#define SHRT_MAX      __SHRT_MAX__
#define INT_MAX       __INT_MAX__
#define LONG_MAX      __LONG_MAX__
#define LLONG_MAX     LONG_LONG_MAX
#define LONG_LONG_MAX __LONG_LONG_MAX__
#define SIZE_MAX      __SIZE_MAX__
#define INT8_MAX      __INT8_MAX__
#define INT16_MAX     __INT16_MAX__
#define INT32_MAX     __INT32_MAX__
#define INT64_MAX     __INT64_MAX__
#define WINT_MAX      __WCHAR_MAX__
#define WCHAR_MAX     __WCHAR_MAX__
#define INTPTR_MAX    __INTPTR_MAX__
#define PTRDIFF_MAX   __PTRDIFF_MAX__

#define SCHAR_MIN     __MAX2MIN(SCHAR_MAX)
#define SHRT_MIN      __MAX2MIN(SHRT_MAX)
#define INT_MIN       __MAX2MIN(INT_MAX)
#define LONG_MIN      __MAX2MIN(LONG_MAX)
#define LLONG_MIN     __MAX2MIN(LLONG_MAX)
#define LONG_LONG_MIN __MAX2MIN(LONG_LONG_MAX)
#define SIZE_MIN      __MAX2MIN(SIZE_MAX)
#define INT8_MIN      __MAX2MIN(INT8_MAX)
#define INT16_MIN     __MAX2MIN(INT16_MAX)
#define INT32_MIN     __MAX2MIN(INT32_MAX)
#define INT64_MIN     __MAX2MIN(INT64_MAX)
#define INTMAX_MIN    __MAX2MIN(INTMAX_MAX)
#define INTPTR_MIN    __MAX2MIN(INTPTR_MAX)
#define WINT_MIN      __MAX2MIN(WINT_MAX)
#define WCHAR_MIN     __MAX2MIN(WCHAR_MAX)
#define PTRDIFF_MIN   __MAX2MIN(PTRDIFF_MAX)

#define USHRT_MAX      (SHRT_MAX << 1 | 1)
#define UINT_MAX       (~0U)
#define ULONG_MAX      (~0LU)
#define ULLONG_MAX     (~0LLU)
#define ULONG_LONG_MAX (~0LLU)

#define UINTPTR_MAX __UINTPTR_MAX__
#define UINT8_MAX   __UINT8_MAX__
#define UINT16_MAX  __UINT16_MAX__
#define UINT32_MAX  __UINT32_MAX__
#define UINT64_MAX  __UINT64_MAX__

#define USHRT_MIN      ((unsigned short)0)
#define UINT_MIN       ((unsigned)0)
#define ULONG_MIN      ((unsigned long)0)
#define ULLONG_MIN     ((unsigned long long)0)
#define ULONG_LONG_MIN ((unsigned long long)0)
#define UINT8_MIN      ((uint8_t)0)
#define UINT16_MIN     ((uint16_t)0)
#define UINT32_MIN     ((uint32_t)0)
#define UINT64_MIN     ((uint64_t)0)
#define UINTMAX_MIN    ((uintmax_t)0)
#define UINTPTR_MIN    ((uintptr_t)0)

#define MB_CUR_MAX 6
#define MB_LEN_MAX 6

#if !(__ASSEMBLER__ + __LINKER__ + 0)

#if __GNUC__ * 100 + __GNUC_MINOR__ >= 406 || defined(__llvm__)
#define INTMAX_MAX \
  (((intmax_t)0x7fffffffffffffff) << 64 | (intmax_t)0xffffffffffffffff)
#define UINTMAX_MAX \
  (((uintmax_t)0xffffffffffffffff) << 64 | (uintmax_t)0xffffffffffffffff)
#define INT128_MIN  INTMAX_MIN
#define INT128_MAX  INTMAX_MAX
#define UINT128_MIN ((uintmax_t)0)
#define UINT128_MAX UINTMAX_MAX
#else
#define INTMAX_MAX  __INT64_MAX__
#define UINTMAX_MAX __UINT64_MAX__
#endif /* GCC 4.6+ */

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_LIMITS_H_ */
