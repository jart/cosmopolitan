/*
  Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 1999-Oct-05 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, both of these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html
*/

#ifndef _OSDEP_H_
#define _OSDEP_H_

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/unistd.h>

#define USE_EF_UT_TIME          /* Enable use of "UT" extra field time info */

#define EB_L_AT_LEN 5           /* min size is an unsigned long and flag */
#define EB_C_AT_LEN 5           /* Length of data in local EF and flag.  */

#define EB_AT_FL_NATURAL    0x01    /* data is 'natural' (not compressed) */
#define EB_AT_FL_BADBITS    0xfe    /* bits currently undefined           */

#ifndef ZP_NEED_MEMCOMPR
  define ZP_NEED_MEMCOMPR
#endif

#define deletedir(d) rmdir(d);

/* Set a file's MIME type. */
void setfiletype( const char *file, const char *type );

/*
'At' extra-field layout:

'At'      - signature
ef_size   - size of data in this EF (little-endian unsigned short)
full_size - uncompressed data size (little-endian unsigned long)
flag      - flags (byte)
            flags & EB_AT_FL_NATURAL    = the data is not compressed
            flags & EB_AT_FL_BADBITS    = the data is corrupted or we
                                          can't handle it properly
data      - compressed or uncompressed file attribute data

If flag & EB_AT_FL_NATURAL, the data is not compressed; this optimisation is
necessary to prevent wasted space for files with small attributes. In this
case, there should be ( ef_size - EB_L_AT_LEN ) bytes of data, and full_size
should equal ( ef_size - EB_L_AT_LEN ).

If the data is compressed, there will be ( ef_size - EB_L_AT_LEN ) bytes of
compressed data, and full_size bytes of uncompressed data.

If a file has absolutely no attributes, there will not be a 'At' extra field.

The uncompressed data is arranged like this:

attr_name\0 - C string
struct attr_info (little-endian)
attr_data (length in attr_info.ai_size)
*/

#endif

