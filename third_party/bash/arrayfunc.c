/* arrayfunc.c -- High-level array functions used by other parts of the shell. */

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

#include "config.h"

#if defined (ARRAY_VARS)

#if defined (HAVE_UNISTD_H)
#  include <unistd.h>
#endif
#include <stdio.h>

#include "bashintl.h"

#include "shell.h"
#include "execute_cmd.h"
#include "pathexp.h"

#include "shmbutil.h"
#if defined (HAVE_MBSTR_H) && defined (HAVE_MBSCHR)
#  include <mbstr.h>		/* mbschr */
#endif

#include "common.h"

#ifndef LBRACK
#  define LBRACK '['
#  define RBRACK ']'
#endif

/* This variable means to not expand associative array subscripts more than
   once, when performing variable expansion. */
int assoc_expand_once = 0;

/* Ditto for indexed array subscripts -- currently unused */
int array_expand_once = 0;

static SHELL_VAR *bind_array_var_internal PARAMS((SHELL_VAR *, arrayind_t, char *, char *, int));
static SHELL_VAR *assign_array_element_internal PARAMS((SHELL_VAR *, char *, char *, char *, int, char *, int, array_eltstate_t *));

static void assign_assoc_from_kvlist PARAMS((SHELL_VAR *, WORD_LIST *, HASH_TABLE *, int));

static char *quote_assign PARAMS((const char *));
static void quote_array_assignment_chars PARAMS((WORD_LIST *));
static char *quote_compound_array_word PARAMS((char *, int));
static char *array_value_internal PARAMS((const char *, int, int, array_eltstate_t *));

/* Standard error message to use when encountering an invalid array subscript */
const char * const bash_badsub_errmsg = N_("bad array subscript");

/* **************************************************************** */
/*								    */
/*  Functions to manipulate array variables and perform assignments */
/*								    */
/* **************************************************************** */

/* Convert a shell variable to an array variable.  The original value is
   saved as array[0]. */
SHELL_VAR *
convert_var_to_array (var)
     SHELL_VAR *var;
{
  char *oldval;
  ARRAY *array;

  oldval = value_cell (var);
  array = array_create ();
  if (oldval)
    array_insert (array, 0, oldval);

  FREE (value_cell (var));
  var_setarray (var, array);

  /* these aren't valid anymore */
  var->dynamic_value = (sh_var_value_func_t *)NULL;
  var->assign_func = (sh_var_assign_func_t *)NULL;

  INVALIDATE_EXPORTSTR (var);
  if (exported_p (var))
    array_needs_making++;

  VSETATTR (var, att_array);
  if (oldval)
    VUNSETATTR (var, att_invisible);

  /* Make sure it's not marked as an associative array any more */
  VUNSETATTR (var, att_assoc);

  /* Since namerefs can't be array variables, turn off nameref attribute */
  VUNSETATTR (var, att_nameref);

  return var;
}

/* Convert a shell variable to an array variable.  The original value is
   saved as array[0]. */
SHELL_VAR *
convert_var_to_assoc (var)
     SHELL_VAR *var;
{
  char *oldval;
  HASH_TABLE *hash;

  oldval = value_cell (var);
  hash = assoc_create (0);
  if (oldval)
    assoc_insert (hash, savestring ("0"), oldval);

  FREE (value_cell (var));
  var_setassoc (var, hash);

  /* these aren't valid anymore */
  var->dynamic_value = (sh_var_value_func_t *)NULL;
  var->assign_func = (sh_var_assign_func_t *)NULL;

  INVALIDATE_EXPORTSTR (var);
  if (exported_p (var))
    array_needs_making++;

  VSETATTR (var, att_assoc);
  if (oldval)
    VUNSETATTR (var, att_invisible);

  /* Make sure it's not marked as an indexed array any more */
  VUNSETATTR (var, att_array);

  /* Since namerefs can't be array variables, turn off nameref attribute */
  VUNSETATTR (var, att_nameref);

  return var;
}

char *
make_array_variable_value (entry, ind, key, value, flags)
     SHELL_VAR *entry;
     arrayind_t ind;
     char *key;
     char *value;
     int flags;
{
  SHELL_VAR *dentry;
  char *newval;

  /* If we're appending, we need the old value of the array reference, so
     fake out make_variable_value with a dummy SHELL_VAR */
  if (flags & ASS_APPEND)
    {
      dentry = (SHELL_VAR *)xmalloc (sizeof (SHELL_VAR));
      dentry->name = savestring (entry->name);
      if (assoc_p (entry))
	newval = assoc_reference (assoc_cell (entry), key);
      else
	newval = array_reference (array_cell (entry), ind);
      if (newval)
	dentry->value = savestring (newval);
      else
	{
	  dentry->value = (char *)xmalloc (1);
	  dentry->value[0] = '\0';
	}
      dentry->exportstr = 0;
      dentry->attributes = entry->attributes & ~(att_array|att_assoc|att_exported);
      /* Leave the rest of the members uninitialized; the code doesn't look
	 at them. */
      newval = make_variable_value (dentry, value, flags);	 
      dispose_variable (dentry);
    }
  else
    newval = make_variable_value (entry, value, flags);

  return newval;
}

/* Assign HASH[KEY]=VALUE according to FLAGS. ENTRY is an associative array
   variable; HASH is the hash table to assign into. HASH may or may not be
   the hash table associated with ENTRY; if it's not, the caller takes care
   of it.
   XXX - make sure that any dynamic associative array variables recreate the
   hash table on each assignment. BASH_CMDS and BASH_ALIASES already do this */
static SHELL_VAR *
bind_assoc_var_internal (entry, hash, key, value, flags)
     SHELL_VAR *entry;
     HASH_TABLE *hash;
     char *key;
     char *value;
     int flags;
{
  char *newval;

  /* Use the existing array contents to expand the value */
  newval = make_array_variable_value (entry, 0, key, value, flags);

  if (entry->assign_func)
    (*entry->assign_func) (entry, newval, 0, key);
  else
    assoc_insert (hash, key, newval);

  FREE (newval);

  VUNSETATTR (entry, att_invisible);	/* no longer invisible */

  /* check mark_modified_variables if we ever want to export array vars */
  return (entry);
}

/* Perform ENTRY[IND]=VALUE or ENTRY[KEY]=VALUE. This is not called for every
   assignment to an associative array; see assign_compound_array_list below. */
