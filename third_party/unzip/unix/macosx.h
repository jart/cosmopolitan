/*
  macosx.h - UnZip 6

  Copyright (c) 2008 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2007-Mar-4 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/

#ifndef __MACOSX_H
#  define __MACOSX_H

#  if defined( UNIX) && defined( __APPLE__)

#    include <sys/attr.h>
#    include <sys/vnode.h>

#    define APL_DBL_PFX "._"
#    define APL_DBL_PFX_SQR "__MACOSX/"
#    define APL_DBL_SFX "/rsrc"

#    define APL_DBL_HDR_SIZE           82
#    define APL_DBL_HDR_RSRC_FORK_SIZE 46
#    define APL_DBL_HDR_FNDR_INFO_OFFS 50
#    define APL_DBL_OFS_MAGIC           0
#    define APL_DBL_OFS_VERSION         4
#    define APL_DBL_OFS_FILLER          8
#    define APL_DBL_OFS_ENT_CNT        24
#    define APL_DBL_OFS_ENT_DSCR       28

#    define APL_FNDR_INFO_SIZE         32

/* Finder info attribute buffer structure for setattrlist(). */
typedef struct {
  char          fndr_info[ APL_FNDR_INFO_SIZE];
} attr_bufr_fndr_t;

/* Resource fork attribute buffer structure for getattrlist(). */
typedef struct {
  unsigned int  ret_length;
  off_t         size;
} attr_bufr_rsrc_t;

#  endif /* defined( unix) && defined( __APPLE__) */

#endif /* ndef __MACOSX_H */

