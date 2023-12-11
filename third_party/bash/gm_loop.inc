/* Copyright (C) 1991-2017 Free Software Foundation, Inc.

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

#if EXTENDED_GLOB
int
EXTGLOB_PATTERN_P (pat)
     const CHAR *pat;
{
  switch (pat[0])
    {
    case L('*'):
    case L('+'):
    case L('!'):
    case L('@'):
    case L('?'):
      return (pat[1] == L('('));	/* ) */
    default:
      return 0;
    }
    
  return 0;
}
#endif

/* Return 1 of the first character of STRING could match the first
   character of pattern PAT.  Compiled to both single and wiide character
   versions.  FLAGS is a subset of strmatch flags; used to do case-insensitive
   matching for now. */
int
MATCH_PATTERN_CHAR (pat, string, flags)
     CHAR *pat, *string;
     int flags;
{
  CHAR c;

  if (*string == 0)
    return (*pat == L('*'));	/* XXX  - allow only * to match empty string */

  switch (c = *pat++)
    {
    default:
      return (FOLD(*string) == FOLD(c));
    case L('\\'):
      return (FOLD(*string) == FOLD(*pat));
    case L('?'):
      return (*pat == L('(') ? 1 : (*string != L'\0'));
    case L('*'):
      return (1);
    case L('+'):
    case L('!'):
    case L('@'):
      return (*pat ==  L('(') ? 1 : (FOLD(*string) == FOLD(c)));
    case L('['):
      return (*string != L('\0'));
    }
}

int
MATCHLEN (pat, max)
     CHAR *pat;
     size_t max;
{
  CHAR c;
  int matlen, bracklen, t, in_cclass, in_collsym, in_equiv;

  if (*pat == 0)
    return (0);

  matlen = in_cclass = in_collsym = in_equiv = 0;
  while (c = *pat++)
    {
      switch (c)
	{
	default:
	  matlen++;
	  break;
	case L('\\'):
	  if (*pat == 0)
	    return ++matlen;
	  else
	    {
	      matlen++;
	      pat++;
	    }
	  break;
	case L('?'):
	  if (*pat == LPAREN)
	    return (matlen = -1);		/* XXX for now */
	  else
	    matlen++;
	  break;
	case L('*'):
	  return (matlen = -1);
	case L('+'):
	case L('!'):
	case L('@'):
	  if (*pat == LPAREN)
	    return (matlen = -1);		/* XXX for now */
	  else
	    matlen++;
	  break;
	case L('['):
	  /* scan for ending `]', skipping over embedded [:...:] */
	  bracklen = 1;
	  c = *pat++;
	  do
	    {
	      if (c == 0)
		{
		  pat--;			/* back up to NUL */
	          matlen += bracklen;
	          goto bad_bracket;
	        }
	      else if (c == L('\\'))
		{
		  /* *pat == backslash-escaped character */
		  bracklen++;
		  /* If the backslash or backslash-escape ends the string,
		     bail.  The ++pat skips over the backslash escape */
		  if (*pat == 0 || *++pat == 0)
		    {
		      matlen += bracklen;
		      goto bad_bracket;
		    }
		}
	      else if (c == L('[') && *pat == L(':'))	/* character class */
		{
		  pat++;
		  bracklen++;
		  in_cclass = 1;
		}
	      else if (in_cclass && c == L(':') && *pat == L(']'))
		{
		  pat++;
		  bracklen++;
		  in_cclass = 0;
		}
	      else if (c == L('[') && *pat == L('.'))	/* collating symbol */
		{
		  pat++;
		  bracklen++;
		  if (*pat == L(']'))	/* right bracket can appear as collating symbol */
		    {
		      pat++;
		      bracklen++;
		    }
		  in_collsym = 1;
		}
	      else if (in_collsym && c == L('.') && *pat == L(']'))
		{
		  pat++;
		  bracklen++;
		  in_collsym = 0;
		}
	      else if (c == L('[') && *pat == L('='))	/* equivalence class */
		{
		  pat++;
		  bracklen++;
		  if (*pat == L(']'))	/* right bracket can appear as equivalence class */
		    {
		      pat++;
		      bracklen++;
		    }
		  in_equiv = 1;
		}
	      else if (in_equiv && c == L('=') && *pat == L(']'))
		{
		  pat++;
		  bracklen++;
		  in_equiv = 0;
		}
	      else
		bracklen++;
	    }
	  while ((c = *pat++) != L(']'));
	  matlen++;		/* bracket expression can only match one char */
bad_bracket:
	  break;
	}
    }

  return matlen;
}

#undef EXTGLOB_PATTERN_P
#undef MATCH_PATTERN_CHAR
#undef MATCHLEN
#undef FOLD
#undef L
#undef LPAREN
#undef RPAREN
#undef INT
#undef CHAR
