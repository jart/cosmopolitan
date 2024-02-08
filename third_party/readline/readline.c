/* readline.c -- a general facility for reading lines of input
   with emacs style editing and completion. */

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
#include "posixstat.h"
#include <fcntl.h>
#if defined (HAVE_SYS_FILE_H)
#  include <sys/file.h>
#endif /* HAVE_SYS_FILE_H */

#if defined (HAVE_UNISTD_H)
#  include <unistd.h>
#endif /* HAVE_UNISTD_H */

#if defined (HAVE_STDLIB_H)
#  include <stdlib.h>
#else
#  include "ansi_stdlib.h"
#endif /* HAVE_STDLIB_H */

#if defined (HAVE_LOCALE_H)
#  include <locale.h>
#endif

#include <stdio.h>
#include "posixjmp.h"
#include <errno.h>

#if !defined (errno)
extern int errno;
#endif /* !errno */

/* System-specific feature definitions and include files. */
#include "rldefs.h"
#include "rlmbutil.h"

#if defined (__EMX__)
#  define INCL_DOSPROCESS
#  include <os2.h>
#endif /* __EMX__ */

/* Some standard library routines. */
#include "readline.h"
#include "history.h"

#include "rlprivate.h"
#include "rlshell.h"
#include "xmalloc.h"

#if defined (COLOR_SUPPORT)
#  include "parse-colors.h"
#endif

#ifndef RL_LIBRARY_VERSION
#  define RL_LIBRARY_VERSION "8.1"
#endif

#ifndef RL_READLINE_VERSION
#  define RL_READLINE_VERSION	0x0801
#endif

/* Forward declarations used in this file. */
static char *readline_internal (void);
static void readline_initialize_everything (void);

static void run_startup_hooks (void);

static void bind_arrow_keys_internal (Keymap);
static void bind_arrow_keys (void);

static void bind_bracketed_paste_prefix (void);

static void readline_default_bindings (void);
static void reset_default_bindings (void);

static int _rl_subseq_result (int, Keymap, int, int);
static int _rl_subseq_getchar (int);

/* **************************************************************** */
/*								    */
/*			Line editing input utility		    */
/*								    */
/* **************************************************************** */

const char *rl_library_version = RL_LIBRARY_VERSION;

int rl_readline_version = RL_READLINE_VERSION;

/* True if this is `real' readline as opposed to some stub substitute. */
int rl_gnu_readline_p = 1;

/* A pointer to the keymap that is currently in use.
   By default, it is the standard emacs keymap. */
Keymap _rl_keymap = emacs_standard_keymap;

/* The current style of editing. */
int rl_editing_mode = emacs_mode;

/* The current insert mode:  input (the default) or overwrite */
int rl_insert_mode = RL_IM_DEFAULT;

/* Non-zero if we called this function from _rl_dispatch().  It's present
   so functions can find out whether they were called from a key binding
   or directly from an application. */
int rl_dispatching;

/* Non-zero if the previous command was a kill command. */
int _rl_last_command_was_kill = 0;

/* The current value of the numeric argument specified by the user. */
int rl_numeric_arg = 1;

/* Non-zero if an argument was typed. */
int rl_explicit_arg = 0;

/* Temporary value used while generating the argument. */
int rl_arg_sign = 1;

/* Non-zero means we have been called at least once before. */
static int rl_initialized;

#if 0
/* If non-zero, this program is running in an EMACS buffer. */
static int running_in_emacs;
#endif

/* Flags word encapsulating the current readline state. */
unsigned long rl_readline_state = RL_STATE_NONE;

/* The current offset in the current input line. */
int rl_point;

/* Mark in the current input line. */
int rl_mark;

/* Length of the current input line. */
int rl_end;

/* Make this non-zero to return the current input_line. */
int rl_done;

/* If non-zero when readline_internal returns, it means we found EOF */
int rl_eof_found = 0;

/* The last function executed by readline. */
rl_command_func_t *rl_last_func = (rl_command_func_t *)NULL;

/* Top level environment for readline_internal (). */
procenv_t _rl_top_level;

/* The streams we interact with. */
FILE *_rl_in_stream, *_rl_out_stream;

/* The names of the streams that we do input and output to. */
FILE *rl_instream = (FILE *)NULL;
FILE *rl_outstream = (FILE *)NULL;

/* Non-zero means echo characters as they are read.  Defaults to no echo;
   set to 1 if there is a controlling terminal, we can get its attributes,
   and the attributes include `echo'.  Look at rltty.c:prepare_terminal_settings
   for the code that sets it. */
int _rl_echoing_p = 0;

/* Current prompt. */
char *rl_prompt = (char *)NULL;
int rl_visible_prompt_length = 0;

/* Set to non-zero by calling application if it has already printed rl_prompt
   and does not want readline to do it the first time. */
int rl_already_prompted = 0;

/* The number of characters read in order to type this complete command. */
int rl_key_sequence_length = 0;

/* If non-zero, then this is the address of a function to call just
   before readline_internal_setup () prints the first prompt. */
rl_hook_func_t *rl_startup_hook = (rl_hook_func_t *)NULL;

/* Any readline function can set this and have it run just before the user's
   rl_startup_hook. */
rl_hook_func_t *_rl_internal_startup_hook = (rl_hook_func_t *)NULL;

/* If non-zero, this is the address of a function to call just before
   readline_internal_setup () returns and readline_internal starts
   reading input characters. */
rl_hook_func_t *rl_pre_input_hook = (rl_hook_func_t *)NULL;

/* What we use internally.  You should always refer to RL_LINE_BUFFER. */
static char *the_line;

/* The character that can generate an EOF.  Really read from
   the terminal driver... just defaulted here. */
int _rl_eof_char = CTRL ('D');

/* Non-zero makes this the next keystroke to read. */
int rl_pending_input = 0;

/* Pointer to a useful terminal name. */
const char *rl_terminal_name = (const char *)NULL;

/* Non-zero means to always use horizontal scrolling in line display. */
int _rl_horizontal_scroll_mode = 0;

/* Non-zero means to display an asterisk at the starts of history lines
   which have been modified. */
int _rl_mark_modified_lines = 0;

/* The style of `bell' notification preferred.  This can be set to NO_BELL,
   AUDIBLE_BELL, or VISIBLE_BELL. */
int _rl_bell_preference = AUDIBLE_BELL;
     
/* String inserted into the line by rl_insert_comment (). */
char *_rl_comment_begin;

/* Keymap holding the function currently being executed. */
Keymap rl_executing_keymap;

