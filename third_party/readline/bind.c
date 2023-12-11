/* bind.c -- key binding and startup file support for the readline library. */

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

#if defined (__TANDEM)
#  include <floss.h>
#endif

#if defined (HAVE_CONFIG_H)
#  include "config.h"
#endif

#include <stdio.h>
#include <sys/types.h>
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

#include <errno.h>

#if !defined (errno)
extern int errno;
#endif /* !errno */

#include "posixstat.h"

/* System-specific feature definitions and include files. */
#include "rldefs.h"

/* Some standard library routines. */
#include "readline.h"
#include "history.h"

#include "rlprivate.h"
#include "rlshell.h"
#include "xmalloc.h"

#if !defined (strchr) && !defined (__STDC__)
extern char *strchr (), *strrchr ();
#endif /* !strchr && !__STDC__ */

/* Variables exported by this file. */
Keymap rl_binding_keymap;

static int _rl_skip_to_delim (char *, int, int);

#if defined (USE_VARARGS) && defined (PREFER_STDARG)
static void _rl_init_file_error (const char *, ...)  __attribute__((__format__ (printf, 1, 2)));
#else
static void _rl_init_file_error ();
#endif

static rl_command_func_t *_rl_function_of_keyseq_internal (const char *, size_t, Keymap, int *);

static char *_rl_read_file (char *, size_t *);
static int _rl_read_init_file (const char *, int);
static int glean_key_from_name (char *);

static int find_boolean_var (const char *);
static int find_string_var (const char *);

static const char *boolean_varname (int);
static const char *string_varname (int);

static char *_rl_get_string_variable_value (const char *);
static int substring_member_of_array (const char *, const char * const *);

static int _rl_get_keymap_by_name (const char *);
static int _rl_get_keymap_by_map (Keymap);

static int currently_reading_init_file;

/* used only in this file */
static int _rl_prefer_visible_bell = 1;

#define OP_EQ	1
#define OP_NE	2
#define OP_GT	3
#define OP_GE	4
#define OP_LT	5
#define OP_LE	6

#define OPSTART(c)	((c) == '=' || (c) == '!' || (c) == '<' || (c) == '>')
#define CMPSTART(c)	((c) == '=' || (c) == '!')

/* **************************************************************** */
/*								    */
/*			Binding keys				    */
/*								    */
/* **************************************************************** */

/* rl_add_defun (char *name, rl_command_func_t *function, int key)
   Add NAME to the list of named functions.  Make FUNCTION be the function
   that gets called.  If KEY is not -1, then bind it. */
int
rl_add_defun (const char *name, rl_command_func_t *function, int key)
{
  if (key != -1)
    rl_bind_key (key, function);
  rl_add_funmap_entry (name, function);
  return 0;
}

/* Bind KEY to FUNCTION.  Returns non-zero if KEY is out of range. */
int
rl_bind_key (int key, rl_command_func_t *function)
{
  char keyseq[4];
  int l;

  if (key < 0 || key > largest_char)
    return (key);

  /* Want to make this a multi-character key sequence with an ESC prefix */
  if (META_CHAR (key) && _rl_convert_meta_chars_to_ascii)
    {
      if (_rl_keymap[ESC].type == ISKMAP)
	{
	  Keymap escmap;

	  escmap = FUNCTION_TO_KEYMAP (_rl_keymap, ESC);
	  key = UNMETA (key);
	  escmap[key].type = ISFUNC;
	  escmap[key].function = function;
	  return (0);
	}

      /* Otherwise, let's just let rl_generic_bind handle the key sequence.
	 We start it off with ESC here and let the code below add the rest
	 of the sequence. */
      keyseq[0] = ESC;
      l = 1;
      key = UNMETA(key);
      goto bind_keyseq;
    }

  /* If it's bound to a function or macro, just overwrite.  Otherwise we have
     to treat it as a key sequence so rl_generic_bind handles shadow keymaps
     for us.  If we are binding '\' or \C-@ (NUL) make sure to escape it so
     it makes it through the call to rl_translate_keyseq. */
  if (_rl_keymap[key].type != ISKMAP)
    {
      if (_rl_keymap[key].type == ISMACR)
	xfree ((char *)_rl_keymap[key].function);
      _rl_keymap[key].type = ISFUNC;
      _rl_keymap[key].function = function;
    }
  else
    {
      l = 0;
bind_keyseq:
      if (key == '\\')
	{
	  keyseq[l++] = '\\';
	  keyseq[l++] = '\\';
	}
      else if (key == '\0')	  
	{
	  keyseq[l++] = '\\';
	  keyseq[l++] = '0';
	}
      else
	keyseq[l++] = key;
      keyseq[l] = '\0';
      rl_bind_keyseq (keyseq, function);
    }
  rl_binding_keymap = _rl_keymap;
  return (0);
}

/* Bind KEY to FUNCTION in MAP.  Returns non-zero in case of invalid
   KEY. */
int
rl_bind_key_in_map (int key, rl_command_func_t *function, Keymap map)
{
  int result;
  Keymap oldmap;

  oldmap = _rl_keymap;
  _rl_keymap = map;
  result = rl_bind_key (key, function);
  _rl_keymap = oldmap;
  return (result);
}

/* Bind key sequence KEYSEQ to DEFAULT_FUNC if KEYSEQ is unbound.  Right
   now, this is always used to attempt to bind the arrow keys. */
int
rl_bind_key_if_unbound_in_map (int key, rl_command_func_t *default_func, Keymap kmap)
{
  char *keyseq;

  keyseq = rl_untranslate_keyseq ((unsigned char)key);
  return (rl_bind_keyseq_if_unbound_in_map (keyseq, default_func, kmap));
}

int
rl_bind_key_if_unbound (int key, rl_command_func_t *default_func)
{
  char *keyseq;

  keyseq = rl_untranslate_keyseq ((unsigned char)key);
  return (rl_bind_keyseq_if_unbound_in_map (keyseq, default_func, _rl_keymap));
}

/* Make KEY do nothing in the currently selected keymap.
   Returns non-zero in case of error.  This is not the same as self-insert;
   this makes it a dead key. */
int
rl_unbind_key (int key)
{
  return (rl_bind_key (key, (rl_command_func_t *)NULL));
}

/* Make KEY do nothing in MAP. Returns non-zero in case of error. */
int
rl_unbind_key_in_map (int key, Keymap map)
{
  return (rl_bind_key_in_map (key, (rl_command_func_t *)NULL, map));
}

/* Unbind all keys bound to FUNCTION in MAP. */
int
rl_unbind_function_in_map (rl_command_func_t *func, Keymap map)
{
  register int i, rval;

  for (i = rval = 0; i < KEYMAP_SIZE; i++)
    {
      if (map[i].type == ISFUNC && map[i].function == func)
	{
	  map[i].function = (rl_command_func_t *)NULL;
	  rval = 1;
	}
      else if (map[i].type == ISKMAP)		/* TAG:readline-8.1 */
	{
	  int r;
	  r = rl_unbind_function_in_map (func, FUNCTION_TO_KEYMAP (map, i));
	  if (r == 1)
	    rval = 1;
	}
    }
  return rval;
}

/* Unbind all keys bound to COMMAND, which is a bindable command name, in MAP */
int
rl_unbind_command_in_map (const char *command, Keymap map)
{
  rl_command_func_t *func;

  func = rl_named_function (command);
  if (func == 0)
    return 0;
  return (rl_unbind_function_in_map (func, map));
}

/* Bind the key sequence represented by the string KEYSEQ to
   FUNCTION, starting in the current keymap.  This makes new
   keymaps as necessary. */
int
rl_bind_keyseq (const char *keyseq, rl_command_func_t *function)
{
  return (rl_generic_bind (ISFUNC, keyseq, (char *)function, _rl_keymap));
}

/* Bind the key sequence represented by the string KEYSEQ to
   FUNCTION.  This makes new keymaps as necessary.  The initial
   place to do bindings is in MAP. */
int
rl_bind_keyseq_in_map (const char *keyseq, rl_command_func_t *function, Keymap map)
{
  return (rl_generic_bind (ISFUNC, keyseq, (char *)function, map));
}

/* Backwards compatibility; equivalent to rl_bind_keyseq_in_map() */
int
rl_set_key (const char *keyseq, rl_command_func_t *function, Keymap map)
{
  return (rl_generic_bind (ISFUNC, keyseq, (char *)function, map));
}

/* Bind key sequence KEYSEQ to DEFAULT_FUNC if KEYSEQ is unbound.  Right
   now, this is always used to attempt to bind the arrow keys, hence the
   check for rl_vi_movement_mode. */
int
rl_bind_keyseq_if_unbound_in_map (const char *keyseq, rl_command_func_t *default_func, Keymap kmap)
{
  rl_command_func_t *func;
  char *keys;
  int keys_len;

  if (keyseq)
    {
      /* Handle key sequences that require translations and `raw' ones that
	 don't. This might be a problem with backslashes. */
      keys = (char *)xmalloc (1 + (2 * strlen (keyseq)));
      if (rl_translate_keyseq (keyseq, keys, &keys_len))
	{
	  xfree (keys);
	  return -1;
	}
      func = rl_function_of_keyseq_len (keys, keys_len, kmap, (int *)NULL);
      xfree (keys);
#if defined (VI_MODE)
      if (!func || func == rl_do_lowercase_version || func == rl_vi_movement_mode)
#else
      if (!func || func == rl_do_lowercase_version)
#endif
	return (rl_bind_keyseq_in_map (keyseq, default_func, kmap));
      else
	return 1;
    }
  return 0;
}

int
rl_bind_keyseq_if_unbound (const char *keyseq, rl_command_func_t *default_func)
{
  return (rl_bind_keyseq_if_unbound_in_map (keyseq, default_func, _rl_keymap));
}

/* Bind the key sequence represented by the string KEYSEQ to
   the string of characters MACRO.  This makes new keymaps as
   necessary.  The initial place to do bindings is in MAP. */
int
rl_macro_bind (const char *keyseq, const char *macro, Keymap map)
{
  char *macro_keys;
  int macro_keys_len;

  macro_keys = (char *)xmalloc ((2 * strlen (macro)) + 1);

  if (rl_translate_keyseq (macro, macro_keys, &macro_keys_len))
    {
      xfree (macro_keys);
      return -1;
    }
  rl_generic_bind (ISMACR, keyseq, macro_keys, map);
  return 0;
}

/* Bind the key sequence represented by the string KEYSEQ to
   the arbitrary pointer DATA.  TYPE says what kind of data is
   pointed to by DATA, right now this can be a function (ISFUNC),
   a macro (ISMACR), or a keymap (ISKMAP).  This makes new keymaps
   as necessary.  The initial place to do bindings is in MAP. */