static SHELL_VAR *
bind_array_var_internal (entry, ind, key, value, flags)
     SHELL_VAR *entry;
     arrayind_t ind;
     char *key;
     char *value;
     int flags;
{
  char *newval;

  newval = make_array_variable_value (entry, ind, key, value, flags);

  if (entry->assign_func)
    (*entry->assign_func) (entry, newval, ind, key);
  else if (assoc_p (entry))
    assoc_insert (assoc_cell (entry), key, newval);
  else
    array_insert (array_cell (entry), ind, newval);
  FREE (newval);

  VUNSETATTR (entry, att_invisible);	/* no longer invisible */

  /* check mark_modified_variables if we ever want to export array vars */
  return (entry);
}

/* Perform an array assignment name[ind]=value.  If NAME already exists and
   is not an array, and IND is 0, perform name=value instead.  If NAME exists
   and is not an array, and IND is not 0, convert it into an array with the
   existing value as name[0].

   If NAME does not exist, just create an array variable, no matter what
   IND's value may be. */
SHELL_VAR *
bind_array_variable (name, ind, value, flags)
     char *name;
     arrayind_t ind;
     char *value;
     int flags;
{
  SHELL_VAR *entry;

  entry = find_shell_variable (name);

  if (entry == (SHELL_VAR *) 0)
    {
      /* Is NAME a nameref variable that points to an unset variable? */
      entry = find_variable_nameref_for_create (name, 0);
      if (entry == INVALID_NAMEREF_VALUE)
	return ((SHELL_VAR *)0);
      if (entry && nameref_p (entry))
	entry = make_new_array_variable (nameref_cell (entry));
    }
  if (entry == (SHELL_VAR *) 0)
    entry = make_new_array_variable (name);
  else if ((readonly_p (entry) && (flags&ASS_FORCE) == 0) || noassign_p (entry))
    {
      if (readonly_p (entry))
	err_readonly (name);
      return (entry);
    }
  else if (array_p (entry) == 0)
    entry = convert_var_to_array (entry);

  /* ENTRY is an array variable, and ARRAY points to the value. */
  return (bind_array_var_internal (entry, ind, 0, value, flags));
}

SHELL_VAR *
bind_array_element (entry, ind, value, flags)
     SHELL_VAR *entry;
     arrayind_t ind;
     char *value;
     int flags;
{
  return (bind_array_var_internal (entry, ind, 0, value, flags));
}
                    
SHELL_VAR *
bind_assoc_variable (entry, name, key, value, flags)
     SHELL_VAR *entry;
     char *name;
     char *key;
     char *value;
     int flags;
{
  if ((readonly_p (entry) && (flags&ASS_FORCE) == 0) || noassign_p (entry))
    {
      if (readonly_p (entry))
	err_readonly (name);
      return (entry);
    }

  return (bind_assoc_var_internal (entry, assoc_cell (entry), key, value, flags));
}

inline void
init_eltstate (array_eltstate_t *estatep)
{
  if (estatep)
    {
      estatep->type = ARRAY_INVALID;
      estatep->subtype = 0;
      estatep->key = estatep->value = 0;
      estatep->ind = INTMAX_MIN;
    }
}

inline void
flush_eltstate (array_eltstate_t *estatep)
{
  if (estatep)
    FREE (estatep->key);
}

/* Parse NAME, a lhs of an assignment statement of the form v[s], and
   assign VALUE to that array element by calling bind_array_variable().
   Flags are ASS_ assignment flags */
SHELL_VAR *
assign_array_element (name, value, flags, estatep)
     char *name, *value;
     int flags;
     array_eltstate_t *estatep;
{
  char *sub, *vname;
  int sublen, isassoc, avflags;
  SHELL_VAR *entry;

  avflags = 0;
  if (flags & ASS_NOEXPAND)
    avflags |= AV_NOEXPAND;
  if (flags & ASS_ONEWORD)
    avflags |= AV_ONEWORD;
  vname = array_variable_name (name, avflags, &sub, &sublen);

  if (vname == 0)
    return ((SHELL_VAR *)NULL);

  entry = find_variable (vname);
  isassoc = entry && assoc_p (entry);

  /* We don't allow assignment to `*' or `@' associative array keys if the
     caller hasn't told us the subscript has already been expanded
     (ASS_NOEXPAND). If the caller has explicitly told us it's ok
     (ASS_ALLOWALLSUB) we allow it. */
  if (((isassoc == 0 || (flags & (ASS_NOEXPAND|ASS_ALLOWALLSUB)) == 0) &&
	(ALL_ELEMENT_SUB (sub[0]) && sub[1] == ']')) ||
      (sublen <= 1) ||
      (sub[sublen] != '\0'))		/* sanity check */
    {
      free (vname);
      err_badarraysub (name);
      return ((SHELL_VAR *)NULL);
    }

  entry = assign_array_element_internal (entry, name, vname, sub, sublen, value, flags, estatep);

#if ARRAY_EXPORT
  if (entry && exported_p (entry))
    {
      INVALIDATE_EXPORTSTR (entry);
      array_needs_making = 1;
    }
#endif

  free (vname);
  return entry;
}

static SHELL_VAR *
assign_array_element_internal (entry, name, vname, sub, sublen, value, flags, estatep)
     SHELL_VAR *entry;
     char *name;		/* only used for error messages */
     char *vname;
     char *sub;
     int sublen;
     char *value;
     int flags;
     array_eltstate_t *estatep;
{
  char *akey, *nkey;
  arrayind_t ind;
  char *newval;

  /* rely on the caller to initialize estatep */

  if (entry && assoc_p (entry))
    {
      sub[sublen-1] = '\0';
      if ((flags & ASS_NOEXPAND) == 0)
	akey = expand_subscript_string (sub, 0);	/* [ */
      else
	akey = savestring (sub);
      sub[sublen-1] = ']';
      if (akey == 0 || *akey == 0)
	{
	  err_badarraysub (name);
	  FREE (akey);
	  return ((SHELL_VAR *)NULL);
	}
      if (estatep)
	nkey = savestring (akey);	/* assoc_insert/assoc_replace frees akey */
      entry = bind_assoc_variable (entry, vname, akey, value, flags);
      if (estatep)
	{
	  estatep->type = ARRAY_ASSOC;
	  estatep->key = nkey;
	  estatep->value = entry ? assoc_reference (assoc_cell (entry), nkey) : 0;
	}
    }
  else
    {
      ind = array_expand_index (entry, sub, sublen, 0);
      /* negative subscripts to indexed arrays count back from end */
      if (entry && ind < 0)
	ind = (array_p (entry) ? array_max_index (array_cell (entry)) : 0) + 1 + ind;
      if (ind < 0)
	{
	  err_badarraysub (name);
	  return ((SHELL_VAR *)NULL);
	}
      entry = bind_array_variable (vname, ind, value, flags);
      if (estatep)
	{
	  estatep->type = ARRAY_INDEXED;
	  estatep->ind = ind;
	  estatep->value = entry ? array_reference (array_cell (entry), ind) : 0;
	}
    }

  return (entry);
}