/* The function currently being executed. */
rl_command_func_t *_rl_executing_func;

/* Keymap we're currently using to dispatch. */
Keymap _rl_dispatching_keymap;

/* Non-zero means to erase entire line, including prompt, on empty input lines. */
int rl_erase_empty_line = 0;

/* Non-zero means to read only this many characters rather than up to a
   character bound to accept-line. */
int rl_num_chars_to_read = 0;

/* Line buffer and maintenance. */
char *rl_line_buffer = (char *)NULL;
int rl_line_buffer_len = 0;

/* Key sequence `contexts' */
_rl_keyseq_cxt *_rl_kscxt = 0;

int rl_executing_key;
char *rl_executing_keyseq = 0;
int _rl_executing_keyseq_size = 0;

struct _rl_cmd _rl_pending_command;
struct _rl_cmd *_rl_command_to_execute = (struct _rl_cmd *)NULL;

/* Timeout (specified in milliseconds) when reading characters making up an
   ambiguous multiple-key sequence */
int _rl_keyseq_timeout = 500;

#define RESIZE_KEYSEQ_BUFFER() \
  do \
    { \
      if (rl_key_sequence_length + 2 >= _rl_executing_keyseq_size) \
	{ \
	  _rl_executing_keyseq_size += 16; \
	  rl_executing_keyseq = xrealloc (rl_executing_keyseq, _rl_executing_keyseq_size); \
	} \
    } \
  while (0);
        
/* Forward declarations used by the display, termcap, and history code. */

/* **************************************************************** */
/*								    */
/*			`Forward' declarations  		    */
/*								    */
/* **************************************************************** */

/* Non-zero means do not parse any lines other than comments and
   parser directives. */
unsigned char _rl_parsing_conditionalized_out = 0;

/* Non-zero means to convert characters with the meta bit set to
   escape-prefixed characters so we can indirect through
   emacs_meta_keymap or vi_escape_keymap. */
int _rl_convert_meta_chars_to_ascii = 1;

/* Non-zero means to output characters with the meta bit set directly
   rather than as a meta-prefixed escape sequence. */
int _rl_output_meta_chars = 0;

/* Non-zero means to look at the termios special characters and bind
   them to equivalent readline functions at startup. */
int _rl_bind_stty_chars = 1;

/* Non-zero means to go through the history list at every newline (or
   whenever rl_done is set and readline returns) and revert each line to
   its initial state. */
int _rl_revert_all_at_newline = 0;

/* Non-zero means to honor the termios ECHOCTL bit and echo control
   characters corresponding to keyboard-generated signals. */
int _rl_echo_control_chars = 1;

/* Non-zero means to prefix the displayed prompt with a character indicating
   the editing mode: @ for emacs, : for vi-command, + for vi-insert. */
int _rl_show_mode_in_prompt = 0;

/* Non-zero means to attempt to put the terminal in `bracketed paste mode',
   where it will prefix pasted text with an escape sequence and send
   another to mark the end of the paste. */
int _rl_enable_bracketed_paste = BRACKETED_PASTE_DEFAULT;
int _rl_enable_active_region = BRACKETED_PASTE_DEFAULT;

/* **************************************************************** */
/*								    */
/*			Top Level Functions			    */
/*								    */
/* **************************************************************** */

/* Non-zero means treat 0200 bit in terminal input as Meta bit. */
int _rl_meta_flag = 0;	/* Forward declaration */

/* Set up the prompt and expand it.  Called from readline() and
   rl_callback_handler_install (). */
int
rl_set_prompt (const char *prompt)
{
  FREE (rl_prompt);
  rl_prompt = prompt ? savestring (prompt) : (char *)NULL;
  rl_display_prompt = rl_prompt ? rl_prompt : "";

  rl_visible_prompt_length = rl_expand_prompt (rl_prompt);
  return 0;
}
  
/* Read a line of input.  Prompt with PROMPT.  An empty PROMPT means
   none.  A return value of NULL means that EOF was encountered. */
char *
readline (const char *prompt)
{
  char *value;
#if 0
  int in_callback;
#endif

  /* If we are at EOF return a NULL string. */
  if (rl_pending_input == EOF)
    {
      rl_clear_pending_input ();
      return ((char *)NULL);
    }

#if 0
  /* If readline() is called after installing a callback handler, temporarily
     turn off the callback state to avoid ensuing messiness.  Patch supplied
     by the gdb folks.  XXX -- disabled.  This can be fooled and readline
     left in a strange state by a poorly-timed longjmp. */
  if (in_callback = RL_ISSTATE (RL_STATE_CALLBACK))
    RL_UNSETSTATE (RL_STATE_CALLBACK);
#endif

  rl_set_prompt (prompt);

  rl_initialize ();
  if (rl_prep_term_function)
    (*rl_prep_term_function) (_rl_meta_flag);

#if defined (HANDLE_SIGNALS)
  rl_set_signals ();
#endif

  value = readline_internal ();
  if (rl_deprep_term_function)
    (*rl_deprep_term_function) ();

#if defined (HANDLE_SIGNALS)
  rl_clear_signals ();
#endif

#if 0
  if (in_callback)
    RL_SETSTATE (RL_STATE_CALLBACK);
#endif

#if HAVE_DECL_AUDIT_USER_TTY && defined (HAVE_LIBAUDIT_H) && defined (ENABLE_TTY_AUDIT_SUPPORT)
  if (value)
    _rl_audit_tty (value);
#endif

  return (value);
}

static void
run_startup_hooks (void)
{
  if (rl_startup_hook)
    (*rl_startup_hook) ();

  if (_rl_internal_startup_hook)
    (*_rl_internal_startup_hook) ();
}

#if defined (READLINE_CALLBACKS)
#  define STATIC_CALLBACK
#else
#  define STATIC_CALLBACK static
#endif

