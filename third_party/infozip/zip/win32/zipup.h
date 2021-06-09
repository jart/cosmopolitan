/*
  win32/zipup.h - Zip 3

  Copyright (c) 1990-2007 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2007-Mar-4 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
#ifndef __CYGWIN__
#  include <share.h>
#endif
#ifndef O_RDONLY
#  define O_RDONLY   0
#endif
#ifndef O_BINARY
#  define O_BINARY   0
#endif
#if (defined(_SH_DENYNO) && !defined(SH_DENYNO))
#  define SH_DENYNO _SH_DENYNO
#endif
#if (defined(SH_DENYNO) && !defined(_SH_DENYNO))
#  define _SH_DENYNO SH_DENYNO
#endif
#define fhow         (O_RDONLY|O_BINARY)
#define fbad         (-1)
typedef int          ftype;

#if defined(__WATCOMC__) || defined(__BORLANDC__) || defined(__EMX__)
#  define zopen(n,p) sopen(n,p,SH_DENYNO)
#elif defined(__CYGWIN__) || defined(__IBMC__)
#  define zopen(n,p) open(n,p)
#else
#  define zopen(n,p) _sopen(n,p,_SH_DENYNO)
#endif
#ifdef UNICODE_SUPPORT
# if defined(__CYGWIN__) || defined(__IBMC__)
#  define zwopen(n,p) wopen(n,p)
# else
#  define zwopen(n,p) _wsopen(n,p,_SH_DENYNO)
# endif
#endif

#define zread(f,b,n) read(f,b,n)
#define zclose(f)    close(f)
#define zerr(f)      (k == (extent)(-1L))
#define zstdin       0