int
rl_generic_bind (int type, const char *keyseq, char *data, Keymap map)
{
  char *keys;
  int keys_len, prevkey, ic;
  register int i;
  KEYMAP_ENTRY k;
  Keymap prevmap;  

  k.function = 0;

  /* If no keys to bind to, exit right away. */
  if (keyseq == 0 || *keyseq == 0)
    {
      if (type == ISMACR)
	xfree (data);
      return -1;
    }

  keys = (char *)xmalloc (1 + (2 * strlen (keyseq)));

  /* Translate the ASCII representation of KEYSEQ into an array of
     characters.  Stuff the characters into KEYS, and the length of
     KEYS into KEYS_LEN. */
  if (rl_translate_keyseq (keyseq, keys, &keys_len))
    {
      xfree (keys);
      return -1;
    }

  prevmap = map;
  prevkey = keys[0];

  /* Bind keys, making new keymaps as necessary. */
  for (i = 0; i < keys_len; i++)
    {
      unsigned char uc = keys[i];

      if (i > 0)
	prevkey = ic;

      ic = uc;
      if (ic < 0 || ic >= KEYMAP_SIZE)
        {
          xfree (keys);
	  return -1;
        }

      /* We now rely on rl_translate_keyseq to do this conversion, so this
	 check is superfluous. */
#if 0
      if (META_CHAR (ic) && _rl_convert_meta_chars_to_ascii)
	{
	  ic = UNMETA (ic);
	  if (map[ESC].type == ISKMAP)
	    {
	      prevmap = map;
	      map = FUNCTION_TO_KEYMAP (map, ESC);
	    }
	}
#endif

      if ((i + 1) < keys_len)
	{
	  if (map[ic].type != ISKMAP)
	    {
	      /* We allow subsequences of keys.  If a keymap is being
		 created that will `shadow' an existing function or macro
		 key binding, we save that keybinding into the ANYOTHERKEY
		 index in the new map.  The dispatch code will look there
		 to find the function to execute if the subsequence is not
		 matched.  ANYOTHERKEY was chosen to be greater than
		 UCHAR_MAX. */
	      k = map[ic];

	      map[ic].type = ISKMAP;
	      map[ic].function = KEYMAP_TO_FUNCTION (rl_make_bare_keymap());
	    }
	  prevmap = map;
	  map = FUNCTION_TO_KEYMAP (map, ic);
	  /* The dispatch code will return this function if no matching
	     key sequence is found in the keymap.  This (with a little
	     help from the dispatch code in readline.c) allows `a' to be
	     mapped to something, `abc' to be mapped to something else,
	     and the function bound  to `a' to be executed when the user
	     types `abx', leaving `bx' in the input queue. */
	  if (k.function && ((k.type == ISFUNC && k.function != rl_do_lowercase_version) || k.type == ISMACR))
	    {
	      map[ANYOTHERKEY] = k;
	      k.function = 0;
	    }
	}
      else
	{
	  if (map[ic].type == ISKMAP)
	    {
	      prevmap = map;
	      map = FUNCTION_TO_KEYMAP (map, ic);
	      ic = ANYOTHERKEY;
	      /* If we're trying to override a keymap with a null function
		 (e.g., trying to unbind it), we can't use a null pointer
		 here because that's indistinguishable from having not been
		 overridden.  We use a special bindable function that does
		 nothing. */
	      if (type == ISFUNC && data == 0)
		data = (char *)_rl_null_function;
	    }
	  if (map[ic].type == ISMACR)
	    xfree ((char *)map[ic].function);

	  map[ic].function = KEYMAP_TO_FUNCTION (data);
	  map[ic].type = type;
	}

      rl_binding_keymap = map;

    }

  /* If we unbound a key (type == ISFUNC, data == 0), and the prev keymap
     points to the keymap where we unbound the key (sanity check), and the
     current binding keymap is empty (rl_empty_keymap() returns non-zero),
     and the binding keymap has ANYOTHERKEY set with type == ISFUNC
     (overridden function), delete the now-empty keymap, take the previously-
     overridden function and remove the override. */
  /* Right now, this only works one level back. */
  if (type == ISFUNC && data == 0 &&
      prevmap[prevkey].type == ISKMAP &&
      (FUNCTION_TO_KEYMAP(prevmap, prevkey) == rl_binding_keymap) &&
      rl_binding_keymap[ANYOTHERKEY].type == ISFUNC &&
      rl_empty_keymap (rl_binding_keymap))
    {
      prevmap[prevkey].type = rl_binding_keymap[ANYOTHERKEY].type;
      prevmap[prevkey].function = rl_binding_keymap[ANYOTHERKEY].function;
      rl_discard_keymap (rl_binding_keymap);
      rl_binding_keymap = prevmap;
    }

  xfree (keys);
  return 0;
}

/* Translate the ASCII representation of SEQ, stuffing the values into ARRAY,
   an array of characters.  LEN gets the final length of ARRAY.  Return
   non-zero if there was an error parsing SEQ. */
int
rl_translate_keyseq (const char *seq, char *array, int *len)
{
  register int i, l, temp;
  int has_control, has_meta;
  unsigned char c;

  has_control = 0;
  has_meta = 0;

  /* When there are incomplete prefixes \C- or \M- (has_control || has_meta)
     without base character at the end of SEQ, they are processed as the
     prefixes for '\0'.
  */
  for (i = l = 0; (c = seq[i]) || has_control || has_meta; i++)
    {
      /* Only backslashes followed by a non-null character are handled
	 specially.  Trailing backslash (backslash followed by '\0') is
	 processed as a normal character.
      */
      if (c == '\\' && seq[i + 1] != '\0')
	{
	  c = seq[++i];

	  /* Handle \C- and \M- prefixes. */
	  if (c == 'C' && seq[i + 1] == '-')
	    {
	      i++;
	      has_control = 1;
	      continue;
	    }
	  else if (c == 'M' && seq[i + 1] == '-')
	    {
	      i++;
	      has_meta = 1;
	      continue;
	    }	      

	  /* Translate other backslash-escaped characters.  These are the
	     same escape sequences that bash's `echo' and `printf' builtins
	     handle, with the addition of \d -> RUBOUT.  A backslash
	     preceding a character that is not special is stripped. */
	  switch (c)
	    {
	    case 'a':
	      c = '\007';
	      break;
	    case 'b':
	      c = '\b';
	      break;
	    case 'd':
	      c = RUBOUT;	/* readline-specific */
	      break;
	    case 'e':
	      c = ESC;
	      break;
	    case 'f':
	      c = '\f';
	      break;
	    case 'n':
	      c = NEWLINE;
	      break;
	    case 'r':
	      c = RETURN;
	      break;
	    case 't':
	      c = TAB;
	      break;
	    case 'v':
	      c = 0x0B;
	      break;
	    case '\\':
	      c = '\\';
	      break;
	    case '0': case '1': case '2': case '3':
	    case '4': case '5': case '6': case '7':
	      i++;
	      for (temp = 2, c -= '0'; ISOCTAL ((unsigned char)seq[i]) && temp--; i++)
	        c = (c * 8) + OCTVALUE (seq[i]);
	      i--;	/* auto-increment in for loop */
	      c &= largest_char;
	      break;
	    case 'x':
	      i++;
	      for (temp = 2, c = 0; ISXDIGIT ((unsigned char)seq[i]) && temp--; i++)
	        c = (c * 16) + HEXVALUE (seq[i]);
	      if (temp == 2)
	        c = 'x';
	      i--;	/* auto-increment in for loop */
	      c &= largest_char;
	      break;
	    default:	/* backslashes before non-special chars just add the char */
	      c &= largest_char;
	      break;	/* the backslash is stripped */
	    }
	}

      /* Process \C- and \M- flags */
      if (has_control)
	{
	  /* Special treatment for C-? */
	  c = (c == '?') ? RUBOUT : CTRL (_rl_to_upper (c));
	  has_control = 0;
	}
      if (has_meta)
	{
	  c = META (c);
	  has_meta = 0;
	}

      /* If convert-meta is turned on, convert a meta char to a key sequence  */
      if (META_CHAR (c) && _rl_convert_meta_chars_to_ascii)
	{
	  array[l++] = ESC;	/* ESC is meta-prefix */
	  array[l++] = UNMETA (c);
	}
      else
	array[l++] = (c);

      /* Null characters may be processed for incomplete prefixes at the end of
	 sequence */
      if (seq[i] == '\0')
	break;
    }

  *len = l;
  array[l] = '\0';
  return (0);
}

static int
_rl_isescape (int c)
{
  switch (c)
    {
    case '\007':
    case '\b':
    case '\f':
    case '\n':
    case '\r':
    case TAB:
    case 0x0b:  return (1);
    default: return (0);
    }
}

static int
_rl_escchar (int c)
{
  switch (c)
    {
    case '\007':  return ('a');
    case '\b':  return ('b');
    case '\f':  return ('f');
    case '\n':  return ('n');
    case '\r':  return ('r');
    case TAB:  return ('t');
    case 0x0b:  return ('v');
    default: return (c);
    }
}

char *
rl_untranslate_keyseq (int seq)
{
  static char kseq[16];
  int i, c;

  i = 0;
  c = seq;
  if (META_CHAR (c))
    {
      kseq[i++] = '\\';
      kseq[i++] = 'M';
      kseq[i++] = '-';
      c = UNMETA (c);
    }
  else if (c == ESC)
    {
      kseq[i++] = '\\';
      c = 'e';
    }
  else if (CTRL_CHAR (c))
    {
      kseq[i++] = '\\';
      kseq[i++] = 'C';
      kseq[i++] = '-';
      c = _rl_to_lower (UNCTRL (c));
    }
  else if (c == RUBOUT)
    {
      kseq[i++] = '\\';
      kseq[i++] = 'C';
      kseq[i++] = '-';
      c = '?';
    }

  if (c == ESC)
    {
      kseq[i++] = '\\';
      c = 'e';
    }
  else if (c == '\\' || c == '"')
    {
      kseq[i++] = '\\';
    }

  kseq[i++] = (unsigned char) c;
  kseq[i] = '\0';
  return kseq;
}

char *
_rl_untranslate_macro_value (char *seq, int use_escapes)
{
  char *ret, *r, *s;
  int c;

  r = ret = (char *)xmalloc (7 * strlen (seq) + 1);
  for (s = seq; *s; s++)
    {
      c = *s;
      if (META_CHAR (c))
	{
	  *r++ = '\\';
	  *r++ = 'M';
	  *r++ = '-';
	  c = UNMETA (c);
	}
      else if (c == ESC)
	{
	  *r++ = '\\';
	  c = 'e';
	}
      else if (CTRL_CHAR (c))
	{
	  *r++ = '\\';
	  if (use_escapes && _rl_isescape (c))
	    c = _rl_escchar (c);
	  else
	    {
	      *r++ = 'C';
	      *r++ = '-';
	      c = _rl_to_lower (UNCTRL (c));
	    }
	}
      else if (c == RUBOUT)
 	{
 	  *r++ = '\\';
 	  *r++ = 'C';
 	  *r++ = '-';
 	  c = '?';
 	}

      if (c == ESC)
	{
	  *r++ = '\\';
	  c = 'e';
	}
      else if (c == '\\' || c == '"')
	*r++ = '\\';

      *r++ = (unsigned char)c;
    }
  *r = '\0';
  return ret;
}

