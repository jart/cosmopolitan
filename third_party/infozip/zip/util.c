/*
  util.c

  Copyright (c) 1990-2008 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2007-Mar-4 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
/*
 *  util.c by Mark Adler.
 */
#define __UTIL_C

#include "zip.h"
#include "ebcdic.h"
#include <ctype.h>

#ifdef MSDOS16
#  include <dos.h>
#endif

#ifdef NO_MKTIME
#  ifndef IZ_MKTIME_ONLY
#    define IZ_MKTIME_ONLY      /* only mktime() related code is pulled in */
#  endif
#  include "timezone.c"
#endif

uch upper[256], lower[256];
/* Country-dependent case map table */


#ifndef UTIL /* UTIL picks out namecmp code (all utils) */

/* RISC OS uses # as its single-character wildcard */
#ifdef RISCOS
#  define WILDCHR_SINGLE '#'
#  define WILDCHR_MULTI  '*'
#  define DIRSEP_CHR '.'
#endif

#ifdef VMS
#  define WILDCHR_SINGLE '%'
#  define WILDCHR_MULTI  '*'
#  define DIRSEP_CHR '.'
#endif

#ifndef WILDCHR_SINGLE
#  define WILDCHR_SINGLE '?'
#endif
#ifndef WILDCHR_MULTI
#  define WILDCHR_MULTI '*'
#endif
#ifndef DIRSEP_CHR
#  define DIRSEP_CHR '/'
#endif

/* Local functions */
local int recmatch OF((ZCONST char *, ZCONST char *, int));
#if defined(UNICODE_SUPPORT) && defined(WIN32)
  local long recmatchw OF((ZCONST wchar_t *, ZCONST wchar_t *, int));
#endif
local int count_args OF((char *s));

#ifdef MSDOS16
  local unsigned ident OF((unsigned chr));
#endif

#ifndef HAVE_FSEEKABLE

/* 2004-11-12 SMS.
   Changed to use z*o() functions, and ftell() test from >= 0 to != -1.
   This solves problems with negative 32-bit offsets, even on small-file
   products.
*/
int fseekable( fp)
FILE *fp;
{
    zoff_t x;

    return (fp == NULL ||
     ((zfseeko( fp, ((zoff_t) -1), SEEK_CUR) == 0) &&   /* Seek ok. */
     ((x = zftello( fp)) != ((zoff_t) -1)) &&           /* Tell ok. */
     (zfseeko( fp, ((zoff_t) 1), SEEK_CUR) == 0) &&     /* Seek ok. */
     (zftello( fp) == x+ 1)));                          /* Tells agree. */
}
#endif /* HAVE_FSEEKABLE */


char *isshexp(p)
char *p;                /* candidate sh expression */
/* If p is a sh expression, a pointer to the first special character is
   returned.  Otherwise, NULL is returned. */
{
  for (; *p; INCSTR(p))
    if (*p == '\\' && *(p+1))
      p++;
#ifdef VMS
    else if (*p == WILDCHR_SINGLE || *p == WILDCHR_MULTI)
#else /* !VMS */
    else if (*p == WILDCHR_SINGLE || *p == WILDCHR_MULTI || *p == '[')
#endif /* ?VMS */
      return p;
  return NULL;
}

#ifdef UNICODE_SUPPORT
# ifdef WIN32

wchar_t *isshexpw(pw)
  wchar_t *pw;          /* candidate sh expression */
/* If pw is a sh expression, a pointer to the first special character is
   returned.  Otherwise, NULL is returned. */
{
  for (; *pw; pw++)
    if (*pw == (wchar_t)'\\' && *(pw+1))
      pw++;
    else if (*pw == (wchar_t)WILDCHR_SINGLE || *pw == (wchar_t)WILDCHR_MULTI ||
             *pw == (wchar_t)'[')
      return pw;
  return NULL;
}

# endif
#endif


#ifdef UNICODE_SUPPORT
# ifdef WIN32

