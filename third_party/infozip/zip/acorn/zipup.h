/*
  Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 1999-Oct-05 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, both of these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html
*/
#define fhow "r"
#define fbad (NULL)
typedef FILE *ftype;
#define zopen(n,p) fopen(n,p)
#define zread(f,b,n) fread((b),1,(n),(FILE*)(f))
#define zclose(f) fclose(f)
#define zerr(f) (k==(extent)(-1L))
#define zstdin 0
