/* display.c -- readline redisplay facility. */

/* Copyright (C) 1987-2022 Free Software Foundation, Inc.

   This file is part of the GNU Readline Library (Readline), a library    
   for reading lines of text with interactive input and history editing.

   Readline is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Readline is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Readline.  If not, see <http://www.gnu.org/licenses/>.
*/

#define READLINE_LIBRARY

#if defined (HAVE_CONFIG_H)
#  include "config.h"
#endif

#include <sys/types.h>

#if defined (HAVE_UNISTD_H)
#  include <unistd.h>
#endif /* HAVE_UNISTD_H */

#include "posixstat.h"

#if defined (HAVE_STDLIB_H)
#  include <stdlib.h>
#else
#  include "ansi_stdlib.h"
#endif /* HAVE_STDLIB_H */

#include <stdio.h>

#ifdef __MSDOS__
#  include <pc.h>
#endif

/* System-specific feature definitions and include files. */
#include "rldefs.h"
#include "rlmbutil.h"

/* Termcap library stuff. */
#include "tcap.h"

/* Some standard library routines. */
#include "readline.h"
#include "history.h"

#include "rlprivate.h"
#include "xmalloc.h"

#if !defined (strchr) && !defined (__STDC__)
extern char *strchr (), *strrchr ();
#endif /* !strchr && !__STDC__ */

static void putc_face (int, int, char *);
static void puts_face (const char *, const char *, int);
static void norm_face (char *, int);

static void update_line (char *, char *, char *, char *, int, int, int, int);
static void space_to_eol (int);
static void delete_chars (int);
static void insert_some_chars (char *, int, int);
static void open_some_spaces (int);
static void cr (void);
static void redraw_prompt (char *);
static void _rl_move_cursor_relative (int, const char *, const char *);

/* Values for FLAGS */
#define PMT_MULTILINE	0x01

static char *expand_prompt (char *, int, int *, int *, int *, int *);

#define DEFAULT_LINE_BUFFER_SIZE	1024

/* State of visible and invisible lines. */
struct line_state
  {
    char *line;
    char *lface;
    int *lbreaks;
    int lbsize;
#if defined (HANDLE_MULTIBYTE)
    int wbsize;
    int *wrapped_line;
#endif
  };

/* The line display buffers.  One is the line currently displayed on
   the screen.  The other is the line about to be displayed. */
static struct line_state line_state_array[2];
static struct line_state *line_state_visible = &line_state_array[0];
static struct line_state *line_state_invisible = &line_state_array[1];
static int line_structures_initialized = 0;

/* Backwards-compatible names. */
#define inv_lbreaks	(line_state_invisible->lbreaks)
#define inv_lbsize	(line_state_invisible->lbsize)
#define vis_lbreaks	(line_state_visible->lbreaks)
#define vis_lbsize	(line_state_visible->lbsize)

#define visible_line	(line_state_visible->line)
#define vis_face	(line_state_visible->lface)
#define invisible_line	(line_state_invisible->line)
#define inv_face	(line_state_invisible->lface)

#if defined (HANDLE_MULTIBYTE)
static int _rl_col_width (const char *, int, int, int);
#else
#  define _rl_col_width(l, s, e, f)	(((e) <= (s)) ? 0 : (e) - (s))
#endif

/* Heuristic used to decide whether it is faster to move from CUR to NEW
   by backing up or outputting a carriage return and moving forward.  CUR
   and NEW are either both buffer positions or absolute screen positions. */
#define CR_FASTER(new, cur) (((new) + 1) < ((cur) - (new)))

/* _rl_last_c_pos is an absolute cursor position in multibyte locales and a
   buffer index in others.  This macro is used when deciding whether the
   current cursor position is in the middle of a prompt string containing
   invisible characters.  XXX - might need to take `modmark' into account. */
/* XXX - only valid when tested against _rl_last_c_pos; buffer indices need
   to use prompt_last_invisible directly. */
#define PROMPT_ENDING_INDEX \
  ((MB_CUR_MAX > 1 && rl_byte_oriented == 0) ? prompt_physical_chars : prompt_last_invisible+1)

#define FACE_NORMAL	'0'
#define FACE_STANDOUT	'1'
#define FACE_INVALID	((char)1)
  
/* **************************************************************** */
/*								    */
/*			Display stuff				    */
/*								    */
/* **************************************************************** */

/* This is the stuff that is hard for me.  I never seem to write good
   display routines in C.  Let's see how I do this time. */

/* (PWP) Well... Good for a simple line updater, but totally ignores
   the problems of input lines longer than the screen width.

   update_line and the code that calls it makes a multiple line,
   automatically wrapping line update.  Careful attention needs
   to be paid to the vertical position variables. */

/* Keep two buffers; one which reflects the current contents of the
   screen, and the other to draw what we think the new contents should
   be.  Then compare the buffers, and make whatever changes to the
   screen itself that we should.  Finally, make the buffer that we
   just drew into be the one which reflects the current contents of the
   screen, and place the cursor where it belongs.

   Commands that want to can fix the display themselves, and then let
   this function know that the display has been fixed by setting the
   RL_DISPLAY_FIXED variable.  This is good for efficiency. */

/* Application-specific redisplay function. */
rl_voidfunc_t *rl_redisplay_function = rl_redisplay;

/* Global variables declared here. */
/* What YOU turn on when you have handled all redisplay yourself. */
int rl_display_fixed = 0;

/* The stuff that gets printed out before the actual text of the line.
   This is usually pointing to rl_prompt. */
char *rl_display_prompt = (char *)NULL;

/* Variables used to include the editing mode in the prompt. */
char *_rl_emacs_mode_str;
int _rl_emacs_modestr_len;

char *_rl_vi_ins_mode_str;
int _rl_vi_ins_modestr_len;

char *_rl_vi_cmd_mode_str;
int _rl_vi_cmd_modestr_len;

/* Pseudo-global variables declared here. */

/* Hints for other parts of readline to give to the display engine. */
int _rl_suppress_redisplay = 0;
int _rl_want_redisplay = 0;

/* The visible cursor position.  If you print some text, adjust this. */
/* NOTE: _rl_last_c_pos is used as a buffer index when not in a locale
   supporting multibyte characters, and an absolute cursor position when
   in such a locale.  This is an artifact of the donated multibyte support.
   Care must be taken when modifying its value. */
int _rl_last_c_pos = 0;
int _rl_last_v_pos = 0;

/* Number of physical lines consumed by the current line buffer currently
  on screen minus 1. */
int _rl_vis_botlin = 0;

static int _rl_quick_redisplay = 0;

/* This is a hint update_line gives to rl_redisplay that it has adjusted the
   value of _rl_last_c_pos *and* taken the presence of any invisible chars in
   the prompt into account.  rl_redisplay notes this and does not do the
   adjustment itself. */
static int cpos_adjusted;

/* The index into the line buffer corresponding to the cursor position */
static int cpos_buffer_position;

/* A flag to note when we're displaying the first line of the prompt */
static int displaying_prompt_first_line;
/* The number of multibyte characters in the prompt, if any */
static int prompt_multibyte_chars;

static int _rl_inv_botlin = 0;

/* Variables used only in this file. */
/* The last left edge of text that was displayed.  This is used when
   doing horizontal scrolling.  It shifts in thirds of a screenwidth. */
static int last_lmargin;

/* A buffer for `modeline' messages. */
static char *msg_buf = 0;
static int msg_bufsiz = 0;

/* Non-zero forces the redisplay even if we thought it was unnecessary. */
static int forced_display;

/* Default and initial buffer size.  Can grow. */
static int line_size  = 0;

/* Set to a non-zero value if horizontal scrolling has been enabled
   automatically because the terminal was resized to height 1. */
static int horizontal_scrolling_autoset = 0;	/* explicit initialization */

/* Variables to keep track of the expanded prompt string, which may
   include invisible characters. */

static char *local_prompt, *local_prompt_prefix;
static int local_prompt_len;
static int prompt_prefix_length;
/* Number of chars in the buffer that contribute to visible chars on the screen.
   This might be different from the number of physical chars in the presence
   of multibyte characters */
static int prompt_visible_length;

/* The number of invisible characters in the line currently being
   displayed on the screen. */
static int visible_wrap_offset;

/* The number of invisible characters in the prompt string.  Static so it
   can be shared between rl_redisplay and update_line */
static int wrap_offset;

/* The index of the last invisible character in the prompt string. */
static int prompt_last_invisible;

/* The length (buffer offset) of the first line of the last (possibly
   multi-line) buffer displayed on the screen. */
static int visible_first_line_len;

/* Number of invisible characters on the first physical line of the prompt.
   Only valid when the number of physical characters in the prompt exceeds
   (or is equal to) _rl_screenwidth. */
static int prompt_invis_chars_first_line;

static int prompt_last_screen_line;

static int prompt_physical_chars;

/* An array of indexes into the prompt string where we will break physical
   screen lines.  It's easier to compute in expand_prompt and use later in
   rl_redisplay instead of having rl_redisplay try to guess about invisible
   characters in the prompt or use heuristics about where they are. */
static int *local_prompt_newlines;

/* set to a non-zero value by rl_redisplay if we are marking modified history
   lines and the current line is so marked. */
static int modmark;

static int line_totbytes;

/* Variables to save and restore prompt and display information. */

/* These are getting numerous enough that it's time to create a struct. */

static char *saved_local_prompt;
static char *saved_local_prefix;
static int *saved_local_prompt_newlines;

static int saved_last_invisible;
static int saved_visible_length;
static int saved_prefix_length;
static int saved_local_length;
static int saved_invis_chars_first_line;
static int saved_physical_chars;

/* Return a string indicating the editing mode, for use in the prompt. */

static char *
prompt_modestr (int *lenp)
{
  if (rl_editing_mode == emacs_mode)
    {
      if (lenp)
	*lenp = _rl_emacs_mode_str ? _rl_emacs_modestr_len : RL_EMACS_MODESTR_DEFLEN;
      return _rl_emacs_mode_str ? _rl_emacs_mode_str : RL_EMACS_MODESTR_DEFAULT;
    }
  else if (_rl_keymap == vi_insertion_keymap)
    {
      if (lenp)
	*lenp = _rl_vi_ins_mode_str ? _rl_vi_ins_modestr_len : RL_VI_INS_MODESTR_DEFLEN;
      return _rl_vi_ins_mode_str ? _rl_vi_ins_mode_str : RL_VI_INS_MODESTR_DEFAULT;		/* vi insert mode */
    }
  else
    {
      if (lenp)
	*lenp = _rl_vi_cmd_mode_str ? _rl_vi_cmd_modestr_len : RL_VI_CMD_MODESTR_DEFLEN;
      return _rl_vi_cmd_mode_str ? _rl_vi_cmd_mode_str : RL_VI_CMD_MODESTR_DEFAULT;		/* vi command mode */
    }
}

/* Expand the prompt string S and return the number of visible
   characters in *LP, if LP is not null.  This is currently more-or-less
   a placeholder for expansion.  LIP, if non-null is a place to store the
   index of the last invisible character in the returned string. NIFLP,
   if non-zero, is a place to store the number of invisible characters in
   the first prompt line.  The previous are used as byte counts -- indexes
   into a character buffer.  *VLP gets the number of physical characters in
   the expanded prompt (visible length) */

/* Current implementation:
	\001 (^A) start non-visible characters
	\002 (^B) end non-visible characters
   all characters except \001 and \002 (following a \001) are copied to
   the returned string; all characters except those between \001 and
   \002 are assumed to be `visible'. */	

/* Possible values for FLAGS:
	PMT_MULTILINE	caller indicates that this is part of a multiline prompt
*/

/* This approximates the number of lines the prompt will take when displayed */
#define APPROX_DIV(n, d)	(((n) < (d)) ? 1 : ((n) / (d)) + 1)

static char *
expand_prompt (char *pmt, int flags, int *lp, int *lip, int *niflp, int *vlp)
{
  char *r, *ret, *p, *igstart, *nprompt, *ms;
  int l, rl, last, ignoring, ninvis, invfl, invflset, ind, pind, physchars;
  int mlen, newlines, newlines_guess, bound, can_add_invis;
  int mb_cur_max;

  /* We only expand the mode string for the last line of a multiline prompt
     (a prompt with embedded newlines). */
  ms = (((pmt == rl_prompt) ^ (flags & PMT_MULTILINE)) && _rl_show_mode_in_prompt) ? prompt_modestr (&mlen) : 0;
  if (ms)
    {
      l = strlen (pmt);
      nprompt = (char *)xmalloc (l + mlen + 1);
      memcpy (nprompt, ms, mlen);
      strcpy (nprompt + mlen, pmt);
    }
  else
    nprompt = pmt;

  can_add_invis = 0;
  mb_cur_max = MB_CUR_MAX;

  if (_rl_screenwidth == 0)
    _rl_get_screen_size (0, 0);	/* avoid division by zero */

  /* Short-circuit if we can.  We can do this if we are treating the prompt as
     a sequence of bytes and there are no invisible characters in the prompt
     to deal with. Since we populate local_prompt_newlines, we have to run
     through the rest of the function if this prompt looks like it's going to
     be longer than one screen line. */
  if ((mb_cur_max <= 1 || rl_byte_oriented) && strchr (nprompt, RL_PROMPT_START_IGNORE) == 0)
    {
      l = strlen (nprompt);
      if (l < (_rl_screenwidth > 0 ? _rl_screenwidth : 80))
        {
	  r = (nprompt == pmt) ? savestring (pmt) : nprompt;
	  if (lp)
	    *lp = l;
	  if (lip)
	    *lip = 0;
	  if (niflp)
	    *niflp = 0;
	  if (vlp)
	    *vlp = l;

	  local_prompt_newlines = (int *) xrealloc (local_prompt_newlines, sizeof (int) * 2);
	  local_prompt_newlines[0] = 0;
	  local_prompt_newlines[1] = -1;

	  return r;
        }
    }

  l = strlen (nprompt);			/* XXX */
  r = ret = (char *)xmalloc (l + 1);

  /* Guess at how many screen lines the prompt will take to size the array that
     keeps track of where the line wraps happen */
  newlines_guess = (_rl_screenwidth > 0) ? APPROX_DIV(l,  _rl_screenwidth) : APPROX_DIV(l, 80);
  local_prompt_newlines = (int *) xrealloc (local_prompt_newlines, sizeof (int) * (newlines_guess + 1));
  local_prompt_newlines[newlines = 0] = 0;
  for (rl = 1; rl <= newlines_guess; rl++)
    local_prompt_newlines[rl] = -1;

  rl = physchars = 0;	/* mode string now part of nprompt */
  invfl = 0;		/* invisible chars in first line of prompt */
  invflset = 0;		/* we only want to set invfl once */
  igstart = 0;		/* we're not ignoring any characters yet */

  for (ignoring = last = ninvis = 0, p = nprompt; p && *p; p++)
    {
      /* This code strips the invisible character string markers
	 RL_PROMPT_START_IGNORE and RL_PROMPT_END_IGNORE */
      if (ignoring == 0 && *p == RL_PROMPT_START_IGNORE)		/* XXX - check ignoring? */
	{
	  ignoring = 1;
	  igstart = p;
	  continue;
	}
      else if (ignoring && *p == RL_PROMPT_END_IGNORE)
	{
	  ignoring = 0;
	  /* If we have a run of invisible characters, adjust local_prompt_newlines
	     to add them, since update_line expects them to be counted before
	     wrapping the line. */
	  if (can_add_invis)
	    {
	      local_prompt_newlines[newlines] = r - ret;
	      /* If we're adding to the number of invisible characters on the
		 first line of the prompt, but we've already set the number of
		 invisible characters on that line, we need to adjust the
		 counter. */
	      if (invflset && newlines == 1)
		invfl = ninvis;
	    }
	  if (p != (igstart + 1))
	    last = r - ret - 1;
	  continue;
	}
      else
	{
#if defined (HANDLE_MULTIBYTE)
	  if (mb_cur_max > 1 && rl_byte_oriented == 0)
	    {
	      pind = p - nprompt;
	      ind = _rl_find_next_mbchar (nprompt, pind, 1, MB_FIND_NONZERO);
	      l = ind - pind;
	      while (l--)
	        *r++ = *p++;
	      if (!ignoring)
		{
		  /* rl ends up being assigned to prompt_visible_length,
		     which is the number of characters in the buffer that
		     contribute to characters on the screen, which might
		     not be the same as the number of physical characters
		     on the screen in the presence of multibyte characters */
		  rl += ind - pind;
		  physchars += _rl_col_width (nprompt, pind, ind, 0);
		}
	      else
		ninvis += ind - pind;
	      p--;			/* compensate for later increment */
	    }
	  else
#endif
	    {
	      *r++ = *p;
	      if (!ignoring)
		{
		  rl++;			/* visible length byte counter */
		  physchars++;
		}
	      else
		ninvis++;		/* invisible chars byte counter */
	    }

	  if (invflset == 0 && physchars >= _rl_screenwidth)
	    {
	      invfl = ninvis;
	      invflset = 1;
	    }

	  if (physchars >= (bound = (newlines + 1) * _rl_screenwidth) && local_prompt_newlines[newlines+1] == -1)
	    {
	      int new;
	      if (physchars > bound)		/* should rarely happen */
		{
#if defined (HANDLE_MULTIBYTE)
		  *r = '\0';	/* need null-termination for strlen */
		  if (mb_cur_max > 1 && rl_byte_oriented == 0)
		    new = _rl_find_prev_mbchar (ret, r - ret, MB_FIND_ANY);
		  else
#endif
		    new = r - ret - (physchars - bound);	/* XXX */
		}
	      else
	        new = r - ret;
	      local_prompt_newlines[++newlines] = new;
	    }

	  /* What if a physical character of width >= 2 is split? There is
	     code that wraps before the physical screen width if the character
	     width would exceed it, but it needs to be checked against this
	     code and local_prompt_newlines[]. */
	  if (ignoring == 0)
	    can_add_invis = (physchars == bound); 
	}
    }

  if (rl <= _rl_screenwidth)
    invfl = ninvis;

  *r = '\0';
  if (lp)
    *lp = rl;
  if (lip)
    *lip = last;
  if (niflp)
    *niflp = invfl;
  if  (vlp)
    *vlp = physchars;

  if (nprompt != pmt)
    xfree (nprompt);

  return ret;
}

