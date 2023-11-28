#ifndef ZUTIL_H
#define ZUTIL_H
#include "libc/intrin/kprintf.h"
#include "libc/limits.h"
#include "third_party/zlib/zlib.h"

/* default windowBits for decompression. MAX_WBITS is for compression only */
#ifndef DEF_WBITS
#define DEF_WBITS MAX_WBITS
#endif

/* default memLevel */

/* The three kinds of block type */
#define STORED_BLOCK 0
#define STATIC_TREES 1
#define DYN_TREES    2

/* The minimum and maximum match lengths */
#define MIN_MATCH 3
#define MAX_MATCH 258

#define PRESET_DICT 0x20 /* preset dictionary flag in zlib header */

#define OS_CODE 3 /* assume Unix */

#if defined(STDC) && !defined(HAVE_MEMCPY) && !defined(NO_MEMCPY)
#define HAVE_MEMCPY
#endif

COSMOPOLITAN_C_START_

#ifndef local
#define local static
#endif
/* since "static" is used to mean two completely different things in C, we
   define "local" for the non-static meaning of "static", for readability
   (compile with -Dlocal if your debugger can't find static symbols) */

extern const char *const z_errmsg[10]; /* indexed by 2-zlib_error */
/* (size given to avoid silly warnings with Visual C++) */

#define ERR_MSG(err) z_errmsg[Z_NEED_DICT - (err)]

#define ERR_RETURN(strm, err) return (strm->msg = ERR_MSG(err), (err))
/* To be used only when the state is known to be valid */

#ifndef F_OPEN
#define F_OPEN(name, mode) fopen((name), (mode))
#endif

/* Diagnostic functions */
#ifdef ZLIB_DEBUG
extern int z_verbose;
extern void z_error(const char *, int, char *);
#define Assert(cond, msg)               \
  {                                     \
    if (!(cond)) {                      \
      z_error(__FILE__, __LINE__, msg); \
    }                                   \
  }
#define Trace(x)                   \
  {                                \
    if (z_verbose >= 0) kprintf x; \
  }
#define Tracev(x)                 \
  {                               \
    if (z_verbose > 0) kprintf x; \
  }
#define Tracevv(x)                \
  {                               \
    if (z_verbose > 1) kprintf x; \
  }
#define Tracec(c, x)                     \
  {                                      \
    if (z_verbose > 0 && (c)) kprintf x; \
  }
#define Tracecv(c, x)                    \
  {                                      \
    if (z_verbose > 1 && (c)) kprintf x; \
  }
#else
#define Assert(cond, msg)
#define Trace(x)
#define Tracev(x)
#define Tracevv(x)
#define Tracec(c, x)
#define Tracecv(c, x)
#endif

#define ZALLOC(strm, items, size) \
  (*((strm)->zalloc))((strm)->opaque, (items), (size))
#define ZFREE(strm, addr) (*((strm)->zfree))((strm)->opaque, (voidpf)(addr))
#define TRY_FREE(s, p)  \
  {                     \
    if (p) ZFREE(s, p); \
  }

/* Reverse the bytes in a 32-bit value */
#define ZSWAP32(q)                                                      \
  ((((q) >> 24) & 0xff) + (((q) >> 8) & 0xff00) + (((q)&0xff00) << 8) + \
   (((q)&0xff) << 24))

typedef unsigned char uch;
typedef uch uchf;
typedef unsigned short ush;
typedef ush ushf;
typedef unsigned long ulg;

#ifdef HAVE_HIDDEN
#define ZLIB_INTERNAL __attribute__((__visibility__("hidden")))
#else
#define ZLIB_INTERNAL
#endif

#ifndef local
#define local static
#endif

#if !defined(Z_U8) && !defined(Z_SOLO) && defined(STDC)
#if (ULONG_MAX == 0xffffffffffffffff)
#define Z_U8 unsigned long
#elif (ULLONG_MAX == 0xffffffffffffffff)
#define Z_U8 unsigned long long
#elif (UINT_MAX == 0xffffffffffffffff)
#define Z_U8 unsigned
#endif
#endif

#ifdef _MSC_VER
#define zalign(x) __declspec(align(x))
#else
#define zalign(x) __attribute__((aligned((x))))
#endif

COSMOPOLITAN_C_END_
#endif /* ZUTIL_H */