STATIC_CALLBACK void
readline_internal_setup (void)
{
  char *nprompt;

  _rl_in_stream = rl_instream;
  _rl_out_stream = rl_outstream;

  /* Enable the meta key only for the duration of readline(), if this
     terminal has one and the terminal has been initialized */
  if (_rl_enable_meta & RL_ISSTATE (RL_STATE_TERMPREPPED))
    _rl_enable_meta_key ();

  run_startup_hooks ();

  rl_deactivate_mark ();

#if defined (VI_MODE)
  if (rl_editing_mode == vi_mode)
    rl_vi_insertion_mode (1, 'i');	/* don't want to reset last */
  else
#endif /* VI_MODE */
    if (_rl_show_mode_in_prompt)
      _rl_reset_prompt ();

  /* If we're not echoing, we still want to at least print a prompt, because
     rl_redisplay will not do it for us.  If the calling application has a
     custom redisplay function, though, let that function handle it. */
  if (_rl_echoing_p == 0 && rl_redisplay_function == rl_redisplay)
    {
      if (rl_prompt && rl_already_prompted == 0)
	{
	  nprompt = _rl_strip_prompt (rl_prompt);
	  fprintf (_rl_out_stream, "%s", nprompt);
	  fflush (_rl_out_stream);
	  xfree (nprompt);
	}
    }
  else
    {
      if (rl_prompt && rl_already_prompted)
	rl_on_new_line_with_prompt ();
      else
	rl_on_new_line ();
      (*rl_redisplay_function) ();
    }

  if (rl_pre_input_hook)
    (*rl_pre_input_hook) ();

  RL_CHECK_SIGNALS ();
}

STATIC_CALLBACK char *
readline_internal_teardown (int eof)
{
  char *temp;
  HIST_ENTRY *entry;

  RL_CHECK_SIGNALS ();

  if (eof)
    RL_SETSTATE (RL_STATE_EOF);		/* XXX */

  /* Restore the original of this history line, iff the line that we
     are editing was originally in the history, AND the line has changed. */
  entry = current_history ();

  /* We don't want to do this if we executed functions that call
     history_set_pos to set the history offset to the line containing the
     non-incremental search string. */
  if (entry && rl_undo_list)
   {
      temp = savestring (the_line);
      rl_revert_line (1, 0);
      entry = replace_history_entry (where_history (), the_line, (histdata_t)NULL);
      _rl_free_history_entry (entry);

      strcpy (the_line, temp);
      xfree (temp);
    }

  if (_rl_revert_all_at_newline)
    _rl_revert_all_lines ();

  /* At any rate, it is highly likely that this line has an undo list.  Get
     rid of it now. */
  if (rl_undo_list)
    rl_free_undo_list ();

  /* Disable the meta key, if this terminal has one and we were told to use it.
     The check whether or not we sent the enable string is in
     _rl_disable_meta_key(); the flag is set in _rl_enable_meta_key */
  _rl_disable_meta_key ();

  /* Restore normal cursor, if available. */
  _rl_set_insert_mode (RL_IM_INSERT, 0);

  return (eof ? (char *)NULL : savestring (the_line));
}

void
_rl_internal_char_cleanup (void)
{
  if (_rl_keep_mark_active)
    _rl_keep_mark_active = 0;
  else if (rl_mark_active_p ())
    rl_deactivate_mark ();

#if defined (VI_MODE)
  /* In vi mode, when you exit insert mode, the cursor moves back
     over the previous character.  We explicitly check for that here. */
  if (rl_editing_mode == vi_mode && _rl_keymap == vi_movement_keymap)
    rl_vi_check ();
#endif /* VI_MODE */

  if (rl_num_chars_to_read && rl_end >= rl_num_chars_to_read)
    {
      (*rl_redisplay_function) ();
      _rl_want_redisplay = 0;
      rl_newline (1, '\n');
    }

  if (rl_done == 0)
    {
      (*rl_redisplay_function) ();
      _rl_want_redisplay = 0;
    }

  /* If the application writer has told us to erase the entire line if
     the only character typed was something bound to rl_newline, do so. */
  if (rl_erase_empty_line && rl_done && rl_last_func == rl_newline &&
      rl_point == 0 && rl_end == 0)
    _rl_erase_entire_line ();
}

STATIC_CALLBACK int
#if defined (READLINE_CALLBACKS)
readline_internal_char (void)
#else
readline_internal_charloop (void)
#endif
{
  static int lastc, eof_found;
  int c, code, lk, r;

  lastc = EOF;

#if !defined (READLINE_CALLBACKS)
  eof_found = 0;
  while (rl_done == 0)
    {
#endif
      lk = _rl_last_command_was_kill;

#if defined (HAVE_POSIX_SIGSETJMP)
      code = sigsetjmp (_rl_top_level, 0);
#else
      code = setjmp (_rl_top_level);
#endif

      if (code)
	{
	  (*rl_redisplay_function) ();
	  _rl_want_redisplay = 0;

	  /* If we longjmped because of a timeout, handle it here. */
	  if (RL_ISSTATE (RL_STATE_TIMEOUT))
	    {
	      RL_SETSTATE (RL_STATE_DONE);
	      rl_done = 1;
	      return 1;
	    }

	  /* If we get here, we're not being called from something dispatched
	     from _rl_callback_read_char(), which sets up its own value of
	     _rl_top_level (saving and restoring the old, of course), so
	     we can just return here. */
	  if (RL_ISSTATE (RL_STATE_CALLBACK))
	    return (0);
	}

      if (rl_pending_input == 0)
	{
	  /* Then initialize the argument and number of keys read. */
	  _rl_reset_argument ();
	  rl_executing_keyseq[rl_key_sequence_length = 0] = '\0';
	}

      RL_SETSTATE(RL_STATE_READCMD);
      c = rl_read_key ();
      RL_UNSETSTATE(RL_STATE_READCMD);

      /* look at input.c:rl_getc() for the circumstances under which this will
	 be returned; punt immediately on read error without converting it to
	 a newline; assume that rl_read_key has already called the signal
	 handler. */
      if (c == READERR)
	{
#if defined (READLINE_CALLBACKS)
	  RL_SETSTATE(RL_STATE_DONE);
	  return (rl_done = 1);
#else
	  RL_SETSTATE(RL_STATE_EOF);
	  eof_found = 1;
	  break;
#endif
	}

      /* EOF typed to a non-blank line is ^D the first time, EOF the second
	 time in a row.  This won't return any partial line read from the tty.
	 If we want to change this, to force any existing line to be returned
	 when read(2) reads EOF, for example, this is the place to change. */
      if (c == EOF && rl_end)
	{
	  if (RL_SIG_RECEIVED ())
	    {
	      RL_CHECK_SIGNALS ();
	      if (rl_signal_event_hook)
		(*rl_signal_event_hook) ();		/* XXX */
	    }

	  /* XXX - reading two consecutive EOFs returns EOF */
	  if (RL_ISSTATE (RL_STATE_TERMPREPPED))
	    {
	      if (lastc == _rl_eof_char || lastc == EOF)
		rl_end = 0;
	      else
	        c = _rl_eof_char;
	    }
	  else
	    c = NEWLINE;
	}

      /* The character _rl_eof_char typed to blank line, and not as the
	 previous character is interpreted as EOF.  This doesn't work when
	 READLINE_CALLBACKS is defined, so hitting a series of ^Ds will
	 erase all the chars on the line and then return EOF. */
      if (((c == _rl_eof_char && lastc != c) || c == EOF) && rl_end == 0)
	{
#if defined (READLINE_CALLBACKS)
	  RL_SETSTATE(RL_STATE_DONE);
	  return (rl_done = 1);
#else
	  RL_SETSTATE(RL_STATE_EOF);
	  eof_found = 1;
	  break;
#endif
	}

      lastc = c;
      r = _rl_dispatch ((unsigned char)c, _rl_keymap);
      RL_CHECK_SIGNALS ();

      if (_rl_command_to_execute)
	{
	  (*rl_redisplay_function) ();

	  rl_executing_keymap = _rl_command_to_execute->map;
	  rl_executing_key = _rl_command_to_execute->key;

	  _rl_executing_func = _rl_command_to_execute->func;

	  rl_dispatching = 1;
	  RL_SETSTATE(RL_STATE_DISPATCHING);
	  r = (*(_rl_command_to_execute->func)) (_rl_command_to_execute->count, _rl_command_to_execute->key);
	  _rl_command_to_execute = 0;
	  RL_UNSETSTATE(RL_STATE_DISPATCHING);
	  rl_dispatching = 0;

	  RL_CHECK_SIGNALS ();
	}

      /* If there was no change in _rl_last_command_was_kill, then no kill
	 has taken place.  Note that if input is pending we are reading
	 a prefix command, so nothing has changed yet. */
      if (rl_pending_input == 0 && lk == _rl_last_command_was_kill)
	_rl_last_command_was_kill = 0;

      _rl_internal_char_cleanup ();

#if defined (READLINE_CALLBACKS)
      return 0;
#else
    }

  return (eof_found);
#endif
}

