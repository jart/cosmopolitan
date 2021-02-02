
#define __VSCODE_INTELLISENSE__ 1
#define __BIGGEST_ALIGNMENT__ 1
#define __CHAR_MAX__ 0x7F
#define __SCHAR_MAX__ __CHAR_MAX__
#define __SHRT_MAX__ 0x7FFF
#define __INT_MAX__ 0x7FFFFFFF
#define __LONG_MAX__ 0x7FFFFFFFFFFFFFFF
#define __LONG_LONG_MAX__ __LONG_MAX__
#define __CHAR_MIN__ -((__CHAR_MAX__)+1)
#define __SHRT_MIN__ -((__SHRT_MAX__)+1)
#define __INT_MIN__ -((__INT_MAX__)+1)
#define __LONG_MIN__ -((__LONG_MAX__)+1)
#define __LONG_LONG_MIN__ -((__LONG_LONG_MAX__)+1)
#define __UCHAR_MAX__ 0xFF
#define __USHRT_MAX__ 0xFFFF
#define __UINT_MAX__ 0xFFFFFFFF
#define __ULONG_MAX__ 0xFFFFFFFFFFFFFFFF
#define __SIZE_MAX__ __ULONG_MAX__
#define __SSIZE_MAX__ __LONG_MAX__
#define __UINTPTR_MAX__ __SIZE_MAX__
#define __INTPTR_MAX__ __SSIZE_MAX__
#define __WCHAR_MAX__ __UINT_MAX__
#define __PTRDIFF_MAX__ __UINTPTR_MAX__
#define __INTMAX_MAX__ __ULONG_MAX__
#define __SIZEOF_POINTER__ 8
#define __SIZEOF_SHORT__ 2
#define __SIZEOF_INT__ 4
#define __SIZEOF_LONG__ 8
#define __SIZEOF_LONG_LONG__ 8
#define __SIZEOF_PTRDIFF_T__ 8
#define __SIZEOF_SIZE_T__ 8
#define __SIZEOF_WCHAR_T__ 4
#define __SIZEOF_WINT_T__ 4
#define __SIZEOF_FLOAT__ 4
#define __SIZEOF_FLOAT128__ 16
#define __SIZEOF_DOUBLE__ 8
#define __SIZEOF_FLOAT80__ 10
#define __SIZEOF_LONG_DOUBLE__ 16
#define __FLT_MAX__ 1
#define __DBL_MAX__ 1

#define __INT_LEAST8_TYPE__ signed char;
#define __INT_LEAST16_TYPE__ signed short;
#define __INT_LEAST32_TYPE__ signed int;
#define __INT_LEAST64_TYPE__ signed long long;
#define __UINT_LEAST8_TYPE__ unsigned char;
#define __UINT_LEAST16_TYPE__ unsigned short;
#define __UINT_LEAST32_TYPE__ unsigned int;
#define __UINT_LEAST64_TYPE__ unsigned long long;

#define __UINT8_MAX__ __UCHAR_MAX__
#define __UINT16_MAX__ __USHRT_MAX__
#define __UINT32_MAX__ __UINT_MAX__
#define __UINT64_MAX__ __ULONG_MAX__

#define __INT8_MAX__ __CHAR_MAX__
#define __INT16_MAX__ __SHRT_MAX__
#define __INT32_MAX__ __INT_MAX__
#define __INT64_MAX__ __LONG_MAX__

#define ATEXIT_MAX 1
#define STACKSIZE 0x10000
#define ARG_MAX 255
#define OPEN_MAX 0x7FFF
#define CHAR_BIT 7
#define NSIG 1
#define NULL (0)
#define false 0
#define true (!(false))
#define COSMOPOLITAN_C_START_
#define COSMOPOLITAN_C_END_
#define PAGESIZE 0x1000
#define BIGPAGESIZE 0x10000
#define NAME_MAX 255

#define SWITCHEROO(...) 0

extern unsigned g_loglevel;

typedef void * va_list;

#define va_start(...) 0
#define va_end(...) 0
#define va_arg(x, y) (y)(0)

typedef signed char int8_t;
typedef signed short int16_t;
typedef signed int int32_t;
#define int64_t long long
#define int128_t int64_t
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;
typedef unsigned long long uint128_t;

typedef int errno_t;
typedef uint64_t size_t;
typedef unsigned long long ssize_t;
typedef uint64_t uintptr_t;
typedef long long intptr_t;

typedef unsigned long long uintmax_t;
typedef long long intmax_t;

typedef uint32_t bool32;

#ifndef __cplusplus
typedef uint8_t bool;
typedef uint16_t char16_t;
typedef uint32_t char32_t;
#endif /* __cplusplus */

typedef char32_t wint_t;
typedef char32_t wchar_t;
typedef struct { int ax, dx; } axdx_t;

/* duplicate and replace xmmintrin.internal.h to fix it for IntelliSense
 * SEE: <> */

#define _Vector_size(x) __attribute__(( vector_size( x ) ))

#define __SIGACTION(...) (0)
#define VEIL(x, y) (y)
#define STATIC_YOINK(x)
#define BUFSIZ 1
#define FRAMESIZE 0x10000
#define PATH_MAX 252
#define concat(x, y, z) 0
#define CONCAT(a, b, c, d, e) 0
#define shuffle(...) 0
#define reverse(x, y) 0
#define atomic_load(...) 0
#define atomic_store(...) 0

#define _Generic_(...) (void*)(0)
#define _Generic(...) _Generic_

#define offsetof(x, y) 0
#define cmpxchg(...) 0
#define lockxchg(...) 0
#define lockcmpxchg(...) 0
#define xgetbv(...) 0
#define rdmsr(...) 0
#define INITIALIZER(...) struct _dummy
#define __far
#define tinystrstr(...) 0
#define BENCHLOOP(...) 0

#ifdef __hook
#undef __hook
#endif /* __hook */

#ifdef UriKeyval
#undef UriKeyval
#endif /* UriKeyval */

#define _Static_assert(...) 0
