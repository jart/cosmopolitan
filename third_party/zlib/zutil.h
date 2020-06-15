#ifndef ZUTIL_H
#define ZUTIL_H
#include "third_party/zlib/zlib.h"

/* default windowBits for decompression. MAX_WBITS is for compression only */
#ifndef DEF_WBITS
#define DEF_WBITS MAX_WBITS
#endif

/* default memLevel */

/* The three kinds of block type */
#define STORED_BLOCK 0
#define STATIC_TREES 1
#define DYN_TREES 2

/* The minimum and maximum match lengths */
#define MIN_MATCH 3
#define MAX_MATCH 258

#define PRESET_DICT 0x20 /* preset dictionary flag in zlib header */

#define OS_CODE 3 /* assume Unix */

#if !(__ASSEMBLER__ + __LINKER__ + 0)
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
#include "libc/stdio/stdio.h"
extern int z_verbose hidden;
extern void z_error(char *m) hidden;
#define Assert(cond, msg)      \
  {                            \
    if (!(cond)) z_error(msg); \
  }
#define Trace(x)                   \
  {                                \
    if (z_verbose >= 0) fprintf x; \
  }
#define Tracev(x)                 \
  {                               \
    if (z_verbose > 0) fprintf x; \
  }
#define Tracevv(x)                \
  {                               \
    if (z_verbose > 1) fprintf x; \
  }
#define Tracec(c, x)                     \
  {                                      \
    if (z_verbose > 0 && (c)) fprintf x; \
  }
#define Tracecv(c, x)                    \
  {                                      \
    if (z_verbose > 1 && (c)) fprintf x; \
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

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* ZUTIL_H */
