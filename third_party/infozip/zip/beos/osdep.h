/*
  Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 1999-Oct-05 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, both of these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html
*/
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <support/Errors.h>     /* for B_NO_ERROR */

#define USE_EF_UT_TIME          /* Enable use of "UT" extra field time info */

#define EB_L_BE_LEN 5           /* min size is an unsigned long and flag */
#define EB_C_BE_LEN 5           /* Length of data in local EF and flag.  */

#define EB_BE_FL_NATURAL    0x01    /* data is 'natural' (not compressed) */
#define EB_BE_FL_BADBITS    0xfe    /* bits currently undefined           */

#ifndef ZP_NEED_MEMCOMPR
#  define ZP_NEED_MEMCOMPR
#endif

/* Set a file's MIME type. */
#define BE_FILE_TYPE_NAME   "BEOS:TYPE"
void setfiletype( const char *file, const char *type );

/*
DR9 'Be' extra-field layout:

'Be'      - signature
ef_size   - size of data in this EF (little-endian unsigned short)
full_size - uncompressed data size (little-endian unsigned long)
flag      - flags (byte)
            flags & EB_BE_FL_NATURAL    = the data is not compressed
            flags & EB_BE_FL_BADBITS    = the data is corrupted or we
                                          can't handle it properly
data      - compressed or uncompressed file attribute data

If flag & EB_BE_FL_NATURAL, the data is not compressed; this optimisation is
necessary to prevent wasted space for files with small attributes (which
appears to be quite common on the Advanced Access DR9 release).  In this
case, there should be ( ef_size - EB_L_BE_LEN ) bytes of data, and full_size
should equal ( ef_size - EB_L_BE_LEN ).

If the data is compressed, there will be ( ef_size - EB_L_BE_LEN ) bytes of
compressed data, and full_size bytes of uncompressed data.

If a file has absolutely no attributes, there will not be a 'Be' extra field.

The uncompressed data is arranged like this:

attr_name\0 - C string
struct attr_info (big-endian)
attr_data (length in attr_info.size)
*/
