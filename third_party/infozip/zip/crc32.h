/*
  Copyright (c) 1990-2008 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2000-Apr-09 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
/* crc32.h -- compute the CRC-32 of a data stream
 * Copyright (C) 1995 Mark Adler
 * For conditions of distribution and use, see copyright notice in zlib.h
 */

#ifndef __crc32_h
#define __crc32_h       /* identifies this source module */

/* This header should be read AFTER zip.h resp. unzip.h
 * (the latter with UNZIP_INTERNAL defined...).
 */

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