/* Find the array variable corresponding to NAME.  If there is no variable,
   create a new array variable.  If the variable exists but is not an array,
   convert it to an indexed array.  If FLAGS&1 is non-zero, an existing
   variable is checked for the readonly or noassign attribute in preparation
   for assignment (e.g., by the `read' builtin).  If FLAGS&2 is non-zero, we
   create an associative array. */
SHELL_VAR *
find_or_make_array_variable (name, flags)
     char *name;
     int flags;
{
  SHELL_VAR *var;

  var = find_variable (name);
  if (var == 0)
    {
      /* See if we have a nameref pointing to a variable that hasn't been
	 created yet. */
      var = find_variable_last_nameref (name, 1);
      if (var && nameref_p (var) && invisible_p (var))
	{
	  internal_warning (_("%s: removing nameref attribute"), name);
	  VUNSETATTR (var, att_nameref);
	}
      if (var && nameref_p (var))
	{
	  if (valid_nameref_value (nameref_cell (var), 2) == 0)
	    {
	      sh_invalidid (nameref_cell (var));
	      return ((SHELL_VAR *)NULL);
	    }
	  var = (flags & 2) ? make_new_assoc_variable (nameref_cell (var)) : make_new_array_variable (nameref_cell (var));
	}
    }

  if (var == 0)
    var = (flags & 2) ? make_new_assoc_variable (name) : make_new_array_variable (name);
  else if ((flags & 1) && (readonly_p (var) || noassign_p (var)))
    {
      if (readonly_p (var))
	err_readonly (name);
      return ((SHELL_VAR *)NULL);
    }
  else if ((flags & 2) && array_p (var))
    {
      set_exit_status (EXECUTION_FAILURE);
      report_error (_("%s: cannot convert indexed to associative array"), name);
      return ((SHELL_VAR *)NULL);
    }
  else if (flags & 2)
    var = assoc_p (var) ? var : convert_var_to_assoc (var);
  else if (array_p (var) == 0 && assoc_p (var) == 0)
    var = convert_var_to_array (var);

  return (var);
}
  
/* Perform a compound assignment statement for array NAME, where VALUE is
   the text between the parens:  NAME=( VALUE ) */
SHELL_VAR *
assign_array_from_string (name, value, flags)
     char *name, *value;
     int flags;
{
  SHELL_VAR *var;
  int vflags;

  vflags = 1;
  if (flags & ASS_MKASSOC)
    vflags |= 2;

  var = find_or_make_array_variable (name, vflags);
  if (var == 0)
    return ((SHELL_VAR *)NULL);

  return (assign_array_var_from_string (var, value, flags));
}

/* Sequentially assign the indices of indexed array variable VAR from the
   words in LIST. */
SHELL_VAR *
assign_array_var_from_word_list (var, list, flags)
     SHELL_VAR *var;
     WORD_LIST *list;
     int flags;
{
  register arrayind_t i;
  register WORD_LIST *l;
  ARRAY *a;

  a = array_cell (var);
  i = (flags & ASS_APPEND) ? array_max_index (a) + 1 : 0;

  for (l = list; l; l = l->next, i++)
    bind_array_var_internal (var, i, 0, l->word->word, flags & ~ASS_APPEND);

  VUNSETATTR (var, att_invisible);	/* no longer invisible */

  return var;
}

WORD_LIST *
expand_compound_array_assignment (var, value, flags)
     SHELL_VAR *var;
     char *value;
     int flags;
{
  WORD_LIST *list, *nlist;
  char *val;
  int ni;

  /* This condition is true when invoked from the declare builtin with a
     command like
	declare -a d='([1]="" [2]="bdef" [5]="hello world" "test")' */
  if (*value == '(')	/*)*/
    {
      ni = 1;
      val = extract_array_assignment_list (value, &ni);
      if (val == 0)
	return (WORD_LIST *)NULL;
    }
  else
    val = value;

  /* Expand the value string into a list of words, performing all the
     shell expansions including pathname generation and word splitting. */
  /* First we split the string on whitespace, using the shell parser
     (ksh93 seems to do this). */
  /* XXX - this needs a rethink, maybe use split_at_delims */
  list = parse_string_to_word_list (val, 1, "array assign");

  /* If the parser has quoted CTLESC and CTNLNUL with CTLESC in unquoted
     words, we need to remove those here because the code below assumes
     they are there because they exist in the original word. */
  /* XXX - if we rethink parse_string_to_word_list above, change this. */
  for (nlist = list; nlist; nlist = nlist->next)
    if ((nlist->word->flags & W_QUOTED) == 0)
      remove_quoted_escapes (nlist->word->word);

  /* Note that we defer expansion of the assignment statements for associative
     arrays here, so we don't have to scan the subscript and find the ending
     bracket twice. See the caller below. */
  if (var && assoc_p (var))
    {
      if (val != value)
	free (val);
      return list;
    }

  /* If we're using [subscript]=value, we need to quote each [ and ] to
     prevent unwanted filename expansion.  This doesn't need to be done
     for associative array expansion, since that uses a different expansion
     function (see assign_compound_array_list below). */
  if (list)
    quote_array_assignment_chars (list);

  /* Now that we've split it, perform the shell expansions on each
     word in the list. */
  nlist = list ? expand_words_no_vars (list) : (WORD_LIST *)NULL;

  dispose_words (list);

  if (val != value)
    free (val);

  return nlist;
}

#if ASSOC_KVPAIR_ASSIGNMENT
static void
assign_assoc_from_kvlist (var, nlist, h, flags)
     SHELL_VAR *var;
     WORD_LIST *nlist;
     HASH_TABLE *h;
     int flags;
{
  WORD_LIST *list;
  char *akey, *aval, *k, *v;

  for (list = nlist; list; list = list->next)
    {
      k = list->word->word;
      v = list->next ? list->next->word->word : 0;

      if (list->next)
        list = list->next;

      akey = expand_subscript_string (k, 0);
      if (akey == 0 || *akey == 0)
	{
	  err_badarraysub (k);
	  FREE (akey);
	  continue;
	}	      

      aval = expand_subscript_string (v, 0);
      if (aval == 0)
	{
	  aval = (char *)xmalloc (1);
	  aval[0] = '\0';	/* like do_assignment_internal */
	}

      bind_assoc_var_internal (var, h, akey, aval, flags);
      free (aval);
    }
}