/* Just strip out RL_PROMPT_START_IGNORE and RL_PROMPT_END_IGNORE from
   PMT and return the rest of PMT. */
char *
_rl_strip_prompt (char *pmt)
{
  char *ret;

  ret = expand_prompt (pmt, 0, (int *)NULL, (int *)NULL, (int *)NULL, (int *)NULL);
  return ret;
}

void
_rl_reset_prompt (void)
{
  rl_visible_prompt_length = rl_expand_prompt (rl_prompt);
}

/*
 * Expand the prompt string into the various display components, if
 * necessary.
 *
 * local_prompt = expanded last line of string in rl_display_prompt
 *		  (portion after the final newline)
 * local_prompt_prefix = portion before last newline of rl_display_prompt,
 *			 expanded via expand_prompt
 * prompt_visible_length = number of visible characters in local_prompt
 * prompt_prefix_length = number of visible characters in local_prompt_prefix
 *
 * It also tries to keep track of the number of invisible characters in the
 * prompt string, and where they are.
 *
 * This function is called once per call to readline().  It may also be
 * called arbitrarily to expand the primary prompt.
 *
 * The return value is the number of visible characters on the last line
 * of the (possibly multi-line) prompt.  In this case, multi-line means
 * there are embedded newlines in the prompt string itself, not that the
 * number of physical characters exceeds the screen width and the prompt
 * wraps.
 */
int
rl_expand_prompt (char *prompt)
{
  char *p, *t;
  int c;

  /* Clear out any saved values. */
  FREE (local_prompt);
  FREE (local_prompt_prefix);

  local_prompt = local_prompt_prefix = (char *)0;
  local_prompt_len = 0;
  prompt_last_invisible = prompt_invis_chars_first_line = 0;
  prompt_visible_length = prompt_physical_chars = 0;

  if (prompt == 0 || *prompt == 0)
    return (0);

  p = strrchr (prompt, '\n');
  if (p == 0)
    {
      /* The prompt is only one logical line, though it might wrap. */
      local_prompt = expand_prompt (prompt, 0, &prompt_visible_length,
					       &prompt_last_invisible,
					       &prompt_invis_chars_first_line,
					       &prompt_physical_chars);
      local_prompt_prefix = (char *)0;
      local_prompt_len = local_prompt ? strlen (local_prompt) : 0;
      return (prompt_visible_length);
    }
  else
    {
      /* The prompt spans multiple lines. */
      t = ++p;
      c = *t; *t = '\0';
      /* The portion of the prompt string up to and including the
	 final newline is now null-terminated. */
      local_prompt_prefix = expand_prompt (prompt, PMT_MULTILINE,
						   &prompt_prefix_length,
						   (int *)NULL,
						   (int *)NULL,
						   (int *)NULL);
      *t = c;

      local_prompt = expand_prompt (p, PMT_MULTILINE,
				       &prompt_visible_length,
				       &prompt_last_invisible,
				       &prompt_invis_chars_first_line,
				       &prompt_physical_chars);
      local_prompt_len = local_prompt ? strlen (local_prompt) : 0;
      return (prompt_prefix_length);
    }
}

/* Allocate the various line structures, making sure they can hold MINSIZE
   bytes. If the existing line size can accommodate MINSIZE bytes, don't do
   anything. */
static void
realloc_line (int minsize)
{
  int minimum_size;
  int newsize, delta;

  minimum_size = DEFAULT_LINE_BUFFER_SIZE;
  if (minsize < minimum_size)
    minsize = minimum_size;
  if (minsize <= _rl_screenwidth)	/* XXX - for gdb */
    minsize = _rl_screenwidth + 1;
  if (line_size >= minsize)
    return;

  newsize = minimum_size;
  while (newsize < minsize)
    newsize *= 2;

  visible_line = (char *)xrealloc (visible_line, newsize);
  vis_face = (char *)xrealloc (vis_face, newsize);

  invisible_line = (char *)xrealloc (invisible_line, newsize);
  inv_face = (char *)xrealloc (inv_face, newsize);

  delta = newsize - line_size;  
  memset (visible_line + line_size, 0, delta);
  memset (vis_face + line_size, FACE_NORMAL, delta);
  memset (invisible_line + line_size, 1, delta);
  memset (inv_face + line_size, FACE_INVALID, delta);

  line_size = newsize;
}

/* Initialize the VISIBLE_LINE and INVISIBLE_LINE arrays, and their associated
   arrays of line break markers.  MINSIZE is the minimum size of VISIBLE_LINE
   and INVISIBLE_LINE; if it is greater than LINE_SIZE, LINE_SIZE is
   increased.  If the lines have already been allocated, this ensures that
   they can hold at least MINSIZE characters. */
static void
init_line_structures (int minsize)
{
  if (invisible_line == 0)	/* initialize it */
    {
      if (line_size > minsize)
	minsize = line_size;
    }
   realloc_line (minsize); 

  if (vis_lbreaks == 0)
    {
      /* should be enough. */
      inv_lbsize = vis_lbsize = 256;

#if defined (HANDLE_MULTIBYTE)
      line_state_visible->wbsize = vis_lbsize;
      line_state_visible->wrapped_line = (int *)xmalloc (line_state_visible->wbsize * sizeof (int));

      line_state_invisible->wbsize = inv_lbsize;
      line_state_invisible->wrapped_line = (int *)xmalloc (line_state_invisible->wbsize * sizeof (int));
#endif

      inv_lbreaks = (int *)xmalloc (inv_lbsize * sizeof (int));
      vis_lbreaks = (int *)xmalloc (vis_lbsize * sizeof (int));
      inv_lbreaks[0] = vis_lbreaks[0] = 0;
    }

  line_structures_initialized = 1;
}

/* Convenience functions to add chars to the invisible line that update the
   face information at the same time. */
static void		/* XXX - change this */
invis_addc (int *outp, char c, char face)
{
  realloc_line (*outp + 1);
  invisible_line[*outp] = c;
  inv_face[*outp] = face;
  *outp += 1;
}

static void
invis_adds (int *outp, const char *str, int n, char face)
{
  int i;

  for (i = 0; i < n; i++)
    invis_addc (outp, str[i], face);
}

static void
invis_nul (int *outp)
{
  invis_addc (outp, '\0', 0);
  *outp -= 1;
}

static void
set_active_region (int *beg, int *end)
{
  if (rl_point >= 0 && rl_point <= rl_end && rl_mark >= 0 && rl_mark <= rl_end)
    {
      *beg = (rl_mark < rl_point) ? rl_mark : rl_point;
      *end = (rl_mark < rl_point) ? rl_point : rl_mark;
    }
}

/* Do whatever tests are necessary and tell update_line that it can do a
   quick, dumb redisplay on the assumption that there are so many
   differences between the old and new lines that it would be a waste to
   compute all the differences.
   Right now, it just sets _rl_quick_redisplay if the current visible line
   is a single line (so we don't have to move vertically or mess with line
   wrapping). */
void
_rl_optimize_redisplay (void)
{
  if (_rl_vis_botlin == 0)
    _rl_quick_redisplay = 1;
}  

