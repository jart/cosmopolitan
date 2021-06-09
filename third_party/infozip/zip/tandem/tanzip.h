/*
  Copyright (c) 1990-2002 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2000-Apr-09 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
#ifndef __tanzip_h   /* prevent multiple inclusions */
#define __tanzip_h

# define fopen nskopen  /*  To allow us to set extent sizes */

#  define USE_CASE_MAP

  FILE     *nskopen(const char *, const char *);
  int       zopen  (const char *, int);
  int       zclose (int);
  unsigned  zread (int, char *, unsigned);
  void      nskformatopt(char *);

  #define   getpid  zgetpid
  unsigned  zgetpid (void);

#define CBSZ 0x10000  /* Was used for both fcopy and file_read.        */
                      /* Created separate define (SBSZ) for file_read  */
                      /* fcopy param is type size_t (unsigned long)    */
                      /* For Guardian we choose a multiple of 4K       */

#define ZBSZ 0x10000  /* This is used in call to setvbuf, 64K seems to work  */
                      /* in all memory models. Again it is an unsigned long  */
                      /* For Guardian we choose a multiple of 4K             */

#ifndef __INT32
#define SBSZ 0x0e000  /* Maximum of size unsigned (int). Only used in STORE  */
                      /* method.  We can use up to 56K bytes thanks to large */
                      /* transfer mode.  Note WSIZE is limited to 32K, which */
                      /* limits the DEFLATE read size to same value.         */
#else
#define SBSZ 0x10000  /* WIDE model so we can use 64K                        */
#endif /* __INT32 */

#endif /* !__tanzip_h */
