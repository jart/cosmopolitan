/* `dir', `vdir' and `ls' directory listing programs for GNU.

   Modified by Chet Ramey for Readline.

   Copyright (C) 1985, 1988, 1990-1991, 1995-2010, 2012, 2017
   Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* Written by Richard Stallman and David MacKenzie.  */

/* Color support by Peter Anvin <Peter.Anvin@linux.org> and Dennis
   Flaherty <dennisf@denix.elk.miles.com> based on original patches by
   Greg Lee <lee@uhunix.uhcc.hawaii.edu>.  */

#define READLINE_LIBRARY

#if defined (HAVE_CONFIG_H)
#  include "config.h"
#endif

#include <stdio.h>

// strdup() / strcpy()
#if defined (HAVE_STRING_H)
#  include <string.h>
#else /* !HAVE_STRING_H */
#  include <strings.h>
#endif /* !HAVE_STRING_H */

// abort()
#if defined (HAVE_STDLIB_H)
#  include <stdlib.h>
#else
#  include "ansi_stdlib.h"
#endif /* HAVE_STDLIB_H */

#include "rldefs.h"	// STREQ, savestring
#include "readline.h"
#include "rlprivate.h"
#include "rlshell.h"
#include "xmalloc.h"

#include "colors.h"
#include "parse-colors.h"

#if defined (COLOR_SUPPORT)

static bool get_funky_string (char **dest, const char **src, bool equals_end, size_t *output_count);

struct bin_str _rl_color_indicator[] =
  {
    { LEN_STR_PAIR ("\033[") },         //  lc: Left of color sequence
    { LEN_STR_PAIR ("m") },             //  rc: Right of color sequence
    { 0, NULL },                        //  ec: End color (replaces lc+no+rc)
    { LEN_STR_PAIR ("0") },             //  rs: Reset to ordinary colors
    { 0, NULL },                        //  no: Normal
    { 0, NULL },                        //  fi: File: default
    { LEN_STR_PAIR ("01;34") },         //  di: Directory: bright blue
    { LEN_STR_PAIR ("01;36") },         //  ln: Symlink: bright cyan
    { LEN_STR_PAIR ("33") },            //  pi: Pipe: yellow/brown
    { LEN_STR_PAIR ("01;35") },         //  so: Socket: bright magenta
    { LEN_STR_PAIR ("01;33") },         //  bd: Block device: bright yellow
    { LEN_STR_PAIR ("01;33") },         //  cd: Char device: bright yellow
    { 0, NULL },                        //  mi: Missing file: undefined
    { 0, NULL },                        //  or: Orphaned symlink: undefined
    { LEN_STR_PAIR ("01;32") },         //  ex: Executable: bright green
    { LEN_STR_PAIR ("01;35") },         //  do: Door: bright magenta
    { LEN_STR_PAIR ("37;41") },         //  su: setuid: white on red
    { LEN_STR_PAIR ("30;43") },         //  sg: setgid: black on yellow
    { LEN_STR_PAIR ("37;44") },         //  st: sticky: black on blue
    { LEN_STR_PAIR ("34;42") },         //  ow: other-writable: blue on green
    { LEN_STR_PAIR ("30;42") },         //  tw: ow w/ sticky: black on green
    { LEN_STR_PAIR ("30;41") },         //  ca: black on red
    { 0, NULL },                        //  mh: disabled by default
    { LEN_STR_PAIR ("\033[K") },        //  cl: clear to end of line
  };

/* Parse a string as part of the LS_COLORS variable; this may involve
   decoding all kinds of escape characters.  If equals_end is set an
   unescaped equal sign ends the string, otherwise only a : or \0
   does.  Set *OUTPUT_COUNT to the number of bytes output.  Return
   true if successful.

   The resulting string is *not* null-terminated, but may contain
   embedded nulls.

   Note that both dest and src are char **; on return they point to
   the first free byte after the array and the character that ended
   the input string, respectively.  */