/* Return a pointer to the function that STRING represents.
   If STRING doesn't have a matching function, then a NULL pointer
   is returned. The string match is case-insensitive. */
rl_command_func_t *
rl_named_function (const char *string)
{
  register int i;

  rl_initialize_funmap ();

  for (i = 0; funmap[i]; i++)
    if (_rl_stricmp (funmap[i]->name, string) == 0)
      return (funmap[i]->function);
  return ((rl_command_func_t *)NULL);
}

/* Return the function (or macro) definition which would be invoked via
   KEYSEQ if executed in MAP.  If MAP is NULL, then the current keymap is
   used.  TYPE, if non-NULL, is a pointer to an int which will receive the
   type of the object pointed to.  One of ISFUNC (function), ISKMAP (keymap),
   or ISMACR (macro). */
static rl_command_func_t *
_rl_function_of_keyseq_internal (const char *keyseq, size_t len, Keymap map, int *type)
{
  register int i;

  if (map == 0)
    map = _rl_keymap;

  for (i = 0; keyseq && i < len; i++)
    {
      unsigned char ic = keyseq[i];

      if (META_CHAR (ic) && _rl_convert_meta_chars_to_ascii)
	{
	  if (map[ESC].type == ISKMAP)
	    {
	      map = FUNCTION_TO_KEYMAP (map, ESC);
	      ic = UNMETA (ic);
	    }
	  /* XXX - should we just return NULL here, since this obviously
	     doesn't match? */
	  else
	    {
	      if (type)
		*type = map[ESC].type;

	      return (map[ESC].function);
	    }
	}

      if (map[ic].type == ISKMAP)
	{
	  /* If this is the last key in the key sequence, return the
	     map. */
	  if (i + 1 == len)
	    {
	      if (type)
		*type = ISKMAP;

	      return (map[ic].function);
	    }
	  else
	    map = FUNCTION_TO_KEYMAP (map, ic);
	}
      /* If we're not at the end of the key sequence, and the current key
	 is bound to something other than a keymap, then the entire key
	 sequence is not bound. */
      else if (map[ic].type != ISKMAP && i+1 < len)
	return ((rl_command_func_t *)NULL);
      else	/* map[ic].type != ISKMAP && i+1 == len */
	{
	  if (type)
	    *type = map[ic].type;

	  return (map[ic].function);
	}
    }
  return ((rl_command_func_t *) NULL);
}

rl_command_func_t *
rl_function_of_keyseq (const char *keyseq, Keymap map, int *type)
{
  return _rl_function_of_keyseq_internal (keyseq, strlen (keyseq), map, type);
}

rl_command_func_t *
rl_function_of_keyseq_len (const char *keyseq, size_t len, Keymap map, int *type)
{
  return _rl_function_of_keyseq_internal (keyseq, len, map, type);
}

/* Assuming there is a numeric argument at the beginning of KEYSEQ (the
   caller is responsible for checking), return the index of the portion of
   the key sequence following the numeric argument. If there's no numeric
   argument (?), or if KEYSEQ consists solely of a numeric argument (?),
   return -1. */
int
rl_trim_arg_from_keyseq	(const char *keyseq, size_t len, Keymap map)
{
  register int i, j, parsing_digits;
  unsigned char ic;
  Keymap map0;

  if (map == 0)
    map = _rl_keymap;
  map0 = map;

  /* The digits following the initial one (e.g., the binding to digit-argument)
    or the optional `-' in a binding to digit-argument or universal-argument
    are not added to rl_executing_keyseq. This is basically everything read by
    rl_digit_loop. The parsing_digits logic is here in case they ever are. */
  for (i = j = parsing_digits = 0; keyseq && i < len; i++)
    {
      ic = keyseq[i];

      if (parsing_digits)
	{
	  if (_rl_digit_p (ic))
	    {
	      j = i + 1;
	      continue;
	    }
	  parsing_digits = 0;
	}

      if (map[ic].type == ISKMAP)
	{
	  if (i + 1 == len)
	    return -1;
	  map = FUNCTION_TO_KEYMAP (map, ic);
	  continue;
	}
      if (map[ic].type == ISFUNC)
	{
#if defined (VI_MODE)
	  if (map[ic].function != rl_digit_argument && map[ic].function != rl_universal_argument && map[ic].function != rl_vi_arg_digit)
#else
	  if (map[ic].function != rl_digit_argument && map[ic].function != rl_universal_argument)
#endif
	    return (j);

	  /* We don't bother with a keyseq that is only a numeric argument */
	  if (i + 1 == len)
	    return -1;

	  parsing_digits = 1;

	  /* This logic should be identical to rl_digit_loop */
	  /* We accept M-- as equivalent to M--1, C-u- as equivalent to C-u-1
	     but set parsing_digits to 2 to note that we saw `-' */
	  if (map[ic].function == rl_universal_argument && (i + 1 == '-'))
	    {
	      i++;
	      parsing_digits = 2;
	    }
	  if (map[ic].function == rl_digit_argument && ic == '-')
	    {
	      parsing_digits = 2;
	    }

	  map = map0;
	  j = i + 1;
	}
    }

  /* If we're still parsing digits by the time we get here, we don't allow a
     key sequence that consists solely of a numeric argument */
  return -1;
}
  
/* The last key bindings file read. */
static char *last_readline_init_file = (char *)NULL;

/* The file we're currently reading key bindings from. */
static const char *current_readline_init_file;
static int current_readline_init_include_level;
static int current_readline_init_lineno;

/* Read FILENAME into a locally-allocated buffer and return the buffer.
   The size of the buffer is returned in *SIZEP.  Returns NULL if any
   errors were encountered. */
static char *
_rl_read_file (char *filename, size_t *sizep)
{
  struct stat finfo;
  size_t file_size;
  char *buffer;
  int i, file;

  file = -1;
  if (((file = open (filename, O_RDONLY, 0666)) < 0) || (fstat (file, &finfo) < 0))
    {
      if (file >= 0)
	close (file);
      return ((char *)NULL);
    }

  file_size = (size_t)finfo.st_size;

  /* check for overflow on very large files */
  if (file_size != finfo.st_size || file_size + 1 < file_size)
    {
      if (file >= 0)
	close (file);
#if defined (EFBIG)
      errno = EFBIG;
#endif
      return ((char *)NULL);
    }

  /* Read the file into BUFFER. */
  buffer = (char *)xmalloc (file_size + 1);
  i = read (file, buffer, file_size);
  close (file);

  if (i < 0)
    {
      xfree (buffer);
      return ((char *)NULL);
    }

  RL_CHECK_SIGNALS ();

  buffer[i] = '\0';
  if (sizep)
    *sizep = i;

  return (buffer);
}

/* Re-read the current keybindings file. */
int
rl_re_read_init_file (int count, int ignore)
{
  int r;
  r = rl_read_init_file ((const char *)NULL);
  rl_set_keymap_from_edit_mode ();
  return r;
}

/* Do key bindings from a file.  If FILENAME is NULL it defaults
   to the first non-null filename from this list:
     1. the filename used for the previous call
     2. the value of the shell variable `INPUTRC'
     3. ~/.inputrc
     4. /etc/inputrc
   If the file existed and could be opened and read, 0 is returned,
   otherwise errno is returned. */
int
rl_read_init_file (const char *filename)
{
  /* Default the filename. */
  if (filename == 0)
    filename = last_readline_init_file;
  if (filename == 0)
    filename = sh_get_env_value ("INPUTRC");
  if (filename == 0 || *filename == 0)
    {
      filename = DEFAULT_INPUTRC;
      /* Try to read DEFAULT_INPUTRC; fall back to SYS_INPUTRC on failure */
      if (_rl_read_init_file (filename, 0) == 0)
	return 0;
      filename = SYS_INPUTRC;
    }

#if defined (__MSDOS__)
  if (_rl_read_init_file (filename, 0) == 0)
    return 0;
  filename = "~/_inputrc";
#endif
  return (_rl_read_init_file (filename, 0));
}

static int
_rl_read_init_file (const char *filename, int include_level)
{
  register int i;
  char *buffer, *openname, *line, *end;
  size_t file_size;

  current_readline_init_file = filename;
  current_readline_init_include_level = include_level;

  openname = tilde_expand (filename);
  buffer = _rl_read_file (openname, &file_size);
  xfree (openname);

  RL_CHECK_SIGNALS ();
  if (buffer == 0)
    return (errno);
  
  if (include_level == 0 && filename != last_readline_init_file)
    {
      FREE (last_readline_init_file);
      last_readline_init_file = savestring (filename);
    }

  currently_reading_init_file = 1;

  /* Loop over the lines in the file.  Lines that start with `#' are
     comments; all other lines are commands for readline initialization. */
  current_readline_init_lineno = 1;
  line = buffer;
  end = buffer + file_size;
  while (line < end)
    {
      /* Find the end of this line. */
      for (i = 0; line + i != end && line[i] != '\n'; i++);

#if defined (__CYGWIN__)
      /* ``Be liberal in what you accept.'' */
      if (line[i] == '\n' && line[i-1] == '\r')
	line[i - 1] = '\0';
#endif

      /* Mark end of line. */
      line[i] = '\0';

      /* Skip leading whitespace. */
      while (*line && whitespace (*line))
        {
	  line++;
	  i--;
        }

      /* If the line is not a comment, then parse it. */
      if (*line && *line != '#')
	rl_parse_and_bind (line);

      /* Move to the next line. */
      line += i + 1;
      current_readline_init_lineno++;
    }

  xfree (buffer);
  currently_reading_init_file = 0;
  return (0);
}

static void
#if defined (PREFER_STDARG)
_rl_init_file_error (const char *format, ...)
#else
_rl_init_file_error (va_alist)
     va_dcl
#endif
{
  va_list args;
#if defined (PREFER_VARARGS)
  char *format;
#endif

#if defined (PREFER_STDARG)
  va_start (args, format);
#else
  va_start (args);
  format = va_arg (args, char *);
#endif

  fprintf (stderr, "readline: ");
  if (currently_reading_init_file)
    fprintf (stderr, "%s: line %d: ", current_readline_init_file,
		     current_readline_init_lineno);

  vfprintf (stderr, format, args);
  fprintf (stderr, "\n");
  fflush (stderr);

  va_end (args);
}

/* **************************************************************** */
/*								    */
/*			Parser Helper Functions       		    */
/*								    */
/* **************************************************************** */

static int
parse_comparison_op (s, indp)
     const char *s;
     int *indp;
{
  int i, peekc, op;

  if (OPSTART (s[*indp]) == 0)
    return -1;
  i = *indp;
  peekc = s[i] ? s[i+1] : 0;
  op = -1;

  if (s[i] == '=')
    {
      op = OP_EQ;
      if (peekc == '=')
        i++;
      i++;
    }
  else if (s[i] == '!' && peekc == '=')
    {
      op = OP_NE;
      i += 2;
    }
  else if (s[i] == '<' && peekc == '=')
    {
      op = OP_LE;
      i += 2;
    }
  else if (s[i] == '>' && peekc == '=')
    {
      op = OP_GE;
      i += 2;
    }
  else if (s[i] == '<')
    {
      op = OP_LT;
      i += 1;
    }
  else if (s[i] == '>')
    {
      op = OP_GT;
      i += 1;
    }

  *indp = i;
  return op;        
}

