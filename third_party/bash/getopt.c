/* getopt.c - getopt for Bash.  Used by the getopt builtin. */

/* Copyright (C) 1993-2009 Free Software Foundation, Inc.

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

#include "config.h"

#if defined (HAVE_UNISTD_H)
#  ifdef _MINIX
#    include <sys/types.h>
#  endif
#  include <unistd.h>
#endif

#include <stdio.h>
#include "memalloc.h"
#include "bashintl.h"
#include "shell.h"
#include "getopt.h"

/* For communication from `sh_getopt' to the caller.
   When `sh_getopt' finds an option that takes an argument,
   the argument value is returned here. */
char *sh_optarg = 0;

/* Index in ARGV of the next element to be scanned.
   This is used for communication to and from the caller
   and for communication between successive calls to `sh_getopt'.

   On entry to `sh_getopt', zero means this is the first call; initialize.

   When `sh_getopt' returns EOF, this is the index of the first of the
   non-option elements that the caller should itself scan.

   Otherwise, `sh_optind' communicates from one call to the next
   how much of ARGV has been scanned so far.  */

/* XXX 1003.2 says this must be 1 before any call.  */
int sh_optind = 0;

/* Index of the current argument. */
static int sh_curopt;

/* The next char to be scanned in the option-element
   in which the last option character we returned was found.
   This allows us to pick up the scan where we left off.

   If this is zero, or a null string, it means resume the scan
   by advancing to the next ARGV-element.  */

static char *nextchar;
static int sh_charindex;

/* Callers store zero here to inhibit the error message
   for unrecognized options.  */

int sh_opterr = 1;

/* Set to an option character which was unrecognized.
   This must be initialized on some systems to avoid linking in the
   system's own getopt implementation.  */

int sh_optopt = '?';

/* Set to 1 when we see an invalid option; public so getopts can reset it. */
int sh_badopt = 0;

/* Scan elements of ARGV (whose length is ARGC) for option characters
   given in OPTSTRING.

   If an element of ARGV starts with '-', and is not exactly "-" or "--",
   then it is an option element.  The characters of this element
   (aside from the initial '-') are option characters.  If `sh_getopt'
   is called repeatedly, it returns successively each of the option characters
   from each of the option elements.

   If `sh_getopt' finds another option character, it returns that character,
   updating `sh_optind' and `nextchar' so that the next call to `sh_getopt' can
   resume the scan with the following option character or ARGV-element.

   If there are no more option characters, `sh_getopt' returns `EOF'.
   Then `sh_optind' is the index in ARGV of the first ARGV-element
   that is not an option.

   OPTSTRING is a string containing the legitimate option characters.
   If an option character is seen that is not listed in OPTSTRING,
   return '?' after printing an error message.  If you set `sh_opterr' to
   zero, the error message is suppressed but we still return '?'.

   If a char in OPTSTRING is followed by a colon, that means it wants an arg,
   so the following text in the same ARGV-element, or the text of the following
   ARGV-element, is returned in `sh_optarg'. */

/* 1003.2 specifies the format of this message.  */
#define BADOPT(x)  fprintf (stderr, _("%s: illegal option -- %c\n"), argv[0], x)
#define NEEDARG(x) fprintf (stderr, _("%s: option requires an argument -- %c\n"), argv[0], x)