/* Return non-zero if L appears to be a key-value pair associative array
   compound assignment. */ 
int
kvpair_assignment_p (l)
     WORD_LIST *l;
{
  return (l && (l->word->flags & W_ASSIGNMENT) == 0 && l->word->word[0] != '[');	/*]*/
}

char *
expand_and_quote_kvpair_word (w)
     char *w;
{
  char *r, *s, *t;

  t = w ? expand_subscript_string (w, 0) : 0;
  s = (t && strchr (t, CTLESC)) ? quote_escapes (t) : t;
  r = sh_single_quote (s ? s : "");
  if (s != t)
    free (s);
  free (t);
  return r;
}
#endif
     
/* Callers ensure that VAR is not NULL. Associative array assignments have not
   been expanded when this is called, or have been expanded once and single-
   quoted, so we don't have to scan through an unquoted expanded subscript to
   find the ending bracket; indexed array assignments have been expanded and
   possibly single-quoted to prevent further expansion.

   If this is an associative array, we perform the assignments into NHASH and
   set NHASH to be the value of VAR after processing the assignments in NLIST */
void
assign_compound_array_list (var, nlist, flags)
     SHELL_VAR *var;
     WORD_LIST *nlist;
     int flags;
{
  ARRAY *a;
  HASH_TABLE *h, *nhash;
  WORD_LIST *list;
  char *w, *val, *nval, *savecmd;
  int len, iflags, free_val;
  arrayind_t ind, last_ind;
  char *akey;

  a = (var && array_p (var)) ? array_cell (var) : (ARRAY *)0;
  nhash = h = (var && assoc_p (var)) ? assoc_cell (var) : (HASH_TABLE *)0;

  akey = (char *)0;
  ind = 0;

  /* Now that we are ready to assign values to the array, kill the existing
     value. */
  if ((flags & ASS_APPEND) == 0)
    {
      if (a && array_p (var))
	array_flush (a);
      else if (h && assoc_p (var))
	nhash = assoc_create (h->nbuckets);
    }

  last_ind = (a && (flags & ASS_APPEND)) ? array_max_index (a) + 1 : 0;

#if ASSOC_KVPAIR_ASSIGNMENT
  if (assoc_p (var) && kvpair_assignment_p (nlist))
    {
      iflags = flags & ~ASS_APPEND;
      assign_assoc_from_kvlist (var, nlist, nhash, iflags);
      if (nhash && nhash != h)
	{
	  h = assoc_cell (var);
	  var_setassoc (var, nhash);
	  assoc_dispose (h);
	}
      return;
    }
#endif

  for (list = nlist; list; list = list->next)
    {
      /* Don't allow var+=(values) to make assignments in VALUES append to
	 existing values by default. */
      iflags = flags & ~ASS_APPEND;
      w = list->word->word;

      /* We have a word of the form [ind]=value */
      if ((list->word->flags & W_ASSIGNMENT) && w[0] == '[')
	{
	  /* Don't have to handle embedded quotes specially any more, since
	     associative array subscripts have not been expanded yet (see
	     above). */
	  len = skipsubscript (w, 0, 0);

	  /* XXX - changes for `+=' */
 	  if (w[len] != ']' || (w[len+1] != '=' && (w[len+1] != '+' || w[len+2] != '=')))
	    {
	      if (assoc_p (var))
		{
		  err_badarraysub (w);
		  continue;
		}
	      nval = make_variable_value (var, w, flags);
	      if (var->assign_func)
		(*var->assign_func) (var, nval, last_ind, 0);
	      else
		array_insert (a, last_ind, nval);
	      FREE (nval);
	      last_ind++;
	      continue;
	    }

	  if (len == 1)
	    {
	      err_badarraysub (w);
	      continue;
	    }

	  if (ALL_ELEMENT_SUB (w[1]) && len == 2 && array_p (var))
	    {
	      set_exit_status (EXECUTION_FAILURE);
	      report_error (_("%s: cannot assign to non-numeric index"), w);
	      continue;
	    }

	  if (array_p (var))
	    {
	      ind = array_expand_index (var, w + 1, len, 0);
	      /* negative subscripts to indexed arrays count back from end */
	      if (ind < 0)
		ind = array_max_index (array_cell (var)) + 1 + ind;
	      if (ind < 0)
		{
		  err_badarraysub (w);
		  continue;
		}

	      last_ind = ind;
	    }
	  else if (assoc_p (var))
	    {
	      /* This is not performed above, see expand_compound_array_assignment */
	      w[len] = '\0';	/*[*/
	      akey = expand_subscript_string (w+1, 0);
	      w[len] = ']';
	      /* And we need to expand the value also, see below */
	      if (akey == 0 || *akey == 0)
		{
		  err_badarraysub (w);
		  FREE (akey);
		  continue;
		}
	    }

	  /* XXX - changes for `+=' -- just accept the syntax.  ksh93 doesn't do this */
	  if (w[len + 1] == '+' && w[len + 2] == '=')
	    {
	      iflags |= ASS_APPEND;
	      val = w + len + 3;
	    }
	  else
	    val = w + len + 2;	    
	}
      else if (assoc_p (var))
	{
	  set_exit_status (EXECUTION_FAILURE);
	  report_error (_("%s: %s: must use subscript when assigning associative array"), var->name, w);
	  continue;
	}
      else		/* No [ind]=value, just a stray `=' */
	{
	  ind = last_ind;
	  val = w;
	}

      free_val = 0;
      /* See above; we need to expand the value here */
      if (assoc_p (var))
	{
	  val = expand_subscript_string (val, 0);
	  if (val == 0)
	    {
	      val = (char *)xmalloc (1);
	      val[0] = '\0';	/* like do_assignment_internal */
	    }
	  free_val = 1;
	}

      savecmd = this_command_name;
      if (integer_p (var))
	this_command_name = (char *)NULL;	/* no command name for errors */
      if (assoc_p (var))
	bind_assoc_var_internal (var, nhash, akey, val, iflags);
      else
	bind_array_var_internal (var, ind, akey, val, iflags);
      last_ind++;
      this_command_name = savecmd;

      if (free_val)
	free (val);
    }

  if (assoc_p (var) && nhash && nhash != h)
    {
      h = assoc_cell (var);
      var_setassoc (var, nhash);
      assoc_dispose (h);
    }
}