/* **************************************************************** */
/*								    */
/*			Parser Directives       		    */
/*								    */
/* **************************************************************** */

typedef int _rl_parser_func_t (char *);

/* Things that mean `Control'. */
const char * const _rl_possible_control_prefixes[] = {
  "Control-", "C-", "CTRL-", (const char *)NULL
};

const char * const _rl_possible_meta_prefixes[] = {
  "Meta", "M-", (const char *)NULL
};

/* Forward declarations */
static int parser_if (char *);
static int parser_else (char *);
static int parser_endif (char *);
static int parser_include (char *);

/* Conditionals. */

/* Calling programs set this to have their argv[0]. */
const char *rl_readline_name = "other";

/* Stack of previous values of parsing_conditionalized_out. */
static unsigned char *if_stack = (unsigned char *)NULL;
static int if_stack_depth;
static int if_stack_size;

/* Push _rl_parsing_conditionalized_out, and set parser state based
   on ARGS. */
static int
parser_if (char *args)
{
  int i, llen, boolvar, strvar;

  boolvar = strvar = -1;

  /* Push parser state. */
  if (if_stack_depth + 1 >= if_stack_size)
    {
      if (!if_stack)
	if_stack = (unsigned char *)xmalloc (if_stack_size = 20);
      else
	if_stack = (unsigned char *)xrealloc (if_stack, if_stack_size += 20);
    }
  if_stack[if_stack_depth++] = _rl_parsing_conditionalized_out;

  /* If parsing is turned off, then nothing can turn it back on except
     for finding the matching endif.  In that case, return right now. */
  if (_rl_parsing_conditionalized_out)
    return 0;

  llen = strlen (args);

  /* Isolate first argument. */
  for (i = 0; args[i] && !whitespace (args[i]); i++);

  if (args[i])
    args[i++] = '\0';

  /* Handle "$if term=foo" and "$if mode=emacs" constructs.  If this
     isn't term=foo, or mode=emacs, then check to see if the first
     word in ARGS is the same as the value stored in rl_readline_name. */
  if (rl_terminal_name && _rl_strnicmp (args, "term=", 5) == 0)
    {
      char *tem, *tname;

      /* Terminals like "aaa-60" are equivalent to "aaa". */
      tname = savestring (rl_terminal_name);
      tem = strchr (tname, '-');
      if (tem)
	*tem = '\0';

      /* Test the `long' and `short' forms of the terminal name so that
	 if someone has a `sun-cmd' and does not want to have bindings
	 that will be executed if the terminal is a `sun', they can put
	 `$if term=sun-cmd' into their .inputrc. */
      _rl_parsing_conditionalized_out = _rl_stricmp (args + 5, tname) &&
					_rl_stricmp (args + 5, rl_terminal_name);
      xfree (tname);
    }
#if defined (VI_MODE)
  else if (_rl_strnicmp (args, "mode=", 5) == 0)
    {
      int mode;

      if (_rl_stricmp (args + 5, "emacs") == 0)
	mode = emacs_mode;
      else if (_rl_stricmp (args + 5, "vi") == 0)
	mode = vi_mode;
      else
	mode = no_mode;

      _rl_parsing_conditionalized_out = mode != rl_editing_mode;
    }
#endif /* VI_MODE */
  else if (_rl_strnicmp (args, "version", 7) == 0)
    {
      int rlversion, versionarg, op, previ, major, minor, opresult;

      _rl_parsing_conditionalized_out = 1;
      rlversion = RL_VERSION_MAJOR*10 + RL_VERSION_MINOR;
      /* if "version" is separated from the operator by whitespace, or the
         operand is separated from the operator by whitespace, restore it.
         We're more liberal with allowed whitespace for this variable. */
      if (i > 0 && i <= llen && args[i-1] == '\0')
        args[i-1] = ' ';
      args[llen] = '\0';		/* just in case */
      for (i = 7; whitespace (args[i]); i++)
	;
      if (OPSTART(args[i]) == 0)
	{
	  _rl_init_file_error ("comparison operator expected, found `%s'", args[i] ? args + i : "end-of-line");
	  return 0;
	}
      previ = i;
      op = parse_comparison_op (args, &i);
      if (op <= 0)
	{
	  _rl_init_file_error ("comparison operator expected, found `%s'", args+previ);
	  return 0;
	}
      for ( ; args[i] && whitespace (args[i]); i++)
	;
      if (args[i] == 0 || _rl_digit_p (args[i]) == 0)
	{
	  _rl_init_file_error ("numeric argument expected, found `%s'", args+i);
	  return 0;
	}
      major = minor = 0;
      previ = i;
      for ( ; args[i] && _rl_digit_p (args[i]); i++)
	major = major*10 + _rl_digit_value (args[i]);
      if (args[i] == '.')
	{
	  if (args[i + 1] && _rl_digit_p (args [i + 1]) == 0)
	    {
	      _rl_init_file_error ("numeric argument expected, found `%s'", args+previ);
	      return 0;
	    }
	  for (++i; args[i] && _rl_digit_p (args[i]); i++)
	    minor = minor*10 + _rl_digit_value (args[i]);
	}
      /* optional - check for trailing garbage on the line, allow whitespace
	 and a trailing comment */
      previ = i;
      for ( ; args[i] && whitespace (args[i]); i++)
	;
      if (args[i] && args[i] != '#')
	{
	  _rl_init_file_error ("trailing garbage on line: `%s'", args+previ);
	  return 0;
	}
      versionarg = major*10 + minor;

      switch (op)
	{
	case OP_EQ:
 	  opresult = rlversion == versionarg;
	  break;
	case OP_NE:
	  opresult = rlversion != versionarg;
	  break;
	case OP_GT:
	  opresult = rlversion > versionarg;
	  break;
	case OP_GE:
	  opresult = rlversion >= versionarg;
	  break;
	case OP_LT:
	  opresult = rlversion < versionarg;
	  break;
	case OP_LE:
	  opresult = rlversion <= versionarg;
	  break;
	}
      _rl_parsing_conditionalized_out = 1 - opresult;
    }
  /* Check to see if the first word in ARGS is the same as the
     value stored in rl_readline_name. */
  else if (_rl_stricmp (args, rl_readline_name) == 0)
    _rl_parsing_conditionalized_out = 0;
  else if ((boolvar = find_boolean_var (args)) >= 0 || (strvar = find_string_var (args)) >= 0)
    {
      int op, previ;
      size_t vlen;
      const char *vname;
      char *valuearg, *vval, prevc;

      _rl_parsing_conditionalized_out = 1;
      vname = (boolvar >= 0) ? boolean_varname (boolvar) : string_varname (strvar);
      vlen = strlen (vname);
      if (i > 0 && i <= llen && args[i-1] == '\0')
        args[i-1] = ' ';
      args[llen] = '\0';		/* just in case */
      for (i = vlen; whitespace (args[i]); i++)
	;
      if (CMPSTART(args[i]) == 0)
	{
	  _rl_init_file_error ("equality comparison operator expected, found `%s'", args[i] ? args + i : "end-of-line");
	  return 0;
	}
      previ = i;
      op = parse_comparison_op (args, &i);
      if (op != OP_EQ && op != OP_NE)
	{
	  _rl_init_file_error ("equality comparison operator expected, found `%s'", args+previ);
	  return 0;
	}
      for ( ; args[i] && whitespace (args[i]); i++)
	;
      if (args[i] == 0)
	{
	  _rl_init_file_error ("argument expected, found `%s'", args+i);
	  return 0;
	}
      previ = i;
      valuearg = args + i;
      for ( ; args[i] && whitespace (args[i]) == 0; i++)
	;
      prevc = args[i];
      args[i] = '\0';		/* null-terminate valuearg */
      vval = rl_variable_value (vname);
      if (op == OP_EQ)
        _rl_parsing_conditionalized_out = _rl_stricmp (vval, valuearg) != 0;
      else if (op == OP_NE)
        _rl_parsing_conditionalized_out = _rl_stricmp (vval, valuearg) == 0;
      args[i] = prevc;
    }
  else
    _rl_parsing_conditionalized_out = 1;
  return 0;
}

/* Invert the current parser state if there is anything on the stack. */
static int
parser_else (char *args)
{
  register int i;

  if (if_stack_depth == 0)
    {
      _rl_init_file_error ("$else found without matching $if");
      return 0;
    }

#if 0
  /* Check the previous (n - 1) levels of the stack to make sure that
     we haven't previously turned off parsing. */
  for (i = 0; i < if_stack_depth - 1; i++)
#else
  /* Check the previous (n) levels of the stack to make sure that
     we haven't previously turned off parsing. */
  for (i = 0; i < if_stack_depth; i++)
#endif
    if (if_stack[i] == 1)
      return 0;

  /* Invert the state of parsing if at top level. */
  _rl_parsing_conditionalized_out = !_rl_parsing_conditionalized_out;
  return 0;
}

/* Terminate a conditional, popping the value of
   _rl_parsing_conditionalized_out from the stack. */
static int
parser_endif (char *args)
{
  if (if_stack_depth)
    _rl_parsing_conditionalized_out = if_stack[--if_stack_depth];
  else
    _rl_init_file_error ("$endif without matching $if");
  return 0;
}

static int
parser_include (char *args)
{
  const char *old_init_file;
  char *e;
  int old_line_number, old_include_level, r;

  if (_rl_parsing_conditionalized_out)
    return (0);

  old_init_file = current_readline_init_file;
  old_line_number = current_readline_init_lineno;
  old_include_level = current_readline_init_include_level;

  e = strchr (args, '\n');
  if (e)
    *e = '\0';
  r = _rl_read_init_file ((const char *)args, old_include_level + 1);

  current_readline_init_file = old_init_file;
  current_readline_init_lineno = old_line_number;
  current_readline_init_include_level = old_include_level;

  return r;
}
  
/* Associate textual names with actual functions. */
static const struct {
  const char * const name;
  _rl_parser_func_t *function;
} parser_directives [] = {
  { "if", parser_if },
  { "endif", parser_endif },
  { "else", parser_else },
  { "include", parser_include },
  { (char *)0x0, (_rl_parser_func_t *)0x0 }
};

/* Handle a parser directive.  STATEMENT is the line of the directive
   without any leading `$'. */
