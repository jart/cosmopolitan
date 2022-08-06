#ifndef __crypt_h /* don't include more than once */
#define __crypt_h
/* clang-format off */

#define CRYPT  1  /* full version for zip and main unzip */

#if CRYPT
/* full version */

#ifdef CR_BETA
#  undef CR_BETA    /* this is not a beta release */
#endif

#define CR_MAJORVER        2
#define CR_MINORVER        91
#ifdef CR_BETA
#  define CR_BETA_VER      "c BETA"
#  define CR_VERSION_DATE  "05 Jan 2007"       /* last real code change */
#else
#  define CR_BETA_VER      ""
#  define CR_VERSION_DATE  "05 Jan 2007"       /* last public release date */
#  define CR_RELEASE
#endif

#ifndef __G         /* UnZip only, for now (DLL stuff) */
#  define __G
#  define __G__
#  define __GDEF
#  define __GPRO    void
#  define __GPRO__
#endif

#if defined(MSDOS) || defined(OS2) || defined(WIN32)
#  ifndef DOS_OS2_W32
#    define DOS_OS2_W32
#  endif
#endif

#if defined(DOS_OS2_W32) || defined(__human68k__)
#  ifndef DOS_H68_OS2_W32
#    define DOS_H68_OS2_W32
#  endif
#endif

#if defined(VM_CMS) || defined(MVS)
#  ifndef CMS_MVS
#    define CMS_MVS
#  endif
#endif

/* To allow combining of Zip and UnZip static libraries in a single binary,
 * the Zip and UnZip versions of the crypt core functions have to be named
 * differently.
 */
#ifdef ZIP
#  ifdef REALLY_SHORT_SYMS
#    define decrypt_byte   zdcrby
#  else
#    define decrypt_byte   zp_decrypt_byte
#  endif
#  define  update_keys     zp_update_keys
#  define  init_keys       zp_init_keys
#else /* !ZIP */
#  ifdef REALLY_SHORT_SYMS
#    define decrypt_byte   dcrbyt
#  endif
#endif /* ?ZIP */

#define IZ_PWLEN  80    /* input buffer size for reading encryption key */
#ifndef PWLEN           /* for compatibility with previous zcrypt release... */
#  define PWLEN IZ_PWLEN
#endif
#define RAND_HEAD_LEN  12       /* length of encryption random header */

/* the crc_32_tab array has to be provided externally for the crypt calculus */

/* encode byte c, using temp t.  Warning: c must not have side effects. */
#define zencode(c,t)  (t=decrypt_byte(__G), update_keys(c), t^(c))

/* decode byte c in place */
#define zdecode(c)   update_keys(__G__ c ^= decrypt_byte(__G))

int  decrypt_byte (__GPRO);
int  update_keys (__GPRO__ int c);
void init_keys (__GPRO__ ZCONST char *passwd);

#ifdef ZIP
   void crypthead (ZCONST char *, ulg);
#  ifdef UTIL
     int zipcloak (struct zlist far *, ZCONST char *);
     int zipbare (struct zlist far *, ZCONST char *);
#  else
     unsigned zfwrite (zvoid *, extent, extent);
#  endif
#endif /* ZIP */

#if (defined(UNZIP) && !defined(FUNZIP))
   int  decrypt (__GPRO__ ZCONST char *passwrd);
#endif

#ifdef FUNZIP
   extern int encrypted;
#  ifdef NEXTBYTE
#    undef NEXTBYTE
#  endif
#  define NEXTBYTE \
   (encrypted? update_keys(__G__ getc(G.in)^decrypt_byte(__G)) : getc(G.in))
#endif /* FUNZIP */

#else /* !CRYPT */
/* dummy version */

#define zencode
#define zdecode

#define zfwrite(b,s,c) bfwrite(b,s,c,BFWRITE_DATA)

#endif /* ?CRYPT */
#endif /* !__crypt_h */