/* Basic redisplay algorithm.  See comments inline. */
void
rl_redisplay (void)
{
  int in, out, c, linenum, cursor_linenum;
  int inv_botlin, lb_botlin, lb_linenum, o_cpos;
  int newlines, lpos, temp, n0, num, prompt_lines_estimate;
  char *prompt_this_line;
  char cur_face;
  int hl_begin, hl_end;
  int mb_cur_max = MB_CUR_MAX;
#if defined (HANDLE_MULTIBYTE)
  WCHAR_T wc;
  size_t wc_bytes;
  int wc_width;
  mbstate_t ps;
  int _rl_wrapped_multicolumn = 0;
#endif

  if (_rl_echoing_p == 0)
    return;

  /* Block keyboard interrupts because this function manipulates global
     data structures. */
  _rl_block_sigint ();  
  RL_SETSTATE (RL_STATE_REDISPLAYING);

  cur_face = FACE_NORMAL;
  /* Can turn this into an array for multiple highlighted objects in addition
     to the region */
  hl_begin = hl_end = -1;

  if (rl_mark_active_p ())
    set_active_region (&hl_begin, &hl_end);

  if (!rl_display_prompt)
    rl_display_prompt = "";

  if (line_structures_initialized == 0)
    {
      init_line_structures (0);
      rl_on_new_line ();
    }
  else if (line_size <= _rl_screenwidth)
    init_line_structures (_rl_screenwidth + 1);

  /* Enable horizontal scrolling automatically for terminals of height 1
     where wrapping lines doesn't work. Disable it as soon as the terminal
     height is increased again if it was automatically enabled. */
  if (_rl_screenheight <= 1)
    {
      if (_rl_horizontal_scroll_mode == 0)
	 horizontal_scrolling_autoset = 1;
      _rl_horizontal_scroll_mode = 1;
    }
  else if (horizontal_scrolling_autoset)
    _rl_horizontal_scroll_mode = 0;

  /* Draw the line into the buffer. */
  cpos_buffer_position = -1;

  prompt_multibyte_chars = prompt_visible_length - prompt_physical_chars;

  out = inv_botlin = 0;

  /* Mark the line as modified or not.  We only do this for history
     lines. */
  modmark = 0;
  if (_rl_mark_modified_lines && current_history () && rl_undo_list)
    {
      invis_addc (&out, '*', cur_face);
      invis_nul (&out);
      modmark = 1;
    }

  /* If someone thought that the redisplay was handled, but the currently
     visible line has a different modification state than the one about
     to become visible, then correct the caller's misconception. */
  if (visible_line[0] != invisible_line[0])
    rl_display_fixed = 0;

  /* If the prompt to be displayed is the `primary' readline prompt (the
     one passed to readline()), use the values we have already expanded.
     If not, use what's already in rl_display_prompt.  WRAP_OFFSET is the
     number of non-visible characters (bytes) in the prompt string. */
  /* This is where we output the characters in the prompt before the last
     newline, if any.  If there aren't any embedded newlines, we don't
     write anything. Copy the last line of the prompt string into the line in
     any case */
  if (rl_display_prompt == rl_prompt || local_prompt)
    {
      if (local_prompt_prefix && forced_display)
	_rl_output_some_chars (local_prompt_prefix, strlen (local_prompt_prefix));

      if (local_prompt_len > 0)
	invis_adds (&out, local_prompt, local_prompt_len, cur_face);
      invis_nul (&out);
      wrap_offset = local_prompt_len - prompt_visible_length;
    }
  else
    {
      int pmtlen;
      prompt_this_line = strrchr (rl_display_prompt, '\n');
      if (!prompt_this_line)
	prompt_this_line = rl_display_prompt;
      else
	{
	  prompt_this_line++;
	  pmtlen = prompt_this_line - rl_display_prompt;	/* temp var */
	  if (forced_display)
	    {
	      _rl_output_some_chars (rl_display_prompt, pmtlen);
	      /* Make sure we are at column zero even after a newline,
		 regardless of the state of terminal output processing. */
	      if (pmtlen < 2 || prompt_this_line[-2] != '\r')
		cr ();
	    }
	}

      prompt_physical_chars = pmtlen = strlen (prompt_this_line);	/* XXX */
      invis_adds (&out, prompt_this_line, pmtlen, cur_face);
      invis_nul (&out);
      wrap_offset = prompt_invis_chars_first_line = 0;
    }

#if defined (HANDLE_MULTIBYTE)
#define CHECK_INV_LBREAKS() \
      do { \
	if (newlines >= (inv_lbsize - 2)) \
	  { \
	    inv_lbsize *= 2; \
	    inv_lbreaks = (int *)xrealloc (inv_lbreaks, inv_lbsize * sizeof (int)); \
	  } \
	if (newlines >= (line_state_invisible->wbsize - 2)) \
	  { \
	    line_state_invisible->wbsize *= 2; \
	    line_state_invisible->wrapped_line = (int *)xrealloc (line_state_invisible->wrapped_line, line_state_invisible->wbsize * sizeof(int)); \
	  } \
      } while (0)
#else
#define CHECK_INV_LBREAKS() \
      do { \
	if (newlines >= (inv_lbsize - 2)) \
	  { \
	    inv_lbsize *= 2; \
	    inv_lbreaks = (int *)xrealloc (inv_lbreaks, inv_lbsize * sizeof (int)); \
	  } \
      } while (0)
#endif /* !HANDLE_MULTIBYTE */

#if defined (HANDLE_MULTIBYTE)	  
#define CHECK_LPOS() \
      do { \
	lpos++; \
	if (lpos >= _rl_screenwidth) \
	  { \
	    if (newlines >= (inv_lbsize - 2)) \
	      { \
		inv_lbsize *= 2; \
		inv_lbreaks = (int *)xrealloc (inv_lbreaks, inv_lbsize * sizeof (int)); \
	      } \
	    inv_lbreaks[++newlines] = out; \
	    if (newlines >= (line_state_invisible->wbsize - 2)) \
	      { \
		line_state_invisible->wbsize *= 2; \
		line_state_invisible->wrapped_line = (int *)xrealloc (line_state_invisible->wrapped_line, line_state_invisible->wbsize * sizeof(int)); \
	      } \
	    line_state_invisible->wrapped_line[newlines] = _rl_wrapped_multicolumn; \
	    lpos = 0; \
	  } \
      } while (0)
#else
#define CHECK_LPOS() \
      do { \
	lpos++; \
	if (lpos >= _rl_screenwidth) \
	  { \
	    if (newlines >= (inv_lbsize - 2)) \
	      { \
		inv_lbsize *= 2; \
		inv_lbreaks = (int *)xrealloc (inv_lbreaks, inv_lbsize * sizeof (int)); \
	      } \
	    inv_lbreaks[++newlines] = out; \
	    lpos = 0; \
	  } \
      } while (0)
#endif

  /* inv_lbreaks[i] is where line i starts in the buffer. */
  inv_lbreaks[newlines = 0] = 0;
  /* lpos is a physical cursor position, so it needs to be adjusted by the
     number of invisible characters in the prompt, per line.  We compute
     the line breaks in the prompt string in expand_prompt, taking invisible
     characters into account, and if lpos exceeds the screen width, we copy
     the data in the loop below. */
  lpos = prompt_physical_chars + modmark;

#if defined (HANDLE_MULTIBYTE)
  memset (line_state_invisible->wrapped_line, 0, line_state_invisible->wbsize * sizeof (int));
  num = 0;
#endif

  /* prompt_invis_chars_first_line is the number of invisible characters (bytes)
     in the first physical line of the prompt.
     wrap_offset - prompt_invis_chars_first_line is usually the number of
     invis chars on the second (or, more generally, last) line. */

  /* This is zero-based, used to set the newlines */
  prompt_lines_estimate = lpos / _rl_screenwidth;

  /* what if lpos is already >= _rl_screenwidth before we start drawing the
     contents of the command line? */
  if (lpos >= _rl_screenwidth)
    {
      temp = 0;

      /* first copy the linebreaks array we computed in expand_prompt */
      while (local_prompt_newlines[newlines+1] != -1)
	{
	  temp = local_prompt_newlines[newlines+1];
	  inv_lbreaks[++newlines] = temp;
	}  

      /* Now set lpos from the last newline */
      if (mb_cur_max > 1 && rl_byte_oriented == 0 && prompt_multibyte_chars > 0)
        lpos = _rl_col_width (local_prompt, temp, local_prompt_len, 1) - (wrap_offset - prompt_invis_chars_first_line);
      else
        lpos -= (_rl_screenwidth * newlines);
    }

  prompt_last_screen_line = newlines;

  /* Draw the rest of the line (after the prompt) into invisible_line, keeping
     track of where the cursor is (cpos_buffer_position), the number of the
     line containing the cursor (lb_linenum), the last line number (lb_botlin
     and inv_botlin).
     It maintains an array of line breaks for display (inv_lbreaks).
     This handles expanding tabs for display and displaying meta characters. */
  lb_linenum = 0;
#if defined (HANDLE_MULTIBYTE)
  in = 0;
  if (mb_cur_max > 1 && rl_byte_oriented == 0)
    {
      memset (&ps, 0, sizeof (mbstate_t));
      if (_rl_utf8locale && UTF8_SINGLEBYTE(rl_line_buffer[0]))
	{
	  wc = (WCHAR_T)rl_line_buffer[0];
	  wc_bytes = 1;
	}
      else
	wc_bytes = MBRTOWC (&wc, rl_line_buffer, rl_end, &ps);
    }
  else
    wc_bytes = 1;
  while (in < rl_end)
#else
  for (in = 0; in < rl_end; in++)
#endif
    {
      if (in == hl_begin)
	cur_face = FACE_STANDOUT;
      else if (in == hl_end)
	cur_face = FACE_NORMAL;

      c = (unsigned char)rl_line_buffer[in];

#if defined (HANDLE_MULTIBYTE)
      if (mb_cur_max > 1 && rl_byte_oriented == 0)
	{
	  if (MB_INVALIDCH (wc_bytes))
	    {
	      /* Byte sequence is invalid or shortened.  Assume that the
	         first byte represents a character. */
	      wc_bytes = 1;
	      /* Assume that a character occupies a single column. */
	      wc_width = 1;
	      memset (&ps, 0, sizeof (mbstate_t));
	    }
	  else if (MB_NULLWCH (wc_bytes))
	    break;			/* Found '\0' */
	  else
	    {
	      temp = WCWIDTH (wc);
	      wc_width = (temp >= 0) ? temp : 1;
	    }
	}
#endif

      if (in == rl_point)
	{
	  cpos_buffer_position = out;
	  lb_linenum = newlines;
	}

#if defined (HANDLE_MULTIBYTE)
      if (META_CHAR (c) && _rl_output_meta_chars == 0)	/* XXX - clean up */
#else
      if (META_CHAR (c))
#endif
	{
	  if (_rl_output_meta_chars == 0)
	    {
	      char obuf[5];
	      int olen;

	      olen = sprintf (obuf, "\\%o", c);
	  
	      if (lpos + olen >= _rl_screenwidth)
		{
		  temp = _rl_screenwidth - lpos;
		  CHECK_INV_LBREAKS ();
		  inv_lbreaks[++newlines] = out + temp;
#if defined (HANDLE_MULTIBYTE)
		  line_state_invisible->wrapped_line[newlines] = _rl_wrapped_multicolumn;
#endif
		  lpos = olen - temp;
		}
	      else
		lpos += olen;

	      for (temp = 0; temp < olen; temp++)
		{
		  invis_addc (&out, obuf[temp], cur_face);
		  CHECK_LPOS ();
		}
	    }
	  else
	    {
	      invis_addc (&out, c, cur_face);
	      CHECK_LPOS();
	    }
	}
#if defined (DISPLAY_TABS)
      else if (c == '\t')
	{
	  register int newout;

	  newout = out + 8 - lpos % 8;
	  temp = newout - out;
	  if (lpos + temp >= _rl_screenwidth)
	    {
	      register int temp2;
	      temp2 = _rl_screenwidth - lpos;
	      CHECK_INV_LBREAKS ();
	      inv_lbreaks[++newlines] = out + temp2;
#if defined (HANDLE_MULTIBYTE)
	      line_state_invisible->wrapped_line[newlines] = _rl_wrapped_multicolumn;
#endif
	      lpos = temp - temp2;
	      while (out < newout)
		invis_addc (&out, ' ', cur_face);
	    }
	  else
	    {
	      while (out < newout)
		invis_addc (&out, ' ', cur_face);
	      lpos += temp;
	    }
	}
#endif
      else if (c == '\n' && _rl_horizontal_scroll_mode == 0 && _rl_term_up && *_rl_term_up)
	{
	  invis_addc (&out, '\0', cur_face);
	  CHECK_INV_LBREAKS ();
	  inv_lbreaks[++newlines] = out;
#if defined (HANDLE_MULTIBYTE)
	  line_state_invisible->wrapped_line[newlines] = _rl_wrapped_multicolumn;
#endif
	  lpos = 0;
	}
      else if (CTRL_CHAR (c) || c == RUBOUT)
	{
	  invis_addc (&out, '^', cur_face);
	  CHECK_LPOS();
	  invis_addc (&out, CTRL_CHAR (c) ? UNCTRL (c) : '?', cur_face);
	  CHECK_LPOS();
	}
      else
	{
#if defined (HANDLE_MULTIBYTE)
	  if (mb_cur_max > 1 && rl_byte_oriented == 0)
	    {
	      register int i;

	      _rl_wrapped_multicolumn = 0;

	      if (_rl_screenwidth < lpos + wc_width)
		for (i = lpos; i < _rl_screenwidth; i++)
		  {
		    /* The space will be removed in update_line() */
		    invis_addc (&out, ' ', cur_face);
		    _rl_wrapped_multicolumn++;
		    CHECK_LPOS();
		  }
	      if (in == rl_point)
		{
		  cpos_buffer_position = out;
		  lb_linenum = newlines;
		}
	      for (i = in; i < in+wc_bytes; i++)
		invis_addc (&out, rl_line_buffer[i], cur_face);
	      for (i = 0; i < wc_width; i++)
		CHECK_LPOS();
	    }
	  else
	    {
	      invis_addc (&out, c, cur_face);
	      CHECK_LPOS();
	    }
#else
	  invis_addc (&out, c, cur_face);
	  CHECK_LPOS();
#endif
	}

#if defined (HANDLE_MULTIBYTE)
      if (mb_cur_max > 1 && rl_byte_oriented == 0)
	{
	  in += wc_bytes;
	  if (_rl_utf8locale && UTF8_SINGLEBYTE(rl_line_buffer[in]))
	    {
	      wc = (WCHAR_T)rl_line_buffer[in];
	      wc_bytes = 1;
	      memset (&ps, 0, sizeof (mbstate_t));	/* re-init state */
	    }
	  else
	    wc_bytes = MBRTOWC (&wc, rl_line_buffer + in, rl_end - in, &ps);
	}
      else
        in++;
#endif
    }
  invis_nul (&out);
  line_totbytes = out;
  if (cpos_buffer_position < 0)
    {
      cpos_buffer_position = out;
      lb_linenum = newlines;
    }

  /* If we are switching from one line to multiple wrapped lines, we don't
     want to do a dumb update (or we want to make it smarter). */
  if (_rl_quick_redisplay && newlines > 0)
    _rl_quick_redisplay = 0;

  inv_botlin = lb_botlin = _rl_inv_botlin = newlines;
  CHECK_INV_LBREAKS ();
  inv_lbreaks[newlines+1] = out;
#if defined (HANDLE_MULTIBYTE)
  /* This should be 0 anyway */
  line_state_invisible->wrapped_line[newlines+1] = _rl_wrapped_multicolumn;
#endif
  cursor_linenum = lb_linenum;

  /* CPOS_BUFFER_POSITION == position in buffer where cursor should be placed.
     CURSOR_LINENUM == line number where the cursor should be placed. */

  /* PWP: now is when things get a bit hairy.  The visible and invisible
     line buffers are really multiple lines, which would wrap every
     (screenwidth - 1) characters.  Go through each in turn, finding
     the changed region and updating it.  The line order is top to bottom. */

  /* If we can move the cursor up and down, then use multiple lines,
     otherwise, let long lines display in a single terminal line, and
     horizontally scroll it. */
  displaying_prompt_first_line = 1;
  if (_rl_horizontal_scroll_mode == 0 && _rl_term_up && *_rl_term_up)
    {
      int nleft, pos, changed_screen_line, tx;

      if (!rl_display_fixed || forced_display)
	{
	  forced_display = 0;

	  /* If we have more than a screenful of material to display, then
	     only display a screenful.  We should display the last screen,
	     not the first.  */
	  if (out >= _rl_screenchars)
	    {
#if defined (HANDLE_MULTIBYTE)
	      if (mb_cur_max > 1 && rl_byte_oriented == 0)
		out = _rl_find_prev_mbchar (invisible_line, _rl_screenchars, MB_FIND_ANY);
	      else
#endif
		out = _rl_screenchars - 1;
	    }

	  /* The first line is at character position 0 in the buffer.  The
	     second and subsequent lines start at inv_lbreaks[N], offset by
	     OFFSET (which has already been calculated above).  */

#define INVIS_FIRST()	(prompt_physical_chars > _rl_screenwidth ? prompt_invis_chars_first_line : wrap_offset)
#define WRAP_OFFSET(line, offset)  ((line == 0) \
					? (offset ? INVIS_FIRST() : 0) \
					: ((line == prompt_last_screen_line) ? wrap_offset-prompt_invis_chars_first_line : 0))
#define W_OFFSET(line, offset) ((line) == 0 ? offset : 0)
#define VIS_LLEN(l)	((l) > _rl_vis_botlin ? 0 : (vis_lbreaks[l+1] - vis_lbreaks[l]))
#define INV_LLEN(l)	(inv_lbreaks[l+1] - inv_lbreaks[l])
#define VIS_CHARS(line) (visible_line + vis_lbreaks[line])
#define VIS_FACE(line) (vis_face + vis_lbreaks[line])
#define VIS_LINE(line) ((line) > _rl_vis_botlin) ? "" : VIS_CHARS(line)
#define VIS_LINE_FACE(line) ((line) > _rl_vis_botlin) ? "" : VIS_FACE(line)
#define INV_LINE(line) (invisible_line + inv_lbreaks[line])
#define INV_LINE_FACE(line) (inv_face + inv_lbreaks[line])

#define OLD_CPOS_IN_PROMPT() (cpos_adjusted == 0 && \
			_rl_last_c_pos != o_cpos && \
			_rl_last_c_pos > wrap_offset && \
			o_cpos < prompt_last_invisible)


	  /* We don't want to highlight anything that's going to be off the top
	     of the display; if the current line takes up more than an entire
	    screen, just mark the lines that won't be displayed as having a
	    `normal' face.
	    It's imperfect, but better than display corruption. */
	  if (rl_mark_active_p () && inv_botlin > _rl_screenheight)
	    {
	      int extra;

	      extra = inv_botlin - _rl_screenheight;
	      for (linenum = 0; linenum <= extra; linenum++)
		norm_face (INV_LINE_FACE(linenum), INV_LLEN (linenum));
	    }

	  /* For each line in the buffer, do the updating display. */
	  for (linenum = 0; linenum <= inv_botlin; linenum++)
	    {
	      /* This can lead us astray if we execute a program that changes
		 the locale from a non-multibyte to a multibyte one. */
	      o_cpos = _rl_last_c_pos;
	      cpos_adjusted = 0;
	      update_line (VIS_LINE(linenum), VIS_LINE_FACE(linenum),
			   INV_LINE(linenum), INV_LINE_FACE(linenum),
			   linenum,
			   VIS_LLEN(linenum), INV_LLEN(linenum), inv_botlin);

	      /* update_line potentially changes _rl_last_c_pos, but doesn't
		 take invisible characters into account, since _rl_last_c_pos
		 is an absolute cursor position in a multibyte locale.  We
		 choose to (mostly) compensate for that here, rather than
		 change update_line itself.  There are several cases in which
		 update_line adjusts _rl_last_c_pos itself (so it can pass
		 _rl_move_cursor_relative accurate values); it communicates
		 this back by setting cpos_adjusted.  If we assume that
		 _rl_last_c_pos is correct (an absolute cursor position) each
		 time update_line is called, then we can assume in our
		 calculations that o_cpos does not need to be adjusted by
		 wrap_offset. */
	      if (linenum == 0 && (mb_cur_max > 1 && rl_byte_oriented == 0) && OLD_CPOS_IN_PROMPT())
		_rl_last_c_pos -= prompt_invis_chars_first_line;	/* XXX - was wrap_offset */
	      else if (cpos_adjusted == 0 &&
			linenum == prompt_last_screen_line &&
			prompt_physical_chars > _rl_screenwidth &&
			(mb_cur_max > 1 && rl_byte_oriented == 0) &&
			_rl_last_c_pos != o_cpos &&
			_rl_last_c_pos > (prompt_last_invisible - _rl_screenwidth - prompt_invis_chars_first_line))	/* XXX - rethink this last one */
		/* This assumes that all the invisible characters are split
		   between the first and last lines of the prompt, if the 
		   prompt consumes more than two lines. It's usually right */
		/* XXX - not sure this is ever executed */
		_rl_last_c_pos -= (wrap_offset-prompt_invis_chars_first_line);

	      /* If this is the line with the prompt, we might need to
		 compensate for invisible characters in the new line. Do
		 this only if there is not more than one new line (which
		 implies that we completely overwrite the old visible line)
		 and the new line is shorter than the old.  Make sure we are
		 at the end of the new line before clearing. */
	      if (linenum == 0 &&
		  inv_botlin == 0 && _rl_last_c_pos == out &&
		  (wrap_offset > visible_wrap_offset) &&
		  (_rl_last_c_pos < visible_first_line_len))
		{
		  if (mb_cur_max > 1 && rl_byte_oriented == 0)
		    nleft = _rl_screenwidth - _rl_last_c_pos;
		  else
		    nleft = _rl_screenwidth + wrap_offset - _rl_last_c_pos;
		  if (nleft)
		    _rl_clear_to_eol (nleft);
		}
#if 0
	      /* This segment is intended to handle the case where the old
		 visible prompt has invisible characters and the new line
		 to be displayed needs to clear the rest of the old characters
		 out (e.g., when printing the i-search prompt): in general,
		 the case of the new line being shorter than the old.  We need
		 to be at the end of the new line and the old line needs to be
		 longer than the current cursor position. It's not perfect,
		 since it uses the byte length of the first line, but this will
		 at worst result in some extra clear-to-end-of-lines. We can't
		 use the prompt length variables because they may not
		 correspond to the visible line (see printing the i-search
		 prompt above). The tests for differing numbers of invisible
		 characters may not matter and can probably be removed. */
	      else if (linenum == 0 &&
		       linenum == prompt_last_screen_line &&
		       _rl_last_c_pos == out &&
		       _rl_last_c_pos < visible_first_line_len &&
		       visible_wrap_offset &&
		       visible_wrap_offset != wrap_offset)
		{
		  if (mb_cur_max > 1 && rl_byte_oriented == 0)
		    nleft = _rl_screenwidth - _rl_last_c_pos;
		  else
		    nleft = _rl_screenwidth + wrap_offset - _rl_last_c_pos;
		  if (nleft)
		    _rl_clear_to_eol (nleft);
		}
#endif

	      /* Since the new first line is now visible, save its length. */
	      if (linenum == 0)
		visible_first_line_len = (inv_botlin > 0) ? inv_lbreaks[1] : out - wrap_offset;
	    }

	  /* We may have deleted some lines.  If so, clear the left over
	     blank ones at the bottom out. */
	  if (_rl_vis_botlin > inv_botlin)
	    {
	      char *tt;
	      for (; linenum <= _rl_vis_botlin; linenum++)
		{
		  tt = VIS_CHARS (linenum);
		  _rl_move_vert (linenum);
		  _rl_move_cursor_relative (0, tt, VIS_FACE(linenum));
		  _rl_clear_to_eol
		    ((linenum == _rl_vis_botlin) ? strlen (tt) : _rl_screenwidth);
		}
	    }
	  _rl_vis_botlin = inv_botlin;

	  /* CHANGED_SCREEN_LINE is set to 1 if we have moved to a
	     different screen line during this redisplay. */
	  changed_screen_line = _rl_last_v_pos != cursor_linenum;
	  if (changed_screen_line)
	    {
	      _rl_move_vert (cursor_linenum);
	      /* If we moved up to the line with the prompt using _rl_term_up,
		 the physical cursor position on the screen stays the same,
		 but the buffer position needs to be adjusted to account
		 for invisible characters. */
	      if ((mb_cur_max == 1 || rl_byte_oriented) && cursor_linenum == 0 && wrap_offset)
		_rl_last_c_pos += wrap_offset;
	    }

	  /* Now we move the cursor to where it needs to be.  First, make
	     sure we are on the correct line (cursor_linenum). */

	  /* We have to reprint the prompt if it contains invisible
	     characters, since it's not generally OK to just reprint
	     the characters from the current cursor position.  But we
	     only need to reprint it if the cursor is before the last
	     invisible character in the prompt string. */
	  /* XXX - why not use local_prompt_len? */
	  nleft = prompt_visible_length + wrap_offset;
	  if (cursor_linenum == 0 && wrap_offset > 0 && _rl_last_c_pos > 0 &&
	      _rl_last_c_pos < PROMPT_ENDING_INDEX && local_prompt)
	    {
	      _rl_cr ();
	      if (modmark)
		_rl_output_some_chars ("*", 1);

	      _rl_output_some_chars (local_prompt, nleft);
	      if (mb_cur_max > 1 && rl_byte_oriented == 0)
		_rl_last_c_pos = _rl_col_width (local_prompt, 0, nleft, 1) - wrap_offset + modmark;
	      else
		_rl_last_c_pos = nleft + modmark;
	    }

	  /* Where on that line?  And where does that line start
	     in the buffer? */
	  pos = inv_lbreaks[cursor_linenum];
	  /* nleft == number of characters (bytes) in the line buffer between
	     the start of the line and the desired cursor position. */
	  nleft = cpos_buffer_position - pos;

	  /* NLEFT is now a number of characters in a buffer.  When in a
	     multibyte locale, however, _rl_last_c_pos is an absolute cursor
	     position that doesn't take invisible characters in the prompt
	     into account.  We use a fudge factor to compensate. */

	  /* Since _rl_backspace() doesn't know about invisible characters in
	     the prompt, and there's no good way to tell it, we compensate for
	     those characters here and call _rl_backspace() directly if
	     necessary */
	  if (wrap_offset && cursor_linenum == 0 && nleft < _rl_last_c_pos)
	    {
	      /* TX == new physical cursor position in multibyte locale. */
	      if (mb_cur_max > 1 && rl_byte_oriented == 0)
		tx = _rl_col_width (&visible_line[pos], 0, nleft, 1) - visible_wrap_offset;
	      else
		tx = nleft;
	      if (tx >= 0 && _rl_last_c_pos > tx)
		{
	          _rl_backspace (_rl_last_c_pos - tx);	/* XXX */
	          _rl_last_c_pos = tx;
		}
	    }

	  /* We need to note that in a multibyte locale we are dealing with
	     _rl_last_c_pos as an absolute cursor position, but moving to a
	     point specified by a buffer position (NLEFT) that doesn't take
	     invisible characters into account. */
	  if (mb_cur_max > 1 && rl_byte_oriented == 0)
	    _rl_move_cursor_relative (nleft, &invisible_line[pos], &inv_face[pos]);
	  else if (nleft != _rl_last_c_pos)
	    _rl_move_cursor_relative (nleft, &invisible_line[pos], &inv_face[pos]);
	}
    }
  else				/* Do horizontal scrolling. Much simpler */
    {
#define M_OFFSET(margin, offset) ((margin) == 0 ? offset : 0)
      int lmargin, ndisp, nleft, phys_c_pos, t;

      /* Always at top line. */
      _rl_last_v_pos = 0;

      /* Compute where in the buffer the displayed line should start.  This
	 will be LMARGIN. */

      /* The number of characters that will be displayed before the cursor. */
      ndisp = cpos_buffer_position - wrap_offset;
      nleft  = prompt_visible_length + wrap_offset;
      /* Where the new cursor position will be on the screen.  This can be
	 longer than SCREENWIDTH; if it is, lmargin will be adjusted. */
      phys_c_pos = cpos_buffer_position - (last_lmargin ? last_lmargin : wrap_offset);
      t = _rl_screenwidth / 3;

      /* If the number of characters had already exceeded the screenwidth,
	 last_lmargin will be > 0. */

      /* If the number of characters to be displayed is more than the screen
	 width, compute the starting offset so that the cursor is about
	 two-thirds of the way across the screen. */
      if (phys_c_pos > _rl_screenwidth - 2)
	{
	  lmargin = cpos_buffer_position - (2 * t);
	  if (lmargin < 0)
	    lmargin = 0;
	  /* If the left margin would be in the middle of a prompt with
	     invisible characters, don't display the prompt at all. */
	  if (wrap_offset && lmargin > 0 && lmargin < nleft)
	    lmargin = nleft;
	}
      else if (ndisp < _rl_screenwidth - 2)		/* XXX - was -1 */
	lmargin = 0;
      else if (phys_c_pos < 1)
	{
	  /* If we are moving back towards the beginning of the line and
	     the last margin is no longer correct, compute a new one. */
	  lmargin = ((cpos_buffer_position - 1) / t) * t;	/* XXX */
	  if (wrap_offset && lmargin > 0 && lmargin < nleft)
	    lmargin = nleft;
	}
      else
	lmargin = last_lmargin;

      displaying_prompt_first_line = lmargin < nleft;

      /* If the first character on the screen isn't the first character
	 in the display line, indicate this with a special character. */
      if (lmargin > 0)
	invisible_line[lmargin] = '<';

      /* If SCREENWIDTH characters starting at LMARGIN do not encompass
	 the whole line, indicate that with a special character at the
	 right edge of the screen.  If LMARGIN is 0, we need to take the
	 wrap offset into account. */
      t = lmargin + M_OFFSET (lmargin, wrap_offset) + _rl_screenwidth;
      if (t > 0 && t < out)
	invisible_line[t - 1] = '>';

      if (rl_display_fixed == 0 || forced_display || lmargin != last_lmargin)
	{
	  forced_display = 0;
	  o_cpos = _rl_last_c_pos;
	  cpos_adjusted = 0;
	  update_line (&visible_line[last_lmargin], &vis_face[last_lmargin],
		       &invisible_line[lmargin], &inv_face[lmargin],
		       0,
		       _rl_screenwidth + visible_wrap_offset,
		       _rl_screenwidth + (lmargin ? 0 : wrap_offset),
		       0);

	  if ((mb_cur_max > 1 && rl_byte_oriented == 0) &&
		displaying_prompt_first_line && OLD_CPOS_IN_PROMPT())
	    _rl_last_c_pos -= prompt_invis_chars_first_line;	/* XXX - was wrap_offset */

	  /* If the visible new line is shorter than the old, but the number
	     of invisible characters is greater, and we are at the end of
	     the new line, we need to clear to eol. */
	  t = _rl_last_c_pos - M_OFFSET (lmargin, wrap_offset);
	  if ((M_OFFSET (lmargin, wrap_offset) > visible_wrap_offset) &&
	      (_rl_last_c_pos == out) && displaying_prompt_first_line &&
	      t < visible_first_line_len)
	    {
	      nleft = _rl_screenwidth - t;
	      _rl_clear_to_eol (nleft);
	    }
	  visible_first_line_len = out - lmargin - M_OFFSET (lmargin, wrap_offset);
	  if (visible_first_line_len > _rl_screenwidth)
	    visible_first_line_len = _rl_screenwidth;

	  _rl_move_cursor_relative (cpos_buffer_position - lmargin, &invisible_line[lmargin], &inv_face[lmargin]);
	  last_lmargin = lmargin;
	}
    }
  fflush (rl_outstream);

  /* Swap visible and non-visible lines. */
  {
    struct line_state *vtemp = line_state_visible;

    line_state_visible = line_state_invisible;
    line_state_invisible = vtemp;

    rl_display_fixed = 0;
    /* If we are displaying on a single line, and last_lmargin is > 0, we
       are not displaying any invisible characters, so set visible_wrap_offset
       to 0. */
    if (_rl_horizontal_scroll_mode && last_lmargin)
      visible_wrap_offset = 0;
    else
      visible_wrap_offset = wrap_offset;

    _rl_quick_redisplay = 0;
  }

  RL_UNSETSTATE (RL_STATE_REDISPLAYING);
  _rl_release_sigint ();
}