static int
handle_parser_directive (char *statement)
{
  register int i;
  char *directive, *args;

  /* Isolate the actual directive. */

  /* Skip whitespace. */
  for (i = 0; whitespace (statement[i]); i++);

  directive = &statement[i];

  for (; statement[i] && !whitespace (statement[i]); i++);

  if (statement[i])
    statement[i++] = '\0';

  for (; statement[i] && whitespace (statement[i]); i++);

  args = &statement[i];

  /* Lookup the command, and act on it. */
  for (i = 0; parser_directives[i].name; i++)
    if (_rl_stricmp (directive, parser_directives[i].name) == 0)
      {
	(*parser_directives[i].function) (args);
	return (0);
      }

  /* display an error message about the unknown parser directive */
  _rl_init_file_error ("%s: unknown parser directive", directive);
  return (1);
}

/* Start at STRING[START] and look for DELIM.  Return I where STRING[I] ==
   DELIM or STRING[I] == 0.  DELIM is usually a double quote. */
static int
_rl_skip_to_delim (char *string, int start, int delim)
{
  int i, c, passc;

  for (i = start,passc = 0; c = string[i]; i++)
    {
      if (passc)
	{
	  passc = 0;
	  if (c == 0)
	    break;
	  continue;
	}

      if (c == '\\')
	{
	  passc = 1;
	  continue;
	}

      if (c == delim)
	break;
    }

  return i;
}

/* Read the binding command from STRING and perform it.
   A key binding command looks like: Keyname: function-name\0,
   a variable binding command looks like: set variable value.
   A new-style keybinding looks like "\C-x\C-x": exchange-point-and-mark. */
int
rl_parse_and_bind (char *string)
{
  char *funname, *kname;
  register int c, i;
  int key, equivalency, foundmod, foundsep;

  while (string && whitespace (*string))
    string++;

  if (string == 0 || *string == 0 || *string == '#')
    return 0;

  /* If this is a parser directive, act on it. */
  if (*string == '$')
    {
      handle_parser_directive (&string[1]);
      return 0;
    }

  /* If we aren't supposed to be parsing right now, then we're done. */
  if (_rl_parsing_conditionalized_out)
    return 0;

  i = 0;
  /* If this keyname is a complex key expression surrounded by quotes,
     advance to after the matching close quote.  This code allows the
     backslash to quote characters in the key expression. */
  if (*string == '"')
    {
      i = _rl_skip_to_delim (string, 1, '"');

      /* If we didn't find a closing quote, abort the line. */
      if (string[i] == '\0')
        {
          _rl_init_file_error ("%s: no closing `\"' in key binding", string);
          return 1;
        }
      else
        i++;	/* skip past closing double quote */
    }

  /* Advance to the colon (:) or whitespace which separates the two objects. */
  for (; (c = string[i]) && c != ':' && c != ' ' && c != '\t'; i++ );

  if (i == 0)
    {
      _rl_init_file_error ("`%s': invalid key binding: missing key sequence", string);
      return 1;
    }

  equivalency = (c == ':' && string[i + 1] == '=');

  foundsep = c != 0;

  /* Mark the end of the command (or keyname). */
  if (string[i])
    string[i++] = '\0';

  /* If doing assignment, skip the '=' sign as well. */
  if (equivalency)
    string[i++] = '\0';

  /* If this is a command to set a variable, then do that. */
  if (_rl_stricmp (string, "set") == 0)
    {
      char *var, *value, *e;
      int s;

      var = string + i;
      /* Make VAR point to start of variable name. */
      while (*var && whitespace (*var)) var++;

      /* Make VALUE point to start of value string. */
      value = var;
      while (*value && whitespace (*value) == 0) value++;
      if (*value)
	*value++ = '\0';
      while (*value && whitespace (*value)) value++;

      /* Strip trailing whitespace from values of boolean variables. */
      if (find_boolean_var (var) >= 0)
	{
	  /* just read a whitespace-delimited word or empty string */
	  for (e = value; *e && whitespace (*e) == 0; e++)
	    ;
	  if (e > value)
	    *e = '\0';		/* cut off everything trailing */
	}
      else if ((i = find_string_var (var)) >= 0)
	{
	  /* Allow quoted strings in variable values */
	  if (*value == '"')
	    {
	      i = _rl_skip_to_delim (value, 1, *value);
	      value[i] = '\0';
	      value++;	/* skip past the quote */
	    }
	  else
	    {
	      /* remove trailing whitespace */
	      e = value + strlen (value) - 1;
	      while (e >= value && whitespace (*e))
		e--;
	      e++;		/* skip back to whitespace or EOS */
	  
	      if (*e && e >= value)
		*e = '\0';
	    }
	}
      else
	{
	  /* avoid calling rl_variable_bind just to find this out */
	  _rl_init_file_error ("%s: unknown variable name", var);
	  return 1;
	}

      rl_variable_bind (var, value);
      return 0;
    }

  /* Skip any whitespace between keyname and funname. */
  for (; string[i] && whitespace (string[i]); i++);
  funname = &string[i];

  /* Now isolate funname.
     For straight function names just look for whitespace, since
     that will signify the end of the string.  But this could be a
     macro definition.  In that case, the string is quoted, so skip
     to the matching delimiter.  We allow the backslash to quote the
     delimiter characters in the macro body. */
  /* This code exists to allow whitespace in macro expansions, which
     would otherwise be gobbled up by the next `for' loop.*/
  /* XXX - it may be desirable to allow backslash quoting only if " is
     the quoted string delimiter, like the shell. */
  if (*funname == '\'' || *funname == '"')
    {
      i = _rl_skip_to_delim (string, i+1, *funname);
      if (string[i])
	i++;
      else
	{
	  _rl_init_file_error ("`%s': missing closing quote for macro", funname);
	  return 1;
	}
    }

  /* Advance to the end of the string.  */
  for (; string[i] && whitespace (string[i]) == 0; i++);

  /* No extra whitespace at the end of the string. */
  string[i] = '\0';

  /* Handle equivalency bindings here.  Make the left-hand side be exactly
     whatever the right-hand evaluates to, including keymaps. */
  if (equivalency)
    {
      return 0;
    }

  if (foundsep == 0)
    {
      _rl_init_file_error ("%s: no key sequence terminator", string);
      return 1;
    }

  /* If this is a new-style key-binding, then do the binding with
     rl_bind_keyseq ().  Otherwise, let the older code deal with it. */
  if (*string == '"')
    {
      char *seq;
      register int j, k, passc;

      seq = (char *)xmalloc (1 + strlen (string));
      for (j = 1, k = passc = 0; string[j]; j++)
	{
	  /* Allow backslash to quote characters, but leave them in place.
	     This allows a string to end with a backslash quoting another
	     backslash, or with a backslash quoting a double quote.  The
	     backslashes are left in place for rl_translate_keyseq (). */
	  if (passc || (string[j] == '\\'))
	    {
	      seq[k++] = string[j];
	      passc = !passc;
	      continue;
	    }

	  if (string[j] == '"')
	    break;

	  seq[k++] = string[j];
	}
      seq[k] = '\0';

      /* Binding macro? */
      if (*funname == '\'' || *funname == '"')
	{
	  j = strlen (funname);

	  /* Remove the delimiting quotes from each end of FUNNAME. */
	  if (j && funname[j - 1] == *funname)
	    funname[j - 1] = '\0';

	  rl_macro_bind (seq, &funname[1], _rl_keymap);
	}
      else
	rl_bind_keyseq (seq, rl_named_function (funname));

      xfree (seq);
      return 0;
    }

  /* Get the actual character we want to deal with. */
  kname = strrchr (string, '-');
  if (kname == 0)
    kname = string;
  else
    kname++;

  key = glean_key_from_name (kname);

  /* Add in control and meta bits. */
  foundmod = 0;
  if (substring_member_of_array (string, _rl_possible_control_prefixes))
    {
      key = CTRL (_rl_to_upper (key));
      foundmod = 1;
    }

  if (substring_member_of_array (string, _rl_possible_meta_prefixes))
    {
      key = META (key);
      foundmod = 1;
    }

  if (foundmod == 0 && kname != string)
    {
      _rl_init_file_error ("%s: unknown key modifier", string);
      return 1;
    }

  /* Temporary.  Handle old-style keyname with macro-binding. */
  if (*funname == '\'' || *funname == '"')
    {
      char useq[2];
      int fl = strlen (funname);

      useq[0] = key; useq[1] = '\0';
      if (fl && funname[fl - 1] == *funname)
	funname[fl - 1] = '\0';

      rl_macro_bind (useq, &funname[1], _rl_keymap);
    }
#if defined (PREFIX_META_HACK)
  /* Ugly, but working hack to keep prefix-meta around. */
  else if (_rl_stricmp (funname, "prefix-meta") == 0)
    {
      char seq[2];

      seq[0] = key;
      seq[1] = '\0';
      rl_generic_bind (ISKMAP, seq, (char *)emacs_meta_keymap, _rl_keymap);
    }
#endif /* PREFIX_META_HACK */
  else
    rl_bind_key (key, rl_named_function (funname));

  return 0;
}

/* Simple structure for boolean readline variables (i.e., those that can
   have one of two values; either "On" or 1 for truth, or "Off" or 0 for
   false. */

#define V_SPECIAL	0x1

static const struct {
  const char * const name;
  int *value;
  int flags;
} boolean_varlist [] = {
  { "bind-tty-special-chars",	&_rl_bind_stty_chars,		0 },
  { "blink-matching-paren",	&rl_blink_matching_paren,	V_SPECIAL },
  { "byte-oriented",		&rl_byte_oriented,		0 },
#if defined (COLOR_SUPPORT)
  { "colored-completion-prefix",&_rl_colored_completion_prefix,	0 },
  { "colored-stats",		&_rl_colored_stats,		0 },
#endif
  { "completion-ignore-case",	&_rl_completion_case_fold,	0 },
  { "completion-map-case",	&_rl_completion_case_map,	0 },
  { "convert-meta",		&_rl_convert_meta_chars_to_ascii, 0 },
  { "disable-completion",	&rl_inhibit_completion,		0 },
  { "echo-control-characters",	&_rl_echo_control_chars,	0 },
  { "enable-active-region",	&_rl_enable_active_region,	0 },
  { "enable-bracketed-paste",	&_rl_enable_bracketed_paste,	V_SPECIAL },
  { "enable-keypad",		&_rl_enable_keypad,		0 },
  { "enable-meta-key",		&_rl_enable_meta,		0 },
  { "expand-tilde",		&rl_complete_with_tilde_expansion, 0 },
  { "history-preserve-point",	&_rl_history_preserve_point,	0 },
  { "horizontal-scroll-mode",	&_rl_horizontal_scroll_mode,	0 },
  { "input-meta",		&_rl_meta_flag,			0 },
  { "mark-directories",		&_rl_complete_mark_directories,	0 },
  { "mark-modified-lines",	&_rl_mark_modified_lines,	0 },
  { "mark-symlinked-directories", &_rl_complete_mark_symlink_dirs, 0 },
  { "match-hidden-files",	&_rl_match_hidden_files,	0 },
  { "menu-complete-display-prefix", &_rl_menu_complete_prefix_first, 0 },
  { "meta-flag",		&_rl_meta_flag,			0 },
  { "output-meta",		&_rl_output_meta_chars,		0 },
  { "page-completions",		&_rl_page_completions,		0 },
  { "prefer-visible-bell",	&_rl_prefer_visible_bell,	V_SPECIAL },
  { "print-completions-horizontally", &_rl_print_completions_horizontally, 0 },
  { "revert-all-at-newline",	&_rl_revert_all_at_newline,	0 },
  { "show-all-if-ambiguous",	&_rl_complete_show_all,		0 },
  { "show-all-if-unmodified",	&_rl_complete_show_unmodified,	0 },
  { "show-mode-in-prompt",	&_rl_show_mode_in_prompt,	0 },
  { "skip-completed-text",	&_rl_skip_completed_text,	0 },
#if defined (VISIBLE_STATS)
  { "visible-stats",		&rl_visible_stats,		0 },
#endif /* VISIBLE_STATS */
  { (char *)NULL, (int *)NULL, 0 }
};

