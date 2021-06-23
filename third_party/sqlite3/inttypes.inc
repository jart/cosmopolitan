#ifndef COSMOPOLITAN_THIRD_PARTY_SQLITE3_INTTYPES_H_
#define COSMOPOLITAN_THIRD_PARTY_SQLITE3_INTTYPES_H_
#include "third_party/sqlite3/sqlite3.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

/*
** Integers of known sizes.  These typedefs might change for architectures
** where the sizes very.  Preprocessor macros are available so that the
** types can be conveniently redefined at compile-type.  Like this:
**
**         cc '-DUINTPTR_TYPE=long long int' ...
*/
#ifndef UINT32_TYPE
#ifdef HAVE_UINT32_T
#define UINT32_TYPE uint32_t
#else
#define UINT32_TYPE unsigned int
#endif
#endif
#ifndef UINT16_TYPE
#ifdef HAVE_UINT16_T
#define UINT16_TYPE uint16_t
#else
#define UINT16_TYPE unsigned short int
#endif
#endif
#ifndef INT16_TYPE
#ifdef HAVE_INT16_T
#define INT16_TYPE int16_t
#else
#define INT16_TYPE short int
#endif
#endif
#ifndef UINT8_TYPE
#ifdef HAVE_UINT8_T
#define UINT8_TYPE uint8_t
#else
#define UINT8_TYPE unsigned char
#endif
#endif
#ifndef INT8_TYPE
#ifdef HAVE_INT8_T
#define INT8_TYPE int8_t
#else
#define INT8_TYPE signed char
#endif
#endif
#ifndef LONGDOUBLE_TYPE
#define LONGDOUBLE_TYPE long double
#endif
typedef sqlite_int64 i64;  /* 8-byte signed integer */
typedef sqlite_uint64 u64; /* 8-byte unsigned integer */
typedef UINT32_TYPE u32;   /* 4-byte unsigned integer */
typedef UINT16_TYPE u16;   /* 2-byte unsigned integer */
typedef INT16_TYPE i16;    /* 2-byte signed integer */
typedef UINT8_TYPE u8;     /* 1-byte unsigned integer */
typedef INT8_TYPE i8;      /* 1-byte signed integer */

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_THIRD_PARTY_SQLITE3_INTTYPES_H_ */