static void
putc_face (int c, int face, char *cur_face)
{
  char cf;
  cf = *cur_face;
  if (cf != face)
    {
      if (cf != FACE_NORMAL && cf != FACE_STANDOUT)
	return;
      if (face != FACE_NORMAL && face != FACE_STANDOUT)
	return;
      if (face == FACE_STANDOUT && cf == FACE_NORMAL)
	_rl_region_color_on ();
      if (face == FACE_NORMAL && cf == FACE_STANDOUT)
	_rl_region_color_off ();
      *cur_face = face;
    }
  if (c != EOF)
    putc (c, rl_outstream);
}

static void
puts_face (const char *str, const char *face, int n)
{
  int i;
  char cur_face;

  for (cur_face = FACE_NORMAL, i = 0; i < n; i++)
    putc_face ((unsigned char) str[i], face[i], &cur_face);
  putc_face (EOF, FACE_NORMAL, &cur_face);
}

static void
norm_face (char *face, int n)
{
  memset (face, FACE_NORMAL, n);
}

#define ADJUST_CPOS(x) do { _rl_last_c_pos -= (x) ; cpos_adjusted = 1; } while (0)

/* PWP: update_line() is based on finding the middle difference of each
   line on the screen; vis:

			     /old first difference
	/beginning of line   |              /old last same       /old EOL
	v 		     v              v                    v
old:	eddie> Oh, my little gruntle-buggy is to me, as lurgid as
new:	eddie> Oh, my little buggy says to me, as lurgid as
	^		     ^        ^			   ^
	\beginning of line   |	      \new last same	   \new end of line
			     \new first difference

   All are character pointers for the sake of speed.  Special cases for
   no differences, as well as for end of line additions must be handled.

   Could be made even smarter, but this works well enough */