static int
find_boolean_var (const char *name)
{
  register int i;

  for (i = 0; boolean_varlist[i].name; i++)
    if (_rl_stricmp (name, boolean_varlist[i].name) == 0)
      return i;
  return -1;
}

static const char *
boolean_varname (int i)
{
  return ((i >= 0) ? boolean_varlist[i].name : (char *)NULL);
}  

/* Hooks for handling special boolean variables, where a
   function needs to be called or another variable needs
   to be changed when they're changed. */
static void
hack_special_boolean_var (int i)
{
  const char *name;

  name = boolean_varlist[i].name;

  if (_rl_stricmp (name, "blink-matching-paren") == 0)
    _rl_enable_paren_matching (rl_blink_matching_paren);
  else if (_rl_stricmp (name, "prefer-visible-bell") == 0)
    {
      if (_rl_prefer_visible_bell)
	_rl_bell_preference = VISIBLE_BELL;
      else
	_rl_bell_preference = AUDIBLE_BELL;
    }
  else if (_rl_stricmp (name, "show-mode-in-prompt") == 0)
    _rl_reset_prompt ();
  else if (_rl_stricmp (name, "enable-bracketed-paste") == 0)
    _rl_enable_active_region = _rl_enable_bracketed_paste;
}

typedef int _rl_sv_func_t (const char *);

/* These *must* correspond to the array indices for the appropriate
   string variable.  (Though they're not used right now.) */
#define V_BELLSTYLE	0
#define V_COMBEGIN	1
#define V_EDITMODE	2
#define V_ISRCHTERM	3
#define V_KEYMAP	4

#define	V_STRING	1
#define V_INT		2

/* Forward declarations */
static int sv_region_start_color (const char *);
static int sv_region_end_color (const char *);
static int sv_bell_style (const char *);
static int sv_combegin (const char *);
static int sv_dispprefix (const char *);
static int sv_compquery (const char *);
static int sv_compwidth (const char *);
static int sv_editmode (const char *);
static int sv_emacs_modestr (const char *);
static int sv_histsize (const char *);
static int sv_isrchterm (const char *);
static int sv_keymap (const char *);
static int sv_seqtimeout (const char *);
static int sv_viins_modestr (const char *);
static int sv_vicmd_modestr (const char *);

static const struct {
  const char * const name;
  int flags;
  _rl_sv_func_t *set_func;
} string_varlist[] = {
  { "active-region-end-color", V_STRING, sv_region_end_color },
  { "active-region-start-color", V_STRING, sv_region_start_color },
  { "bell-style",	V_STRING,	sv_bell_style },
  { "comment-begin",	V_STRING,	sv_combegin },
  { "completion-display-width", V_INT,	sv_compwidth },
  { "completion-prefix-display-length", V_INT,	sv_dispprefix },
  { "completion-query-items", V_INT,	sv_compquery },
  { "editing-mode",	V_STRING,	sv_editmode },
  { "emacs-mode-string", V_STRING,	sv_emacs_modestr },  
  { "history-size",	V_INT,		sv_histsize },
  { "isearch-terminators", V_STRING,	sv_isrchterm },
  { "keymap",		V_STRING,	sv_keymap },
  { "keyseq-timeout",	V_INT,		sv_seqtimeout },
  { "vi-cmd-mode-string", V_STRING,	sv_vicmd_modestr }, 
  { "vi-ins-mode-string", V_STRING,	sv_viins_modestr }, 
  { (char *)NULL,	0, (_rl_sv_func_t *)0 }
};

static int
find_string_var (const char *name)
{
  register int i;

  for (i = 0; string_varlist[i].name; i++)
    if (_rl_stricmp (name, string_varlist[i].name) == 0)
      return i;
  return -1;
}

static const char *
string_varname (int i)
{
  return ((i >= 0) ? string_varlist[i].name : (char *)NULL);
}  

/* A boolean value that can appear in a `set variable' command is true if
   the value is null or empty, `on' (case-insensitive), or "1".  All other
   values result in 0 (false). */
static int
bool_to_int (const char *value)
{
  return (value == 0 || *value == '\0' ||
		(_rl_stricmp (value, "on") == 0) ||
		(value[0] == '1' && value[1] == '\0'));
}

char *
rl_variable_value (const char *name)
{
  register int i;

  /* Check for simple variables first. */
  i = find_boolean_var (name);
  if (i >= 0)
    return (*boolean_varlist[i].value ? "on" : "off");

  i = find_string_var (name);
  if (i >= 0)
    return (_rl_get_string_variable_value (string_varlist[i].name));

  /* Unknown variable names return NULL. */
  return (char *)NULL;
}

int
rl_variable_bind (const char *name, const char *value)
{
  register int i;
  int	v;

  /* Check for simple variables first. */
  i = find_boolean_var (name);
  if (i >= 0)
    {
      *boolean_varlist[i].value = bool_to_int (value);
      if (boolean_varlist[i].flags & V_SPECIAL)
	hack_special_boolean_var (i);
      return 0;
    }

  i = find_string_var (name);

  /* For the time being, string names without a handler function are simply
     ignored. */
  if (i < 0 || string_varlist[i].set_func == 0)
    {
      if (i < 0)
	_rl_init_file_error ("%s: unknown variable name", name);
      return 0;
    }

  v = (*string_varlist[i].set_func) (value);
  if (v != 0)
    _rl_init_file_error ("%s: could not set value to `%s'", name, value);
  return v;
}

static int
sv_editmode (const char *value)
{
  if (_rl_strnicmp (value, "vi", 2) == 0)
    {
#if defined (VI_MODE)
      _rl_keymap = vi_insertion_keymap;
      rl_editing_mode = vi_mode;
#endif /* VI_MODE */
      return 0;
    }
  else if (_rl_strnicmp (value, "emacs", 5) == 0)
    {
      _rl_keymap = emacs_standard_keymap;
      rl_editing_mode = emacs_mode;
      return 0;
    }
  return 1;
}

static int
sv_combegin (const char *value)
{
  if (value && *value)
    {
      FREE (_rl_comment_begin);
      _rl_comment_begin = savestring (value);
      return 0;
    }
  return 1;
}

static int
sv_dispprefix (const char *value)
{
  int nval = 0;

  if (value && *value)
    {
      nval = atoi (value);
      if (nval < 0)
	nval = 0;
    }
  _rl_completion_prefix_display_length = nval;
  return 0;
}

static int
sv_compquery (const char *value)
{
  int nval = 100;

  if (value && *value)
    {
      nval = atoi (value);
      if (nval < 0)
	nval = 0;
    }
  rl_completion_query_items = nval;
  return 0;
}

static int
sv_compwidth (const char *value)
{
  int nval = -1;

  if (value && *value)
    nval = atoi (value);

  _rl_completion_columns = nval;
  return 0;
}

static int
sv_histsize (const char *value)
{
  int nval;

  nval = 500;
  if (value && *value)
    {
      nval = atoi (value);
      if (nval < 0)
	{
	  unstifle_history ();
	  return 0;
	}
    }
  stifle_history (nval);
  return 0;
}

static int
sv_keymap (const char *value)
{
  Keymap kmap;

  kmap = rl_get_keymap_by_name (value);
  if (kmap)
    {
      rl_set_keymap (kmap);
      return 0;
    }
  return 1;
}

static int
sv_seqtimeout (const char *value)
{
  int nval;

  nval = 0;
  if (value && *value)
    {
      nval = atoi (value);
      if (nval < 0)
	nval = 0;
    }
  _rl_keyseq_timeout = nval;
  return 0;
}

static int
sv_region_start_color (const char *value)
{
  return (_rl_reset_region_color (0, value));
}

static int
sv_region_end_color (const char *value)
{
  return (_rl_reset_region_color (1, value));
}

static int
sv_bell_style (const char *value)
{
  if (value == 0 || *value == '\0')
    _rl_bell_preference = AUDIBLE_BELL;
  else if (_rl_stricmp (value, "none") == 0 || _rl_stricmp (value, "off") == 0)
    _rl_bell_preference = NO_BELL;
  else if (_rl_stricmp (value, "audible") == 0 || _rl_stricmp (value, "on") == 0)
    _rl_bell_preference = AUDIBLE_BELL;
  else if (_rl_stricmp (value, "visible") == 0)
    _rl_bell_preference = VISIBLE_BELL;
  else
    return 1;
  return 0;
}

static int
sv_isrchterm (const char *value)
{
  int beg, end, delim;
  char *v;

  if (value == 0)
    return 1;

  /* Isolate the value and translate it into a character string. */
  v = savestring (value);
  FREE (_rl_isearch_terminators);
  if (v[0] == '"' || v[0] == '\'')
    {
      delim = v[0];
      for (beg = end = 1; v[end] && v[end] != delim; end++)
	;
    }
  else
    {
      for (beg = end = 0; v[end] && whitespace (v[end]) == 0; end++)
	;
    }

  v[end] = '\0';

  /* The value starts at v + beg.  Translate it into a character string. */
  _rl_isearch_terminators = (char *)xmalloc (2 * strlen (v) + 1);
  rl_translate_keyseq (v + beg, _rl_isearch_terminators, &end);
  _rl_isearch_terminators[end] = '\0';

  xfree (v);
  return 0;
}

extern char *_rl_emacs_mode_str;

static int
sv_emacs_modestr (const char *value)
{
  if (value && *value)
    {
      FREE (_rl_emacs_mode_str);
      _rl_emacs_mode_str = (char *)xmalloc (2 * strlen (value) + 1);
      rl_translate_keyseq (value, _rl_emacs_mode_str, &_rl_emacs_modestr_len);
      _rl_emacs_mode_str[_rl_emacs_modestr_len] = '\0';
      return 0;
    }
  else if (value)
    {
      FREE (_rl_emacs_mode_str);
      _rl_emacs_mode_str = (char *)xmalloc (1);
      _rl_emacs_mode_str[_rl_emacs_modestr_len = 0] = '\0';
      return 0;
    }
  else if (value == 0)
    {
      FREE (_rl_emacs_mode_str);
      _rl_emacs_mode_str = 0;	/* prompt_modestr does the right thing */
      _rl_emacs_modestr_len = 0;
      return 0;
    }
  return 1;
}

