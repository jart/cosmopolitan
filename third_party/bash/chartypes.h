/* chartypes.h -- extend ctype.h */

/* Copyright (C) 2001-2021 Free Software Foundation, Inc.

   This file is part of GNU Bash, the Bourne Again SHell.

   Bash is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Bash is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Bash.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _SH_CHARTYPES_H
#define _SH_CHARTYPES_H

#include <ctype.h>

#ifndef UCHAR_MAX
#  define UCHAR_MAX 255
#endif
#ifndef CHAR_MAX
#  define CHAR_MAX 127
#endif

/* use this as a proxy for C89 */
#if defined (HAVE_STDLIB_H) && defined (HAVE_STRING_H)
#  define IN_CTYPE_DOMAIN(c) 1
#else
#  define IN_CTYPE_DOMAIN(c) ((c) >= 0 && (c) <= CHAR_MAX)
#endif

#if !defined (isspace) && !defined (HAVE_ISSPACE)
#  define isspace(c) ((c) == ' ' || (c) == '\t' || (c) == '\n' || (c) == '\f')
#endif

#if !defined (isprint) && !defined (HAVE_ISPRINT)
#  define isprint(c) (isalpha((unsigned char)c) || isdigit((unsigned char)c) || ispunct((unsigned char)c))
#endif

#if defined (isblank) || defined (HAVE_ISBLANK)
#  define ISBLANK(c) (IN_CTYPE_DOMAIN (c) && isblank ((unsigned char)c))
#else
#  define ISBLANK(c) ((c) == ' ' || (c) == '\t')
#endif

#if defined (isgraph) || defined (HAVE_ISGRAPH)
#  define ISGRAPH(c) (IN_CTYPE_DOMAIN (c) && isgraph (c))
#else
#  define ISGRAPH(c) (IN_CTYPE_DOMAIN (c) && isprint ((unsigned char)c) && !isspace ((unsigned char)c))
#endif

#if !defined (isxdigit) && !defined (HAVE_ISXDIGIT)
#  define isxdigit(c)	(((c) >= '0' && (c) <= '9') || ((c) >= 'a' && (c) <= 'f') || ((c) >= 'A' && (c) <= 'F'))
#endif

#undef ISPRINT

#define ISPRINT(c) (IN_CTYPE_DOMAIN (c) && isprint ((unsigned char)c))
#define ISDIGIT(c) (IN_CTYPE_DOMAIN (c) && isdigit ((unsigned char)c))
#define ISALNUM(c) (IN_CTYPE_DOMAIN (c) && isalnum ((unsigned char)c))
#define ISALPHA(c) (IN_CTYPE_DOMAIN (c) && isalpha ((unsigned char)c))
#define ISCNTRL(c) (IN_CTYPE_DOMAIN (c) && iscntrl ((unsigned char)c))
#define ISLOWER(c) (IN_CTYPE_DOMAIN (c) && islower ((unsigned char)c))
#define ISPUNCT(c) (IN_CTYPE_DOMAIN (c) && ispunct ((unsigned char)c))
#define ISSPACE(c) (IN_CTYPE_DOMAIN (c) && isspace ((unsigned char)c))
#define ISUPPER(c) (IN_CTYPE_DOMAIN (c) && isupper ((unsigned char)c))
#define ISXDIGIT(c) (IN_CTYPE_DOMAIN (c) && isxdigit ((unsigned char)c))

#define ISLETTER(c)	(ISALPHA(c))

#define DIGIT(c)	((c) >= '0' && (c) <= '9')

#define ISWORD(c)	(ISLETTER(c) || DIGIT(c) || ((c) == '_'))

#define HEXVALUE(c) \
  (((c) >= 'a' && (c) <= 'f') \
	? (c)-'a'+10 \
	: (c) >= 'A' && (c) <= 'F' ? (c)-'A'+10 : (c)-'0')
                  
#ifndef ISOCTAL
#  define ISOCTAL(c)	((c) >= '0' && (c) <= '7')
#endif
#define OCTVALUE(c)	((c) - '0')

#define TODIGIT(c)	((c) - '0')
#define TOCHAR(c)	((c) + '0')

#define TOLOWER(c)	(ISUPPER(c) ? tolower(c) : (c))
#define TOUPPER(c)	(ISLOWER(c) ? toupper(c) : (c))

#ifndef TOCTRL
   /* letter to control char -- ASCII.  The TOUPPER is in there so \ce and
      \cE will map to the same character in $'...' expansions. */
#  define TOCTRL(x)	((x) == '?' ? 0x7f : (TOUPPER(x) & 0x1f))
#endif
#ifndef UNCTRL
   /* control char to letter -- ASCII */
#  define UNCTRL(x)	(TOUPPER(x ^ 0x40))
#endif

#endif /* _SH_CHARTYPES_H */
