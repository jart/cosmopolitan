#ifndef __crc32_h
#define __crc32_h
#include "third_party/infozip/zip/zip.h"
/* clang-format off */

#ifndef OF
#  define OF(a) a
#endif
#ifndef ZCONST
#  define ZCONST const
#endif

#ifdef DYNALLOC_CRCTAB
   void     free_crc_table  OF((void));
#endif
#ifndef USE_ZLIB
   ZCONST ulg near *get_crc_table  OF((void));
#endif
#if (defined(USE_ZLIB) || defined(CRC_TABLE_ONLY))
#  ifdef IZ_CRC_BE_OPTIMIZ
#    undef IZ_CRC_BE_OPTIMIZ
#  endif
#else /* !(USE_ZLIB || CRC_TABLE_ONLY) */
   ulg      crc32           OF((ulg crc, ZCONST uch *buf, extent len));
#endif /* ?(USE_ZLIB || CRC_TABLE_ONLY) */

#ifndef CRC_32_TAB
#  define CRC_32_TAB     crc_32_tab
#endif

#ifdef CRC32
#  undef CRC32
#endif
#ifdef IZ_CRC_BE_OPTIMIZ
#  define CRC32UPD(c, crctab) (crctab[((c) >> 24)] ^ ((c) << 8))
#  define CRC32(c, b, crctab) (crctab[(((int)(c) >> 24) ^ (b))] ^ ((c) << 8))
#  define REV_BE(w) (((w)>>24)+(((w)>>8)&0xff00)+ \
                    (((w)&0xff00)<<8)+(((w)&0xff)<<24))
#else
#  define CRC32UPD(c, crctab) (crctab[((int)(c)) & 0xff] ^ ((c) >> 8))
#  define CRC32(c, b, crctab) (crctab[((int)(c) ^ (b)) & 0xff] ^ ((c) >> 8))
#  define REV_BE(w) w
#endif

#endif /* !__crc32_h */