static int
sv_viins_modestr (const char *value)
{
  if (value && *value)
    {
      FREE (_rl_vi_ins_mode_str);
      _rl_vi_ins_mode_str = (char *)xmalloc (2 * strlen (value) + 1);
      rl_translate_keyseq (value, _rl_vi_ins_mode_str, &_rl_vi_ins_modestr_len);
      _rl_vi_ins_mode_str[_rl_vi_ins_modestr_len] = '\0';
      return 0;
    }
  else if (value)
    {
      FREE (_rl_vi_ins_mode_str);
      _rl_vi_ins_mode_str = (char *)xmalloc (1);
      _rl_vi_ins_mode_str[_rl_vi_ins_modestr_len = 0] = '\0';
      return 0;
    }
  else if (value == 0)
    {
      FREE (_rl_vi_ins_mode_str);
      _rl_vi_ins_mode_str = 0;	/* prompt_modestr does the right thing */
      _rl_vi_ins_modestr_len = 0;
      return 0;
    }
  return 1;
}

static int
sv_vicmd_modestr (const char *value)
{
  if (value && *value)
    {
      FREE (_rl_vi_cmd_mode_str);
      _rl_vi_cmd_mode_str = (char *)xmalloc (2 * strlen (value) + 1);
      rl_translate_keyseq (value, _rl_vi_cmd_mode_str, &_rl_vi_cmd_modestr_len);
      _rl_vi_cmd_mode_str[_rl_vi_cmd_modestr_len] = '\0';
      return 0;
    }
  else if (value)
    {
      FREE (_rl_vi_cmd_mode_str);
      _rl_vi_cmd_mode_str = (char *)xmalloc (1);
      _rl_vi_cmd_mode_str[_rl_vi_cmd_modestr_len = 0] = '\0';
      return 0;
    }
  else if (value == 0)
    {
      FREE (_rl_vi_cmd_mode_str);
      _rl_vi_cmd_mode_str = 0;	/* prompt_modestr does the right thing */
      _rl_vi_cmd_modestr_len = 0;
      return 0;
    }
  return 1;
}

/* Return the character which matches NAME.
   For example, `Space' returns ' '. */

typedef struct {
  const char * const name;
  int value;
} assoc_list;

static const assoc_list name_key_alist[] = {
  { "DEL", 0x7f },
  { "ESC", '\033' },
  { "Escape", '\033' },
  { "LFD", '\n' },
  { "Newline", '\n' },
  { "RET", '\r' },
  { "Return", '\r' },
  { "Rubout", 0x7f },
  { "SPC", ' ' },
  { "Space", ' ' },
  { "Tab", 0x09 },
  { (char *)0x0, 0 }
};

static int
glean_key_from_name (char *name)
{
  register int i;

  for (i = 0; name_key_alist[i].name; i++)
    if (_rl_stricmp (name, name_key_alist[i].name) == 0)
      return (name_key_alist[i].value);

  return (*(unsigned char *)name);	/* XXX was return (*name) */
}

/* Auxiliary functions to manage keymaps. */
struct name_and_keymap {
  char *name;
  Keymap map;
};

static struct name_and_keymap builtin_keymap_names[] = {
  { "emacs", emacs_standard_keymap },
  { "emacs-standard", emacs_standard_keymap },
  { "emacs-meta", emacs_meta_keymap },
  { "emacs-ctlx", emacs_ctlx_keymap },
#if defined (VI_MODE)
  { "vi", vi_movement_keymap },
  { "vi-move", vi_movement_keymap },
  { "vi-command", vi_movement_keymap },
  { "vi-insert", vi_insertion_keymap },
#endif /* VI_MODE */
  { (char *)0x0, (Keymap)0x0 }
};

/* -1 for NULL entry */
#define NUM_BUILTIN_KEYMAPS (sizeof (builtin_keymap_names) / sizeof (builtin_keymap_names[0]) - 1)

static struct name_and_keymap *keymap_names = builtin_keymap_names;

static int
_rl_get_keymap_by_name (const char *name)
{
  register int i;

  for (i = 0; keymap_names[i].name; i++)
    if (_rl_stricmp (name, keymap_names[i].name) == 0)
      return (i);
  return -1;
}

Keymap
rl_get_keymap_by_name (const char *name)
{
  int i;

  i = _rl_get_keymap_by_name (name);
  return ((i >= 0) ? keymap_names[i].map : (Keymap) NULL);
}

static int
_rl_get_keymap_by_map (Keymap map)
{
  register int i;

  for (i = 0; keymap_names[i].name; i++)
    if (map == keymap_names[i].map)
      return (i);
  return -1;
}

char *
rl_get_keymap_name (Keymap map)
{
  int i;

  i = _rl_get_keymap_by_map (map);
  return ((i >= 0) ? keymap_names[i].name : (char *)NULL);
}

int
rl_set_keymap_name (const char *name, Keymap map)
{
  int i, ni, mi;

  /* First check whether or not we're trying to rename a builtin keymap */
  mi = _rl_get_keymap_by_map (map);
  if (mi >= 0 && mi < NUM_BUILTIN_KEYMAPS)
    return -1;

  /* Then reject attempts to set one of the builtin names to a new map */
  ni = _rl_get_keymap_by_name (name);
  if (ni >= 0 && ni < NUM_BUILTIN_KEYMAPS)
    return -1;

  /* Renaming a keymap we already added */
  if (mi >= 0)	/* XXX - could be >= NUM_BUILTIN_KEYMAPS */
    {
      xfree (keymap_names[mi].name);
      keymap_names[mi].name = savestring (name);
      return mi;
    }

  /* Associating new keymap with existing name */
  if (ni >= 0)
    {
      keymap_names[ni].map = map;
      return ni;
    }

  for (i = 0; keymap_names[i].name; i++)
    ;

  if (keymap_names == builtin_keymap_names)
    {
      keymap_names = xmalloc ((i + 2) * sizeof (struct name_and_keymap));
      memcpy (keymap_names, builtin_keymap_names, i * sizeof (struct name_and_keymap));
    }
  else
    keymap_names = xrealloc (keymap_names, (i + 2) * sizeof (struct name_and_keymap));

  keymap_names[i].name = savestring (name);
  keymap_names[i].map = map;

  keymap_names[i+1].name = NULL;
  keymap_names[i+1].map = NULL;

  return i;
}

void
rl_set_keymap (Keymap map)
{
  if (map)
    _rl_keymap = map;
}

Keymap
rl_get_keymap (void)
{
  return (_rl_keymap);
}

void
rl_set_keymap_from_edit_mode (void)
{
  if (rl_editing_mode == emacs_mode)
    _rl_keymap = emacs_standard_keymap;
#if defined (VI_MODE)
  else if (rl_editing_mode == vi_mode)
    _rl_keymap = vi_insertion_keymap;
#endif /* VI_MODE */
}

char *
rl_get_keymap_name_from_edit_mode (void)
{
  if (rl_editing_mode == emacs_mode)
    return "emacs";
#if defined (VI_MODE)
  else if (rl_editing_mode == vi_mode)
    return "vi";
#endif /* VI_MODE */
  else
    return "none";
}

/* **************************************************************** */
/*								    */
/*		  Key Binding and Function Information		    */
/*								    */
/* **************************************************************** */

/* Each of the following functions produces information about the
   state of keybindings and functions known to Readline.  The info
   is always printed to rl_outstream, and in such a way that it can
   be read back in (i.e., passed to rl_parse_and_bind ()). */

/* Print the names of functions known to Readline. */
void
rl_list_funmap_names (void)
{
  register int i;
  const char **funmap_names;

  funmap_names = rl_funmap_names ();

  if (!funmap_names)
    return;

  for (i = 0; funmap_names[i]; i++)
    fprintf (rl_outstream, "%s\n", funmap_names[i]);

  xfree (funmap_names);
}

static char *
_rl_get_keyname (int key)
{
  char *keyname;
  int i, c;

  keyname = (char *)xmalloc (8);

  c = key;
  /* Since this is going to be used to write out keysequence-function
     pairs for possible inclusion in an inputrc file, we don't want to
     do any special meta processing on KEY. */

#if 1
  /* XXX - Experimental */
  /* We might want to do this, but the old version of the code did not. */

  /* If this is an escape character, we don't want to do any more processing.
     Just add the special ESC key sequence and return. */
  if (c == ESC)
    {
      keyname[0] = '\\';
      keyname[1] = 'e';
      keyname[2] = '\0';
      return keyname;
    }
#endif

  /* RUBOUT is translated directly into \C-? */
  if (key == RUBOUT)
    {
      keyname[0] = '\\';
      keyname[1] = 'C';
      keyname[2] = '-';
      keyname[3] = '?';
      keyname[4] = '\0';
      return keyname;
    }

  i = 0;
  /* Now add special prefixes needed for control characters.  This can
     potentially change C. */
  if (CTRL_CHAR (c))
    {
      keyname[i++] = '\\';
      keyname[i++] = 'C';
      keyname[i++] = '-';
      c = _rl_to_lower (UNCTRL (c));
    }

  /* XXX experimental code.  Turn the characters that are not ASCII or
     ISO Latin 1 (128 - 159) into octal escape sequences (\200 - \237).
     This changes C. */
  if (c >= 128 && c <= 159)
    {
      keyname[i++] = '\\';
      keyname[i++] = '2';
      c -= 128;
      keyname[i++] = (c / 8) + '0';
      c = (c % 8) + '0';
    }
  /* These characters are valid UTF-8; convert them into octal escape
     sequences as well. This changes C. */
  else if (c >= 160)
    {
      keyname[i++] = '\\';
      keyname[i++] = '0' + ((((unsigned char)c) >> 6) & 0x07);
      keyname[i++] = '0' + ((((unsigned char)c) >> 3) & 0x07);
      c = (c % 8) + '0';
    }

  /* Now, if the character needs to be quoted with a backslash, do that. */
  if (c == '\\' || c == '"')
    keyname[i++] = '\\';

  /* Now add the key, terminate the string, and return it. */
  keyname[i++] = (char) c;
  keyname[i] = '\0';

  return keyname;
}

/* Return a NULL terminated array of strings which represent the key
   sequences that are used to invoke FUNCTION in MAP. */
