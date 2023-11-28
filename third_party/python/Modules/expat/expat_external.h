#ifndef Expat_External_INCLUDED
#define Expat_External_INCLUDED 1
#include "third_party/python/Modules/expat/pyexpatns.h"
COSMOPOLITAN_C_START_

#if defined(__GNUC__)                                                          \
    && (__GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 96))
#  define XML_ATTR_MALLOC __attribute__((__malloc__))
#else
#  define XML_ATTR_MALLOC
#endif

#if defined(__GNUC__)                                                          \
    && ((__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 3))
#  define XML_ATTR_ALLOC_SIZE(x) __attribute__((__alloc_size__(x)))
#else
#  define XML_ATTR_ALLOC_SIZE(x)
#endif

typedef char XML_Char;
typedef char XML_LChar;

typedef long long XML_Index;
typedef unsigned long long XML_Size;

COSMOPOLITAN_C_END_
#endif /* not Expat_External_INCLUDED */