#if defined (READLINE_CALLBACKS)
static int
readline_internal_charloop (void)
{
  int eof = 1;

  while (rl_done == 0)
    eof = readline_internal_char ();
  return (eof);
}
#endif /* READLINE_CALLBACKS */

/* Read a line of input from the global rl_instream, doing output on
   the global rl_outstream.
   If rl_prompt is non-null, then that is our prompt. */
static char *
readline_internal (void)
{
  readline_internal_setup ();
  rl_eof_found = readline_internal_charloop ();
  return (readline_internal_teardown (rl_eof_found));
}

void
_rl_init_line_state (void)
{
  rl_point = rl_end = rl_mark = 0;
  the_line = rl_line_buffer;
  the_line[0] = 0;
}

void
_rl_set_the_line (void)
{
  the_line = rl_line_buffer;
}

#if defined (READLINE_CALLBACKS)
_rl_keyseq_cxt *
_rl_keyseq_cxt_alloc (void)
{
  _rl_keyseq_cxt *cxt;

  cxt = (_rl_keyseq_cxt *)xmalloc (sizeof (_rl_keyseq_cxt));

  cxt->flags = cxt->subseq_arg = cxt->subseq_retval = 0;

  cxt->okey = 0;
  cxt->ocxt = _rl_kscxt;
  cxt->childval = 42;		/* sentinel value */

  return cxt;
}

void
_rl_keyseq_cxt_dispose (_rl_keyseq_cxt *cxt)
{
  xfree (cxt);
}

void
_rl_keyseq_chain_dispose (void)
{
  _rl_keyseq_cxt *cxt;

  while (_rl_kscxt)
    {
      cxt = _rl_kscxt;
      _rl_kscxt = _rl_kscxt->ocxt;
      _rl_keyseq_cxt_dispose (cxt);
    }
}
#endif

static int
_rl_subseq_getchar (int key)
{
  int k;

  if (key == ESC)
    RL_SETSTATE(RL_STATE_METANEXT);
  RL_SETSTATE(RL_STATE_MOREINPUT);
  k = rl_read_key ();
  RL_UNSETSTATE(RL_STATE_MOREINPUT);
  if (key == ESC)
    RL_UNSETSTATE(RL_STATE_METANEXT);

  return k;
}

#if defined (READLINE_CALLBACKS)
int
_rl_dispatch_callback (_rl_keyseq_cxt *cxt)
{
  int nkey, r;

  /* For now */
  /* The first time this context is used, we want to read input and dispatch
     on it.  When traversing the chain of contexts back `up', we want to use
     the value from the next context down.  We're simulating recursion using
     a chain of contexts. */
  if ((cxt->flags & KSEQ_DISPATCHED) == 0)
    {
      nkey = _rl_subseq_getchar (cxt->okey);
      if (nkey < 0)
	{
	  _rl_abort_internal ();
	  return -1;
	}
      r = _rl_dispatch_subseq (nkey, cxt->dmap, cxt->subseq_arg);
      cxt->flags |= KSEQ_DISPATCHED;
    }
  else
    r = cxt->childval;

  /* For now */
  if (r != -3)	/* don't do this if we indicate there will be other matches */
    r = _rl_subseq_result (r, cxt->oldmap, cxt->okey, (cxt->flags & KSEQ_SUBSEQ));

  RL_CHECK_SIGNALS ();
  /* We only treat values < 0 specially to simulate recursion. */
  if (r >= 0 || (r == -1 && (cxt->flags & KSEQ_SUBSEQ) == 0))	/* success! or failure! */
    {
      _rl_keyseq_chain_dispose ();
      RL_UNSETSTATE (RL_STATE_MULTIKEY);
      return r;
    }

  if (r != -3)			/* magic value that says we added to the chain */
    _rl_kscxt = cxt->ocxt;
  if (_rl_kscxt)
    _rl_kscxt->childval = r;
  if (r != -3)
    _rl_keyseq_cxt_dispose (cxt);

  return r;
}
#endif /* READLINE_CALLBACKS */
  
/* Do the command associated with KEY in MAP.
   If the associated command is really a keymap, then read
   another key, and dispatch into that map. */
int
_rl_dispatch (register int key, Keymap map)
{
  _rl_dispatching_keymap = map;
  return _rl_dispatch_subseq (key, map, 0);
}