static void
update_line (char *old, char *old_face, char *new, char *new_face, int current_line, int omax, int nmax, int inv_botlin)
{
  char *ofd, *ols, *oe, *nfd, *nls, *ne;
  char *ofdf, *nfdf, *olsf, *nlsf;
  int temp, lendiff, wsatend, od, nd, twidth, o_cpos;
  int current_invis_chars;
  int col_lendiff, col_temp;
  int bytes_to_insert;
  int mb_cur_max = MB_CUR_MAX;
#if defined (HANDLE_MULTIBYTE)
  mbstate_t ps_new, ps_old;
  int new_offset, old_offset;
#endif

  /* If we're at the right edge of a terminal that supports xn, we're
     ready to wrap around, so do so.  This fixes problems with knowing
     the exact cursor position and cut-and-paste with certain terminal
     emulators.  In this calculation, TEMP is the physical screen
     position of the cursor. */
  if (mb_cur_max > 1 && rl_byte_oriented == 0)
    temp = _rl_last_c_pos;
  else
    temp = _rl_last_c_pos - WRAP_OFFSET (_rl_last_v_pos, visible_wrap_offset);
  if (temp == _rl_screenwidth && _rl_term_autowrap && !_rl_horizontal_scroll_mode
	&& _rl_last_v_pos == current_line - 1)
    {
      /* We're going to wrap around by writing the first character of NEW to
	 the screen and dealing with changes to what's visible by modifying
	 OLD to match it.  Complicated by the presence of multi-width
	 characters at the end of the line or beginning of the new one. */
      /* old is always somewhere in visible_line; new is always somewhere in
         invisible_line.  These should always be null-terminated. */
#if defined (HANDLE_MULTIBYTE)
      if (mb_cur_max > 1 && rl_byte_oriented == 0)
	{
	  WCHAR_T wc;
	  mbstate_t ps;
	  int oldwidth, newwidth;
	  int oldbytes, newbytes;
	  size_t ret;

	  /* This fixes only double-column characters, but if the wrapped
	     character consumes more than three columns, spaces will be
	     inserted in the string buffer. */
	  /* XXX remember that we are working on the invisible line right now;
	     we don't swap visible and invisible until just before rl_redisplay
	     returns */
	  /* This will remove the extra placeholder space we added with
	     _rl_wrapped_multicolumn */
	  if (current_line < line_state_invisible->wbsize && line_state_invisible->wrapped_line[current_line] > 0)
	    _rl_clear_to_eol (line_state_invisible->wrapped_line[current_line]);

	  /* 1. how many screen positions does first char in old consume? */
	  memset (&ps, 0, sizeof (mbstate_t));
	  ret = MBRTOWC (&wc, old, mb_cur_max, &ps);
	  oldbytes = ret;
	  if (MB_INVALIDCH (ret))
	    {
	      oldwidth = 1;
	      oldbytes = 1;
	    }
	  else if (MB_NULLWCH (ret))
	    oldwidth = 0;
	  else
	    oldwidth = WCWIDTH (wc);
	  if (oldwidth < 0)
	    oldwidth = 1;

	  /* 2. how many screen positions does the first char in new consume? */
	  memset (&ps, 0, sizeof (mbstate_t));
	  ret = MBRTOWC (&wc, new, mb_cur_max, &ps);
	  newbytes = ret;
	  if (MB_INVALIDCH (ret))
	    {
	      newwidth = 1;
	      newbytes = 1;
	    }
	  else if (MB_NULLWCH (ret))
	    newwidth = 0;
	  else
	    newwidth = WCWIDTH (wc);
	  if (newwidth < 0)
	    newwidth = 1;

	  /* 3. if the new width is less than the old width, we need to keep
	     going in new until we have consumed at least that many screen
	     positions, and figure out how many bytes that will take */
	  while (newbytes < nmax && newwidth < oldwidth)
	    {
	      int t;

	      ret = MBRTOWC (&wc, new+newbytes, mb_cur_max, &ps);
	      if (MB_INVALIDCH (ret))
		{
		  newwidth += 1;
		  newbytes += 1;
		}
	      else if (MB_NULLWCH (ret))
	        break;
	      else
		{
		  t = WCWIDTH (wc);
		  newwidth += (t >= 0) ? t : 1;
		  newbytes += ret;
		}
	    }
	  /* 4. If the new width is more than the old width, keep going in old
	     until we have consumed exactly that many screen positions, and
	     figure out how many bytes that will take.  This is an optimization */
	  while (oldbytes < omax && oldwidth < newwidth)
	    {
	      int t;

	      ret = MBRTOWC (&wc, old+oldbytes, mb_cur_max, &ps);
	      if (MB_INVALIDCH (ret))
		{
		  oldwidth += 1;
		  oldbytes += 1;
		}
	      else if (MB_NULLWCH (ret))
	        break;
	      else
		{
		  t = WCWIDTH (wc);
		  oldwidth += (t >= 0) ? t : 1;
		  oldbytes += ret;
		}
	    }
	  /* 5. write the first newbytes of new, which takes newwidth.  This is
	     where the screen wrapping takes place, and we are now writing
	     characters onto the new line. We need to fix up old so it
	     accurately reflects what is on the screen after the
	     _rl_output_some_chars below. */
	  if (newwidth > 0)
	    {
	      int count, i, j;
	      char *optr;

	      puts_face (new, new_face, newbytes);
	      _rl_last_c_pos = newwidth;
	      _rl_last_v_pos++;

	      /* 5a. If the number of screen positions doesn't match, punt
		 and do a dumb update.
		 5b. If the number of bytes is greater in the new line than
		 the old, do a dumb update, because there is no guarantee we
		 can extend the old line enough to fit the new bytes. */
	      if (newwidth != oldwidth || newbytes > oldbytes)
		{
		  oe = old + omax;
		  ne = new + nmax;
		  nd = newbytes;
		  nfd = new + nd;
		  ofdf = old_face + oldbytes;
		  nfdf = new_face + newbytes;

		  goto dumb_update;
		}
	      if (oldbytes != 0 && newbytes != 0)
		{
		  /* We have written as many bytes from new as we need to
		     consume the first character of old. Fix up `old' so it
		     reflects the new screen contents.  We use +1 in the
		     memmove call to copy the trailing NUL. */
		  /* (strlen(old+oldbytes) == (omax - oldbytes - 1)) */

		  /* Don't bother trying to fit the bytes if the number of bytes
		     doesn't change. */
		  if (oldbytes != newbytes)
		    {
		      memmove (old+newbytes, old+oldbytes, strlen (old+oldbytes) + 1);
		      memmove (old_face+newbytes, old_face+oldbytes, strlen (old+oldbytes) + 1);
		    }
		  memcpy (old, new, newbytes);
		  memcpy (old_face, new_face, newbytes);
		  j = newbytes - oldbytes;
		  omax += j;
		  /* Fix up indices if we copy data from one line to another */
		  for (i = current_line+1; j != 0 && i <= inv_botlin+1 && i <=_rl_vis_botlin+1; i++)
		    vis_lbreaks[i] += j;
		}
	    }
	  else
	    {
	      putc (' ', rl_outstream);
	      _rl_last_c_pos = 1;
	      _rl_last_v_pos++;
	      if (old[0] && new[0])
		{
		  old[0] = new[0];
		  old_face[0] = new_face[0];
		}
	    }
	}
      else
#endif
	{
	  if (new[0])
	    puts_face (new, new_face, 1);
	  else
	    putc (' ', rl_outstream);
	  _rl_last_c_pos = 1;
	  _rl_last_v_pos++;
	  if (old[0] && new[0])
	    {
	      old[0] = new[0];
	      old_face[0] = new_face[0];
	    }
	}
    }

  /* We know that we are dealing with a single screen line here */
  if (_rl_quick_redisplay)
    {
      nfd = new;
      nfdf = new_face;
      ofd = old;
      ofdf = old_face;
      for (od = 0, oe = ofd; od < omax && *oe; oe++, od++);
      for (nd = 0, ne = nfd; nd < nmax && *ne; ne++, nd++);
      od = nd = 0;
      _rl_move_cursor_relative (0, old, old_face);

      bytes_to_insert = ne - nfd;
      if (bytes_to_insert < local_prompt_len)	/* ??? */
	goto dumb_update;

      /* output the prompt, output the line contents, clear the rest */
      _rl_output_some_chars (nfd, local_prompt_len);
      if (mb_cur_max > 1 && rl_byte_oriented == 0)
	_rl_last_c_pos = prompt_physical_chars;
      else
	_rl_last_c_pos = local_prompt_len;

      bytes_to_insert -= local_prompt_len;
      if (bytes_to_insert > 0)
	{
	  puts_face (new+local_prompt_len, nfdf+local_prompt_len, bytes_to_insert);
	  if (mb_cur_max > 1 && rl_byte_oriented)
	    _rl_last_c_pos += _rl_col_width (new, local_prompt_len, ne-new, 1);
	  else
	    _rl_last_c_pos += bytes_to_insert;
	}

      /* See comments at dumb_update: for an explanation of this heuristic */
      if (nmax < omax)
	goto clear_rest_of_line;
      else if ((nmax - W_OFFSET(current_line, wrap_offset)) < (omax - W_OFFSET (current_line, visible_wrap_offset)))
	goto clear_rest_of_line;
      else
	return;
    }

  /* Find first difference. */
#if defined (HANDLE_MULTIBYTE)
  if (mb_cur_max > 1 && rl_byte_oriented == 0)
    {
      /* See if the old line is a subset of the new line, so that the
	 only change is adding characters. */
      temp = (omax < nmax) ? omax : nmax;
      if (memcmp (old, new, temp) == 0 && memcmp (old_face, new_face, temp) == 0)
	{
	  new_offset = old_offset = temp;	/* adding at the end */
	  ofd = old + temp;
	  ofdf = old_face + temp;
	  nfd = new + temp;
	  nfdf = new_face + temp;
	}
      else
	{      
	  memset (&ps_new, 0, sizeof(mbstate_t));
	  memset (&ps_old, 0, sizeof(mbstate_t));

	  /* Are the old and new lines the same? */
	  if (omax == nmax && memcmp (new, old, omax) == 0 && memcmp (new_face, old_face, omax) == 0)
	    {
	      old_offset = omax;
	      new_offset = nmax;
	      ofd = old + omax;
	      ofdf = old_face + omax;
	      nfd = new + nmax;
	      nfdf = new_face + nmax;
	    }
	  else
	    {
	      /* Go through the line from the beginning and find the first
		 difference. We assume that faces change at (possibly multi-
		 byte) character boundaries. */
	      new_offset = old_offset = 0;
	      for (ofd = old, ofdf = old_face, nfd = new, nfdf = new_face;
		    (ofd - old < omax) && *ofd &&
		    _rl_compare_chars(old, old_offset, &ps_old, new, new_offset, &ps_new) &&
		    *ofdf == *nfdf; )
		{
		  old_offset = _rl_find_next_mbchar (old, old_offset, 1, MB_FIND_ANY);
		  new_offset = _rl_find_next_mbchar (new, new_offset, 1, MB_FIND_ANY);

		  ofd = old + old_offset;
		  ofdf = old_face + old_offset;
		  nfd = new + new_offset;
		  nfdf = new_face + new_offset;
		}
	    }
	}
    }
  else
#endif
  for (ofd = old, ofdf = old_face, nfd = new, nfdf = new_face;
       (ofd - old < omax) && *ofd && (*ofd == *nfd) && (*ofdf == *nfdf);
       ofd++, nfd++, ofdf++, nfdf++)
    ;

  /* Move to the end of the screen line.  ND and OD are used to keep track
     of the distance between ne and new and oe and old, respectively, to
     move a subtraction out of each loop. */
  for (od = ofd - old, oe = ofd; od < omax && *oe; oe++, od++);
  for (nd = nfd - new, ne = nfd; nd < nmax && *ne; ne++, nd++);

  /* If no difference, continue to next line. */
  if (ofd == oe && nfd == ne)
    return;

#if defined (HANDLE_MULTIBYTE)
  if (mb_cur_max > 1 && rl_byte_oriented == 0 && _rl_utf8locale)
    {
      WCHAR_T wc;
      mbstate_t ps = { 0 };
      int t;

      /* If the first character in the difference is a zero-width character,
	 assume it's a combining character and back one up so the two base
	 characters no longer compare equivalently. */
      t = MBRTOWC (&wc, ofd, mb_cur_max, &ps);
      if (t > 0 && UNICODE_COMBINING_CHAR (wc) && WCWIDTH (wc) == 0)
	{
	  old_offset = _rl_find_prev_mbchar (old, ofd - old, MB_FIND_ANY);
	  new_offset = _rl_find_prev_mbchar (new, nfd - new, MB_FIND_ANY);
	  ofd = old + old_offset;	/* equal by definition */
	  ofdf = old_face + old_offset;
	  nfd = new + new_offset;
	  nfdf = new_face + new_offset;
	}
    }
#endif

  wsatend = 1;			/* flag for trailing whitespace */

#if defined (HANDLE_MULTIBYTE)
  /* Find the last character that is the same between the two lines.  This
     bounds the region that needs to change. */
  if (mb_cur_max > 1 && rl_byte_oriented == 0)
    {
      ols = old + _rl_find_prev_mbchar (old, oe - old, MB_FIND_ANY);
      olsf = old_face + (ols - old);
      nls = new + _rl_find_prev_mbchar (new, ne - new, MB_FIND_ANY);
      nlsf = new_face + (nls - new);

      while ((ols > ofd) && (nls > nfd))
	{
	  memset (&ps_old, 0, sizeof (mbstate_t));
	  memset (&ps_new, 0, sizeof (mbstate_t));

	  if (_rl_compare_chars (old, ols - old, &ps_old, new, nls - new, &ps_new) == 0 ||
		*olsf != *nlsf)
	    break;

	  if (*ols == ' ')
	    wsatend = 0;

	  ols = old + _rl_find_prev_mbchar (old, ols - old, MB_FIND_ANY);
	  olsf = old_face + (ols - old);
	  nls = new + _rl_find_prev_mbchar (new, nls - new, MB_FIND_ANY);
	  nlsf = new_face + (nls - new);
	}
    }
  else
    {
#endif /* HANDLE_MULTIBYTE */
  ols = oe - 1;			/* find last same */
  olsf = old_face + (ols - old);
  nls = ne - 1;
  nlsf = new_face + (nls - new);
  while ((ols > ofd) && (nls > nfd) && (*ols == *nls) && (*olsf == *nlsf))
    {
      if (*ols != ' ')
	wsatend = 0;
      ols--; olsf--;
      nls--; nlsf--;
    }
#if defined (HANDLE_MULTIBYTE)
    }
#endif

  if (wsatend)
    {
      ols = oe;
      olsf = old_face + (ols - old);
      nls = ne;
      nlsf = new_face + (nls - new);
    }
#if defined (HANDLE_MULTIBYTE)
  /* This may not work for stateful encoding, but who cares?  To handle
     stateful encoding properly, we have to scan each string from the
     beginning and compare. */
  else if (_rl_compare_chars (ols, 0, NULL, nls, 0, NULL) == 0 || *olsf != *nlsf)
#else
  else if (*ols != *nls || *olsf != *nlsf)
#endif
    {
      if (*ols)			/* don't step past the NUL */
	{
	  if (mb_cur_max > 1 && rl_byte_oriented == 0)
	    ols = old + _rl_find_next_mbchar (old, ols - old, 1, MB_FIND_ANY);
	  else
	    ols++;
	}
      if (*nls)
	{
	  if (mb_cur_max > 1 && rl_byte_oriented == 0)
	    nls = new + _rl_find_next_mbchar (new, nls - new, 1, MB_FIND_ANY);
	  else
	    nls++;
	}
      olsf = old_face + (ols - old);
      nlsf = new_face + (nls - new);
    }

  /* count of invisible characters in the current invisible line. */
  current_invis_chars = W_OFFSET (current_line, wrap_offset);
  if (_rl_last_v_pos != current_line)
    {
      _rl_move_vert (current_line);
      /* We have moved up to a new screen line.  This line may or may not have
         invisible characters on it, but we do our best to recalculate
         visible_wrap_offset based on what we know. */
      if (current_line == 0)
	visible_wrap_offset = prompt_invis_chars_first_line;	/* XXX */
#if 0		/* XXX - not yet */
      else if (current_line == prompt_last_screen_line && wrap_offset > prompt_invis_chars_first_line)
	visible_wrap_offset = wrap_offset - prompt_invis_chars_first_line
#endif
      if ((mb_cur_max == 1 || rl_byte_oriented) && current_line == 0 && visible_wrap_offset)
	_rl_last_c_pos += visible_wrap_offset;
    }

  /* If this is the first line and there are invisible characters in the
     prompt string, and the prompt string has not changed, and the current
     cursor position is before the last invisible character in the prompt,
     and the index of the character to move to is past the end of the prompt
     string, then redraw the entire prompt string.  We can only do this
     reliably if the terminal supports a `cr' capability.

     This can also happen if the prompt string has changed, and the first
     difference in the line is in the middle of the prompt string, after a
     sequence of invisible characters (worst case) and before the end of
     the prompt.  In this case, we have to redraw the entire prompt string
     so that the entire sequence of invisible characters is drawn.  We need
     to handle the worst case, when the difference is after (or in the middle
     of) a sequence of invisible characters that changes the text color and
     before the sequence that restores the text color to normal.  Then we have
     to make sure that the lines still differ -- if they don't, we can
     return immediately.

     This is not an efficiency hack -- there is a problem with redrawing
     portions of the prompt string if they contain terminal escape
     sequences (like drawing the `unbold' sequence without a corresponding
     `bold') that manifests itself on certain terminals. */

  lendiff = local_prompt_len;
  if (lendiff > nmax)
    lendiff = nmax;
  od = ofd - old;	/* index of first difference in visible line */
  nd = nfd - new;	/* nd, od are buffer indexes */
  if (current_line == 0 && !_rl_horizontal_scroll_mode &&
      _rl_term_cr && lendiff > prompt_visible_length && _rl_last_c_pos > 0 &&
      (((od > 0 || nd > 0) && (od <= prompt_last_invisible || nd <= prompt_last_invisible)) ||
		((od >= lendiff) && _rl_last_c_pos < PROMPT_ENDING_INDEX)))
    {
      _rl_cr ();
      if (modmark)
	_rl_output_some_chars ("*", 1);
      _rl_output_some_chars (local_prompt, lendiff);
      if (mb_cur_max > 1 && rl_byte_oriented == 0)
	{
	  /* If we just output the entire prompt string we can take advantage
	     of knowing the number of physical characters in the prompt. If
	     the prompt wraps lines (lendiff clamped at nmax), we can't. */
	  if (lendiff == local_prompt_len)
	    _rl_last_c_pos = prompt_physical_chars + modmark;
	  else
	    /* We take wrap_offset into account here so we can pass correct
	       information to _rl_move_cursor_relative. */
	    _rl_last_c_pos = _rl_col_width (local_prompt, 0, lendiff, 1) - wrap_offset + modmark;
	  cpos_adjusted = 1;
	}
      else
	_rl_last_c_pos = lendiff + modmark;

      /* Now if we have printed the prompt string because the first difference
	 was within the prompt, see if we need to recompute where the lines
	 differ.  Check whether where we are now is past the last place where
	 the old and new lines are the same and short-circuit now if we are. */
      if ((od <= prompt_last_invisible || nd <= prompt_last_invisible) &&
          omax == nmax &&
	  lendiff > (ols-old) && lendiff > (nls-new))
	return;

      /* XXX - we need to fix up our calculations if we are now past the
	 old ofd/nfd and the prompt length (or line length) has changed.
	 We punt on the problem and do a dumb update.  We'd like to be able
	 to just output the prompt from the beginning of the line up to the
	 first difference, but you don't know the number of invisible
	 characters in that case.
	 This needs a lot of work to be efficient, but it usually doesn't matter. */
      if ((od <= prompt_last_invisible || nd <= prompt_last_invisible))
	{
	  nfd = new + lendiff;	/* number of characters we output above */
	  nfdf = new_face + lendiff;
	  nd = lendiff;

	  /* Do a dumb update and return */
dumb_update:
	  temp = ne - nfd;
	  if (temp > 0)
	    {
	      puts_face (nfd, nfdf, temp);
	      if (mb_cur_max > 1 && rl_byte_oriented == 0)
		{
		  _rl_last_c_pos += _rl_col_width (new, nd, ne - new, 1);
		  /* Need to adjust here based on wrap_offset. Guess that if
		     this is the line containing the last line of the prompt
		     we need to adjust by
		     	wrap_offset-prompt_invis_chars_first_line
		     on the assumption that this is the number of invisible
		     characters in the last line of the prompt. */
		  if (wrap_offset > prompt_invis_chars_first_line &&
		      current_line == prompt_last_screen_line &&
		      prompt_physical_chars > _rl_screenwidth &&
		      _rl_horizontal_scroll_mode == 0)
		    ADJUST_CPOS (wrap_offset - prompt_invis_chars_first_line);

		  /* If we just output a new line including the prompt, and
		     the prompt includes invisible characters, we need to
		     account for them in the _rl_last_c_pos calculation, since
		     _rl_col_width does not. This happens when other code does
		     a goto dumb_update; */
		  else if (current_line == 0 &&
			   nfd == new &&
			   prompt_invis_chars_first_line &&
			   local_prompt_len <= temp &&
			   wrap_offset >= prompt_invis_chars_first_line &&
			   _rl_horizontal_scroll_mode == 0)
		    ADJUST_CPOS (prompt_invis_chars_first_line);
		}
	      else
		_rl_last_c_pos += temp;
	    }
	  /* This is a useful heuristic, but what we really want is to clear
	     if the new number of visible screen characters is less than the
	     old number of visible screen characters. If the prompt has changed,
	     we don't really have enough information about the visible line to
	     know for sure, so we use another heuristic calclulation below. */
	  if (nmax < omax)
	    goto clear_rest_of_line;	/* XXX */
	  else if ((nmax - W_OFFSET(current_line, wrap_offset)) < (omax - W_OFFSET (current_line, visible_wrap_offset)))
	    goto clear_rest_of_line;
	  else
	    return;
	}
    }

  o_cpos = _rl_last_c_pos;

  /* When this function returns, _rl_last_c_pos is correct, and an absolute
     cursor position in multibyte mode, but a buffer index when not in a
     multibyte locale. */
  _rl_move_cursor_relative (od, old, old_face);

#if defined (HANDLE_MULTIBYTE)
  /* We need to indicate that the cursor position is correct in the presence of
     invisible characters in the prompt string.  Let's see if setting this when
     we make sure we're at the end of the drawn prompt string works. */
  if (current_line == 0 && mb_cur_max > 1 && rl_byte_oriented == 0 &&
      (_rl_last_c_pos > 0 || o_cpos > 0) &&
      _rl_last_c_pos == prompt_physical_chars)
    cpos_adjusted = 1;
#endif

  /* if (len (new) > len (old))
     lendiff == difference in buffer (bytes)
     col_lendiff == difference on screen (columns)
     When not using multibyte characters, these are equal */
  lendiff = (nls - nfd) - (ols - ofd);
  if (mb_cur_max > 1 && rl_byte_oriented == 0)
    {
      int newchars, newwidth, newind;
      int oldchars, oldwidth, oldind;

      newchars = nls - new;
      oldchars = ols - old;

      /* If we can do it, try to adjust nls and ols so that nls-new will
	 contain the entire new prompt string. That way we can use
	 prompt_physical_chars and not have to recompute column widths.
	 _rl_col_width adds wrap_offset and expects the caller to compensate,
	 which we do below, so we do the same thing if we don't call
	 _rl_col_width.
	 We don't have to compare, since we know the characters are the same.
	 The check of differing numbers of invisible chars may be extraneous.
	 XXX - experimental */
      if (current_line == 0 && nfd == new && newchars > prompt_last_invisible &&
	  newchars <= local_prompt_len &&
	  local_prompt_len <= nmax &&
	  current_invis_chars != visible_wrap_offset)
	{
	  while (newchars < nmax && oldchars < omax &&  newchars < local_prompt_len)
	    {
#if defined (HANDLE_MULTIBYTE)
	      newind = _rl_find_next_mbchar (new, newchars, 1, MB_FIND_NONZERO);
	      oldind = _rl_find_next_mbchar (old, oldchars, 1, MB_FIND_NONZERO);

	      nls += newind - newchars;
	      ols += oldind - oldchars;

	      newchars = newind;
	      oldchars = oldind;
#else
	      nls++; ols++;
	      newchars++; oldchars++;
#endif
	    }
	  newwidth = (newchars == local_prompt_len) ? prompt_physical_chars + wrap_offset
	  					    : _rl_col_width (new, 0, nls - new, 1);
	  /* if we changed nls and ols, we need to recompute lendiff */
	  lendiff = (nls - nfd) - (ols - ofd);

	  nlsf = new_face + (nls - new);
	  olsf = old_face + (ols - old);
	}
      else
	newwidth = _rl_col_width (new, nfd - new, nls - new, 1);

      oldwidth = _rl_col_width (old, ofd - old, ols - old, 1);

      col_lendiff = newwidth - oldwidth;
    }
  else
    col_lendiff = lendiff;

  /* col_lendiff uses _rl_col_width(), which doesn't know about whether or not
     the multibyte characters it counts are invisible, so unless we're printing
     the entire prompt string (in which case we can use prompt_physical_chars)
     the count is short by the number of bytes in the invisible multibyte
     characters - the number of multibyte characters.

     We don't have a good way to solve this without moving to something like
     a bitmap that indicates which characters are visible and which are
     invisible. We fix it up (imperfectly) in the caller and by trying to use
     the entire prompt string wherever we can. */
     
  /* If we are changing the number of invisible characters in a line, and
     the spot of first difference is before the end of the invisible chars,
     lendiff needs to be adjusted. */
  if (current_line == 0 && current_invis_chars != visible_wrap_offset)
    {
      if (mb_cur_max > 1 && rl_byte_oriented == 0)
	{
	  lendiff += visible_wrap_offset - current_invis_chars;
	  col_lendiff += visible_wrap_offset - current_invis_chars;
	}
      else
	{
	  lendiff += visible_wrap_offset - current_invis_chars;
	  col_lendiff = lendiff;
	}
    }

  /* We use temp as a count of the number of bytes from the first difference
     to the end of the new line.  col_temp is the corresponding number of
     screen columns.  A `dumb' update moves to the spot of first difference
     and writes TEMP bytes. */
  /* Insert (diff (len (old), len (new)) ch. */
  temp = ne - nfd;
  if (mb_cur_max > 1 && rl_byte_oriented == 0)
    col_temp = _rl_col_width (new, nfd - new, ne - new, 1);
  else
    col_temp = temp;

  /* how many bytes from the new line buffer to write to the display */
  bytes_to_insert = nls - nfd;

  /* col_lendiff > 0 if we are adding characters to the line */
  if (col_lendiff > 0)	/* XXX - was lendiff */
    {
      /* Non-zero if we're increasing the number of lines. */
      int gl = current_line >= _rl_vis_botlin && inv_botlin > _rl_vis_botlin;

      /* If col_lendiff is > 0, implying that the new string takes up more
	 screen real estate than the old, but lendiff is < 0, meaning that it
	 takes fewer bytes, we need to just output the characters starting
	 from the first difference.  These will overwrite what is on the
	 display, so there's no reason to do a smart update.  This can really
	 only happen in a multibyte environment. */
      if (lendiff < 0)
	{
	  puts_face (nfd, nfdf, temp);
	  _rl_last_c_pos += col_temp;
	  /* If nfd begins before any invisible characters in the prompt,
	     adjust _rl_last_c_pos to account for wrap_offset and set
	     cpos_adjusted to let the caller know. */
	  if (current_line == 0 && displaying_prompt_first_line && wrap_offset && ((nfd - new) <= prompt_last_invisible))
	    ADJUST_CPOS (wrap_offset);	/* XXX - prompt_invis_chars_first_line? */
	  return;
	}
      /* Sometimes it is cheaper to print the characters rather than
	 use the terminal's capabilities.  If we're growing the number
	 of lines, make sure we actually cause the new line to wrap
	 around on auto-wrapping terminals. */
      else if (_rl_terminal_can_insert && ((2 * col_temp) >= col_lendiff || _rl_term_IC) && (!_rl_term_autowrap || !gl))
	{
	  /* If lendiff > prompt_visible_length and _rl_last_c_pos == 0 and
	     _rl_horizontal_scroll_mode == 1, inserting the characters with
	     _rl_term_IC or _rl_term_ic will screw up the screen because of the
	     invisible characters.  We need to just draw them. */
	  /* The same thing happens if we're trying to draw before the last
	     invisible character in the prompt string or we're increasing the
	     number of invisible characters in the line and we're not drawing
	     the entire prompt string. */
	  if (*ols && ((_rl_horizontal_scroll_mode &&
			_rl_last_c_pos == 0 &&
			lendiff > prompt_visible_length &&
			current_invis_chars > 0) == 0) &&
		      (((mb_cur_max > 1 && rl_byte_oriented == 0) &&
		        current_line == 0 && wrap_offset &&
		        ((nfd - new) <= prompt_last_invisible) &&
		        (col_lendiff < prompt_visible_length)) == 0) &&
		      (visible_wrap_offset >= current_invis_chars))
	    {
	      open_some_spaces (col_lendiff);
	      puts_face (nfd, nfdf, bytes_to_insert);
	      if (mb_cur_max > 1 && rl_byte_oriented == 0)
		_rl_last_c_pos += _rl_col_width (nfd, 0, bytes_to_insert, 1);
	      else
		_rl_last_c_pos += bytes_to_insert;
	    }
	  else if ((mb_cur_max == 1 || rl_byte_oriented != 0) && *ols == 0 && lendiff > 0)
	    {
	      /* At the end of a line the characters do not have to
		 be "inserted".  They can just be placed on the screen. */
	      puts_face (nfd, nfdf, temp);
	      _rl_last_c_pos += col_temp;
	      return;
	    }
	  else	/* just write from first difference to end of new line */
	    {
	      puts_face (nfd, nfdf, temp);
	      _rl_last_c_pos += col_temp;
	      /* If nfd begins before the last invisible character in the
		 prompt, adjust _rl_last_c_pos to account for wrap_offset
		 and set cpos_adjusted to let the caller know. */
	      if ((mb_cur_max > 1 && rl_byte_oriented == 0) && current_line == 0 && displaying_prompt_first_line && wrap_offset && ((nfd - new) <= prompt_last_invisible))
		ADJUST_CPOS (wrap_offset);	/* XXX - prompt_invis_chars_first_line? */
	      return;
	    }

	  if (bytes_to_insert > lendiff)
	    {
	      /* If nfd begins before the last invisible character in the
		 prompt, adjust _rl_last_c_pos to account for wrap_offset
		 and set cpos_adjusted to let the caller know. */
	      if ((mb_cur_max > 1 && rl_byte_oriented == 0) && current_line == 0 && displaying_prompt_first_line && wrap_offset && ((nfd - new) <= prompt_last_invisible))
		ADJUST_CPOS (wrap_offset);	/* XXX - prompt_invis_chars_first_line? */
	    }
	}
      else
	{
	  /* cannot insert chars, write to EOL */
	  puts_face (nfd, nfdf, temp);
	  _rl_last_c_pos += col_temp;
	  /* If we're in a multibyte locale and were before the last invisible
	     char in the current line (which implies we just output some invisible
	     characters) we need to adjust _rl_last_c_pos, since it represents
	     a physical character position. */
	  /* The current_line*rl_screenwidth+prompt_invis_chars_first_line is a
	     crude attempt to compute how far into the new line buffer we are.
	     It doesn't work well in the face of multibyte characters and needs
	     to be rethought. XXX */
	  if ((mb_cur_max > 1 && rl_byte_oriented == 0) &&
		current_line == prompt_last_screen_line && wrap_offset &&
		displaying_prompt_first_line &&
		wrap_offset != prompt_invis_chars_first_line &&
		((nfd-new) < (prompt_last_invisible-(current_line*_rl_screenwidth+prompt_invis_chars_first_line))))
	    ADJUST_CPOS (wrap_offset - prompt_invis_chars_first_line);

	  /* What happens if wrap_offset == prompt_invis_chars_first_line
	     and we are drawing the first line (current_line == 0), or if we
	     are drawing the first line and changing the number of invisible
	     characters in the line? If we're starting to draw before the last
	     invisible character in the prompt, we need to adjust by
	     _rl_last_c_pos -= prompt_invis_chars_first_line. This can happen
	     when we finish reading a digit argument (with the "(arg: N)"
	     prompt) and are switching back to displaying a line with a prompt
	     containing invisible characters, since we have to redraw the
	     entire prompt string. */
	  if ((mb_cur_max > 1 && rl_byte_oriented == 0) &&
		current_line == 0 && wrap_offset &&
		displaying_prompt_first_line &&
		wrap_offset == prompt_invis_chars_first_line &&
		visible_wrap_offset != current_invis_chars &&
		visible_wrap_offset != prompt_invis_chars_first_line &&
		((nfd-new) < prompt_last_invisible))
	    ADJUST_CPOS (prompt_invis_chars_first_line);
	}
    }
  else				/* Delete characters from line. */
    {
      /* If possible and inexpensive to use terminal deletion, then do so. */
      if (_rl_term_dc && (2 * col_temp) >= -col_lendiff)
	{
	  /* If all we're doing is erasing the invisible characters in the
	     prompt string, don't bother.  It screws up the assumptions
	     about what's on the screen. */
	  if (_rl_horizontal_scroll_mode && _rl_last_c_pos == 0 &&
	      displaying_prompt_first_line &&
	      -lendiff == visible_wrap_offset)
	    col_lendiff = 0;

	  /* If we have moved lmargin and we're shrinking the line, we've
	     already moved the cursor to the first character of the new line,
	     so deleting -col_lendiff characters will mess up the cursor
	     position calculation */
	  if (_rl_horizontal_scroll_mode && displaying_prompt_first_line == 0 &&
		col_lendiff && _rl_last_c_pos < -col_lendiff)
	    col_lendiff = 0;

	  if (col_lendiff)
	    delete_chars (-col_lendiff); /* delete (diff) characters */

	  /* Copy (new) chars to screen from first diff to last match,
	     overwriting what is there. */
	  if (bytes_to_insert > 0)
	    {
	      /* If nfd begins at the prompt, or before the invisible
		 characters in the prompt, we need to adjust _rl_last_c_pos
		 in a multibyte locale to account for the wrap offset and
		 set cpos_adjusted accordingly. */
	      puts_face (nfd, nfdf, bytes_to_insert);
	      if (mb_cur_max > 1 && rl_byte_oriented == 0)
		{
		  /* This still doesn't take into account whether or not the
		     characters that this counts are invisible. */
		  _rl_last_c_pos += _rl_col_width (nfd, 0, bytes_to_insert, 1);
		  if (current_line == 0 && wrap_offset &&
			displaying_prompt_first_line &&
			prompt_invis_chars_first_line &&
			_rl_last_c_pos >= prompt_invis_chars_first_line &&
			((nfd - new) <= prompt_last_invisible))
		    ADJUST_CPOS (prompt_invis_chars_first_line);

#if 1
#ifdef HANDLE_MULTIBYTE
		  /* If we write a non-space into the last screen column,
		     remove the note that we added a space to compensate for
		     a multibyte double-width character that didn't fit, since
		     it's only valid for what was previously there. */
		  /* XXX - watch this */
		  if (_rl_last_c_pos == _rl_screenwidth &&
			line_state_invisible->wrapped_line[current_line+1] &&
			nfd[bytes_to_insert-1] != ' ')
		    line_state_invisible->wrapped_line[current_line+1] = 0;
#endif
#endif
		}
	      else
		_rl_last_c_pos += bytes_to_insert;

	      /* XXX - we only want to do this if we are at the end of the line
		 so we move there with _rl_move_cursor_relative */
	      if (_rl_horizontal_scroll_mode && ((oe-old) > (ne-new)))
		{
		  _rl_move_cursor_relative (ne-new, new, new_face);
		  goto clear_rest_of_line;
		}
	    }
	}
      /* Otherwise, print over the existing material. */
      else
	{
	  if (temp > 0)
	    {
	      /* If nfd begins at the prompt, or before the invisible
		 characters in the prompt, we need to adjust _rl_last_c_pos
		 in a multibyte locale to account for the wrap offset and
		 set cpos_adjusted accordingly. */
	      puts_face (nfd, nfdf, temp);
	      _rl_last_c_pos += col_temp;		/* XXX */
	      if (mb_cur_max > 1 && rl_byte_oriented == 0)
		{
		  if (current_line == 0 && wrap_offset &&
			displaying_prompt_first_line &&
			_rl_last_c_pos > wrap_offset &&
			((nfd - new) <= prompt_last_invisible))
		    ADJUST_CPOS (wrap_offset);	/* XXX - prompt_invis_chars_first_line? */
		}
	    }
clear_rest_of_line:
	  lendiff = (oe - old) - (ne - new);
	  if (mb_cur_max > 1 && rl_byte_oriented == 0)
	    col_lendiff = _rl_col_width (old, 0, oe - old, 1) - _rl_col_width (new, 0, ne - new, 1);
	  else
	    col_lendiff = lendiff;

	  /* If we've already printed over the entire width of the screen,
	     including the old material, then col_lendiff doesn't matter and
	     space_to_eol will insert too many spaces.  XXX - maybe we should
	     adjust col_lendiff based on the difference between _rl_last_c_pos
	     and _rl_screenwidth */
	  if (col_lendiff && ((mb_cur_max == 1 || rl_byte_oriented) || (_rl_last_c_pos < _rl_screenwidth)))
	    {	  
	      if (_rl_term_autowrap && current_line < inv_botlin)
		space_to_eol (col_lendiff);
	      else
		_rl_clear_to_eol (col_lendiff);
	    }
	}
    }
}