static bool
get_funky_string (char **dest, const char **src, bool equals_end, size_t *output_count) {
  char num;			/* For numerical codes */
  size_t count;			/* Something to count with */
  enum {
    ST_GND, ST_BACKSLASH, ST_OCTAL, ST_HEX, ST_CARET, ST_END, ST_ERROR
  } state;
  const char *p;
  char *q;

  p = *src;			/* We don't want to double-indirect */
  q = *dest;			/* the whole darn time.  */

  count = 0;			/* No characters counted in yet.  */
  num = 0;

  state = ST_GND;		/* Start in ground state.  */
  while (state < ST_END)
    {
      switch (state)
        {
        case ST_GND:		/* Ground state (no escapes) */
          switch (*p)
            {
            case ':':
            case '\0':
              state = ST_END;	/* End of string */
              break;
            case '\\':
              state = ST_BACKSLASH; /* Backslash scape sequence */
              ++p;
              break;
            case '^':
              state = ST_CARET; /* Caret escape */
              ++p;
              break;
            case '=':
              if (equals_end)
                {
                  state = ST_END; /* End */
                  break;
                }
              /* else fall through */
            default:
              *(q++) = *(p++);
              ++count;
              break;
            }
          break;

        case ST_BACKSLASH:	/* Backslash escaped character */
          switch (*p)
            {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
              state = ST_OCTAL;	/* Octal sequence */
              num = *p - '0';
              break;
            case 'x':
            case 'X':
              state = ST_HEX;	/* Hex sequence */
              num = 0;
              break;
            case 'a':		/* Bell */
              num = '\a';
              break;
            case 'b':		/* Backspace */
              num = '\b';
              break;
            case 'e':		/* Escape */
              num = 27;
              break;
            case 'f':		/* Form feed */
              num = '\f';
              break;
            case 'n':		/* Newline */
              num = '\n';
              break;
            case 'r':		/* Carriage return */
              num = '\r';
              break;
            case 't':		/* Tab */
              num = '\t';
              break;
            case 'v':		/* Vtab */
              num = '\v';
              break;
            case '?':		/* Delete */
              num = 127;
              break;
            case '_':		/* Space */
              num = ' ';
              break;
            case '\0':		/* End of string */
              state = ST_ERROR;	/* Error! */
              break;
            default:		/* Escaped character like \ ^ : = */
              num = *p;
              break;
            }
          if (state == ST_BACKSLASH)
            {
              *(q++) = num;
              ++count;
              state = ST_GND;
            }
          ++p;
          break;

        case ST_OCTAL:		/* Octal sequence */
          if (*p < '0' || *p > '7')
            {
              *(q++) = num;
              ++count;
              state = ST_GND;
            }
          else
            num = (num << 3) + (*(p++) - '0');
          break;

        case ST_HEX:		/* Hex sequence */
          switch (*p)
            {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
              num = (num << 4) + (*(p++) - '0');
              break;
            case 'a':
            case 'b':
            case 'c':
            case 'd':
            case 'e':
            case 'f':
              num = (num << 4) + (*(p++) - 'a') + 10;
              break;
            case 'A':
            case 'B':
            case 'C':
            case 'D':
            case 'E':
            case 'F':
              num = (num << 4) + (*(p++) - 'A') + 10;
              break;
            default:
              *(q++) = num;
              ++count;
              state = ST_GND;
              break;
            }
          break;

        case ST_CARET:		/* Caret escape */
          state = ST_GND;	/* Should be the next state... */
          if (*p >= '@' && *p <= '~')
            {
              *(q++) = *(p++) & 037;
              ++count;
            }
          else if (*p == '?')
            {
              *(q++) = 127;
              ++count;
            }
          else
            state = ST_ERROR;
          break;

        default:
	  /* should we ? */
          /* abort ();	no, we should not */
          state = ST_ERROR;
          break;
        }
    }

  *dest = q;
  *src = p;
  *output_count = count;

  return state != ST_ERROR;
}
#endif /* COLOR_SUPPORT */