/* Perform a compound array assignment:  VAR->name=( VALUE ).  The
   VALUE has already had the parentheses stripped. */
SHELL_VAR *
assign_array_var_from_string (var, value, flags)
     SHELL_VAR *var;
     char *value;
     int flags;
{
  WORD_LIST *nlist;

  if (value == 0)
    return var;

  nlist = expand_compound_array_assignment (var, value, flags);
  assign_compound_array_list (var, nlist, flags);

  if (nlist)
    dispose_words (nlist);

  if (var)
    VUNSETATTR (var, att_invisible);	/* no longer invisible */

  return (var);
}

/* Quote globbing chars and characters in $IFS before the `=' in an assignment
   statement (usually a compound array assignment) to protect them from
   unwanted filename expansion or word splitting. */
static char *
quote_assign (string)
     const char *string;
{
  size_t slen;
  int saw_eq;
  char *temp, *t, *subs;
  const char *s, *send;
  int ss, se;
  DECLARE_MBSTATE;

  slen = strlen (string);
  send = string + slen;

  t = temp = (char *)xmalloc (slen * 2 + 1);
  saw_eq = 0;
  for (s = string; *s; )
    {
      if (*s == '=')
	saw_eq = 1;
      if (saw_eq == 0 && *s == '[')		/* looks like a subscript */
	{
	  ss = s - string;
	  se = skipsubscript (string, ss, 0);
	  subs = substring (s, ss, se);
	  *t++ = '\\';
	  strcpy (t, subs);
	  t += se - ss;
	  *t++ = '\\';
	  *t++ = ']';
	  s += se + 1;
	  free (subs);
	  continue;
	}
      if (saw_eq == 0 && (glob_char_p (s) || isifs (*s)))
	*t++ = '\\';

      COPY_CHAR_P (t, s, send);
    }
  *t = '\0';
  return temp;
}

/* Take a word W of the form [IND]=VALUE and transform it to ['IND']='VALUE'
   to prevent further expansion. This is called for compound assignments to
   indexed arrays. W has already undergone word expansions. If W has no [IND]=,
   just single-quote and return it. */
static char *
quote_compound_array_word (w, type)
     char *w;
     int type;
{
  char *nword, *sub, *value, *t;
  int ind, wlen, i;

  if (w[0] != LBRACK)
    return (sh_single_quote (w));	/* XXX - quote CTLESC */
  ind = skipsubscript (w, 0, 0);
  if (w[ind] != RBRACK)
    return (sh_single_quote (w));	/* XXX - quote CTLESC */

  wlen = strlen (w);
  w[ind] = '\0';
  t = (strchr (w+1, CTLESC)) ? quote_escapes (w+1) : w+1;
  sub = sh_single_quote (t);
  if (t != w+1)
   free (t);
  w[ind] = RBRACK;

  nword = xmalloc (wlen * 4 + 5);	/* wlen*4 is max single quoted length */
  nword[0] = LBRACK;
  i = STRLEN (sub);
  memcpy (nword+1, sub, i);
  free (sub);
  i++;				/* accommodate the opening LBRACK */
  nword[i++] = w[ind++];	/* RBRACK */
  if (w[ind] == '+')
    nword[i++] = w[ind++];
  nword[i++] = w[ind++];
  t = (strchr (w+ind, CTLESC)) ? quote_escapes (w+ind) : w+ind;
  value = sh_single_quote (t);
  if (t != w+ind)
   free (t);
  strcpy (nword + i, value);

  return nword;
}

/* Expand the key and value in W, which is of the form [KEY]=VALUE, and
   reconstruct W with the expanded and single-quoted version:
   ['expanded-key']='expanded-value'. If there is no [KEY]=, single-quote the
   word and return it. Very similar to previous function, but does not assume
   W has already been expanded, and expands the KEY and VALUE separately.
   Used for compound assignments to associative arrays that are arguments to
   declaration builtins (declare -A a=( list )). */
char *
expand_and_quote_assoc_word (w, type)
     char *w;
     int type;
{
  char *nword, *key, *value, *s, *t;
  int ind, wlen, i;

  if (w[0] != LBRACK)
    return (sh_single_quote (w));	/* XXX - quote_escapes */
  ind = skipsubscript (w, 0, 0);
  if (w[ind] != RBRACK)
    return (sh_single_quote (w));	/* XXX - quote_escapes */

  w[ind] = '\0';
  t = expand_subscript_string (w+1, 0);
  s = (t && strchr (t, CTLESC)) ? quote_escapes (t) : t;
  key = sh_single_quote (s ? s : "");
  if (s != t)
    free (s);
  w[ind] = RBRACK;
  free (t);

  wlen = STRLEN (key);
  nword = xmalloc (wlen + 5);
  nword[0] = LBRACK;
  memcpy (nword+1, key, wlen);
  i = wlen + 1;			/* accommodate the opening LBRACK */

  nword[i++] = w[ind++];	/* RBRACK */
  if (w[ind] == '+')
    nword[i++] = w[ind++];
  nword[i++] = w[ind++];

  t = expand_subscript_string (w+ind, 0);
  s = (t && strchr (t, CTLESC)) ? quote_escapes (t) : t;
  value = sh_single_quote (s ? s : "");
  if (s != t)
    free (s);
  free (t);
  nword = xrealloc (nword, wlen + 5 + STRLEN (value));
  strcpy (nword + i, value);

  free (key);
  free (value);

  return nword;
}

/* For each word in a compound array assignment, if the word looks like
   [ind]=value, single-quote ind and value, but leave the brackets and
   the = sign (and any `+') alone. If it's not an assignment, just single-
   quote the word. This is used for indexed arrays. */
void
quote_compound_array_list (list, type)
     WORD_LIST *list;
     int type;
{
  char *s, *t;
  WORD_LIST *l;

  for (l = list; l; l = l->next)
    {
      if (l->word == 0 || l->word->word == 0)
	continue;	/* should not happen, but just in case... */
      if ((l->word->flags & W_ASSIGNMENT) == 0)
	{
	  s = (strchr (l->word->word, CTLESC)) ? quote_escapes (l->word->word) : l->word->word;
	  t = sh_single_quote (s);
	  if (s != l->word->word)
	    free (s);
	}
      else 
	t = quote_compound_array_word (l->word->word, type);
      free (l->word->word);
      l->word->word = t;
    }
}