/* Tell the update routines that we have moved onto a new (empty) line. */
int
rl_on_new_line (void)
{
  if (visible_line)
    visible_line[0] = '\0';

  _rl_last_c_pos = _rl_last_v_pos = 0;
  _rl_vis_botlin = last_lmargin = 0;
  if (vis_lbreaks)
    vis_lbreaks[0] = vis_lbreaks[1] = 0;
  visible_wrap_offset = 0;
  return 0;
}

/* Clear all screen lines occupied by the current readline line buffer
   (visible line) */
int
rl_clear_visible_line (void)
{
  int curr_line;

  /* Make sure we move to column 0 so we clear the entire line */
  _rl_cr ();
  _rl_last_c_pos = 0;

  /* Move to the last screen line of the current visible line */
  _rl_move_vert (_rl_vis_botlin);

  /* And erase screen lines going up to line 0 (first visible line) */
  for (curr_line = _rl_last_v_pos; curr_line >= 0; curr_line--)
    {
      _rl_move_vert (curr_line);
      _rl_clear_to_eol (_rl_screenwidth);
      _rl_cr ();		/* in case we use space_to_eol() */
    }

  return 0;
}

/* Tell the update routines that we have moved onto a new line with the
   prompt already displayed.  Code originally from the version of readline
   distributed with CLISP.  rl_expand_prompt must have already been called
   (explicitly or implicitly).  This still doesn't work exactly right; it
   should use expand_prompt() */