int
_rl_dispatch_subseq (register int key, Keymap map, int got_subseq)
{
  int r, newkey;
  char *macro;
  rl_command_func_t *func;
#if defined (READLINE_CALLBACKS)
  _rl_keyseq_cxt *cxt;
#endif

  if (META_CHAR (key) && _rl_convert_meta_chars_to_ascii)
    {
      if (map[ESC].type == ISKMAP)
	{
	  if (RL_ISSTATE (RL_STATE_MACRODEF))
	    _rl_add_macro_char (ESC);
	  RESIZE_KEYSEQ_BUFFER ();
	  rl_executing_keyseq[rl_key_sequence_length++] = ESC;
	  map = FUNCTION_TO_KEYMAP (map, ESC);
	  key = UNMETA (key);
	  return (_rl_dispatch (key, map));
	}
      else
	rl_ding ();
      return 0;
    }

  if (RL_ISSTATE (RL_STATE_MACRODEF))
    _rl_add_macro_char (key);

  r = 0;
  switch (map[key].type)
    {
    case ISFUNC:
      func = map[key].function;
      if (func)
	{
	  /* Special case rl_do_lowercase_version (). */
	  if (func == rl_do_lowercase_version)
	    /* Should we do anything special if key == ANYOTHERKEY? */
	    return (_rl_dispatch (_rl_to_lower ((unsigned char)key), map));

	  rl_executing_keymap = map;
	  rl_executing_key = key;

	  _rl_executing_func = func;

	  RESIZE_KEYSEQ_BUFFER();
	  rl_executing_keyseq[rl_key_sequence_length++] = key;
	  rl_executing_keyseq[rl_key_sequence_length] = '\0';

	  rl_dispatching = 1;
	  RL_SETSTATE(RL_STATE_DISPATCHING);
	  r = (*func) (rl_numeric_arg * rl_arg_sign, key);
	  RL_UNSETSTATE(RL_STATE_DISPATCHING);
	  rl_dispatching = 0;

	  /* If we have input pending, then the last command was a prefix
	     command.  Don't change the state of rl_last_func.  Otherwise,
	     remember the last command executed in this variable. */
#if defined (VI_MODE)
	  if (rl_pending_input == 0 && map[key].function != rl_digit_argument && map[key].function != rl_vi_arg_digit)
#else
	  if (rl_pending_input == 0 && map[key].function != rl_digit_argument)
#endif
	    rl_last_func = map[key].function;

	  RL_CHECK_SIGNALS ();
	}
      else if (map[ANYOTHERKEY].function)
	{
	  /* OK, there's no function bound in this map, but there is a
	     shadow function that was overridden when the current keymap
	     was created.  Return -2 to note  that. */
	  if (RL_ISSTATE (RL_STATE_MACROINPUT))
	    _rl_prev_macro_key ();
	  else
	    _rl_unget_char  (key);
	  if (rl_key_sequence_length > 0)
	    rl_executing_keyseq[--rl_key_sequence_length] = '\0';
	  return -2;
	}
      else if (got_subseq)
	{
	  /* Return -1 to note that we're in a subsequence, but  we don't
	     have a matching key, nor was one overridden.  This means
	     we need to back up the recursion chain and find the last
	     subsequence that is bound to a function. */
	  if (RL_ISSTATE (RL_STATE_MACROINPUT))
	    _rl_prev_macro_key ();
	  else
	    _rl_unget_char (key);
	  if (rl_key_sequence_length > 0)
	    rl_executing_keyseq[--rl_key_sequence_length] = '\0';
	  return -1;
	}
      else
	{
#if defined (READLINE_CALLBACKS)
	  RL_UNSETSTATE (RL_STATE_MULTIKEY);
	  _rl_keyseq_chain_dispose ();
#endif
	  _rl_abort_internal ();
	  return -1;
	}
      break;

    case ISKMAP:
      if (map[key].function != 0)
	{
#if defined (VI_MODE)
	  /* The only way this test will be true is if a subsequence has been
	     bound starting with ESC, generally the arrow keys.  What we do is
	     check whether there's input in the queue, which there generally
	     will be if an arrow key has been pressed, and, if there's not,
	     just dispatch to (what we assume is) rl_vi_movement_mode right
	     away.  This is essentially an input test with a zero timeout (by
	     default) or a timeout determined by the value of `keyseq-timeout' */
	  /* _rl_keyseq_timeout specified in milliseconds; _rl_input_queued
	     takes microseconds, so multiply by 1000 */
	  if (rl_editing_mode == vi_mode && key == ESC && map == vi_insertion_keymap &&
	      (RL_ISSTATE (RL_STATE_INPUTPENDING|RL_STATE_MACROINPUT) == 0) &&
              _rl_pushed_input_available () == 0 &&
	      _rl_input_queued ((_rl_keyseq_timeout > 0) ? _rl_keyseq_timeout*1000 : 0) == 0)
	    return (_rl_dispatch (ANYOTHERKEY, FUNCTION_TO_KEYMAP (map, key)));
	  /* This is a very specific test.  It can possibly be generalized in
	     the future, but for now it handles a specific case of ESC being
	     the last character in a keyboard macro. */
	  if (rl_editing_mode == vi_mode && key == ESC && map == vi_insertion_keymap &&
	      (RL_ISSTATE (RL_STATE_INPUTPENDING) == 0) &&
	      (RL_ISSTATE (RL_STATE_MACROINPUT) && _rl_peek_macro_key () == 0) &&
	      _rl_pushed_input_available () == 0 &&
	      _rl_input_queued ((_rl_keyseq_timeout > 0) ? _rl_keyseq_timeout*1000 : 0) == 0)
	    return (_rl_dispatch (ANYOTHERKEY, FUNCTION_TO_KEYMAP (map, key)));	      
#endif

	  RESIZE_KEYSEQ_BUFFER ();
	  rl_executing_keyseq[rl_key_sequence_length++] = key;
	  _rl_dispatching_keymap = FUNCTION_TO_KEYMAP (map, key);

	  /* Allocate new context here.  Use linked contexts (linked through
	     cxt->ocxt) to simulate recursion */
#if defined (READLINE_CALLBACKS)
#  if defined (VI_MODE)
	  /* If we're redoing a vi mode command and we know there is a shadowed
	     function corresponding to this key, just call it -- all the redoable
	     vi mode commands already have all the input they need, and rl_vi_redo
	     assumes that one call to rl_dispatch is sufficient to complete the
	     command. */
	  if (_rl_vi_redoing && RL_ISSTATE (RL_STATE_CALLBACK) &&
	      map[ANYOTHERKEY].function != 0)
	    return (_rl_subseq_result (-2, map, key, got_subseq));
#  endif
	  if (RL_ISSTATE (RL_STATE_CALLBACK))
	    {
	      /* Return 0 only the first time, to indicate success to
		 _rl_callback_read_char.  The rest of the time, we're called
		 from _rl_dispatch_callback, so we return -3 to indicate
		 special handling is necessary. */
	      r = RL_ISSTATE (RL_STATE_MULTIKEY) ? -3 : 0;
	      cxt = _rl_keyseq_cxt_alloc ();

	      if (got_subseq)
		cxt->flags |= KSEQ_SUBSEQ;
	      cxt->okey = key;
	      cxt->oldmap = map;
	      cxt->dmap = _rl_dispatching_keymap;
	      cxt->subseq_arg = got_subseq || cxt->dmap[ANYOTHERKEY].function;

	      RL_SETSTATE (RL_STATE_MULTIKEY);
	      _rl_kscxt = cxt;

	      return r;		/* don't indicate immediate success */
	    }
#endif

	  /* Tentative inter-character timeout for potential multi-key
	     sequences?  If no input within timeout, abort sequence and
	     act as if we got non-matching input. */
	  /* _rl_keyseq_timeout specified in milliseconds; _rl_input_queued
	     takes microseconds, so multiply by 1000 */
	  if (_rl_keyseq_timeout > 0 &&
	  	(RL_ISSTATE (RL_STATE_INPUTPENDING|RL_STATE_MACROINPUT) == 0) &&
	  	_rl_pushed_input_available () == 0 &&
		_rl_dispatching_keymap[ANYOTHERKEY].function &&
		_rl_input_queued (_rl_keyseq_timeout*1000) == 0)
	    {
	      if (rl_key_sequence_length > 0)
		rl_executing_keyseq[--rl_key_sequence_length] = '\0';
	      return (_rl_subseq_result (-2, map, key, got_subseq));
	    }

	  newkey = _rl_subseq_getchar (key);
	  if (newkey < 0)
	    {
	      _rl_abort_internal ();
	      return -1;
	    }

	  r = _rl_dispatch_subseq (newkey, _rl_dispatching_keymap, got_subseq || map[ANYOTHERKEY].function);
	  return _rl_subseq_result (r, map, key, got_subseq);
	}
      else
	{
	  _rl_abort_internal ();	/* XXX */
	  return -1;
	}
      break;

    case ISMACR:
      if (map[key].function != 0)
	{
	  rl_executing_keyseq[rl_key_sequence_length] = '\0';
	  macro = savestring ((char *)map[key].function);
	  _rl_with_macro_input (macro);
	  return 0;
	}
      break;
    }

#if defined (VI_MODE)
  if (rl_editing_mode == vi_mode && _rl_keymap == vi_movement_keymap &&
      key != ANYOTHERKEY &&
      _rl_dispatching_keymap == vi_movement_keymap &&
      _rl_vi_textmod_command (key))
    _rl_vi_set_last (key, rl_numeric_arg, rl_arg_sign);
#endif

  return (r);
}