void _rl_parse_colors(void)
{
#if defined (COLOR_SUPPORT)
  const char *p;		/* Pointer to character being parsed */
  char *buf;			/* color_buf buffer pointer */
  int state;			/* State of parser */
  int ind_no;			/* Indicator number */
  char label[3];		/* Indicator label */
  COLOR_EXT_TYPE *ext;		/* Extension we are working on */

  p = sh_get_env_value ("LS_COLORS");
  if (p == 0 || *p == '\0')
    {
      _rl_color_ext_list = NULL;
      return;
    }

  ext = NULL;
  strcpy (label, "??");

  /* This is an overly conservative estimate, but any possible
     LS_COLORS string will *not* generate a color_buf longer than
     itself, so it is a safe way of allocating a buffer in
     advance.  */
  buf = color_buf = savestring (p);

  state = 1;
  while (state > 0)
    {
      switch (state)
        {
        case 1:		/* First label character */
          switch (*p)
            {
            case ':':
              ++p;
              break;

            case '*':
              /* Allocate new extension block and add to head of
                 linked list (this way a later definition will
                 override an earlier one, which can be useful for
                 having terminal-specific defs override global).  */

              ext = (COLOR_EXT_TYPE *)xmalloc (sizeof *ext);
              ext->next = _rl_color_ext_list;
              _rl_color_ext_list = ext;

              ++p;
              ext->ext.string = buf;

              state = (get_funky_string (&buf, &p, true, &ext->ext.len)
                       ? 4 : -1);
              break;

            case '\0':
              state = 0;	/* Done! */
              break;

            default:	/* Assume it is file type label */
              label[0] = *(p++);
              state = 2;
              break;
            }
          break;

        case 2:		/* Second label character */
          if (*p)
            {
              label[1] = *(p++);
              state = 3;
            }
          else
            state = -1;	/* Error */
          break;

        case 3:		/* Equal sign after indicator label */
          state = -1;	/* Assume failure...  */
          if (*(p++) == '=')/* It *should* be...  */
            {
              for (ind_no = 0; indicator_name[ind_no] != NULL; ++ind_no)
                {
                  if (STREQ (label, indicator_name[ind_no]))
                    {
                      _rl_color_indicator[ind_no].string = buf;
                      state = (get_funky_string (&buf, &p, false,
                                                 &_rl_color_indicator[ind_no].len)
                               ? 1 : -1);
                      break;
                    }
                }
              if (state == -1)
		{
                  _rl_errmsg ("LS_COLORS: unrecognized prefix: %s", label);
                  /* recover from an unrecognized prefix */
                  while (p && *p && *p != ':')
		    p++;
		  if (p && *p == ':')
		    state = 1;
		  else if (p && *p == 0)
		    state = 0;
		}
            }
          break;

        case 4:		/* Equal sign after *.ext */
          if (*(p++) == '=')
            {
              ext->seq.string = buf;
              state = (get_funky_string (&buf, &p, false, &ext->seq.len)
                       ? 1 : -1);
            }
          else
            state = -1;
          /* XXX - recover here as with an unrecognized prefix? */
          if (state == -1 && ext->ext.string)
	    _rl_errmsg ("LS_COLORS: syntax error: %s", ext->ext.string);
          break;
        }
    }

  if (state < 0)
    {
      COLOR_EXT_TYPE *e;
      COLOR_EXT_TYPE *e2;

      _rl_errmsg ("unparsable value for LS_COLORS environment variable");
      free (color_buf);
      for (e = _rl_color_ext_list; e != NULL; /* empty */)
        {
          e2 = e;
          e = e->next;
          free (e2);
        }
      _rl_color_ext_list = NULL;
      _rl_colored_stats = 0;	/* can't have colored stats without colors */
    }
#else /* !COLOR_SUPPORT */
  ;
#endif /* !COLOR_SUPPORT */
}
