#define __INT8_MAX__    0x7f
#define __UINT8_MAX__   0xff
#define __INT16_MAX__   0x7fff
#define __UINT16_MAX__  0xffff
#define __SHRT_MAX__    0x7fff
#define __INT_MAX__     0x7fffffff
#define __INT32_MAX__   0x7fffffff
#define __UINT32_MAX__  0xffffffffu
#define __INT64_MAX__   0x7fffffffffffffffl
#define __UINT64_MAX__  0xfffffffffffffffful
#define __SIZE_MAX__    0xfffffffffffffffful
#define __INTPTR_MAX__  0x7fffffffffffffffl
#define __UINTPTR_MAX__ 0xfffffffffffffffful
#define __WINT_MAX__    0xffffffffu
#define __UINTMAX_MAX__ 0xffffffffffffffffUL
#define __INTMAX_MAX__  0x7fffffffffffffffL

#define __SIZEOF_SHORT__       2
#define __SIZEOF_INT__         4
#define __SIZEOF_LONG__        8
#define __SIZEOF_LONG_LONG__   8
#define __SIZEOF_POINTER__     8
#define __SIZEOF_PTRDIFF_T__   8
#define __SIZEOF_SIZE_T__      8
#define __SIZEOF_WCHAR_T__     4
#define __SIZEOF_WINT_T__      4
#define __SIZEOF_FLOAT__       4
#define __SIZEOF_FLOAT128__    16
#define __SIZEOF_DOUBLE__      8
#define __SIZEOF_FLOAT80__     16
#define __SIZEOF_LONG_DOUBLE__ 16

#if !(__ASSEMBLER__ + __LINKER__ + 0)

#define __CHAR16_TYPE__  short unsigned int
#define __CHAR32_TYPE__  unsigned int
#define __INT16_TYPE__   short int
#define __INT32_TYPE__   int
#define __INT64_TYPE__   long int
#define __INT8_TYPE__    signed char
#define __INTMAX_TYPE__  long int
#define __INTPTR_TYPE__  long int
#define __PTRDIFF_TYPE__ long int
#define __SIZE_TYPE__    long unsigned int
#define __UINT16_TYPE__  short unsigned int
#define __UINT32_TYPE__  unsigned int
#define __UINT64_TYPE__  long unsigned int
#define __UINT8_TYPE__   unsigned char
#define __UINTMAX_TYPE__ long unsigned int
#define __UINTPTR_TYPE__ long unsigned int
#define __WCHAR_TYPE__   int
#define __WINT_TYPE__    unsigned int

#define __INT_LEAST8_TYPE__   __INT8_TYPE__
#define __UINT_LEAST8_TYPE__  __UINT8_TYPE__
#define __INT_LEAST16_TYPE__  __INT32_TYPE__
#define __UINT_LEAST16_TYPE__ __UINT16_TYPE__
#define __INT_LEAST32_TYPE__  __INT16_TYPE__
#define __UINT_LEAST32_TYPE__ __UINT32_TYPE__
#define __INT_LEAST64_TYPE__  __INT64_TYPE__
#define __UINT_LEAST64_TYPE__ __UINT64_TYPE__
#define __INT_FAST8_TYPE__    __INT8_TYPE__
#define __UINT_FAST8_TYPE__   __UINT8_TYPE__
#define __INT_FAST16_TYPE__   __INT32_TYPE__
#define __UINT_FAST16_TYPE__  __UINT32_TYPE__
#define __INT_FAST32_TYPE__   __INT32_TYPE__
#define __UINT_FAST32_TYPE__  __UINT32_TYPE__
#define __INT_FAST64_TYPE__   __INT64_TYPE__
#define __UINT_FAST64_TYPE__  __UINT64_TYPE__

#endif