int
sh_getopt (argc, argv, optstring)
     int argc;
     char *const *argv;
     const char *optstring;
{
  char c, *temp;

  sh_optarg = 0;

  if (sh_optind >= argc || sh_optind < 0)	/* XXX was sh_optind > argc */
    {
      sh_optind = argc;
      return (EOF);
    }

  /* Initialize the internal data when the first call is made.
     Start processing options with ARGV-element 1 (since ARGV-element 0
     is the program name); the sequence of previously skipped
     non-option ARGV-elements is empty.  */

  if (sh_optind == 0)
    {
      sh_optind = 1;
      nextchar = (char *)NULL;
    }

  if (nextchar == 0 || *nextchar == '\0')
    {
      /* If we have done all the ARGV-elements, stop the scan. */
      if (sh_optind >= argc)
	return EOF;

      temp = argv[sh_optind];

      /* Special ARGV-element `--' means premature end of options.
	 Skip it like a null option, and return EOF. */
      if (temp[0] == '-' && temp[1] == '-' && temp[2] == '\0')
	{
	  sh_optind++;
	  return EOF;
	}

      /* If we have come to a non-option, either stop the scan or describe
	 it to the caller and pass it by.  This makes the pseudo-option
	 `-' mean the end of options, but does not skip over it. */
      if (temp[0] != '-' || temp[1] == '\0')
	return EOF;

      /* We have found another option-ARGV-element.
	 Start decoding its characters.  */
      nextchar = argv[sh_curopt = sh_optind] + 1;
      sh_charindex = 1;
    }

  /* Look at and handle the next option-character.  */

  c = *nextchar++; sh_charindex++;
  temp = strchr (optstring, c);

  sh_optopt = c;

  /* Increment `sh_optind' when we start to process its last character.  */
  if (nextchar == 0 || *nextchar == '\0')
    {
      sh_optind++;
      nextchar = (char *)NULL;
    }

  if (sh_badopt = (temp == NULL || c == ':'))
    {
      if (sh_opterr)
	BADOPT (c);

      return '?';
    }

  if (temp[1] == ':')
    {
      if (nextchar && *nextchar)
	{
	  /* This is an option that requires an argument.  */
	  sh_optarg = nextchar;
	  /* If we end this ARGV-element by taking the rest as an arg,
	     we must advance to the next element now.  */
	  sh_optind++;
	}
      else if (sh_optind == argc)
	{
	  if (sh_opterr)
	    NEEDARG (c);

	  sh_optopt = c;
	  sh_optarg = "";	/* Needed by getopts. */
	  c = (optstring[0] == ':') ? ':' : '?';
	}
      else
	/* We already incremented `sh_optind' once;
	   increment it again when taking next ARGV-elt as argument.  */
	sh_optarg = argv[sh_optind++];
      nextchar = (char *)NULL;
    }
  return c;
}

void
sh_getopt_restore_state (argv)
     char **argv;
{
  if (nextchar)
    nextchar = argv[sh_curopt] + sh_charindex;
}

sh_getopt_state_t *
sh_getopt_alloc_istate ()
{
  sh_getopt_state_t *ret;

  ret = (sh_getopt_state_t *)xmalloc (sizeof (sh_getopt_state_t));
  return ret;
}

void
sh_getopt_dispose_istate (gs)
     sh_getopt_state_t *gs;
{
  free (gs);
}

sh_getopt_state_t *
sh_getopt_save_istate ()
{
  sh_getopt_state_t *ret;

  ret = sh_getopt_alloc_istate ();

  ret->gs_optarg = sh_optarg;
  ret->gs_optind = sh_optind;
  ret->gs_curopt = sh_curopt;
  ret->gs_nextchar = nextchar;		/* XXX */
  ret->gs_charindex = sh_charindex;
  ret->gs_flags = 0;			/* XXX for later use */

  return ret;
}

void
sh_getopt_restore_istate (state)
     sh_getopt_state_t *state;
{
  sh_optarg = state->gs_optarg;
  sh_optind = state->gs_optind;
  sh_curopt = state->gs_curopt;
  nextchar = state->gs_nextchar;	/* XXX - probably not usable */
  sh_charindex = state->gs_charindex;

  sh_getopt_dispose_istate (state);
}

#if 0
void
sh_getopt_debug_restore_state (argv)
     char **argv;
{
  if (nextchar && nextchar != argv[sh_curopt] + sh_charindex)
    {
      itrace("sh_getopt_debug_restore_state: resetting nextchar");
      nextchar = argv[sh_curopt] + sh_charindex;
    }
}
#endif
 
#ifdef TEST

/* Compile with -DTEST to make an executable for use in testing
   the above definition of `sh_getopt'.  */

int
main (argc, argv)
     int argc;
     char **argv;
{
  int c;
  int digit_sh_optind = 0;

  while (1)
    {
      int this_option_sh_optind = sh_optind ? sh_optind : 1;

      c = sh_getopt (argc, argv, "abc:d:0123456789");
      if (c == EOF)
	break;

      switch (c)
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
	  if (digit_sh_optind != 0 && digit_sh_optind != this_option_sh_optind)
	    printf ("digits occur in two different argv-elements.\n");
	  digit_sh_optind = this_option_sh_optind;
	  printf ("option %c\n", c);
	  break;

	case 'a':
	  printf ("option a\n");
	  break;

	case 'b':
	  printf ("option b\n");
	  break;

	case 'c':
	  printf ("option c with value `%s'\n", sh_optarg);
	  break;

	case '?':
	  break;

	default:
	  printf ("?? sh_getopt returned character code 0%o ??\n", c);
	}
    }

  if (sh_optind < argc)
    {
      printf ("non-option ARGV-elements: ");
      while (sh_optind < argc)
	printf ("%s ", argv[sh_optind++]);
      printf ("\n");
    }

  exit (0);
}

#endif /* TEST */