local long recmatchw(pw, sw, cs)
ZCONST wchar_t *pw;     /* sh pattern to match */
ZCONST wchar_t *sw;     /* string to match it to */
int cs;                 /* flag: force case-sensitive matching */
/* Recursively compare the sh pattern p with the string s and return 1 if
   they match, and 0 or 2 if they don't or if there is a syntax error in the
   pattern.  This routine recurses on itself no deeper than the number of
   characters in the pattern. */
{
  long c;               /* pattern char or start of range in [-] loop */
  /* Get first character, the pattern for new recmatch calls follows */

  c = (long)*(pw++);

  /* If that was the end of the pattern, match if string empty too */
  if (c == 0)
    return *sw == 0;

  /* '?' matches any character (but not an empty string) */
  if ((wchar_t)c == (wchar_t)WILDCHR_SINGLE) {
    if (wild_stop_at_dir)
      return (*sw && *sw != (wchar_t)DIRSEP_CHR) ? recmatchw(pw, sw + 1, cs) : 0;
    else
      return *sw ? recmatchw(pw, sw + 1, cs) : 0;
  }

  /* WILDCHR_MULTI ('*') matches any number of characters, including zero */
  if (!no_wild && (wchar_t)c == (wchar_t)WILDCHR_MULTI)
  {
    if (wild_stop_at_dir) {
      /* Check for an immediately following WILDCHR_MULTI */
      if (*pw != (wchar_t)WILDCHR_MULTI) {
        /* Single WILDCHR_MULTI ('*'): this doesn't match slashes */
        for (; *sw && *sw != (wchar_t)DIRSEP_CHR; sw++)
          if ((c = recmatchw(pw, sw, cs)) != 0)
            return c;
        /* end of pattern: matched if at end of string, else continue */
        if (*pw == 0)
          return (*sw == 0);
        /* continue to match if at DIRSEP_CHR in pattern, else give up */
        return (*pw == (wchar_t)DIRSEP_CHR || (*pw == (wchar_t)'\\' &&
                pw[1] == (wchar_t)DIRSEP_CHR))
               ? recmatchw(pw, sw, cs) : 2;
      }
      /* Two consecutive WILDCHR_MULTI ("**"): this matches DIRSEP_CHR ('/') */
      pw++;        /* move p past the second WILDCHR_MULTI */
      /* continue with the normal non-WILD_STOP_AT_DIR code */
    } /* wild_stop_at_dir */

    /* Not wild_stop_at_dir */
    if (*pw == 0)
      return 1;
    if (!isshexpw((wchar_t *)pw))
    {
      /* optimization for rest of pattern being a literal string */

      /* optimization to handle patterns like *.txt */
      /* if the first char in the pattern is '*' and there */
      /* are no other shell expression chars, i.e. a literal string */
      /* then just compare the literal string at the end */

      ZCONST wchar_t *swrest;

      swrest = sw + (wcslen(sw) - wcslen(pw));
      if (swrest - sw < 0)
        /* remaining literal string from pattern is longer than rest of
           test string, there can't be a match
         */
        return 0;
      else
        /* compare the remaining literal pattern string with the last bytes
           of the test string to check for a match */
        return ((cs ? wcscmp(pw, swrest) : _wcsicmp(pw, swrest)) == 0);
    }
    else
    {
      /* pattern contains more wildcards, continue with recursion... */
      for (; *sw; sw++)
        if ((c = recmatchw(pw, sw, cs)) != 0)
          return c;
      return 2;           /* 2 means give up--shmatch will return false */
    }
  }

  /* Parse and process the list of characters and ranges in brackets */
  if (!no_wild && allow_regex && (wchar_t)c == '[')
  {
    int e;              /* flag true if next char to be taken literally */
    ZCONST wchar_t *qw; /* pointer to end of [-] group */
    int r;              /* flag true to match anything but the range */

    if (*sw == 0)                        /* need a character to match */
      return 0;
    pw += (r = (*pw == (wchar_t)'!' || *pw == (wchar_t)'^')); /* see if reverse */
    for (qw = pw, e = 0; *qw; qw++)         /* find closing bracket */
      if (e)
        e = 0;
      else
        if (*qw == (wchar_t)'\\')
          e = 1;
        else if (*qw == (wchar_t)']')
          break;
    if (*qw != (wchar_t)']')                      /* nothing matches if bad syntax */
      return 0;
    for (c = 0, e = *pw == (wchar_t)'-'; pw < qw; pw++)      /* go through the list */
    {
      if (e == 0 && *pw == (wchar_t)'\\')         /* set escape flag if \ */
        e = 1;
      else if (e == 0 && *pw == (wchar_t)'-')     /* set start of range if - */
        c = *(pw-1);
      else
      {
        wchar_t cc = (cs ? *sw : towupper(*sw));
        wchar_t uc = (wchar_t) c;

        if (*(pw+1) != (wchar_t)'-')
          for (uc = uc ? uc : *pw; cc <= *pw; uc++)
            /* compare range */
            if ((cs ? uc : towupper(uc)) == cc)
              return r ? 0 : recmatchw(qw + 1, sw + 1, cs);
        c = e = 0;                      /* clear range, escape flags */
      }
    }
    return r ? recmatchw(qw + 1, sw + 1, cs) : 0;
                                        /* bracket match failed */
  }

  /* If escape ('\'), just compare next character */
  if (!no_wild && (wchar_t)c == (wchar_t)'\\')
    if ((c = *pw++) == '\0')            /* if \ at end, then syntax error */
      return 0;

  /* Just a character--compare it */
  return (cs ? (wchar_t)c == *sw : towupper((wchar_t)c) == towupper(*sw)) ?
          recmatchw(pw, sw + 1, cs) : 0;
}

# endif
#endif


