/*
  Copyright (c) 1990-2005 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2005-Feb-10 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/

#ifndef __zipup_h
#define __zipup_h 1

#ifndef NO_ZIPUP_H

#define fbad NULL
typedef void *ftype;
#define zopen(n,p)   (vms_native?vms_open(n)    :(ftype)fopen((n), p))
#define zread(f,b,n) (vms_native?vms_read(f,b,n):fread((b),1,(n),(FILE*)(f)))
#define zclose(f)    (vms_native?vms_close(f)   :fclose((FILE*)(f)))
#define zerr(f)      (vms_native?vms_error(f)   :ferror((FILE*)(f)))
#define zstdin stdin

ftype vms_open OF((char *));
unsigned int vms_read OF((ftype, char *, unsigned int));
int vms_close OF((ftype));
int vms_error OF((ftype));
#ifdef VMS_PK_EXTRA
int vms_get_attributes OF((ftype, struct zlist far *, iztimes *));
#endif

#endif /* !NO_ZIPUP_H */
#endif /* !__zipup_h */


#ifndef __zipup_cb_h
#define __zipup_cb_h 1

#ifdef __DECC

/* File open callback ID values.  (See also OSDEP.H.) */

#  define FHOW_ID 4

/* File open callback ID storage. */

extern int fhow_id;

#define fhow "r", "acc", acc_cb, &fhow_id

#else /* def __DECC */ /* (So, GNU C, VAX C, ...)*/

#define fhow "r", "mbc=60"

#endif /* def __DECC */

#endif /* ndef __zipup_cb_h */