static int
_rl_subseq_result (int r, Keymap map, int key, int got_subseq)
{
  Keymap m;
  int type, nt;
  rl_command_func_t *func, *nf;

  if (r == -2)
    /* We didn't match anything, and the keymap we're indexed into
       shadowed a function previously bound to that prefix.  Call
       the function.  The recursive call to _rl_dispatch_subseq has
       already taken care of pushing any necessary input back onto
       the input queue with _rl_unget_char. */
    {
      m = _rl_dispatching_keymap;
      type = m[ANYOTHERKEY].type;
      func = m[ANYOTHERKEY].function;
      if (type == ISFUNC && func == rl_do_lowercase_version)
	r = _rl_dispatch (_rl_to_lower ((unsigned char)key), map);
      else if (type == ISFUNC)
	{
	  /* If we shadowed a function, whatever it is, we somehow need a
	     keymap with map[key].func == shadowed-function.
	     Let's use this one.  Then we can dispatch using the original
	     key, since there are commands (e.g., in vi mode) for which it
	     matters. */
	  nt = m[key].type;
	  nf = m[key].function;

	  m[key].type = type;
	  m[key].function = func;
	  /* Don't change _rl_dispatching_keymap, set it here */
	  _rl_dispatching_keymap = map;		/* previous map */
	  r = _rl_dispatch_subseq (key, m, 0);
	  m[key].type = nt;
	  m[key].function = nf;
	}
      else
	/* We probably shadowed a keymap, so keep going. */
	r = _rl_dispatch (ANYOTHERKEY, m);
    }
  else if (r < 0 && map[ANYOTHERKEY].function)
    {
      /* We didn't match (r is probably -1), so return something to
	 tell the caller that it should try ANYOTHERKEY for an
	 overridden function. */
      if (RL_ISSTATE (RL_STATE_MACROINPUT))
	_rl_prev_macro_key ();
      else
	_rl_unget_char (key);
      if (rl_key_sequence_length > 0)
	rl_executing_keyseq[--rl_key_sequence_length] = '\0';
      _rl_dispatching_keymap = map;
      return -2;
    }
  else if (r < 0 && got_subseq)		/* XXX */
    {
      /* OK, back up the chain. */
      if (RL_ISSTATE (RL_STATE_MACROINPUT))
	_rl_prev_macro_key ();
      else
	_rl_unget_char (key);
      if (rl_key_sequence_length > 0)
	rl_executing_keyseq[--rl_key_sequence_length] = '\0';
      _rl_dispatching_keymap = map;
      return -1;
    }

  return r;
}

/* **************************************************************** */
/*								    */
/*			Initializations 			    */
/*								    */
/* **************************************************************** */

/* Initialize readline (and terminal if not already). */
int
rl_initialize (void)
{
  /* Initialize the timeout first to get the precise start time. */
  _rl_timeout_init ();

  /* If we have never been called before, initialize the
     terminal and data structures. */
  if (rl_initialized == 0)
    {
      RL_SETSTATE(RL_STATE_INITIALIZING);
      readline_initialize_everything ();
      RL_UNSETSTATE(RL_STATE_INITIALIZING);
      rl_initialized++;
      RL_SETSTATE(RL_STATE_INITIALIZED);
    }
  else
    _rl_reset_locale ();	/* check current locale and set locale variables */

  /* Initialize the current line information. */
  _rl_init_line_state ();

  /* We aren't done yet.  We haven't even gotten started yet! */
  rl_done = 0;
  RL_UNSETSTATE(RL_STATE_DONE|RL_STATE_TIMEOUT|RL_STATE_EOF);

  /* Tell the history routines what is going on. */
  _rl_start_using_history ();

  /* Make the display buffer match the state of the line. */
  rl_reset_line_state ();

  /* No such function typed yet. */
  rl_last_func = (rl_command_func_t *)NULL;

  /* Parsing of key-bindings begins in an enabled state. */
  _rl_parsing_conditionalized_out = 0;

#if defined (VI_MODE)
  if (rl_editing_mode == vi_mode)
    _rl_vi_initialize_line ();
#endif

  /* Each line starts in insert mode (the default). */
  _rl_set_insert_mode (RL_IM_DEFAULT, 1);

  return 0;
}

