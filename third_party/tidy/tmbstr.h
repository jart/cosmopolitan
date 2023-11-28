#ifndef __TMBSTR_H__
#define __TMBSTR_H__

/* tmbstr.h - Tidy string utility functions

  (c) 1998-2006 (W3C) MIT, ERCIM, Keio University
  See tidy.h for the copyright notice.

*/

#include "third_party/tidy/tidy.h"
#include "third_party/tidy/access.h"
#include "third_party/tidy/tidyplatform.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* like strdup but using an allocator */
tmbstr TY_(tmbstrdup)( TidyAllocator *allocator, ctmbstr str );

/* like strndup but using an allocator */
tmbstr TY_(tmbstrndup)( TidyAllocator *allocator, ctmbstr str, uint len);

/* exactly same as strncpy */
uint TY_(tmbstrncpy)( tmbstr s1, ctmbstr s2, uint size );

uint TY_(tmbstrcpy)( tmbstr s1, ctmbstr s2 );

uint TY_(tmbstrcat)( tmbstr s1, ctmbstr s2 );

/* exactly same as strcmp */
int TY_(tmbstrcmp)( ctmbstr s1, ctmbstr s2 );

/* returns byte count, not char count */
uint TY_(tmbstrlen)( ctmbstr str );

/*
  MS C 4.2 doesn't include strcasecmp.
  Note that tolower and toupper won't
  work on chars > 127.

  Neither do Lexer.ToLower() or Lexer.ToUpper()!

  We get away with this because, except for XML tags,
  we are always comparing to ascii element and
  attribute names defined by HTML specs.
*/
int TY_(tmbstrcasecmp)( ctmbstr s1, ctmbstr s2 );

int TY_(tmbstrncmp)( ctmbstr s1, ctmbstr s2, uint n );

int TY_(tmbstrncasecmp)( ctmbstr s1, ctmbstr s2, uint n );

/* return offset of cc from beginning of s1,
** -1 if not found.
*/
/* TY_PRIVATE int TY_(tmbstrnchr)( ctmbstr s1, uint len1, tmbchar cc ); */

ctmbstr TY_(tmbsubstrn)( ctmbstr s1, uint len1, ctmbstr s2 );
/* TY_PRIVATE ctmbstr TY_(tmbsubstrncase)( ctmbstr s1, uint len1, ctmbstr s2 ); */
ctmbstr TY_(tmbsubstr)( ctmbstr s1, ctmbstr s2 );

/* transform string to lower case */
tmbstr TY_(tmbstrtolower)( tmbstr s );

/* Transform ASCII chars in string to upper case */
tmbstr TY_(tmbstrtoupper)( tmbstr s );

/* TY_PRIVATE Bool TY_(tmbsamefile)( ctmbstr filename1, ctmbstr filename2 ); */

int TY_(tmbvsnprintf)(tmbstr buffer, size_t count, ctmbstr format, va_list args)
#ifdef __GNUC__
__attribute__((format(printf, 3, 0)))
#endif
;
int TY_(tmbsnprintf)(tmbstr buffer, size_t count, ctmbstr format, ...)
#ifdef __GNUC__
__attribute__((format(printf, 3, 4)))
#endif
;

void TY_(strrep)(tmbstr buffer, ctmbstr str, ctmbstr rep);


#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* __TMBSTR_H__ */