/* For each word in a compound array assignment, if the word looks like
   [ind]=value, quote globbing chars and characters in $IFS before the `='. */
static void
quote_array_assignment_chars (list)
     WORD_LIST *list;
{
  char *nword;
  WORD_LIST *l;

  for (l = list; l; l = l->next)
    {
      if (l->word == 0 || l->word->word == 0 || l->word->word[0] == '\0')
	continue;	/* should not happen, but just in case... */
      /* Don't bother if it hasn't been recognized as an assignment or
	 doesn't look like [ind]=value */
      if ((l->word->flags & W_ASSIGNMENT) == 0)
	continue;
      if (l->word->word[0] != '[' || mbschr (l->word->word, '=') == 0) /* ] */
	continue;

      nword = quote_assign (l->word->word);
      free (l->word->word);
      l->word->word = nword;
      l->word->flags |= W_NOGLOB;	/* XXX - W_NOSPLIT also? */
    }
}

/* skipsubscript moved to subst.c to use private functions. 2009/02/24. */

/* This function is called with SUB pointing to just after the beginning
   `[' of an array subscript and removes the array element to which SUB
   expands from array VAR.  A subscript of `*' or `@' unsets the array. */
/* If FLAGS&1 (VA_NOEXPAND) we don't expand the subscript; we just use it
   as-is. If FLAGS&VA_ONEWORD, we don't try to use skipsubscript to parse
   the subscript, we just assume the subscript ends with a close bracket,
   if one is present, and use what's inside the brackets. */
int
unbind_array_element (var, sub, flags)
     SHELL_VAR *var;
     char *sub;
     int flags;
{
  arrayind_t ind;
  char *akey;
  ARRAY_ELEMENT *ae;

  /* Assume that the caller (unset_builtin) passes us a null-terminated SUB,
     so we don't have to use VA_ONEWORD or parse the subscript again with
     skipsubscript(). */

  if (ALL_ELEMENT_SUB (sub[0]) && sub[1] == 0)
    {
      if (array_p (var) || assoc_p (var))
	{
	  if (flags & VA_ALLOWALL)
	    {
	      unbind_variable (var->name);	/* XXX -- {array,assoc}_flush ? */
	      return (0);
	    }
	  /* otherwise we fall through and try to unset element `@' or `*' */
	}
      else
	return -2;	/* don't allow this to unset scalar variables */
    }

  if (assoc_p (var))
    {
      akey = (flags & VA_NOEXPAND) ? sub : expand_subscript_string (sub, 0);
      if (akey == 0 || *akey == 0)
	{
	  builtin_error ("[%s]: %s", sub, _(bash_badsub_errmsg));
	  FREE (akey);
	  return -1;
	}
      assoc_remove (assoc_cell (var), akey);
      if (akey != sub)
	free (akey);
    }
  else if (array_p (var))
    {
      if (ALL_ELEMENT_SUB (sub[0]) && sub[1] == 0)
	{
	  /* We can go several ways here:
		1) remove the array (backwards compatible)
		2) empty the array (new behavior)
		3) do nothing; treat the `@' or `*' as an expression and throw
		   an error
	  */
	  /* Behavior 1 */
	  if (shell_compatibility_level <= 51)
	    {
	      unbind_variable (name_cell (var));
	      return 0;
	    }
	  else /* Behavior 2 */
	    {
	      array_flush (array_cell (var));
	      return 0;
	    }
	  /* Fall through for behavior 3 */
	}
      ind = array_expand_index (var, sub, strlen (sub) + 1, 0);
      /* negative subscripts to indexed arrays count back from end */
      if (ind < 0)
	ind = array_max_index (array_cell (var)) + 1 + ind;
      if (ind < 0)
	{
	  builtin_error ("[%s]: %s", sub, _(bash_badsub_errmsg));
	  return -1;
	}
      ae = array_remove (array_cell (var), ind);
      if (ae)
	array_dispose_element (ae);
    }
  else	/* array_p (var) == 0 && assoc_p (var) == 0 */
    {
      akey = this_command_name;
      ind = array_expand_index (var, sub, strlen (sub) + 1, 0);
      this_command_name = akey;
      if (ind == 0)
	{
	  unbind_variable (var->name);
	  return (0);
	}
      else
	return -2;	/* any subscript other than 0 is invalid with scalar variables */
    }

  return 0;
}

/* Format and output an array assignment in compound form VAR=(VALUES),
   suitable for re-use as input. */
void
print_array_assignment (var, quoted)
     SHELL_VAR *var;
     int quoted;
{
  char *vstr;

  vstr = array_to_assign (array_cell (var), quoted);

  if (vstr == 0)
    printf ("%s=%s\n", var->name, quoted ? "'()'" : "()");
  else
    {
      printf ("%s=%s\n", var->name, vstr);
      free (vstr);
    }
}

/* Format and output an associative array assignment in compound form
   VAR=(VALUES), suitable for re-use as input. */
void
print_assoc_assignment (var, quoted)
     SHELL_VAR *var;
     int quoted;
{
  char *vstr;

  vstr = assoc_to_assign (assoc_cell (var), quoted);

  if (vstr == 0)
    printf ("%s=%s\n", var->name, quoted ? "'()'" : "()");
  else
    {
      printf ("%s=%s\n", var->name, vstr);
      free (vstr);
    }
}

/***********************************************************************/
/*								       */
/* Utility functions to manage arrays and their contents for expansion */
/*								       */
/***********************************************************************/

/* Return 1 if NAME is a properly-formed array reference v[sub]. */

/* Return 1 if NAME is a properly-formed array reference v[sub]. */

/* When NAME is a properly-formed array reference and a non-null argument SUBP
   is supplied, '[' and ']' that enclose the subscript are replaced by '\0',
   and the pointer to the subscript in NAME is assigned to *SUBP, so that NAME
   and SUBP can be later used as the array name and the subscript,
   respectively.  When SUBP is the null pointer, the original string NAME will
   not be modified. */