char **
rl_invoking_keyseqs_in_map (rl_command_func_t *function, Keymap map)
{
  register int key;
  char **result;
  int result_index, result_size;

  result = (char **)NULL;
  result_index = result_size = 0;

  for (key = 0; key < KEYMAP_SIZE; key++)
    {
      switch (map[key].type)
	{
	case ISMACR:
	  /* Macros match, if, and only if, the pointers are identical.
	     Thus, they are treated exactly like functions in here. */
	case ISFUNC:
	  /* If the function in the keymap is the one we are looking for,
	     then add the current KEY to the list of invoking keys. */
	  if (map[key].function == function)
	    {
	      char *keyname;

	      keyname = _rl_get_keyname (key);

	      if (result_index + 2 > result_size)
	        {
	          result_size += 10;
		  result = (char **)xrealloc (result, result_size * sizeof (char *));
	        }

	      result[result_index++] = keyname;
	      result[result_index] = (char *)NULL;
	    }
	  break;

	case ISKMAP:
	  {
	    char **seqs;
	    register int i;

	    /* Find the list of keyseqs in this map which have FUNCTION as
	       their target.  Add the key sequences found to RESULT. */
	    if (map[key].function)
	      seqs =
	        rl_invoking_keyseqs_in_map (function, FUNCTION_TO_KEYMAP (map, key));
	    else
	      break;

	    if (seqs == 0)
	      break;

	    for (i = 0; seqs[i]; i++)
	      {
		char *keyname = (char *)xmalloc (6 + strlen (seqs[i]));

		if (key == ESC)
		  {
		    /* If ESC is the meta prefix and we're converting chars
		       with the eighth bit set to ESC-prefixed sequences, then
		       we can use \M-.  Otherwise we need to use the sequence
		       for ESC. */
		    if (_rl_convert_meta_chars_to_ascii && map[ESC].type == ISKMAP)
		      sprintf (keyname, "\\M-");
		    else
		      sprintf (keyname, "\\e");
		  }
		else
		  {
		    int c = key, l = 0;
		    if (CTRL_CHAR (c) || c == RUBOUT)
		      {
			keyname[l++] = '\\';
			keyname[l++] = 'C';
			keyname[l++] = '-';
			c = (c == RUBOUT) ? '?' : _rl_to_lower (UNCTRL (c));
		      }

		    if (c == '\\' || c == '"')
		      keyname[l++] = '\\';

		    keyname[l++] = (char) c;
		    keyname[l++] = '\0';
		  }
		
		strcat (keyname, seqs[i]);
		xfree (seqs[i]);

		if (result_index + 2 > result_size)
		  {
		    result_size += 10;
		    result = (char **)xrealloc (result, result_size * sizeof (char *));
		  }

		result[result_index++] = keyname;
		result[result_index] = (char *)NULL;
	      }

	    xfree (seqs);
	  }
	  break;
	}
    }
  return (result);
}

/* Return a NULL terminated array of strings which represent the key
   sequences that can be used to invoke FUNCTION using the current keymap. */
char **
rl_invoking_keyseqs (rl_command_func_t *function)
{
  return (rl_invoking_keyseqs_in_map (function, _rl_keymap));
}

/* Print all of the functions and their bindings to rl_outstream.  If
   PRINT_READABLY is non-zero, then print the output in such a way
   that it can be read back in. */
void
rl_function_dumper (int print_readably)
{
  register int i;
  const char **names;
  const char *name;

  names = rl_funmap_names ();

  fprintf (rl_outstream, "\n");

  for (i = 0; name = names[i]; i++)
    {
      rl_command_func_t *function;
      char **invokers;

      function = rl_named_function (name);
      invokers = rl_invoking_keyseqs_in_map (function, _rl_keymap);

      if (print_readably)
	{
	  if (!invokers)
	    fprintf (rl_outstream, "# %s (not bound)\n", name);
	  else
	    {
	      register int j;

	      for (j = 0; invokers[j]; j++)
		{
		  fprintf (rl_outstream, "\"%s\": %s\n",
			   invokers[j], name);
		  xfree (invokers[j]);
		}

	      xfree (invokers);
	    }
	}
      else
	{
	  if (!invokers)
	    fprintf (rl_outstream, "%s is not bound to any keys\n",
		     name);
	  else
	    {
	      register int j;

	      fprintf (rl_outstream, "%s can be found on ", name);

	      for (j = 0; invokers[j] && j < 5; j++)
		{
		  fprintf (rl_outstream, "\"%s\"%s", invokers[j],
			   invokers[j + 1] ? ", " : ".\n");
		}

	      if (j == 5 && invokers[j])
		fprintf (rl_outstream, "...\n");

	      for (j = 0; invokers[j]; j++)
		xfree (invokers[j]);

	      xfree (invokers);
	    }
	}
    }

  xfree (names);
}

/* Print all of the current functions and their bindings to
   rl_outstream.  If an explicit argument is given, then print
   the output in such a way that it can be read back in. */
int
rl_dump_functions (int count, int key)
{
  if (rl_dispatching)
    fprintf (rl_outstream, "\r\n");
  rl_function_dumper (rl_explicit_arg);
  rl_on_new_line ();
  return (0);
}

static void
_rl_macro_dumper_internal (int print_readably, Keymap map, char *prefix)
{
  register int key;
  char *keyname, *out;
  int prefix_len;

  for (key = 0; key < KEYMAP_SIZE; key++)
    {
      switch (map[key].type)
	{
	case ISMACR:
	  keyname = _rl_get_keyname (key);
	  out = _rl_untranslate_macro_value ((char *)map[key].function, 0);

	  if (print_readably)
	    fprintf (rl_outstream, "\"%s%s\": \"%s\"\n", prefix ? prefix : "",
						         keyname,
						         out ? out : "");
	  else
	    fprintf (rl_outstream, "%s%s outputs %s\n", prefix ? prefix : "",
							keyname,
							out ? out : "");
	  xfree (keyname);
	  xfree (out);
	  break;
	case ISFUNC:
	  break;
	case ISKMAP:
	  prefix_len = prefix ? strlen (prefix) : 0;
	  if (key == ESC)
	    {
	      keyname = (char *)xmalloc (3 + prefix_len);
	      if (prefix)
		strcpy (keyname, prefix);
	      keyname[prefix_len] = '\\';
	      keyname[prefix_len + 1] = 'e';
	      keyname[prefix_len + 2] = '\0';
	    }
	  else
	    {
	      keyname = _rl_get_keyname (key);
	      if (prefix)
		{
		  out = (char *)xmalloc (strlen (keyname) + prefix_len + 1);
		  strcpy (out, prefix);
		  strcpy (out + prefix_len, keyname);
		  xfree (keyname);
		  keyname = out;
		}
	    }

	  _rl_macro_dumper_internal (print_readably, FUNCTION_TO_KEYMAP (map, key), keyname);
	  xfree (keyname);
	  break;
	}
    }
}

void
rl_macro_dumper (int print_readably)
{
  _rl_macro_dumper_internal (print_readably, _rl_keymap, (char *)NULL);
}

int
rl_dump_macros (int count, int key)
{
  if (rl_dispatching)
    fprintf (rl_outstream, "\r\n");
  rl_macro_dumper (rl_explicit_arg);
  rl_on_new_line ();
  return (0);
}

static char *
_rl_get_string_variable_value (const char *name)
{
  static char numbuf[32];
  char *ret;

  if (_rl_stricmp (name, "bell-style") == 0)
    {
      switch (_rl_bell_preference)
	{
	  case NO_BELL:
	    return "none";
	  case VISIBLE_BELL:
	    return "visible";
	  case AUDIBLE_BELL:
	  default:
	    return "audible";
	}
    }
  else if (_rl_stricmp (name, "comment-begin") == 0)
    return (_rl_comment_begin ? _rl_comment_begin : RL_COMMENT_BEGIN_DEFAULT);
  else if (_rl_stricmp (name, "completion-display-width") == 0)
    {
      sprintf (numbuf, "%d", _rl_completion_columns);
      return (numbuf);
    }
  else if (_rl_stricmp (name, "completion-prefix-display-length") == 0)
    {
      sprintf (numbuf, "%d", _rl_completion_prefix_display_length);
      return (numbuf);
    }
  else if (_rl_stricmp (name, "completion-query-items") == 0)
    {
      sprintf (numbuf, "%d", rl_completion_query_items);
      return (numbuf);
    }
  else if (_rl_stricmp (name, "editing-mode") == 0)
    return (rl_get_keymap_name_from_edit_mode ());
  else if (_rl_stricmp (name, "history-size") == 0)
    {
      sprintf (numbuf, "%d", history_is_stifled() ? history_max_entries : 0);
      return (numbuf);
    }
  else if (_rl_stricmp (name, "isearch-terminators") == 0)
    {
      if (_rl_isearch_terminators == 0)
	return 0;
      ret = _rl_untranslate_macro_value (_rl_isearch_terminators, 0);
      if (ret)
	{
	  strncpy (numbuf, ret, sizeof (numbuf) - 1);
	  xfree (ret);
	  numbuf[sizeof(numbuf) - 1] = '\0';
	}
      else
	numbuf[0] = '\0';
      return numbuf;
    }
  else if (_rl_stricmp (name, "keymap") == 0)
    {
      ret = rl_get_keymap_name (_rl_keymap);
      if (ret == 0)
	ret = rl_get_keymap_name_from_edit_mode ();
      return (ret ? ret : "none");
    }
  else if (_rl_stricmp (name, "keyseq-timeout") == 0)
    {
      sprintf (numbuf, "%d", _rl_keyseq_timeout);    
      return (numbuf);
    }
  else if (_rl_stricmp (name, "emacs-mode-string") == 0)
    return (_rl_emacs_mode_str ? _rl_emacs_mode_str : RL_EMACS_MODESTR_DEFAULT);
  else if (_rl_stricmp (name, "vi-cmd-mode-string") == 0)
    return (_rl_vi_cmd_mode_str ? _rl_vi_cmd_mode_str : RL_VI_CMD_MODESTR_DEFAULT);
  else if (_rl_stricmp (name, "vi-ins-mode-string") == 0)
    return (_rl_vi_ins_mode_str ? _rl_vi_ins_mode_str : RL_VI_INS_MODESTR_DEFAULT);
  else
    return (0);
}

void
rl_variable_dumper (int print_readably)
{
  int i;
  char *v;

  for (i = 0; boolean_varlist[i].name; i++)
    {
      if (print_readably)
        fprintf (rl_outstream, "set %s %s\n", boolean_varlist[i].name,
			       *boolean_varlist[i].value ? "on" : "off");
      else
        fprintf (rl_outstream, "%s is set to `%s'\n", boolean_varlist[i].name,
			       *boolean_varlist[i].value ? "on" : "off");
    }

  for (i = 0; string_varlist[i].name; i++)
    {
      v = _rl_get_string_variable_value (string_varlist[i].name);
      if (v == 0)	/* _rl_isearch_terminators can be NULL */
	continue;
      if (print_readably)
        fprintf (rl_outstream, "set %s %s\n", string_varlist[i].name, v);
      else
        fprintf (rl_outstream, "%s is set to `%s'\n", string_varlist[i].name, v);
    }
}

/* Print all of the current variables and their values to
   rl_outstream.  If an explicit argument is given, then print
   the output in such a way that it can be read back in. */
int
rl_dump_variables (int count, int key)
{
  if (rl_dispatching)
    fprintf (rl_outstream, "\r\n");
  rl_variable_dumper (rl_explicit_arg);
  rl_on_new_line ();
  return (0);
}

/* Return non-zero if any members of ARRAY are a substring in STRING. */
static int
substring_member_of_array (const char *string, const char * const *array)
{
  while (*array)
    {
      if (_rl_strindex (string, *array))
	return (1);
      array++;
    }
  return (0);
}