#if 0
#if defined (__EMX__)
static void
_emx_build_environ (void)
{
  TIB *tibp;
  PIB *pibp;
  char *t, **tp;
  int c;

  DosGetInfoBlocks (&tibp, &pibp);
  t = pibp->pib_pchenv;
  for (c = 1; *t; c++)
    t += strlen (t) + 1;
  tp = environ = (char **)xmalloc ((c + 1) * sizeof (char *));
  t = pibp->pib_pchenv;
  while (*t)
    {
      *tp++ = t;
      t += strlen (t) + 1;
    }
  *tp = 0;
}
#endif /* __EMX__ */
#endif

/* Initialize the entire state of the world. */
static void
readline_initialize_everything (void)
{
#if 0
#if defined (__EMX__)
  if (environ == 0)
    _emx_build_environ ();
#endif
#endif

#if 0
  /* Find out if we are running in Emacs -- UNUSED. */
  running_in_emacs = sh_get_env_value ("EMACS") != (char *)0;
#endif

  /* Set up input and output if they are not already set up. */
  if (!rl_instream)
    rl_instream = stdin;

  if (!rl_outstream)
    rl_outstream = stdout;

  /* Bind _rl_in_stream and _rl_out_stream immediately.  These values
     may change, but they may also be used before readline_internal ()
     is called. */
  _rl_in_stream = rl_instream;
  _rl_out_stream = rl_outstream;

  /* Allocate data structures. */
  if (rl_line_buffer == 0)
    rl_line_buffer = (char *)xmalloc (rl_line_buffer_len = DEFAULT_BUFFER_SIZE);

  /* Initialize the terminal interface. */
  if (rl_terminal_name == 0)
    rl_terminal_name = sh_get_env_value ("TERM");
  _rl_init_terminal_io (rl_terminal_name);

  /* Bind tty characters to readline functions. */
  readline_default_bindings ();

  /* Initialize the function names. */
  rl_initialize_funmap ();

  /* Decide whether we should automatically go into eight-bit mode. */
  _rl_init_eightbit ();
      
  /* Read in the init file. */
  rl_read_init_file ((char *)NULL);

  /* XXX */
  if (_rl_horizontal_scroll_mode && _rl_term_autowrap)
    {
      _rl_screenwidth--;
      _rl_screenchars -= _rl_screenheight;
    }

  /* Override the effect of any `set keymap' assignments in the
     inputrc file. */
  rl_set_keymap_from_edit_mode ();

  /* Try to bind a common arrow key prefix, if not already bound. */
  bind_arrow_keys ();

  /* Bind the bracketed paste prefix assuming that the user will enable
     it on terminals that support it. */
  bind_bracketed_paste_prefix ();

  /* If the completion parser's default word break characters haven't
     been set yet, then do so now. */
  if (rl_completer_word_break_characters == 0)
    rl_completer_word_break_characters = rl_basic_word_break_characters;

#if defined (COLOR_SUPPORT)
  if (_rl_colored_stats || _rl_colored_completion_prefix)
    _rl_parse_colors ();
#endif

  rl_executing_keyseq = malloc (_rl_executing_keyseq_size = 16);
  if (rl_executing_keyseq)
    rl_executing_keyseq[rl_key_sequence_length = 0] = '\0';
}

/* If this system allows us to look at the values of the regular
   input editing characters, then bind them to their readline
   equivalents, iff the characters are not bound to keymaps. */
static void
readline_default_bindings (void)
{
  if (_rl_bind_stty_chars)
    rl_tty_set_default_bindings (_rl_keymap);
}

/* Reset the default bindings for the terminal special characters we're
   interested in back to rl_insert and read the new ones. */
static void
reset_default_bindings (void)
{
  if (_rl_bind_stty_chars)
    {
      rl_tty_unset_default_bindings (_rl_keymap);
      rl_tty_set_default_bindings (_rl_keymap);
    }
}

/* Bind some common arrow key sequences in MAP. */
static void
bind_arrow_keys_internal (Keymap map)
{
  Keymap xkeymap;

  xkeymap = _rl_keymap;
  _rl_keymap = map;

#if defined (__MSDOS__)
  rl_bind_keyseq_if_unbound ("\033[0A", rl_get_previous_history);
  rl_bind_keyseq_if_unbound ("\033[0B", rl_backward_char);
  rl_bind_keyseq_if_unbound ("\033[0C", rl_forward_char);
  rl_bind_keyseq_if_unbound ("\033[0D", rl_get_next_history);
#endif

  rl_bind_keyseq_if_unbound ("\033[A", rl_get_previous_history);
  rl_bind_keyseq_if_unbound ("\033[B", rl_get_next_history);
  rl_bind_keyseq_if_unbound ("\033[C", rl_forward_char);
  rl_bind_keyseq_if_unbound ("\033[D", rl_backward_char);
  rl_bind_keyseq_if_unbound ("\033[H", rl_beg_of_line);
  rl_bind_keyseq_if_unbound ("\033[F", rl_end_of_line);

  rl_bind_keyseq_if_unbound ("\033OA", rl_get_previous_history);
  rl_bind_keyseq_if_unbound ("\033OB", rl_get_next_history);
  rl_bind_keyseq_if_unbound ("\033OC", rl_forward_char);
  rl_bind_keyseq_if_unbound ("\033OD", rl_backward_char);
  rl_bind_keyseq_if_unbound ("\033OH", rl_beg_of_line);
  rl_bind_keyseq_if_unbound ("\033OF", rl_end_of_line);

  /* Key bindings for control-arrow keys */
  rl_bind_keyseq_if_unbound ("\033[1;5C", rl_forward_word);
  rl_bind_keyseq_if_unbound ("\033[1;5D", rl_backward_word);
  rl_bind_keyseq_if_unbound ("\033[3;5~", rl_kill_word);

  /* Key bindings for alt-arrow keys */
  rl_bind_keyseq_if_unbound ("\033[1;3C", rl_forward_word);
  rl_bind_keyseq_if_unbound ("\033[1;3D", rl_backward_word);

#if defined (__MINGW32__)
  rl_bind_keyseq_if_unbound ("\340H", rl_get_previous_history);
  rl_bind_keyseq_if_unbound ("\340P", rl_get_next_history);
  rl_bind_keyseq_if_unbound ("\340M", rl_forward_char);
  rl_bind_keyseq_if_unbound ("\340K", rl_backward_char);
  rl_bind_keyseq_if_unbound ("\340G", rl_beg_of_line);
  rl_bind_keyseq_if_unbound ("\340O", rl_end_of_line);
  rl_bind_keyseq_if_unbound ("\340S", rl_delete);
  rl_bind_keyseq_if_unbound ("\340R", rl_overwrite_mode);

  /* These may or may not work because of the embedded NUL. */
  rl_bind_keyseq_if_unbound ("\\000H", rl_get_previous_history);
  rl_bind_keyseq_if_unbound ("\\000P", rl_get_next_history);
  rl_bind_keyseq_if_unbound ("\\000M", rl_forward_char);
  rl_bind_keyseq_if_unbound ("\\000K", rl_backward_char);
  rl_bind_keyseq_if_unbound ("\\000G", rl_beg_of_line);
  rl_bind_keyseq_if_unbound ("\\000O", rl_end_of_line);
  rl_bind_keyseq_if_unbound ("\\000S", rl_delete);
  rl_bind_keyseq_if_unbound ("\\000R", rl_overwrite_mode);
#endif

  _rl_keymap = xkeymap;
}