/* We need to reserve 1 for FLAGS, which we pass to skipsubscript. */
int
tokenize_array_reference (name, flags, subp)
     char *name;
     int flags;
     char **subp;
{
  char *t;
  int r, len, isassoc, ssflags;
  SHELL_VAR *entry;

  t = mbschr (name, '[');	/* ] */
  isassoc = 0;
  if (t)
    {
      *t = '\0';
      r = legal_identifier (name);
      if (flags & VA_NOEXPAND)	/* Don't waste a lookup if we don't need one */
	isassoc = (entry = find_variable (name)) && assoc_p (entry);      
      *t = '[';
      if (r == 0)
	return 0;

      ssflags = 0;
      if (isassoc && ((flags & (VA_NOEXPAND|VA_ONEWORD)) == (VA_NOEXPAND|VA_ONEWORD)))
	len = strlen (t) - 1;
      else if (isassoc)
	{
	  if (flags & VA_NOEXPAND)
	    ssflags |= 1;
	  len = skipsubscript (t, 0, ssflags);
	}
      else
	/* Check for a properly-terminated non-null subscript. */
	len = skipsubscript (t, 0, 0);		/* arithmetic expression */

      if (t[len] != ']' || len == 1 || t[len+1] != '\0')
	return 0;

#if 0
      /* Could check and allow subscripts consisting only of whitespace for
	 existing associative arrays, using isassoc */
      for (r = 1; r < len; r++)
	if (whitespace (t[r]) == 0)
	  break;
      if (r == len)
	return 0; /* Fail if the subscript contains only whitespaces. */
#endif

      if (subp)
	{
	  t[0] = t[len] = '\0';
	  *subp = t + 1;
	}

      /* This allows blank subscripts */
      return 1;
    }
  return 0;
}

/* Return 1 if NAME is a properly-formed array reference v[sub]. */

/* We need to reserve 1 for FLAGS, which we pass to skipsubscript. */
int
valid_array_reference (name, flags)
     const char *name;
     int flags;
{
  return tokenize_array_reference ((char *)name, flags, (char **)NULL);
}

/* Expand the array index beginning at S and extending LEN characters. */
arrayind_t
array_expand_index (var, s, len, flags)
     SHELL_VAR *var;
     char *s;
     int len;
     int flags;
{
  char *exp, *t, *savecmd;
  int expok, eflag;
  arrayind_t val;

  exp = (char *)xmalloc (len);
  strncpy (exp, s, len - 1);
  exp[len - 1] = '\0';
#if 0	/* TAG: maybe bash-5.2 */
  if ((flags & AV_NOEXPAND) == 0)
    t = expand_arith_string (exp, Q_DOUBLE_QUOTES|Q_ARITH|Q_ARRAYSUB);	/* XXX - Q_ARRAYSUB for future use */
  else
    t = exp;
#else
  t = expand_arith_string (exp, Q_DOUBLE_QUOTES|Q_ARITH|Q_ARRAYSUB);	/* XXX - Q_ARRAYSUB for future use */
#endif
  savecmd = this_command_name;
  this_command_name = (char *)NULL;
  eflag = (shell_compatibility_level > 51) ? 0 : EXP_EXPANDED;
  val = evalexp (t, eflag, &expok);	/* XXX - was 0 but we expanded exp already */
  this_command_name = savecmd;
  if (t != exp)
    free (t);
  free (exp);
  if (expok == 0)
    {
      set_exit_status (EXECUTION_FAILURE);

      if (no_longjmp_on_fatal_error)
	return 0;
      top_level_cleanup ();      
      jump_to_top_level (DISCARD);
    }
  return val;
}

/* Return the name of the variable specified by S without any subscript.
   If SUBP is non-null, return a pointer to the start of the subscript
   in *SUBP. If LENP is non-null, the length of the subscript is returned
   in *LENP.  This returns newly-allocated memory. */
char *
array_variable_name (s, flags, subp, lenp)
     const char *s;
     int flags;
     char **subp;
     int *lenp;
{
  char *t, *ret;
  int ind, ni, ssflags;

  t = mbschr (s, '[');
  if (t == 0)
    {
      if (subp)
      	*subp = t;
      if (lenp)
	*lenp = 0;
      return ((char *)NULL);
    }
  ind = t - s;
  if ((flags & (AV_NOEXPAND|AV_ONEWORD)) == (AV_NOEXPAND|AV_ONEWORD))
    ni = strlen (s) - 1;
  else
    {
      ssflags = 0;
      if (flags & AV_NOEXPAND)
	ssflags |= 1;
      ni = skipsubscript (s, ind, ssflags);
    }
  if (ni <= ind + 1 || s[ni] != ']')
    {
      err_badarraysub (s);
      if (subp)
      	*subp = t;
      if (lenp)
	*lenp = 0;
      return ((char *)NULL);
    }

  *t = '\0';
  ret = savestring (s);
  *t++ = '[';		/* ] */

  if (subp)
    *subp = t;
  if (lenp)
    *lenp = ni - ind;

  return ret;
}

/* Return the variable specified by S without any subscript.  If SUBP is
   non-null, return a pointer to the start of the subscript in *SUBP.
   If LENP is non-null, the length of the subscript is returned in *LENP. */
SHELL_VAR *
array_variable_part (s, flags, subp, lenp)
     const char *s;
     int flags;
     char **subp;
     int *lenp;
{
  char *t;
  SHELL_VAR *var;

  t = array_variable_name (s, flags, subp, lenp);
  if (t == 0)
    return ((SHELL_VAR *)NULL);
  var = find_variable (t);		/* XXX - handle namerefs here? */

  free (t);
  return var;	/* now return invisible variables; caller must handle */
}

#define INDEX_ERROR() \
  do \
    { \
      if (var) \
	err_badarraysub (var->name); \
      else \
	{ \
	  t[-1] = '\0'; \
	  err_badarraysub (s); \
	  t[-1] = '[';	/* ] */\
	} \
      return ((char *)NULL); \
    } \
  while (0)

/* Return a string containing the elements in the array and subscript
   described by S.  If the subscript is * or @, obeys quoting rules akin
   to the expansion of $* and $@ including double quoting.  If RTYPE
   is non-null it gets 1 if the array reference is name[*], 2 if the
   reference is name[@], and 0 otherwise. */