local int recmatch(p, s, cs)
ZCONST char *p;         /* sh pattern to match */
ZCONST char *s;         /* string to match it to */
int cs;                 /* flag: force case-sensitive matching */
/* Recursively compare the sh pattern p with the string s and return 1 if
   they match, and 0 or 2 if they don't or if there is a syntax error in the
   pattern.  This routine recurses on itself no deeper than the number of
   characters in the pattern. */
{
  int c;                /* pattern char or start of range in [-] loop */
  /* Get first character, the pattern for new recmatch calls follows */

  /* This fix provided by akt@m5.dion.ne.jp for Japanese.
     See 21 July 2006 mail.
     It only applies when p is pointing to a doublebyte character and
     things like / and wildcards are not doublebyte.  This probably
     should not be needed. */

#ifdef _MBCS
  if (CLEN(p) == 2) {
    if (CLEN(s) == 2) {
      return (*p == *s && *(p+1) == *(s+1)) ?
        recmatch(p + 2, s + 2, cs) : 0;
    } else {
      return 0;
    }
  }
#endif /* ?_MBCS */

  c = *POSTINCSTR(p);

  /* If that was the end of the pattern, match if string empty too */
  if (c == 0)
    return *s == 0;

  /* '?' (or '%' or '#') matches any character (but not an empty string) */
  if (c == WILDCHR_SINGLE) {
    if (wild_stop_at_dir)
      return (*s && *s != DIRSEP_CHR) ? recmatch(p, s + CLEN(s), cs) : 0;
    else
      return *s ? recmatch(p, s + CLEN(s), cs) : 0;
  }

  /* WILDCHR_MULTI ('*') matches any number of characters, including zero */
#ifdef AMIGA
  if (!no_wild && c == '#' && *p == '?')            /* "#?" is Amiga-ese for "*" */
    c = WILDCHR_MULTI, p++;
#endif /* AMIGA */
  if (!no_wild && c == WILDCHR_MULTI)
  {
    if (wild_stop_at_dir) {
      /* Check for an immediately following WILDCHR_MULTI */
# ifdef AMIGA
      if ((c = p[0]) == '#' && p[1] == '?') /* "#?" is Amiga-ese for "*" */
        c = WILDCHR_MULTI, p++;
      if (c != WILDCHR_MULTI) {
# else /* !AMIGA */
      if (*p != WILDCHR_MULTI) {
# endif /* ?AMIGA */
        /* Single WILDCHR_MULTI ('*'): this doesn't match slashes */
        for (; *s && *s != DIRSEP_CHR; INCSTR(s))
          if ((c = recmatch(p, s, cs)) != 0)
            return c;
        /* end of pattern: matched if at end of string, else continue */
        if (*p == 0)
          return (*s == 0);
        /* continue to match if at DIRSEP_CHR in pattern, else give up */
        return (*p == DIRSEP_CHR || (*p == '\\' && p[1] == DIRSEP_CHR))
               ? recmatch(p, s, cs) : 2;
      }
      /* Two consecutive WILDCHR_MULTI ("**"): this matches DIRSEP_CHR ('/') */
      p++;        /* move p past the second WILDCHR_MULTI */
      /* continue with the normal non-WILD_STOP_AT_DIR code */
    } /* wild_stop_at_dir */

    /* Not wild_stop_at_dir */
    if (*p == 0)
      return 1;
    if (!isshexp((char *)p))
    {
      /* optimization for rest of pattern being a literal string */

      /* optimization to handle patterns like *.txt */
      /* if the first char in the pattern is '*' and there */
      /* are no other shell expression chars, i.e. a literal string */
      /* then just compare the literal string at the end */

      ZCONST char *srest;

      srest = s + (strlen(s) - strlen(p));
      if (srest - s < 0)
        /* remaining literal string from pattern is longer than rest of
           test string, there can't be a match
         */
        return 0;
      else
        /* compare the remaining literal pattern string with the last bytes
           of the test string to check for a match */
#ifdef _MBCS
      {
        ZCONST char *q = s;

        /* MBCS-aware code must not scan backwards into a string from
         * the end.
         * So, we have to move forward by character from our well-known
         * character position s in the test string until we have advanced
         * to the srest position.
         */
        while (q < srest)
          INCSTR(q);
        /* In case the byte *srest is a trailing byte of a multibyte
         * character, we have actually advanced past the position (srest).
         * For this case, the match has failed!
         */
        if (q != srest)
          return 0;
        return ((cs ? strcmp(p, q) : namecmp(p, q)) == 0);
      }
#else /* !_MBCS */
        return ((cs ? strcmp(p, srest) : namecmp(p, srest)) == 0);
#endif /* ?_MBCS */
    }
    else
    {
      /* pattern contains more wildcards, continue with recursion... */
      for (; *s; INCSTR(s))
        if ((c = recmatch(p, s, cs)) != 0)
          return c;
      return 2;           /* 2 means give up--shmatch will return false */
    }
  }

#ifndef VMS             /* No bracket matching in VMS */
  /* Parse and process the list of characters and ranges in brackets */
  if (!no_wild && allow_regex && c == '[')
  {
    int e;              /* flag true if next char to be taken literally */
    ZCONST char *q;     /* pointer to end of [-] group */
    int r;              /* flag true to match anything but the range */

    if (*s == 0)                        /* need a character to match */
      return 0;
    p += (r = (*p == '!' || *p == '^')); /* see if reverse */
    for (q = p, e = 0; *q; q++)         /* find closing bracket */
      if (e)
        e = 0;
      else
        if (*q == '\\')
          e = 1;
        else if (*q == ']')
          break;
    if (*q != ']')                      /* nothing matches if bad syntax */
      return 0;
    for (c = 0, e = *p == '-'; p < q; p++)      /* go through the list */
    {
      if (e == 0 && *p == '\\')         /* set escape flag if \ */
        e = 1;
      else if (e == 0 && *p == '-')     /* set start of range if - */
        c = *(p-1);
      else
      {
        uch cc = (cs ? (uch)*s : case_map((uch)*s));
        uch uc = (uch) c;
        if (*(p+1) != '-')
          for (uc = uc ? uc : (uch)*p; uc <= (uch)*p; uc++)
            /* compare range */
            if ((cs ? uc : case_map(uc)) == cc)
              return r ? 0 : recmatch(q + CLEN(q), s + CLEN(s), cs);
        c = e = 0;                      /* clear range, escape flags */
      }
    }
    return r ? recmatch(q + CLEN(q), s + CLEN(s), cs) : 0;
                                        /* bracket match failed */
  }
#endif /* !VMS */

  /* If escape ('\'), just compare next character */
  if (!no_wild && c == '\\')
    if ((c = *p++) == '\0')             /* if \ at end, then syntax error */
      return 0;

#ifdef VMS
  /* 2005-11-06 SMS.
     Handle "..." wildcard in p with "." or "]" in s.
  */
  if ((c == '.') && (*p == '.') && (*(p+ CLEN( p)) == '.') &&
   ((*s == '.') || (*s == ']')))
  {
    /* Match "...]" with "]".  Continue after "]" in both. */
    if ((*(p+ 2* CLEN( p)) == ']') && (*s == ']'))
      return recmatch( (p+ 3* CLEN( p)), (s+ CLEN( s)), cs);

    /* Else, look for a reduced match in s, until "]" in or end of s. */
    for (; *s && (*s != ']'); INCSTR(s))
      if (*s == '.')
        /* If reduced match, then continue after "..." in p, "." in s. */
        if ((c = recmatch( (p+ CLEN( p)), s, cs)) != 0)
          return (int)c;

    /* Match "...]" with "]".  Continue after "]" in both. */
    if ((*(p+ 2* CLEN( p)) == ']') && (*s == ']'))
      return recmatch( (p+ 3* CLEN( p)), (s+ CLEN( s)), cs);

    /* No reduced match.  Quit. */
    return 2;
  }

#endif /* def VMS */

  /* Just a character--compare it */
  return (cs ? c == *s : case_map((uch)c) == case_map((uch)*s)) ?
          recmatch(p, s + CLEN(s), cs) : 0;
}


int shmatch(p, s, cs)
ZCONST char *p;         /* sh pattern to match */
ZCONST char *s;         /* string to match it to */
int cs;                 /* force case-sensitive match if TRUE */
/* Compare the sh pattern p with the string s and return true if they match,
   false if they don't or if there is a syntax error in the pattern. */
{
  return recmatch(p, s, cs) == 1;
}


#if defined(DOS) || defined(WIN32)

#ifdef UNICODE_SUPPORT

int dosmatchw(pw, sw, cs)
ZCONST wchar_t *pw;     /* dos pattern to match    */
ZCONST wchar_t *sw;     /* string to match it to   */
int cs;                 /* force case-sensitive match if TRUE */
/* Treat filenames without periods as having an implicit trailing period */
{
  wchar_t *sw1;         /* revised string to match */
  int r;                /* result */

  if (wcschr(pw, (wchar_t)'.') && !wcschr(sw, (wchar_t)'.') &&
      ((sw1 = (wchar_t *)malloc((wcslen(sw) + 2) * sizeof(wchar_t))) != NULL))
  {
    wcscpy(sw1, sw);
    wcscat(sw1, L".");
  }
  else
  {
    /* will usually be OK */
    sw1 = (wchar_t *)sw;
  }

  r = recmatchw(pw, sw1, cs) == 1;
  if (sw != sw1)
    free((zvoid *)sw1);
  return r == 1;
}

#endif

/* XXX  also suitable for OS2?  Atari?  Human68K?  TOPS-20?? */

int dosmatch(p, s, cs)
ZCONST char *p;         /* dos pattern to match    */
ZCONST char *s;         /* string to match it to   */
int cs;                 /* force case-sensitive match if TRUE */
/* Treat filenames without periods as having an implicit trailing period */
{
  char *s1;             /* revised string to match */
  int r;                /* result */

  if (strchr(p, '.') && !strchr(s, '.') &&
      ((s1 = malloc(strlen(s) + 2)) != NULL))
  {
    strcpy(s1, s);
    strcat(s1, ".");
  }
  else
  {
    /* will usually be OK */
    s1 = (char *)s;
  }

  r = recmatch(p, s1, cs) == 1;
  if (s != s1)
    free((zvoid *)s1);
  return r == 1;
}

#endif /* DOS || WIN32 */

zvoid far **search(b, a, n, cmp)
ZCONST zvoid *b;        /* pointer to value to search for */
ZCONST zvoid far **a;   /* table of pointers to values, sorted */
extent n;               /* number of pointers in a[] */
int (*cmp) OF((ZCONST zvoid *, ZCONST zvoid far *)); /* comparison function */

/* Search for b in the pointer list a[0..n-1] using the compare function
   cmp(b, c) where c is an element of a[i] and cmp() returns negative if
   *b < *c, zero if *b == *c, or positive if *b > *c.  If *b is found,
   search returns a pointer to the entry in a[], else search() returns
   NULL.  The nature and size of *b and *c (they can be different) are
   left up to the cmp() function.  A binary search is used, and it is
   assumed that the list is sorted in ascending order. */
{
  ZCONST zvoid far **i; /* pointer to midpoint of current range */
  ZCONST zvoid far **l; /* pointer to lower end of current range */
  int r;                /* result of (*cmp)() call */
  ZCONST zvoid far **u; /* pointer to upper end of current range */

  l = (ZCONST zvoid far **)a;  u = l + (n-1);
  while (u >= l) {
    i = l + ((unsigned)(u - l) >> 1);
    if ((r = (*cmp)(b, (ZCONST char far *)*(struct zlist far **)i)) < 0)
      u = i - 1;
    else if (r > 0)
      l = i + 1;
    else
      return (zvoid far **)i;
  }
  return NULL;          /* If b were in list, it would belong at l */
}

#endif /* !UTIL */

#ifdef MSDOS16

local unsigned ident(unsigned chr)
{
   return chr; /* in al */
}

void init_upper()
{
  static struct country {
    uch ignore[18];
    int (far *casemap)(int);
    uch filler[16];
  } country_info;

  struct country far *info = &country_info;
  union REGS regs;
  struct SREGS sregs;
  unsigned int c;

  regs.x.ax = 0x3800; /* get country info */
  regs.x.dx = FP_OFF(info);
  sregs.ds  = FP_SEG(info);
  intdosx(&regs, &regs, &sregs);
  for (c = 0; c < 128; c++) {
    upper[c] = (uch) toupper(c);
    lower[c] = (uch) c;
  }
  for (; c < sizeof(upper); c++) {
    upper[c] = (uch) (*country_info.casemap)(ident(c));
    /* ident() required because casemap takes its parameter in al */
    lower[c] = (uch) c;
  }
  for (c = 0; c < sizeof(upper); c++ ) {
    unsigned int u = upper[c];
    if (u != c && lower[u] == (uch) u) {
      lower[u] = (uch)c;
    }
  }
  for (c = 'A'; c <= 'Z'; c++) {
    lower[c] = (uch) (c - 'A' + 'a');
  }
}
#else /* !MSDOS16 */
#  ifndef OS2

void init_upper()
{
  unsigned int c;
#if defined(ATARI) || defined(CMS_MVS)
#include <ctype.h>
/* this should be valid for all other platforms too.   (HD 11/11/95) */
  for (c = 0; c< sizeof(upper); c++) {
    upper[c] = islower(c) ? toupper(c) : c;
    lower[c] = isupper(c) ? tolower(c) : c;
  }
#else
  for (c = 0; c < sizeof(upper); c++) upper[c] = lower[c] = (uch)c;
  for (c = 'a'; c <= 'z';        c++) upper[c] = (uch)(c - 'a' + 'A');
  for (c = 'A'; c <= 'Z';        c++) lower[c] = (uch)(c - 'A' + 'a');
#endif
}
#  endif /* !OS2 */

#endif /* ?MSDOS16 */

int namecmp(string1, string2)
  ZCONST char *string1, *string2;
/* Compare the two strings ignoring case, and correctly taking into
 * account national language characters. For operating systems with
 * case sensitive file names, this function is equivalent to strcmp.
 */
{
  int d;

  for (;;)
  {
    d = (int) (uch) case_map(*string1)
      - (int) (uch) case_map(*string2);

    if (d || *string1 == 0 || *string2 == 0)
      return d;

    string1++;
    string2++;
  }
}

#ifdef EBCDIC
char *strtoasc(char *str1, ZCONST char *str2)
{
  char *old;
  old = str1;
  while (*str1++ = (char)ascii[(uch)(*str2++)]);
  return old;
}

char *strtoebc(char *str1, ZCONST char *str2)
{
  char *old;
  old = str1;
  while (*str1++ = (char)ebcdic[(uch)(*str2++)]);
  return old;
}

char *memtoasc(char *mem1, ZCONST char *mem2, unsigned len)
{
  char *old;
  old = mem1;
  while (len--)
     *mem1++ = (char)ascii[(uch)(*mem2++)];
  return old;
}

char *memtoebc(char *mem1, ZCONST char *mem2, unsigned len)
{
  char *old;
  old = mem1;
  while (len--)
     *mem1++ = (char)ebcdic[(uch)(*mem2++)];
  return old;
}
#endif /* EBCDIC */

#ifdef IZ_ISO2OEM_ARRAY
char *str_iso_to_oem(dst, src)
  ZCONST char *src;
  char *dst;
{
  char *dest_start = dst;
  while (*dst++ = (char)iso2oem[(uch)(*src++)]);
  return dest_start;
}
#endif

#ifdef IZ_OEM2ISO_ARRAY
char *str_oem_to_iso(dst, src)
  ZCONST char *src;
  char *dst;
{
  char *dest_start = dst;
  while (*dst++ = (char)oem2iso[(uch)(*src++)]);
  return dest_start;
}
#endif



/* DBCS support for Info-ZIP's zip  (mainly for japanese (-: )
 * by Yoshioka Tsuneo (QWF00133@nifty.ne.jp,tsuneo-y@is.aist-nara.ac.jp)
 * This code is public domain!   Date: 1998/12/20
 */
#ifdef _MBCS

char *___tmp_ptr;

int lastchar(ptr)
    ZCONST char *ptr;
{
    ZCONST char *oldptr = ptr;
    while(*ptr != '\0'){
        oldptr = ptr;
        INCSTR(ptr);
    }
    return (int)(unsigned)*oldptr;
}

unsigned char *zmbschr(str, c)
    ZCONST unsigned char *str;
    unsigned int c;
{
    while(*str != '\0'){
        if (*str == c) {return (unsigned char *)str;}
        INCSTR(str);
    }
    return NULL;
}

unsigned char *zmbsrchr(str, c)
    ZCONST unsigned char *str;
    unsigned int c;
{
    unsigned char *match = NULL;
    while(*str != '\0'){
        if (*str == c) {match = (unsigned char*)str;}
        INCSTR(str);
    }
    return match;
}
#endif /* _MBCS */



#ifndef UTIL

/*****************************************************************
 | envargs - add default options from environment to command line
 |----------------------------------------------------------------
 | Author: Bill Davidsen, original 10/13/91, revised 23 Oct 1991.
 | This program is in the public domain.
 |----------------------------------------------------------------
 | Minor program notes:
 |  1. Yes, the indirection is a tad complex
 |  2. Parenthesis were added where not needed in some cases
 |     to make the action of the code less obscure.
 ****************************************************************/

void envargs(Pargc, Pargv, envstr, envstr2)
    int *Pargc;
    char ***Pargv;
    char *envstr;
    char *envstr2;
{
    char *envptr;                     /* value returned by getenv */
    char *bufptr;                     /* copy of env info */
    int argc;                         /* internal arg count */
    register int ch;                  /* spare temp value */
    char **argv;                      /* internal arg vector */
    char **argvect;                   /* copy of vector address */

    /* see if anything in the environment */
    envptr = getenv(envstr);
    if (envptr != NULL)                                /* usual var */
        while (isspace((uch)*envptr))      /* we must discard leading spaces */
            envptr++;
    if (envptr == NULL || *envptr == '\0')
        if ((envptr = getenv(envstr2)) != NULL)                 /* alternate */
            while (isspace((uch)*envptr))
                envptr++;
    if (envptr == NULL || *envptr == '\0')
        return;

    /* count the args so we can allocate room for them */
    argc = count_args(envptr);
    bufptr = malloc(1 + strlen(envptr));
    if (bufptr == NULL)
        ziperr(ZE_MEM, "Can't get memory for arguments");
    strcpy(bufptr, envptr);

    /* allocate a vector large enough for all args */
    argv = (char **)malloc((argc + *Pargc + 1) * sizeof(char *));
    if (argv == NULL) {
        free(bufptr);
        ziperr(ZE_MEM, "Can't get memory for arguments");
    }
    argvect = argv;

    /* copy the program name first, that's always true */
    *(argv++) = *((*Pargv)++);

    /* copy the environment args first, may be changed */
    do {
#if defined(AMIGA) || defined(UNIX)
        if (*bufptr == '"') {
            char *argstart = ++bufptr;
            *(argv++) = argstart;
            for (ch = *bufptr; ch != '\0' && ch != '\"';
                 ch = *PREINCSTR(bufptr))
                if (ch == '\\' && bufptr[1] != '\0')
                    ++bufptr;               /* skip to char after backslash */
            if (ch != '\0')                       /* overwrite trailing '"' */
                *(bufptr++) = '\0';

            /* remove escape characters */
            while ((argstart = MBSCHR(argstart, '\\')) != NULL) {
                strcpy(argstart, argstart + 1);
                if (*argstart)
                    ++argstart;
            }
        } else {
            *(argv++) = bufptr;
            while ((ch = *bufptr) != '\0' && !isspace((uch)ch)) INCSTR(bufptr);
            if (ch != '\0') *(bufptr++) = '\0';
        }
#else
#  ifdef WIN32
        /* We do not support backslash-quoting of quotes in quoted */
        /* strings under Win32, because backslashes are directory  */
        /* separators and double quotes are illegal in filenames.  */
        if (*bufptr == '"') {
            *(argv++) = ++bufptr;
            while ((ch = *bufptr) != '\0' && ch != '\"') INCSTR(bufptr);
            if (ch != '\0') *(bufptr++) = '\0';
        } else {
            *(argv++) = bufptr;
            while ((ch = *bufptr) != '\0' && !isspace((uch)ch)) INCSTR(bufptr);
            if (ch != '\0') *(bufptr++) = '\0';
        }
#  else
        *(argv++) = bufptr;
        while ((ch = *bufptr) != '\0' && !isspace((uch)ch)) INCSTR(bufptr);
        if (ch != '\0') *(bufptr++) = '\0';
#  endif
#endif /* ?(AMIGA || UNIX) */
        while ((ch = *bufptr) != '\0' && isspace((uch)ch)) INCSTR(bufptr);
    } while (ch);

    /* now save old argc and copy in the old args */
    argc += *Pargc;
    while (--(*Pargc)) *(argv++) = *((*Pargv)++);

    /* finally, add a NULL after the last arg, like UNIX */
    *argv = NULL;

    /* save the values and return */
    *Pargv = argvect;
    *Pargc = argc;
}

local int count_args(s)
char *s;
{
    int count = 0;
    char ch;

    do {
        /* count and skip args */
        ++count;
#if defined(AMIGA) || defined(UNIX)
        if (*s == '\"') {
            for (ch = *PREINCSTR(s); ch != '\0' && ch != '\"';
                 ch = *PREINCSTR(s))
                if (ch == '\\' && s[1] != '\0')
                    INCSTR(s);
            if (*s) INCSTR(s);  /* trailing quote */
        } else
            while ((ch = *s) != '\0' && !isspace((uch)ch)) INCSTR(s);
#else
#  ifdef WIN32
        if (*s == '\"') {
            ++s;                /* leading quote */
            while ((ch = *s) != '\0' && ch != '\"') INCSTR(s);
            if (*s) INCSTR(s);  /* trailing quote */
        } else
            while ((ch = *s) != '\0' && !isspace((uch)ch)) INCSTR(s);
#  else
        while ((ch = *s) != '\0' && !isspace((uch)ch)) INCSTR(s);
#  endif
#endif /* ?(AMIGA || UNIX) */
        while ((ch = *s) != '\0' && isspace((uch)ch)) INCSTR(s);
    } while (ch);

    return(count);
}



/* Extended argument processing -- by Rich Wales
 * This function currently deals only with the MKS shell, but could be
 * extended later to understand other conventions.
 *
 * void expand_args(int *argcp, char ***argvp)
 *
 *    Substitutes the extended command line argument list produced by
 *    the MKS Korn Shell in place of the command line info from DOS.
 *
 *    The MKS shell gets around DOS's 128-byte limit on the length of
 *    a command line by passing the "real" command line in the envi-
 *    ronment.  The "real" arguments are flagged by prepending a tilde
 *    (~) to each one.
 *
 *    This "expand_args" routine creates a new argument list by scanning
 *    the environment from the beginning, looking for strings begin-
 *    ning with a tilde character.  The new list replaces the original
 *    "argv" (pointed to by "argvp"), and the number of arguments
 *    in the new list replaces the original "argc" (pointed to by
 *    "argcp").
 */
void expand_args(argcp, argvp)
      int *argcp;
      char ***argvp;
{
#ifdef DOS

/* Do NEVER include (re)definiton of `environ' variable with any version
   of MSC or BORLAND/Turbo C. These compilers supply an incompatible
   definition in <stdlib.h>.  */
#if defined(__GO32__) || defined(__EMX__)
      extern char **environ;          /* environment */
#endif /* __GO32__ || __EMX__ */
      char        **envp;             /* pointer into environment */
      char        **newargv;          /* new argument list */
      char        **argp;             /* pointer into new arg list */
      int           newargc;          /* new argument count */

      /* sanity check */
      if (environ == NULL
          || argcp == NULL
          || argvp == NULL || *argvp == NULL)
              return;
      /* find out how many environment arguments there are */
      for (envp = environ, newargc = 0;
           *envp != NULL && (*envp)[0] == '~';
           envp++, newargc++) ;
      if (newargc == 0)
              return;                 /* no environment arguments */
      /* set up new argument list */
      newargv = (char **) malloc(sizeof(char **) * (newargc+1));
      if (newargv == NULL)
              return;                 /* malloc failed */
      for (argp = newargv, envp = environ;
           *envp != NULL && (*envp)[0] == '~';
           *argp++ = &(*envp++)[1]) ;
      *argp = NULL;                   /* null-terminate the list */
      /* substitute new argument list in place of old one */
      *argcp = newargc;
      *argvp = newargv;
#else /* !DOS */
      if (argcp || argvp) return;
#endif /* ?DOS */
}


/* Fast routine for detection of plain text
 * (ASCII or an ASCII-compatible extension such as ISO-8859, UTF-8, etc.)
 * Author: Cosmin Truta.
 * See "proginfo/txtvsbin.txt" for more information.
 *
 * This function returns the same result as set_file_type() in "trees.c".
 * Unlike in set_file_type(), however, the speed depends on the buffer size,
 * so the optimal implementation is different.
 */
int is_text_buf(buf_ptr, buf_size)
    ZCONST char *buf_ptr;
    unsigned buf_size;
{
    int result = 0;
    unsigned i;
    unsigned char c;

    for (i = 0; i < buf_size; ++i)
    {
        c = (unsigned char)buf_ptr[i];
        if (c >= 32)    /* speed up the loop by checking this first */
            result = 1; /* white-listed character found; keep looping */
        else            /* speed up the loop by inlining the following check */
        if ((c <= 6) || (c >= 14 && c <= 25) || (c >= 28 && c <= 31))
            return 0;   /* black-listed character found; stop */
    }

    return result;
}

#endif /* UTIL */


#ifdef DEBUGNAMES
#undef free
int Free(x)
void *x;
{
    if (x == (void *) 0xdeadbeef)
        exit(-1);
    free(x);
    return 0;
}

int printnames()
{
     struct zlist far *z;

     for (z = zfiles; z != NULL; z = z->nxt)
           fprintf(mesg, "%s %s %s %p %p %p %08x %08x %08x\n",
                            z->name, z->zname, z->iname,
                            z->name, z->zname, z->iname,
                            *((int *) z->name), *((int *) z->zname),
                            *((int *) z->iname));
     return 0;
}

#endif /* DEBUGNAMES */


/* Below is used to format zoff_t values, which can be either long or long long
   depending on if LARGE FILES are supported.  Function provided by SMS.
   10/17/04 EG */

/* 2004-12-01 SMS.
 * Brought in fancy fzofft() from UnZip.
 */

/* This implementation assumes that no more than FZOFF_NUM values will be
   needed in any printf using it.  */

/* zip_fzofft(): Format a zoff_t value in a cylindrical buffer set.
   This version renamed from fzofft because of name conflict in unzip
   when combined in WiZ. */

/* 2004-12-19 SMS.
 * I still claim than the smart move would have been to disable one or
 * the other instance with #if for Wiz.  But fine.  We'll change the
 * name.
 */

/* This is likely not thread safe.  Needs to be done without static storage.
   12/29/04 EG */

/* zip_fzofft(): Format a zoff_t value in a cylindrical buffer set. */

#define FZOFFT_NUM 4            /* Number of chambers. */
#define FZOFFT_LEN 24           /* Number of characters/chamber. */


/* Format a zoff_t value in a cylindrical buffer set. */

char *zip_fzofft( val, pre, post)
  zoff_t val;
  char *pre;
  char *post;
{
    /* Storage cylinder. */
    static char fzofft_buf[ FZOFFT_NUM][ FZOFFT_LEN];
    static int fzofft_index = 0;

    /* Temporary format string storage. */
    static char fmt[ 16] = "%";

    /* Assemble the format string. */
    fmt[ 1] = '\0';             /* Start after initial "%". */
    if (pre == FZOFFT_HEX_WID)  /* Special hex width. */
    {
        strcat( fmt, FZOFFT_HEX_WID_VALUE);
    }
    else if (pre == FZOFFT_HEX_DOT_WID) /* Special hex ".width". */
    {
        strcat( fmt, ".");
        strcat( fmt, FZOFFT_HEX_WID_VALUE);
    }
    else if (pre != NULL)       /* Caller's prefix (width). */
    {
        strcat( fmt, pre);
    }

    strcat( fmt, FZOFFT_FMT);   /* Long or long-long or whatever. */

    if (post == NULL)
        strcat( fmt, "d");      /* Default radix = decimal. */
    else
        strcat( fmt, post);     /* Caller's radix. */

    /* Advance the cylinder. */
    fzofft_index = (fzofft_index+ 1)% FZOFFT_NUM;

    /* Write into the current chamber. */
    sprintf( fzofft_buf[ fzofft_index], fmt, val);

    /* Return a pointer to this chamber. */
    return fzofft_buf[ fzofft_index];
}


/* Format a uzoff_t value in a cylindrical buffer set. */
/* Added to support uzoff_t type.  12/29/04 */

char *zip_fuzofft( val, pre, post)
  uzoff_t val;
  char *pre;
  char *post;
{
    /* Storage cylinder. */
    static char fuzofft_buf[ FZOFFT_NUM][ FZOFFT_LEN];
    static int fuzofft_index = 0;

    /* Temporary format string storage. */
    static char fmt[ 16] = "%";

    /* Assemble the format string. */
    fmt[ 1] = '\0';             /* Start after initial "%". */
    if (pre == FZOFFT_HEX_WID)  /* Special hex width. */
    {
        strcat( fmt, FZOFFT_HEX_WID_VALUE);
    }
    else if (pre == FZOFFT_HEX_DOT_WID) /* Special hex ".width". */
    {
        strcat( fmt, ".");
        strcat( fmt, FZOFFT_HEX_WID_VALUE);
    }
    else if (pre != NULL)       /* Caller's prefix (width). */
    {
        strcat( fmt, pre);
    }

    strcat( fmt, FZOFFT_FMT);   /* Long or long-long or whatever. */

    if (post == NULL)
        strcat( fmt, "u");      /* Default radix = decimal. */
    else
        strcat( fmt, post);     /* Caller's radix. */

    /* Advance the cylinder. */
    fuzofft_index = (fuzofft_index+ 1)% FZOFFT_NUM;

    /* Write into the current chamber. */
    sprintf( fuzofft_buf[ fuzofft_index], fmt, val);

    /* Return a pointer to this chamber. */
    return fuzofft_buf[ fuzofft_index];
}


/* Display number to mesg stream
   5/15/05 EG */

int DisplayNumString(file, i)
  FILE *file;
  uzoff_t i;
{
  char tempstrg[100];
  int j;
  char *s = tempstrg;

  WriteNumString(i, tempstrg);
  /* skip spaces */
  for (j = 0; j < 3; j++) {
    if (*s != ' ') break;
    s++;
  }
  fprintf(file, "%s", s);

  return 0;
}

/* Read numbers with trailing size multiplier (like 10M) and return number.
   10/30/04 EG */

uzoff_t ReadNumString( numstring )
  char *numstring;
{
  zoff_t num = 0;
  char multchar = ' ';
  int i;
  uzoff_t mult = 1;

  /* check if valid number (currently no negatives) */
  if (numstring == NULL) {
    zipwarn("Unable to read empty number in ReadNumString", "");
    return (uzoff_t)-1;
  }
  if (numstring[0] < '0' || numstring[0] > '9') {
    zipwarn("Unable to read number (must start with digit): ", numstring);
    return (uzoff_t)-1;
  }
  if (strlen(numstring) > 8) {
    zipwarn("Number too long to read (8 characters max): ", numstring);
    return (uzoff_t)-1;
  }

  /* get the number part */
  num = atoi(numstring);

  /* find trailing multiplier */
  for (i = 0; numstring[i] && isdigit(numstring[i]); i++) ;

  /* return if no multiplier */
  if (numstring[i] == '\0') {
    return (uzoff_t)num;
  }

  /* nothing follows multiplier */
  if (numstring[i + 1]) {
    return (uzoff_t)-1;
  }

  /* get multiplier */
  multchar = toupper(numstring[i]);

  if (multchar == 'K') {
    mult <<= 10;
  } else if (multchar == 'M') {
    mult <<= 20;
  } else if (multchar == 'G') {
    mult <<= 30;
#ifdef LARGE_FILE_SUPPORT
  } else if (multchar == 'T') {
    mult <<= 40;
#endif
  } else {
    return (uzoff_t)-1;
  }

  return (uzoff_t)num * mult;
}


/* Write the number as a string with a multiplier (like 10M) to outstring.
   Always writes no more than 3 digits followed maybe by a multiplier and
   returns the characters written or -1 if error.
   10/30/04 EG */

int WriteNumString( num, outstring )
  uzoff_t num;
  char *outstring;
{
  int mult;
  int written = 0;
  int i;
  int j;
  char digits[4];
  int dig;

  *outstring = '\0';

  /* shift number 1 K until less than 10000 */
  for (mult = 0; num >= 10240; mult++) {
    num >>= 10;
  }

  /* write digits as "    0" */
  for (i = 1; i < 4; i++) {
    digits[i] = ' ';
  }
  digits[0] = '0';

  if (num >= 1000) {
    i = 3;
    num *= 10;
    num >>= 10;
    mult++;
    digits[0] = (char) (num % 10) + '0';
    digits[1] = '.';
    digits[2] = (char) (num / 10) + '0';
  } else {
    for (i = 0; num; i++) {
      dig = (int) (num % 10);
      num /= 10;
      digits[i] = dig + '0';
    }
  }
  if (i == 0) i = 1;
  for (j = i; j > 0; j--) {
    *outstring = digits[j - 1];
    outstring++;
    written++;
  }

  /* output multiplier */
  if (mult == 0) {
  } else if (mult == 1) {
    *outstring = 'K';
    outstring++;
    written++;
  } else if (mult == 2) {
    *outstring = 'M';
    outstring++;
    written++;
  } else if (mult == 3) {
    *outstring = 'G';
    outstring++;
    written++;
  } else if (mult == 4) {
    *outstring = 'T';
    outstring++;
    written++;
  } else {
    *outstring = '?';
    outstring++;
    written++;
  }

  *outstring = '\0';

  return written;
}


#if 0 /* not used anywhere, should get removed by next release... */

/* Apply the Adler-16 checksum to a set of bytes.
 * Use this function as you would use crc32():
 * - First call this function by passing a NULL pointer instead of buf
 *   OR initialize the checksum register with ADLERVAL_INITIAL.
 * - Iteratively call this function for each buffer fragment.
 * This function returns the updated checksum.
 *
 * IN assertion: chksum is a valid Adler-16 checksum:
 *    (chksum & 0xffU) < ADLER16_BASE && ((chksum >> 8) & 0xffU) < ADLER16_BASE
 *
 * Author: Cosmin Truta.
 * See "proginfo/adler16.txt" for more information.
 */

#define ADLER16_BASE 251        /* The largest prime smaller than 256 */

unsigned int adler16(chksum, buf, len)
    unsigned int chksum;
    ZCONST uch *buf;
    extent len;
{
    unsigned int sum1 = chksum & 0xff;
    unsigned int sum2 = (chksum >> 8) & 0xff;
    extent i;

    Assert((sum1 < ADLER16_BASE) && (sum2 < ADLER16_BASE),
           "adler16: invalid checksum");

    if (buf == NULL)
        return 1;

    for (i = 0; i < len; ++i)
    {
        sum1 += buf[i];
        if (sum1 >= ADLER16_BASE) /* this is faster than modulo ADLER16_BASE */
            sum1 -= ADLER16_BASE;
        sum2 += sum1;
        if (sum2 >= ADLER16_BASE) /* ditto */
            sum2 -= ADLER16_BASE;
    }

    return (sum2 << 8) | sum1;
}

#endif /* 0, not used anywhere */


/* returns true if abbrev is abbreviation for matchstring */
int abbrevmatch (matchstring, abbrev, case_sensitive, minmatch)
  char *matchstring;
  char *abbrev;
  int case_sensitive;
  int minmatch;
{
  int cnt = 0;
  char *m;
  char *a;

  m = matchstring;
  a = abbrev;

  for (; *m && *a; m++, a++) {
    cnt++;
    if (case_sensitive) {
      if (*m != *a) {
        /* mismatch */
        return 0;
      }
    } else {
      if (toupper(*m) != toupper(*a)) {
        /* mismatch */
        return 0;
      }
    }
  }
  if (cnt < minmatch) {
    /* not big enough string */
    return 0;
  }
  if (*a != '\0') {
    /* abbreviation longer than match string */
    return 0;
  }
  /* either abbreviation or match */
  return 1;
}