/* Try and bind the common arrow key prefixes after giving termcap and
   the inputrc file a chance to bind them and create `real' keymaps
   for the arrow key prefix. */
static void
bind_arrow_keys (void)
{
  bind_arrow_keys_internal (emacs_standard_keymap);

#if defined (VI_MODE)
  bind_arrow_keys_internal (vi_movement_keymap);
  /* Unbind vi_movement_keymap[ESC] to allow users to repeatedly hit ESC
     in vi command mode while still allowing the arrow keys to work. */
  if (vi_movement_keymap[ESC].type == ISKMAP)
    rl_bind_keyseq_in_map ("\033", (rl_command_func_t *)NULL, vi_movement_keymap);
  bind_arrow_keys_internal (vi_insertion_keymap);
#endif
}

static void
bind_bracketed_paste_prefix (void)
{
  Keymap xkeymap;

  xkeymap = _rl_keymap;

  _rl_keymap = emacs_standard_keymap;
  rl_bind_keyseq_if_unbound (BRACK_PASTE_PREF, rl_bracketed_paste_begin);

#if defined (VI_MODE)
  _rl_keymap = vi_insertion_keymap;
  rl_bind_keyseq_if_unbound (BRACK_PASTE_PREF, rl_bracketed_paste_begin);
  /* XXX - is there a reason to do this in the vi command keymap? */
#endif

  _rl_keymap = xkeymap;
}
  
/* **************************************************************** */
/*								    */
/*		Saving and Restoring Readline's state		    */
/*								    */
/* **************************************************************** */

int
rl_save_state (struct readline_state *sp)
{
  if (sp == 0)
    return -1;

  sp->point = rl_point;
  sp->end = rl_end;
  sp->mark = rl_mark;
  sp->buffer = rl_line_buffer;
  sp->buflen = rl_line_buffer_len;
  sp->ul = rl_undo_list;
  sp->prompt = rl_prompt;

  sp->rlstate = rl_readline_state;
  sp->done = rl_done;
  sp->kmap = _rl_keymap;

  sp->lastfunc = rl_last_func;
  sp->insmode = rl_insert_mode;
  sp->edmode = rl_editing_mode;
  sp->kseq = rl_executing_keyseq;
  sp->kseqlen = rl_key_sequence_length;
  sp->inf = rl_instream;
  sp->outf = rl_outstream;
  sp->pendingin = rl_pending_input;
  sp->macro = rl_executing_macro;

  sp->catchsigs = rl_catch_signals;
  sp->catchsigwinch = rl_catch_sigwinch;

  sp->entryfunc = rl_completion_entry_function;
  sp->menuentryfunc = rl_menu_completion_entry_function;
  sp->ignorefunc = rl_ignore_some_completions_function;
  sp->attemptfunc = rl_attempted_completion_function;
  sp->wordbreakchars = rl_completer_word_break_characters;

  return (0);
}

int
rl_restore_state (struct readline_state *sp)
{
  if (sp == 0)
    return -1;

  rl_point = sp->point;
  rl_end = sp->end;
  rl_mark = sp->mark;
  the_line = rl_line_buffer = sp->buffer;
  rl_line_buffer_len = sp->buflen;
  rl_undo_list = sp->ul;
  rl_prompt = sp->prompt;

  rl_readline_state = sp->rlstate;
  rl_done = sp->done;
  _rl_keymap = sp->kmap;

  rl_last_func = sp->lastfunc;
  rl_insert_mode = sp->insmode;
  rl_editing_mode = sp->edmode;
  rl_executing_keyseq = sp->kseq;
  rl_key_sequence_length = sp->kseqlen;
  rl_instream = sp->inf;
  rl_outstream = sp->outf;
  rl_pending_input = sp->pendingin;
  rl_executing_macro = sp->macro;

  rl_catch_signals = sp->catchsigs;
  rl_catch_sigwinch = sp->catchsigwinch;

  rl_completion_entry_function = sp->entryfunc;
  rl_menu_completion_entry_function = sp->menuentryfunc;
  rl_ignore_some_completions_function = sp->ignorefunc;
  rl_attempted_completion_function = sp->attemptfunc;
  rl_completer_word_break_characters = sp->wordbreakchars;

  rl_deactivate_mark ();

  return (0);
}

/* Functions to manage the string that is the current key sequence. */

void
_rl_init_executing_keyseq (void)
{
  rl_executing_keyseq[rl_key_sequence_length = 0] = '\0';
}

void
_rl_term_executing_keyseq (void)
{
  rl_executing_keyseq[rl_key_sequence_length] = '\0';
}

void
_rl_end_executing_keyseq (void)
{
  if (rl_key_sequence_length > 0)
    rl_executing_keyseq[--rl_key_sequence_length] = '\0';
}

void
_rl_add_executing_keyseq (int key)
{
  RESIZE_KEYSEQ_BUFFER ();
 rl_executing_keyseq[rl_key_sequence_length++] = key;
}

/* `delete' the last character added to the executing key sequence. Use this
   before calling rl_execute_next to avoid keys being added twice. */
void
_rl_del_executing_keyseq (void)
{
  if (rl_key_sequence_length > 0)
    rl_key_sequence_length--;
}