static char *
array_value_internal (s, quoted, flags, estatep)
     const char *s;
     int quoted, flags;
     array_eltstate_t *estatep;
{
  int len, isassoc, subtype;
  arrayind_t ind;
  char *akey;
  char *retval, *t, *temp;
  WORD_LIST *l;
  SHELL_VAR *var;

  var = array_variable_part (s, flags, &t, &len);	/* XXX */

  /* Expand the index, even if the variable doesn't exist, in case side
     effects are needed, like ${w[i++]} where w is unset. */
#if 0
  if (var == 0)
    return (char *)NULL;
#endif

  if (len == 0)
    return ((char *)NULL);	/* error message already printed */

  isassoc = var && assoc_p (var);
  /* [ */
  akey = 0;
  subtype = 0;
  if (estatep)
    estatep->value = (char *)NULL;

  /* Backwards compatibility: we only change the behavior of A[@] and A[*]
     for associative arrays, and the caller has to request it. */
  if ((isassoc == 0 || (flags & AV_ATSTARKEYS) == 0) && ALL_ELEMENT_SUB (t[0]) && t[1] == ']')
    {
      if (estatep)
	estatep->subtype = (t[0] == '*') ? 1 : 2;
      if ((flags & AV_ALLOWALL) == 0)
	{
	  err_badarraysub (s);
	  return ((char *)NULL);
	}
      else if (var == 0 || value_cell (var) == 0)
	return ((char *)NULL);
      else if (invisible_p (var))
	return ((char *)NULL);
      else if (array_p (var) == 0 && assoc_p (var) == 0)
        {
          if (estatep)
	    estatep->type = ARRAY_SCALAR;
	  l = add_string_to_list (value_cell (var), (WORD_LIST *)NULL);
        }
      else if (assoc_p (var))
	{
	  if (estatep)
	    estatep->type = ARRAY_ASSOC;
	  l = assoc_to_word_list (assoc_cell (var));
	  if (l == (WORD_LIST *)NULL)
	    return ((char *)NULL);
	}
      else
	{
	  if (estatep)
	    estatep->type = ARRAY_INDEXED;
	  l = array_to_word_list (array_cell (var));
	  if (l == (WORD_LIST *)NULL)
	    return ((char *) NULL);
	}

      /* Caller of array_value takes care of inspecting estatep->subtype and
         duplicating retval if subtype == 0, so this is not a memory leak */
      if (t[0] == '*' && (quoted & (Q_HERE_DOCUMENT|Q_DOUBLE_QUOTES)))
	{
	  temp = string_list_dollar_star (l, quoted, (flags & AV_ASSIGNRHS) ? PF_ASSIGNRHS : 0);
	  retval = quote_string (temp);
	  free (temp);
	}
      else	/* ${name[@]} or unquoted ${name[*]} */
	retval = string_list_dollar_at (l, quoted, (flags & AV_ASSIGNRHS) ? PF_ASSIGNRHS : 0);

      dispose_words (l);
    }
  else
    {
      if (estatep)
	estatep->subtype = 0;
      if (var == 0 || array_p (var) || assoc_p (var) == 0)
	{
	  if ((flags & AV_USEIND) == 0 || estatep == 0)
	    {
	      ind = array_expand_index (var, t, len, flags);
	      if (ind < 0)
		{
		  /* negative subscripts to indexed arrays count back from end */
		  if (var && array_p (var))
		    ind = array_max_index (array_cell (var)) + 1 + ind;
		  if (ind < 0)
		    INDEX_ERROR();
		}
	      if (estatep)
		estatep->ind = ind;
	    }
	  else if (estatep && (flags & AV_USEIND))
	    ind = estatep->ind;
	  if (estatep && var)
	    estatep->type = array_p (var) ? ARRAY_INDEXED : ARRAY_SCALAR;
	}
      else if (assoc_p (var))
	{
	  t[len - 1] = '\0';
	  if (estatep)
	    estatep->type = ARRAY_ASSOC;
	  if ((flags & AV_USEIND) && estatep && estatep->key)
	    akey = savestring (estatep->key);
	  else if ((flags & AV_NOEXPAND) == 0)
	    akey = expand_subscript_string (t, 0);	/* [ */
	  else
	    akey = savestring (t);
	  t[len - 1] = ']';
	  if (akey == 0 || *akey == 0)
	    {
	      FREE (akey);
	      INDEX_ERROR();
	    }
	}

      if (var == 0 || value_cell (var) == 0)
	{
	  FREE (akey);
	  return ((char *)NULL);
	}
      else if (invisible_p (var))
	{
	  FREE (akey);
	  return ((char *)NULL);
	}
      if (array_p (var) == 0 && assoc_p (var) == 0)
	retval = (ind == 0) ? value_cell (var) : (char *)NULL;
      else if (assoc_p (var))
        {
	  retval = assoc_reference (assoc_cell (var), akey);
	  if (estatep && estatep->key && (flags & AV_USEIND))
	    free (akey);		/* duplicated estatep->key */
	  else if (estatep)
	    estatep->key = akey;	/* XXX - caller must manage */
	  else				/* not saving it anywhere */
	    free (akey);
        }
      else
	retval = array_reference (array_cell (var), ind);

      if (estatep)
	estatep->value = retval;
    }

  return retval;
}

/* Return a string containing the elements described by the array and
   subscript contained in S, obeying quoting for subscripts * and @. */
char *
array_value (s, quoted, flags, estatep)
     const char *s;
     int quoted, flags;
     array_eltstate_t *estatep;
{
  char *retval;

  retval = array_value_internal (s, quoted, flags|AV_ALLOWALL, estatep);
  return retval;
}

/* Return the value of the array indexing expression S as a single string.
   If (FLAGS & AV_ALLOWALL) is 0, do not allow `@' and `*' subscripts.  This
   is used by other parts of the shell such as the arithmetic expression
   evaluator in expr.c. */
char *
get_array_value (s, flags, estatep)
     const char *s;
     int flags;
     array_eltstate_t *estatep;
{
  char *retval;

  retval = array_value_internal (s, 0, flags, estatep);
  return retval;
}

char *
array_keys (s, quoted, pflags)
     char *s;
     int quoted, pflags;
{
  int len;
  char *retval, *t, *temp;
  WORD_LIST *l;
  SHELL_VAR *var;

  var = array_variable_part (s, 0, &t, &len);

  /* [ */
  if (var == 0 || ALL_ELEMENT_SUB (t[0]) == 0 || t[1] != ']')
    return (char *)NULL;

  if (var_isset (var) == 0 || invisible_p (var))
    return (char *)NULL;

  if (array_p (var) == 0 && assoc_p (var) == 0)
    l = add_string_to_list ("0", (WORD_LIST *)NULL);
  else if (assoc_p (var))
    l = assoc_keys_to_word_list (assoc_cell (var));
  else
    l = array_keys_to_word_list (array_cell (var));
  if (l == (WORD_LIST *)NULL)
    return ((char *) NULL);

  retval = string_list_pos_params (t[0], l, quoted, pflags);

  dispose_words (l);
  return retval;
}
#endif /* ARRAY_VARS */