int
rl_on_new_line_with_prompt (void)
{
  int prompt_size, i, l, real_screenwidth, newlines;
  char *prompt_last_line, *lprompt;

  /* Initialize visible_line and invisible_line to ensure that they can hold
     the already-displayed prompt. */
  prompt_size = strlen (rl_prompt) + 1;
  init_line_structures (prompt_size);

  /* Make sure the line structures hold the already-displayed prompt for
     redisplay. */
  lprompt = local_prompt ? local_prompt : rl_prompt;
  strcpy (visible_line, lprompt);
  strcpy (invisible_line, lprompt);

  /* If the prompt contains newlines, take the last tail. */
  prompt_last_line = strrchr (rl_prompt, '\n');
  if (!prompt_last_line)
    prompt_last_line = rl_prompt;

  l = strlen (prompt_last_line);
  if (MB_CUR_MAX > 1 && rl_byte_oriented == 0)
    _rl_last_c_pos = _rl_col_width (prompt_last_line, 0, l, 1);	/* XXX */
  else
    _rl_last_c_pos = l;

  /* Dissect prompt_last_line into screen lines. Note that here we have
     to use the real screenwidth. Readline's notion of screenwidth might be
     one less, see terminal.c. */
  real_screenwidth = _rl_screenwidth + (_rl_term_autowrap ? 0 : 1);
  _rl_last_v_pos = l / real_screenwidth;
  /* If the prompt length is a multiple of real_screenwidth, we don't know
     whether the cursor is at the end of the last line, or already at the
     beginning of the next line. Output a newline just to be safe. */
  if (l > 0 && (l % real_screenwidth) == 0)
    _rl_output_some_chars ("\n", 1);
  last_lmargin = 0;

  newlines = 0; i = 0;
  while (i <= l)
    {
      _rl_vis_botlin = newlines;
      vis_lbreaks[newlines++] = i;
      i += real_screenwidth;
    }
  vis_lbreaks[newlines] = l;
  visible_wrap_offset = 0;

  rl_display_prompt = rl_prompt;	/* XXX - make sure it's set */

  return 0;
}

/* Actually update the display, period. */
int
rl_forced_update_display (void)
{
  register char *temp;

  if (visible_line)
    {
      temp = visible_line;
      while (*temp)
	*temp++ = '\0';
    }
  rl_on_new_line ();
  forced_display++;
  (*rl_redisplay_function) ();
  return 0;
}

/* Redraw only the last line of a multi-line prompt. */
void
rl_redraw_prompt_last_line (void)
{
  char *t;

  t = strrchr (rl_display_prompt, '\n');
  if (t)
    redraw_prompt (++t);
  else
    rl_forced_update_display ();
}

/* Move the cursor from _rl_last_c_pos to NEW, which are buffer indices.
   (Well, when we don't have multibyte characters, _rl_last_c_pos is a
   buffer index.)
   DATA is the contents of the screen line of interest; i.e., where
   the movement is being done.
   DATA is always the visible line or the invisible line */
static void
_rl_move_cursor_relative (int new, const char *data, const char *dataf)
{
  register int i;
  int woff;			/* number of invisible chars on current line */
  int cpos, dpos;		/* current and desired cursor positions */
  int adjust;
  int in_invisline;
  int mb_cur_max = MB_CUR_MAX;

  woff = WRAP_OFFSET (_rl_last_v_pos, wrap_offset);
  cpos = _rl_last_c_pos;

  if (cpos == 0 && cpos == new)
    return;

#if defined (HANDLE_MULTIBYTE)
  /* If we have multibyte characters, NEW is indexed by the buffer point in
     a multibyte string, but _rl_last_c_pos is the display position.  In
     this case, NEW's display position is not obvious and must be
     calculated.  We need to account for invisible characters in this line,
     as long as we are past them and they are counted by _rl_col_width. */
  if (mb_cur_max > 1 && rl_byte_oriented == 0)
    {
      adjust = 1;
      /* Try to short-circuit common cases and eliminate a bunch of multibyte
	 character function calls. */
      /* 1.  prompt string */
      if (new == local_prompt_len && memcmp (data, local_prompt, new) == 0)
	{
	  dpos = prompt_physical_chars;
	  cpos_adjusted = 1;
	  adjust = 0;
	}
      /* 2.  prompt_string + line contents */
      else if (new > local_prompt_len && local_prompt && memcmp (data, local_prompt, local_prompt_len) == 0)
	{
	  dpos = prompt_physical_chars + _rl_col_width (data, local_prompt_len, new, 1);
	  cpos_adjusted = 1;
	  adjust = 0;
	}
      else
        dpos = _rl_col_width (data, 0, new, 1);

      if (displaying_prompt_first_line == 0)
	adjust = 0;

      /* yet another special case: printing the last line of a prompt with
	 multibyte characters and invisible characters whose printable length
	 exceeds the screen width with the last invisible character
	 (prompt_last_invisible) in the last line.  IN_INVISLINE is the
	 offset of DATA in invisible_line */
      in_invisline = 0;
      if (data > invisible_line && data < invisible_line+inv_lbreaks[_rl_inv_botlin+1])
	in_invisline = data - invisible_line;

      /* Use NEW when comparing against the last invisible character in the
	 prompt string, since they're both buffer indices and DPOS is a
	 desired display position. */
      /* NEW is relative to the current displayed line, while
	 PROMPT_LAST_INVISIBLE is relative to the entire (wrapped) line.
	 Need a way to reconcile these two variables by turning NEW into a
	 buffer position relative to the start of the line */
      if (adjust && ((new > prompt_last_invisible) ||		/* XXX - don't use woff here */
		     (new+in_invisline > prompt_last_invisible) ||	/* invisible line */
	  (prompt_physical_chars >= _rl_screenwidth &&		/* visible line */
	   _rl_last_v_pos == prompt_last_screen_line &&
	   wrap_offset >= woff && dpos >= woff &&
	   new > (prompt_last_invisible-(vis_lbreaks[_rl_last_v_pos])-wrap_offset))))
	   /* XXX last comparison might need to be >= */
	{
	  dpos -= woff;
	  /* Since this will be assigned to _rl_last_c_pos at the end (more
	     precisely, _rl_last_c_pos == dpos when this function returns),
	     let the caller know. */
	  cpos_adjusted = 1;
	}
    }
  else
#endif
    dpos = new;

  /* If we don't have to do anything, then return. */
  if (cpos == dpos)
    return;

  /* It may be faster to output a CR, and then move forwards instead
     of moving backwards. */
  /* i == current physical cursor position. */
#if defined (HANDLE_MULTIBYTE)
  if (mb_cur_max > 1 && rl_byte_oriented == 0)
    i = _rl_last_c_pos;
  else
#endif
  i = _rl_last_c_pos - woff;
  if (dpos == 0 || CR_FASTER (dpos, _rl_last_c_pos) ||
      (_rl_term_autowrap && i == _rl_screenwidth))
    {
      _rl_cr ();
      cpos = _rl_last_c_pos = 0;
    }

  if (cpos < dpos)
    {
      /* Move the cursor forward.  We do it by printing the command
	 to move the cursor forward if there is one, else print that
	 portion of the output buffer again.  Which is cheaper? */

      /* The above comment is left here for posterity.  It is faster
	 to print one character (non-control) than to print a control
	 sequence telling the terminal to move forward one character.
	 That kind of control is for people who don't know what the
	 data is underneath the cursor. */

      /* However, we need a handle on where the current display position is
	 in the buffer for the immediately preceding comment to be true.
	 In multibyte locales, we don't currently have that info available.
	 Without it, we don't know where the data we have to display begins
	 in the buffer and we have to go back to the beginning of the screen
	 line.  In this case, we can use the terminal sequence to move forward
	 if it's available. */
      if (mb_cur_max > 1 && rl_byte_oriented == 0)
	{
	  if (_rl_term_forward_char)
	    {
	      for (i = cpos; i < dpos; i++)
	        tputs (_rl_term_forward_char, 1, _rl_output_character_function);
	    }
	  else
	    {
	      _rl_cr ();
	      puts_face (data, dataf, new);
	    }
	}
      else
	puts_face (data + cpos, dataf + cpos, new - cpos);
    }

#if defined (HANDLE_MULTIBYTE)
  /* NEW points to the buffer point, but _rl_last_c_pos is the display point.
     The byte length of the string is probably bigger than the column width
     of the string, which means that if NEW == _rl_last_c_pos, then NEW's
     display point is less than _rl_last_c_pos. */
#endif
  else if (cpos > dpos)
    _rl_backspace (cpos - dpos);

  _rl_last_c_pos = dpos;
}

/* PWP: move the cursor up or down. */
void
_rl_move_vert (int to)
{
  register int delta, i;

  if (_rl_last_v_pos == to || to > _rl_screenheight)
    return;

  if ((delta = to - _rl_last_v_pos) > 0)
    {
      for (i = 0; i < delta; i++)
	putc ('\n', rl_outstream);
      _rl_cr ();
      _rl_last_c_pos = 0;
    }
  else
    {			/* delta < 0 */
#ifdef __DJGPP__
      int row, col;

      fflush (rl_outstream);
      ScreenGetCursor (&row, &col);
      ScreenSetCursor (row + delta, col);
      i = -delta;
#else
      if (_rl_term_up && *_rl_term_up)
	for (i = 0; i < -delta; i++)
	  tputs (_rl_term_up, 1, _rl_output_character_function);
#endif /* !__DJGPP__ */
    }

  _rl_last_v_pos = to;		/* Now TO is here */
}

/* Physically print C on rl_outstream.  This is for functions which know
   how to optimize the display.  Return the number of characters output. */
int
rl_show_char (int c)
{
  int n = 1;
  if (META_CHAR (c) && (_rl_output_meta_chars == 0))
    {
      fprintf (rl_outstream, "M-");
      n += 2;
      c = UNMETA (c);
    }

#if defined (DISPLAY_TABS)
  if ((CTRL_CHAR (c) && c != '\t') || c == RUBOUT)
#else
  if (CTRL_CHAR (c) || c == RUBOUT)
#endif /* !DISPLAY_TABS */
    {
      fprintf (rl_outstream, "C-");
      n += 2;
      c = CTRL_CHAR (c) ? UNCTRL (c) : '?';
    }

  putc (c, rl_outstream);
  fflush (rl_outstream);
  return n;
}

int
rl_character_len (int c, int pos)
{
  unsigned char uc;

  uc = (unsigned char)c;

  if (META_CHAR (uc))
    return ((_rl_output_meta_chars == 0) ? 4 : 1);

  if (uc == '\t')
    {
#if defined (DISPLAY_TABS)
      return (((pos | 7) + 1) - pos);
#else
      return (2);
#endif /* !DISPLAY_TABS */
    }

  if (CTRL_CHAR (c) || c == RUBOUT)
    return (2);

  return ((ISPRINT (uc)) ? 1 : 2);
}
/* How to print things in the "echo-area".  The prompt is treated as a
   mini-modeline. */
static int msg_saved_prompt = 0;

#if defined (USE_VARARGS)
int
#if defined (PREFER_STDARG)
rl_message (const char *format, ...)
#else
rl_message (va_alist)
     va_dcl
#endif
{
  va_list args;
#if defined (PREFER_VARARGS)
  char *format;
#endif
#if defined (HAVE_VSNPRINTF)
  int bneed;
#endif

#if defined (PREFER_STDARG)
  va_start (args, format);
#else
  va_start (args);
  format = va_arg (args, char *);
#endif

  if (msg_buf == 0)
    msg_buf = xmalloc (msg_bufsiz = 128);

#if defined (HAVE_VSNPRINTF)
  bneed = vsnprintf (msg_buf, msg_bufsiz, format, args);
  if (bneed >= msg_bufsiz - 1)
    {
      msg_bufsiz = bneed + 1;
      msg_buf = xrealloc (msg_buf, msg_bufsiz);
      va_end (args);

#if defined (PREFER_STDARG)
      va_start (args, format);
#else
      va_start (args);
      format = va_arg (args, char *);
#endif
      vsnprintf (msg_buf, msg_bufsiz - 1, format, args);
    }
#else
  vsprintf (msg_buf, format, args);
  msg_buf[msg_bufsiz - 1] = '\0';	/* overflow? */
#endif
  va_end (args);

  if (saved_local_prompt == 0)
    {
      rl_save_prompt ();
      msg_saved_prompt = 1;
    }
  else if (local_prompt != saved_local_prompt)
    {
      FREE (local_prompt);
      FREE (local_prompt_prefix);
      local_prompt = (char *)NULL;
    }
  rl_display_prompt = msg_buf;
  local_prompt = expand_prompt (msg_buf, 0, &prompt_visible_length,
					    &prompt_last_invisible,
					    &prompt_invis_chars_first_line,
					    &prompt_physical_chars);
  local_prompt_prefix = (char *)NULL;
  local_prompt_len = local_prompt ? strlen (local_prompt) : 0;
  (*rl_redisplay_function) ();

  return 0;
}
#else /* !USE_VARARGS */
int
rl_message (format, arg1, arg2)
     char *format;
{
  if (msg_buf == 0)
    msg_buf = xmalloc (msg_bufsiz = 128);

  sprintf (msg_buf, format, arg1, arg2);
  msg_buf[msg_bufsiz - 1] = '\0';	/* overflow? */

  rl_display_prompt = msg_buf;
  if (saved_local_prompt == 0)
    {
      rl_save_prompt ();
      msg_saved_prompt = 1;
    }
  else if (local_prompt != saved_local_prompt)
    {
      FREE (local_prompt);
      FREE (local_prompt_prefix);
      local_prompt = (char *)NULL;
    }
  local_prompt = expand_prompt (msg_buf, 0, &prompt_visible_length,
					    &prompt_last_invisible,
					    &prompt_invis_chars_first_line,
					    &prompt_physical_chars);
  local_prompt_prefix = (char *)NULL;
  local_prompt_len = local_prompt ? strlen (local_prompt) : 0;
  (*rl_redisplay_function) ();
      
  return 0;
}
#endif /* !USE_VARARGS */

/* How to clear things from the "echo-area". */
int
rl_clear_message (void)
{
  rl_display_prompt = rl_prompt;
  if (msg_saved_prompt)
    {
      rl_restore_prompt ();
      msg_saved_prompt = 0;
    }
  (*rl_redisplay_function) ();
  return 0;
}

int
rl_reset_line_state (void)
{
  rl_on_new_line ();

  rl_display_prompt = rl_prompt ? rl_prompt : "";
  forced_display = 1;
  return 0;
}

/* Save all of the variables associated with the prompt and its display. Most
   of the complexity is dealing with the invisible characters in the prompt
   string and where they are. There are enough of these that I should consider
   a struct. */
void
rl_save_prompt (void)
{
  saved_local_prompt = local_prompt;
  saved_local_prefix = local_prompt_prefix;
  saved_prefix_length = prompt_prefix_length;
  saved_local_length = local_prompt_len;
  saved_last_invisible = prompt_last_invisible;
  saved_visible_length = prompt_visible_length;
  saved_invis_chars_first_line = prompt_invis_chars_first_line;
  saved_physical_chars = prompt_physical_chars;
  saved_local_prompt_newlines = local_prompt_newlines;

  local_prompt = local_prompt_prefix = (char *)0;
  local_prompt_len = 0;
  local_prompt_newlines = (int *)0;

  prompt_last_invisible = prompt_visible_length = prompt_prefix_length = 0;
  prompt_invis_chars_first_line = prompt_physical_chars = 0;
}

void
rl_restore_prompt (void)
{
  FREE (local_prompt);
  FREE (local_prompt_prefix);
  FREE (local_prompt_newlines);

  local_prompt = saved_local_prompt;
  local_prompt_prefix = saved_local_prefix;
  local_prompt_len = saved_local_length;
  local_prompt_newlines = saved_local_prompt_newlines;

  prompt_prefix_length = saved_prefix_length;
  prompt_last_invisible = saved_last_invisible;
  prompt_visible_length = saved_visible_length;
  prompt_invis_chars_first_line = saved_invis_chars_first_line;
  prompt_physical_chars = saved_physical_chars;

  /* can test saved_local_prompt to see if prompt info has been saved. */
  saved_local_prompt = saved_local_prefix = (char *)0;
  saved_local_length = 0;
  saved_last_invisible = saved_visible_length = saved_prefix_length = 0;
  saved_invis_chars_first_line = saved_physical_chars = 0;
  saved_local_prompt_newlines = 0;
}

char *
_rl_make_prompt_for_search (int pchar)
{
  int len;
  char *pmt, *p;

  rl_save_prompt ();

  /* We've saved the prompt, and can do anything with the various prompt
     strings we need before they're restored.  We want the unexpanded
     portion of the prompt string after any final newline. */
  p = rl_prompt ? strrchr (rl_prompt, '\n') : 0;
  if (p == 0)
    {
      len = (rl_prompt && *rl_prompt) ? strlen (rl_prompt) : 0;
      pmt = (char *)xmalloc (len + 2);
      if (len)
	strcpy (pmt, rl_prompt);
      pmt[len] = pchar;
      pmt[len+1] = '\0';
    }
  else
    {
      p++;
      len = strlen (p);
      pmt = (char *)xmalloc (len + 2);
      if (len)
	strcpy (pmt, p);
      pmt[len] = pchar;
      pmt[len+1] = '\0';
    }  

  /* will be overwritten by expand_prompt, called from rl_message */
  prompt_physical_chars = saved_physical_chars + 1;
  return pmt;
}

/* Quick redisplay hack when erasing characters at the end of the line. */
void
_rl_erase_at_end_of_line (int l)
{
  register int i;

  _rl_backspace (l);
  for (i = 0; i < l; i++)
    putc (' ', rl_outstream);
  _rl_backspace (l);
  for (i = 0; i < l; i++)
    visible_line[--_rl_last_c_pos] = '\0';
  rl_display_fixed++;
}

/* Clear to the end of the line.  COUNT is the minimum
   number of character spaces to clear, but we use a terminal escape
   sequence if available. */
void
_rl_clear_to_eol (int count)
{
#ifndef __MSDOS__
  if (_rl_term_clreol)
    tputs (_rl_term_clreol, 1, _rl_output_character_function);
  else
#endif
    if (count)
      space_to_eol (count);
}

/* Clear to the end of the line using spaces.  COUNT is the minimum
   number of character spaces to clear, */
static void
space_to_eol (int count)
{
  register int i;

  for (i = 0; i < count; i++)
    putc (' ', rl_outstream);

  _rl_last_c_pos += count;
}

void
_rl_clear_screen (int clrscr)
{
#if defined (__DJGPP__)
  ScreenClear ();
  ScreenSetCursor (0, 0);
#else
  if (_rl_term_clrpag)
    {
      tputs (_rl_term_clrpag, 1, _rl_output_character_function);
      if (clrscr && _rl_term_clrscroll)
	tputs (_rl_term_clrscroll, 1, _rl_output_character_function);
    }
  else
    rl_crlf ();
#endif /* __DJGPP__ */
}

/* Insert COUNT characters from STRING to the output stream at column COL. */
static void
insert_some_chars (char *string, int count, int col)
{
  open_some_spaces (col);
  _rl_output_some_chars (string, count);
}

/* Insert COL spaces, keeping the cursor at the same position.  We follow the
   ncurses documentation and use either im/ei with explicit spaces, or IC/ic
   by itself.  We assume there will either be ei or we don't need to use it. */
static void
open_some_spaces (int col)
{
#if !defined (__MSDOS__) && (!defined (__MINGW32__) || defined (NCURSES_VERSION))
  char *buffer;
  register int i;

  /* If IC is defined, then we do not have to "enter" insert mode. */
  if (_rl_term_IC)
    {
      buffer = tgoto (_rl_term_IC, 0, col);
      tputs (buffer, 1, _rl_output_character_function);
    }
  else if (_rl_term_im && *_rl_term_im)
    {
      tputs (_rl_term_im, 1, _rl_output_character_function);
      /* just output the desired number of spaces */
      for (i = col; i--; )
	_rl_output_character_function (' ');
      /* If there is a string to turn off insert mode, use it now. */
      if (_rl_term_ei && *_rl_term_ei)
	tputs (_rl_term_ei, 1, _rl_output_character_function);
      /* and move back the right number of spaces */
      _rl_backspace (col);
    }
  else if (_rl_term_ic && *_rl_term_ic)
    {
      /* If there is a special command for inserting characters, then
	 use that first to open up the space. */
      for (i = col; i--; )
	tputs (_rl_term_ic, 1, _rl_output_character_function);
    }
#endif /* !__MSDOS__ && (!__MINGW32__ || NCURSES_VERSION)*/
}

/* Delete COUNT characters from the display line. */
static void
delete_chars (int count)
{
  if (count > _rl_screenwidth)	/* XXX */
    return;

#if !defined (__MSDOS__) && (!defined (__MINGW32__) || defined (NCURSES_VERSION))
  if (_rl_term_DC && *_rl_term_DC)
    {
      char *buffer;
      buffer = tgoto (_rl_term_DC, count, count);
      tputs (buffer, count, _rl_output_character_function);
    }
  else
    {
      if (_rl_term_dc && *_rl_term_dc)
	while (count--)
	  tputs (_rl_term_dc, 1, _rl_output_character_function);
    }
#endif /* !__MSDOS__ && (!__MINGW32__ || NCURSES_VERSION)*/
}

void
_rl_update_final (void)
{
  int full_lines, woff, botline_length;

  if (line_structures_initialized == 0)
    return;

  full_lines = 0;
  /* If the cursor is the only thing on an otherwise-blank last line,
     compensate so we don't print an extra CRLF. */
  if (_rl_vis_botlin && _rl_last_c_pos == 0 &&
	visible_line[vis_lbreaks[_rl_vis_botlin]] == 0)
    {
      _rl_vis_botlin--;
      full_lines = 1;
    }
  _rl_move_vert (_rl_vis_botlin);
  woff = W_OFFSET(_rl_vis_botlin, wrap_offset);
  botline_length = VIS_LLEN(_rl_vis_botlin) - woff;
  /* If we've wrapped lines, remove the final xterm line-wrap flag. */
  if (full_lines && _rl_term_autowrap && botline_length == _rl_screenwidth)
    {
      char *last_line, *last_face;

      /* LAST_LINE includes invisible characters, so if you want to get the
	 last character of the first line, you have to take WOFF into account.
	 This needs to be done for both calls to _rl_move_cursor_relative,
	 which takes a buffer position as the first argument, and any direct
	 subscripts of LAST_LINE. */
      last_line = &visible_line[vis_lbreaks[_rl_vis_botlin]]; /* = VIS_CHARS(_rl_vis_botlin); */
      last_face = &vis_face[vis_lbreaks[_rl_vis_botlin]]; /* = VIS_CHARS(_rl_vis_botlin); */
      cpos_buffer_position = -1;	/* don't know where we are in buffer */
      _rl_move_cursor_relative (_rl_screenwidth - 1 + woff, last_line, last_face);	/* XXX */
      _rl_clear_to_eol (0);
      puts_face (&last_line[_rl_screenwidth - 1 + woff],
		 &last_face[_rl_screenwidth - 1 + woff], 1);
    }
  _rl_vis_botlin = 0;
  if (botline_length > 0 || _rl_last_c_pos > 0)
    rl_crlf ();
  fflush (rl_outstream);
  rl_display_fixed++;
}

/* Move to the start of the current line. */
static void
cr (void)
{
  _rl_cr ();
  _rl_last_c_pos = 0;
}

/* Redraw the last line of a multi-line prompt that may possibly contain
   terminal escape sequences.  Called with the cursor at column 0 of the
   line to draw the prompt on. */
static void
redraw_prompt (char *t)
{
  char *oldp;

  oldp = rl_display_prompt;
  rl_save_prompt ();

  rl_display_prompt = t;
  local_prompt = expand_prompt (t, PMT_MULTILINE,
				   &prompt_visible_length,
				   &prompt_last_invisible,
				   &prompt_invis_chars_first_line,
				   &prompt_physical_chars);
  local_prompt_prefix = (char *)NULL;
  local_prompt_len = local_prompt ? strlen (local_prompt) : 0;

  rl_forced_update_display ();

  rl_display_prompt = oldp;
  rl_restore_prompt();
}
      
/* Redisplay the current line after a SIGWINCH is received. */
void
_rl_redisplay_after_sigwinch (void)
{
  char *t;

  /* Clear the last line (assuming that the screen size change will result in
     either more or fewer characters on that line only) and put the cursor at
     column 0.  Make sure the right thing happens if we have wrapped to a new
     screen line. */
  if (_rl_term_cr)
    {
      rl_clear_visible_line ();
      if (_rl_last_v_pos > 0)
	_rl_move_vert (0);
    }
  else
    rl_crlf ();

  if (_rl_screenwidth < prompt_visible_length)
    _rl_reset_prompt ();		/* update local_prompt_newlines array */

  /* Redraw only the last line of a multi-line prompt. */
  t = strrchr (rl_display_prompt, '\n');
  if (t)
    redraw_prompt (++t);
  else
    rl_forced_update_display ();
}

void
_rl_clean_up_for_exit (void)
{
  if (_rl_echoing_p)
    {
      if (_rl_vis_botlin > 0)	/* minor optimization plus bug fix */
	_rl_move_vert (_rl_vis_botlin);
      _rl_vis_botlin = 0;
      fflush (rl_outstream);
      rl_restart_output (1, 0);
    }
}

void
_rl_erase_entire_line (void)
{
  cr ();
  _rl_clear_to_eol (0);
  cr ();
  fflush (rl_outstream);
}

void
_rl_ttyflush (void)
{
  fflush (rl_outstream);
}

/* return the `current display line' of the cursor -- the number of lines to
   move up to get to the first screen line of the current readline line. */
int
_rl_current_display_line (void)
{
  int ret, nleft;

  /* Find out whether or not there might be invisible characters in the
     editing buffer. */
  if (rl_display_prompt == rl_prompt)
    nleft = _rl_last_c_pos - _rl_screenwidth - rl_visible_prompt_length;
  else
    nleft = _rl_last_c_pos - _rl_screenwidth;

  if (nleft > 0)
    ret = 1 + nleft / _rl_screenwidth;
  else
    ret = 0;

  return ret;
}

void
_rl_refresh_line (void)
{
  rl_clear_visible_line ();
  rl_redraw_prompt_last_line ();
  rl_keep_mark_active ();
}

#if defined (HANDLE_MULTIBYTE)
/* Calculate the number of screen columns occupied by STR from START to END.
   In the case of multibyte characters with stateful encoding, we have to
   scan from the beginning of the string to take the state into account. */
static int
_rl_col_width (const char *str, int start, int end, int flags)
{
  WCHAR_T wc;
  mbstate_t ps;
  int tmp, point, width, max;

  if (end <= start)
    return 0;
  if (MB_CUR_MAX == 1 || rl_byte_oriented)
    /* this can happen in some cases where it's inconvenient to check */
    return (end - start);

  memset (&ps, 0, sizeof (mbstate_t));

  point = 0;
  max = end;

  /* Try to short-circuit common cases.  The adjustment to remove wrap_offset
     is done by the caller. */
  /* 1.  prompt string */
  if (flags && start == 0 && end == local_prompt_len && memcmp (str, local_prompt, local_prompt_len) == 0)
    return (prompt_physical_chars + wrap_offset);
  /* 2.  prompt string + line contents */
  else if (flags && start == 0 && local_prompt_len > 0 && end > local_prompt_len && local_prompt && memcmp (str, local_prompt, local_prompt_len) == 0)
    {
      tmp = prompt_physical_chars + wrap_offset;
      /* XXX - try to call ourselves recursively with non-prompt portion */
      tmp += _rl_col_width (str, local_prompt_len, end, flags);
      return (tmp);
    }

  while (point < start)
    {
      if (_rl_utf8locale && UTF8_SINGLEBYTE(str[point]))
	{
	  memset (&ps, 0, sizeof (mbstate_t));
	  tmp = 1;
	}
      else
	tmp = mbrlen (str + point, max, &ps);
      if (MB_INVALIDCH ((size_t)tmp))
	{
	  /* In this case, the bytes are invalid or too short to compose a
	     multibyte character, so we assume that the first byte represents
	     a single character. */
	  point++;
	  max--;

	  /* Clear the state of the byte sequence, because in this case the
	     effect of mbstate is undefined. */
	  memset (&ps, 0, sizeof (mbstate_t));
	}
      else if (MB_NULLWCH (tmp))
	break;		/* Found '\0' */
      else
	{
	  point += tmp;
	  max -= tmp;
	}
    }

  /* If START is not a byte that starts a character, then POINT will be
     greater than START.  In this case, assume that (POINT - START) gives
     a byte count that is the number of columns of difference. */
  width = point - start;

  while (point < end)
    {
      if (_rl_utf8locale && UTF8_SINGLEBYTE(str[point]))
	{
	  tmp = 1;
	  wc = (WCHAR_T) str[point];
	}
      else
	tmp = MBRTOWC (&wc, str + point, max, &ps);
      if (MB_INVALIDCH ((size_t)tmp))
	{
	  /* In this case, the bytes are invalid or too short to compose a
	     multibyte character, so we assume that the first byte represents
	     a single character. */
	  point++;
	  max--;

	  /* and assume that the byte occupies a single column. */
	  width++;

	  /* Clear the state of the byte sequence, because in this case the
	     effect of mbstate is undefined. */
	  memset (&ps, 0, sizeof (mbstate_t));
	}
      else if (MB_NULLWCH (tmp))
	break;			/* Found '\0' */
      else
	{
	  point += tmp;
	  max -= tmp;
	  tmp = WCWIDTH(wc);
	  width += (tmp >= 0) ? tmp : 1;
	}
    }

  width += point - end;

  return width;
}
#endif /* HANDLE_MULTIBYTE */
