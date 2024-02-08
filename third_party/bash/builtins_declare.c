/* declare.c, created from declare.def. */
#line 22 "./declare.def"

#line 64 "./declare.def"

#line 72 "./declare.def"

#include "config.h"

#if defined (HAVE_UNISTD_H)
#  ifdef _MINIX
#    include <sys/types.h>
#  endif
#  include <unistd.h>
#endif

#include <stdio.h>

#include "bashansi.h"
#include "bashintl.h"

#include "shell.h"
#include "flags.h"
#include "common.h"
#include "builtext.h"
#include "bashgetopt.h"

static SHELL_VAR *declare_find_variable PARAMS((const char *, int, int));
static char *declare_build_newname PARAMS((char *, char *, int, char *, int));
static char *declare_transform_name PARAMS((char *, int, int));

static int declare_internal PARAMS((register WORD_LIST *, int));

/* Declare or change variable attributes. */
int
declare_builtin (list)
     register WORD_LIST *list;
{
  return (declare_internal (list, 0));
}

#line 122 "./declare.def"
int
local_builtin (list)
     register WORD_LIST *list;
{
  /* Catch a straight `local --help' before checking function context */
  if (list && list->word && STREQ (list->word->word, "--help"))
    {
      builtin_help ();
      return (EX_USAGE);
    }
    
  if (variable_context)
    return (declare_internal (list, 1));
  else
    {
      builtin_error (_("can only be used in a function"));
      return (EXECUTION_FAILURE);
    }
}

#if defined (ARRAY_VARS)
#  define DECLARE_OPTS	"+acfgilnprtuxAFGI"
#else
#  define DECLARE_OPTS	"+cfgilnprtuxFGI"
#endif

static SHELL_VAR *
declare_find_variable (name, mkglobal, chklocal)
     const char *name;
     int mkglobal, chklocal;
{
  SHELL_VAR *var;

  if (mkglobal == 0)
    return (find_variable (name));
  else if (chklocal)
    {
      var = find_variable (name);
      if (var && local_p (var) && var->context == variable_context)
	return var;
      return (find_global_variable (name));
    }
  else
    return (find_global_variable (name));
}

/* Build a new string
	NAME[SUBSCRIPT][[+]=VALUE]
   from expanding a nameref into NAME */
static char *
declare_build_newname (name, subscript_start, offset, value, aflags)
     char *name, *subscript_start;
     int offset;
     char *value;
     int aflags;
{
  size_t namelen, savelen;
  char *ret;

  savelen = namelen = strlen (name);
  if (subscript_start)
    {
      *subscript_start = '[';		/* ] */
      namelen += strlen (subscript_start);
    }
  ret = xmalloc (namelen + 2 + strlen (value) + 1);
  strcpy (ret, name);
  if (subscript_start)
    strcpy (ret + savelen, subscript_start);
  if (offset)
    {
      if (aflags & ASS_APPEND)
	ret[namelen++] = '+';
      ret[namelen++] = '=';
      if (value && *value)
	strcpy (ret + namelen, value);
      else
	ret[namelen] = '\0';
    }

  return (ret);
}

static char *
declare_transform_name (name, flags_on, flags_off)
     char *name;
     int flags_on, flags_off;
{
  SHELL_VAR *var, *v;
  char *newname;
  
  var = find_variable (name);
  if (var == 0)
    newname = nameref_transform_name (name, ASS_MKLOCAL);
  else if ((flags_on & att_nameref) == 0 && (flags_off & att_nameref) == 0)
    {
      /* Ok, we're following namerefs here, so let's make sure that if
	 we followed one, it was at the same context (see below for
	 more details). */
      v = find_variable_last_nameref (name, 1);
      newname = (v && v->context != variable_context) ? name : name_cell (var);
    }
  else
    newname = name;	/* dealing with nameref attribute */

  return (newname);
}

/* The workhorse function. */
static int
declare_internal (list, local_var)
     register WORD_LIST *list;
     int local_var;
{
  int flags_on, flags_off, *flags;
  int any_failed, assign_error, pflag, nodefs, opt, onref, offref;
  int mkglobal, chklocal, inherit_flag;
  char *t, *subscript_start;
  SHELL_VAR *var, *refvar, *v;
  FUNCTION_DEF *shell_fn;

  flags_on = flags_off = any_failed = assign_error = pflag = nodefs = 0;
  mkglobal = chklocal = inherit_flag = 0;
  refvar = (SHELL_VAR *)NULL;
  reset_internal_getopt ();
  while ((opt = internal_getopt (list, DECLARE_OPTS)) != -1)
    {
      flags = list_opttype == '+' ? &flags_off : &flags_on;

      /* If you add options here, see whether or not they need to be added to
	 the loop in subst.c:shell_expand_word_list() */
      switch (opt)
	{
	case 'a':
#if defined (ARRAY_VARS)
	  *flags |= att_array;
	  break;
#else
	  builtin_usage ();
	  return (EX_USAGE);
#endif
	case 'A':
#if defined (ARRAY_VARS)
	  *flags |= att_assoc;
	  break;
#else
	  builtin_usage ();
	  return (EX_USAGE);
#endif
	case 'p':
	  pflag++;
	  break;
	case 'F':
	  nodefs++;
	  *flags |= att_function;
	  break;
	case 'f':
	  *flags |= att_function;
	  break;
	case 'G':
	  if (flags == &flags_on)
	    chklocal = 1;
	  /*FALLTHROUGH*/
	case 'g':
	  if (flags == &flags_on)
	    mkglobal = 1;
	  break;
	case 'i':
	  *flags |= att_integer;
	  break;
	case 'n':
	  *flags |= att_nameref;
	  break;
	case 'r':
	  *flags |= att_readonly;
	  break;
	case 't':
	  *flags |= att_trace;
	  break;
	case 'x':
	  *flags |= att_exported;
	  array_needs_making = 1;
	  break;
#if defined (CASEMOD_ATTRS)
#  if defined (CASEMOD_CAPCASE)
	 case 'c':
	  *flags |= att_capcase;
	  if (flags == &flags_on)
	    flags_off |= att_uppercase|att_lowercase;
	  break;
#  endif
	case 'l':
	  *flags |= att_lowercase;
	  if (flags == &flags_on)
	    flags_off |= att_capcase|att_uppercase;
	  break;
	case 'u':
	  *flags |= att_uppercase;
	  if (flags == &flags_on)
	    flags_off |= att_capcase|att_lowercase;
	  break;
#endif /* CASEMOD_ATTRS */
	case 'I':
	  inherit_flag = MKLOC_INHERIT;
	  break;
	CASE_HELPOPT;
	default:
	  builtin_usage ();
	  return (EX_USAGE);
	}
    }

  list = loptend;

  /* If there are no more arguments left, then we just want to show
     some variables. */
  if (list == 0)	/* declare -[aAfFilnrtux] */
    {
      /* Show local variables defined at this context level if this is
	 the `local' builtin. */
      if (local_var)
	show_local_var_attributes (0, nodefs);	/* XXX - fix up args later */
      else if (pflag && (flags_on == 0 || flags_on == att_function))
	show_all_var_attributes (flags_on == 0, nodefs);
      else if (flags_on == 0)
	return (set_builtin ((WORD_LIST *)NULL));
      else
	set_or_show_attributes ((WORD_LIST *)NULL, flags_on, nodefs);

      return (sh_chkwrite (EXECUTION_SUCCESS));
    }

  if (pflag)	/* declare -p [-aAfFilnrtux] [name ...] */
    {
      for (any_failed = 0; list; list = list->next)
	{
	  if (flags_on & att_function)
	    pflag = show_func_attributes (list->word->word, nodefs);
	  else if (local_var)
	    pflag = show_localname_attributes (list->word->word, nodefs);
	  else
	    pflag = show_name_attributes (list->word->word, nodefs);
	  if (pflag)
	    {
	      sh_notfound (list->word->word);
	      any_failed++;
	    }
	}
      return (sh_chkwrite (any_failed ? EXECUTION_FAILURE : EXECUTION_SUCCESS));
    }

  /* Some option combinations that don't make any sense */
  if ((flags_on & att_function) && (flags_on & (att_array|att_assoc|att_integer|att_nameref)))
    {
      char *optchar;

      if (flags_on & att_nameref)
	optchar = "-n";
      else if (flags_on & att_integer)
	optchar = "-i";
      else if (flags_on & att_assoc)
	optchar = "-A";
      else if (flags_on & att_array)
	optchar = "-a";

      sh_invalidopt (optchar);	
      return (EXECUTION_FAILURE);
    }

#define NEXT_VARIABLE() free (name); list = list->next; continue

  /* There are arguments left, so we are making variables. */
  while (list)		/* declare [-aAfFilnrtux] name[=value] [name[=value] ...] */
    {
      char *value, *name, *newname;
      int offset, aflags, wflags, created_var;
      int assoc_noexpand;
#if defined (ARRAY_VARS)
      int making_array_special, compound_array_assign, simple_array_assign;
      int var_exists, array_exists, creating_array, array_subscript_assignment;
#endif

      name = savestring (list->word->word);
      wflags = list->word->flags;
#if defined (ARRAY_VARS)
      assoc_noexpand = assoc_expand_once && (wflags & W_ASSIGNMENT);
#else
      assoc_noexpand = 0;
#endif
      /* XXX - we allow unbalanced brackets if assoc_noexpand is set, we count
	 brackets and make sure they match if assoc_noexpand is not set. So we
	 need to make sure we're checking assoc_noexpand and expand_once_flag
	 for backwards compatibility. We also use assoc_noexpand below when
	 we call assign_array_element, so we need to make sure they're
	 consistent in how they count brackets. */
      offset = assignment (name, assoc_noexpand ? 2 : 0);
      aflags = 0;
      created_var = 0;

      if (local_var && variable_context && STREQ (name, "-"))
	{
	  var = make_local_variable ("-", 0);
	  FREE (value_cell (var));		/* just in case */
	  value = get_current_options ();
	  var_setvalue (var, value);
	  VSETATTR (var, att_invisible);
	  NEXT_VARIABLE ();
	}

      /* If we are declaring a function, then complain about it in some way.
	 We don't let people make functions by saying `typeset -f foo=bar'. */

      /* Can't define functions using assignment statements */
      if (offset && (flags_on & att_function))	/* declare -f [-rix] foo=bar */
	{
	  builtin_error (_("cannot use `-f' to make functions"));
	  free (name);
	  return (EXECUTION_FAILURE);
	}

      /* There should be a way, however, to let people look at a particular
	 function definition by saying `typeset -f foo'.  This is the only
	 place in this builtin where we deal with functions. */

      if (flags_on & att_function)
	{
	  /* Should we restrict this when the shell is in posix mode even if
	     the function was created before the shell entered posix mode?
	     Previous versions of the shell enforced the restriction. */
	  if (posixly_correct && legal_identifier (name) == 0)
	    {
	      sh_invalidid (name);
	      assign_error++;
	      NEXT_VARIABLE ();
	    }

	  var = find_function (name);

	  if (var)
	    {
	      if (readonly_p (var) && (flags_off & att_readonly))
		{
		  builtin_error (_("%s: readonly function"), name);
		  any_failed++;
		  NEXT_VARIABLE ();
		}
	      /* declare -[Ff] name [name...] */
	      if (flags_on == att_function && flags_off == 0)
		{
#if defined (DEBUGGER)
		  if (nodefs && debugging_mode)
		    {
		      shell_fn = find_function_def (name_cell (var));
		      if (shell_fn)
			printf ("%s %d %s\n", name_cell (var), shell_fn->line, shell_fn->source_file);
		      else
			printf ("%s\n", name_cell (var));
		    }
		  else
#endif /* DEBUGGER */
		    {	
		      t = nodefs ? name_cell (var) : named_function_string (name, function_cell (var), FUNC_MULTILINE|FUNC_EXTERNAL);
		      printf ("%s\n", t);
		      any_failed = sh_chkwrite (any_failed);
		    }
		}
	      else		/* declare -[fF] -[rx] name [name...] */
		{
		  VSETATTR (var, flags_on);
		  flags_off &= ~att_function;	/* makes no sense */
		  VUNSETATTR (var, flags_off);
		}
	    }
	  else
	    any_failed++;

	  NEXT_VARIABLE ();
	}

      if (offset)	/* declare [-aAfFirx] name=value */
	{
	  name[offset] = '\0';
	  value = name + offset + 1;
	  if (name[offset - 1] == '+')
	    {
	      aflags |= ASS_APPEND;
	      name[offset - 1] = '\0';
	    }
	}
      else
	value = "";

      /* Do some lexical error checking on the LHS and RHS of the assignment
	 that is specific to nameref variables. */
      if (flags_on & att_nameref)
	{
#if defined (ARRAY_VARS)
	  if (valid_array_reference (name, 0))
	    {
	      builtin_error (_("%s: reference variable cannot be an array"), name);
	      any_failed++;
	      NEXT_VARIABLE ();
	    }
	  else
#endif
	  /* disallow self references at global scope, warn at function scope */
	  if (check_selfref (name, value, 0))
	    {
	      if (variable_context == 0)
		{
		  builtin_error (_("%s: nameref variable self references not allowed"), name);
		  assign_error++;	/* XXX any_failed++ instead? */
		  NEXT_VARIABLE ();
		}
	      else
		builtin_warning (_("%s: circular name reference"), name);
	    }
	  if (value && *value && (aflags & ASS_APPEND) == 0 && valid_nameref_value (value, 1) == 0)
	    {
	      builtin_error (_("`%s': invalid variable name for name reference"), value);
	      assign_error++;
	      NEXT_VARIABLE ();
	    }
	}

restart_new_var_name:

      /* The rest of the loop body deals with declare -[aAlinrtux] name [name...]
	 where each NAME can be an assignment statement. */

      subscript_start = (char *)NULL;	/* used below */
#if defined (ARRAY_VARS)
      /* Determine whether we are creating or assigning an array variable */
      var_exists = array_exists = creating_array = 0;
      compound_array_assign = simple_array_assign = 0;
      array_subscript_assignment = 0;
      if (t = strchr (name, '['))	/* ] */
	{
	  /* If offset != 0 we have already validated any array reference
	     because assignment() calls skipsubscript() */
	  if (offset == 0 && valid_array_reference (name, 0) == 0)
	    {
	      sh_invalidid (name);
	      assign_error++;
	      NEXT_VARIABLE ();
	    }
	  subscript_start = t;
	  *t = '\0';
	  making_array_special = 1;	/* XXX - should this check offset? */
	  array_subscript_assignment = offset != 0;
	}
      else
	making_array_special = 0;
#endif

      /* Ensure the argument is a valid, well-formed shell identifier. */
      if (legal_identifier (name) == 0)
	{
	  sh_invalidid (name);
	  assign_error++;
	  NEXT_VARIABLE ();
	}

      /* If VARIABLE_CONTEXT has a non-zero value, then we are executing
	 inside of a function.  This means we should make local variables,
	 not global ones. */

      /* XXX - this has consequences when we're making a local copy of a
	       variable that was in the temporary environment.  Watch out
	       for this. */
      refvar = (SHELL_VAR *)NULL;
      if (variable_context && mkglobal == 0)
	{
	  /* We don't check newname for validity here. We should not have an
	     invalid name assigned as the value of a nameref, but this could
	     cause problems. */
	  newname = declare_transform_name (name, flags_on, flags_off);

#if defined (ARRAY_VARS)
	  /* Pass 1 as second argument to make_local_{assoc,array}_variable
	     return an existing {array,assoc} variable to be flagged as an
	     error below. */
	  if (flags_on & att_assoc)
	    var = make_local_assoc_variable (newname, MKLOC_ARRAYOK|inherit_flag);
	  else if ((flags_on & att_array) || making_array_special)
	    var = make_local_array_variable (newname, MKLOC_ASSOCOK|inherit_flag);
	  else
#endif
	  if (offset == 0 && (flags_on & att_nameref))
	    {
	      /* First look for refvar at current scope */
	      refvar = find_variable_last_nameref (name, 1);
	      /* VARIABLE_CONTEXT != 0, so we are attempting to create or modify
		 the attributes for a local variable at the same scope.  If we've
		 used a reference from a previous context to resolve VAR, we
		 want to throw REFVAR and VAR away and create a new local var. */
	      if (refvar && refvar->context != variable_context)
		{
		  refvar = 0;
		  var = make_local_variable (name, inherit_flag);
		}
	      else if (refvar && refvar->context == variable_context)
		var = refvar;
	      /* Maybe we just want to create a new local variable */
	      else if ((var = find_variable (name)) == 0 || var->context != variable_context)
		var = make_local_variable (name, inherit_flag);
	      /* otherwise we have a var at the right context */
	    }
	  else
	    /* XXX - check name for validity here with valid_nameref_value? */
	    var = make_local_variable ((flags_on & att_nameref) ? name : newname, inherit_flag);	/* sets att_invisible for new vars */

	  if (var == 0)
	    {
	      any_failed++;
	      NEXT_VARIABLE ();
	    }
	  if (var && nameref_p (var) && readonly_p (var) && nameref_cell (var) && (flags_off & att_nameref))
	    {
	      sh_readonly (name);
	      any_failed++;
	      NEXT_VARIABLE ();
	    }
	}
      else
	var = (SHELL_VAR *)NULL;

      /* VAR is non-null if we just created or fetched a local variable. */

      /* Here's what ksh93 seems to do as of the 2012 version: if we are
	 using declare -n to modify the value of an existing nameref
	 variable, don't follow the nameref chain at all and just search
	 for a nameref at the current context.  If we have a nameref,
	 modify its value (changing which variable it references). */
      if (var == 0 && (flags_on & att_nameref))
	{
	  /* See if we are trying to modify an existing nameref variable,
	     but don't follow the nameref chain. */
	  var = mkglobal ? find_global_variable_noref (name) : find_variable_noref (name);
	  if (var && nameref_p (var) == 0)
	    var = 0;
	}

      /* However, if we're turning off the nameref attribute on an existing
	 nameref variable, we first follow the nameref chain to the end,
	 modify the value of the variable this nameref variable references
	 if there is an assignment statement argument,
	 *CHANGING ITS VALUE AS A SIDE EFFECT*, then turn off the nameref
	 flag *LEAVING THE NAMEREF VARIABLE'S VALUE UNCHANGED* */
      else if (var == 0 && (flags_off & att_nameref))
	{
	  /* See if we are trying to modify an existing nameref variable */
	     refvar = mkglobal ? find_global_variable_last_nameref (name, 0) : find_variable_last_nameref (name, 0);
	  if (refvar && nameref_p (refvar) == 0)
	    refvar = 0;
	  /* If the nameref is readonly but doesn't have a value, ksh93
	     allows the nameref attribute to be removed.  If it's readonly
	     and has a value, even if the value doesn't reference an
	     existing variable, we disallow the modification */
	  if (refvar && nameref_cell (refvar) && readonly_p (refvar))
	    {
	      sh_readonly (name);
	      any_failed++;
	      NEXT_VARIABLE ();
	    }

	  /* If all we're doing is turning off the nameref attribute, don't
	     bother with VAR at all, whether it exists or not. Just turn it
	     off and go on. */
	  if (refvar && flags_on == 0 && offset == 0 && flags_off == att_nameref)
	    {
	      VUNSETATTR (refvar, att_nameref);
	      NEXT_VARIABLE ();
	    }

	  if (refvar)
	    var = declare_find_variable (nameref_cell (refvar), mkglobal, 0);
	}
#if defined (ARRAY_VARS)
      /* If we have an array assignment to a nameref, remove the nameref
	 attribute and go on.  This handles
	 declare -n xref[=value]; declare [-a] xref[1]=one */
      else if (var == 0 && offset && array_subscript_assignment)
	{
	  var = mkglobal ? find_global_variable_noref (name) : find_variable_noref (name);
	  if (var && nameref_p (var))
	    {
	      internal_warning (_("%s: removing nameref attribute"), name);
	      FREE (value_cell (var));		/* XXX - bash-4.3 compat */
	      var_setvalue (var, (char *)NULL);
	      VUNSETATTR (var, att_nameref);
	    }
	}
#endif

      /* See if we are trying to set flags or value (or create) for an
	 existing nameref that points to a non-existent variable: e.g.,
		declare -n foo=bar
		unset foo	# unsets bar
		declare -i foo
		foo=4+4
		declare -p foo
      */
      if (var == 0 && (mkglobal || flags_on || flags_off || offset))
	{
	  refvar = mkglobal ? find_global_variable_last_nameref (name, 0) : find_variable_last_nameref (name, 0);
	  if (refvar && nameref_p (refvar) == 0)
	    refvar = 0;
	  if (refvar)
	    var = declare_find_variable (nameref_cell (refvar), mkglobal, 0);
	  if (refvar && var == 0)
	    {
	      /* I'm not sure subscript_start is ever non-null here. In any
		 event, build a new name from the nameref value, including any
		 subscript, and add the [[+]=value] if offset != 0 */
	      newname = declare_build_newname (nameref_cell (refvar), subscript_start, offset, value, aflags);
	      free (name);
	      name = newname;

	      if (offset)
		{
		  offset = assignment (name, 0);
		  /* If offset was valid previously, but substituting the
		     the nameref value results in an invalid assignment,
		     throw an invalid identifier error */
		  if (offset == 0)
		    {
		      sh_invalidid (name);
		      assign_error++;
		      NEXT_VARIABLE ();
		    }
		  name[(aflags & ASS_APPEND) ? offset - 1 : offset] = '\0';
		  value = name + offset + 1;
		}

	      /* OK, let's turn off the nameref attribute.
		 Now everything else applies to VAR. */
	      if (flags_off & att_nameref)
		VUNSETATTR (refvar, att_nameref);

	      goto restart_new_var_name;
	      /* NOTREACHED */
	    }
	}
      if (var == 0)
	var = declare_find_variable (name, mkglobal, chklocal);

      /* At this point, VAR is the variable we are dealing with; REFVAR is the
	 nameref variable we dereferenced to get VAR, if any. */
#if defined (ARRAY_VARS)
      var_exists = var != 0;
      array_exists = var && (array_p (var) || assoc_p (var));
      creating_array = flags_on & (att_array|att_assoc);
#endif

      /* Make a new variable if we need to. */
      if (var == 0)
	{
#if defined (ARRAY_VARS)
	  if (flags_on & att_assoc)
	    var = make_new_assoc_variable (name);
	  else if ((flags_on & att_array) || making_array_special)
	    var = make_new_array_variable (name);
	  else
#endif
	    var = mkglobal ? bind_global_variable (name, (char *)NULL, ASS_FORCE) : bind_variable (name, (char *)NULL, ASS_FORCE);

	  if (var == 0)
	    {
	      /* Has to appear in brackets */
	      NEXT_VARIABLE ();
	    }
	  if (offset == 0)
	    VSETATTR (var, att_invisible);
	  created_var = 1;
	}

      /* Nameref variable error checking. */

      /* Can't take an existing array variable and make it a nameref */
      else if ((array_p (var) || assoc_p (var)) && (flags_on & att_nameref))
	{
	  builtin_error (_("%s: reference variable cannot be an array"), name);
	  any_failed++;
	  NEXT_VARIABLE ();
	}
      /* Can't have an invalid identifier as nameref value */
      else if (nameref_p (var) && (flags_on & att_nameref) == 0 && (flags_off & att_nameref) == 0 && offset && valid_nameref_value (value, 1) == 0)
	{
	  builtin_error (_("`%s': invalid variable name for name reference"), value);
	  any_failed++;
	  NEXT_VARIABLE ();
	}
      /* Can't make an existing variable a nameref if its current value is not
	 a valid identifier. Check of offset is to allow an assignment to a
	 nameref var as part of the declare word to override existing value. */
      else if ((flags_on & att_nameref) && nameref_p (var) == 0 && var_isset (var) && offset == 0 && valid_nameref_value (value_cell (var), 0) == 0)
	{
	  builtin_error (_("`%s': invalid variable name for name reference"), value_cell (var));
	  any_failed++;
	  NEXT_VARIABLE ();
	}
      /* Can't make an existing readonly variable a nameref. */
      else if ((flags_on & att_nameref) && readonly_p (var))
	{
	  sh_readonly (name);
	  any_failed++;
	  NEXT_VARIABLE ();
	}

      /* Readonly variable error checking. */

      /* Cannot use declare +r to turn off readonly attribute. */ 
      if (readonly_p (var) && (flags_off & att_readonly))
	{
	  sh_readonly (name_cell (var));
	  any_failed++;
	  NEXT_VARIABLE ();
	}
      /* Cannot use declare to assign value to readonly or noassign variable. */
      else if ((readonly_p (var) || noassign_p (var)) && offset)
	{
	  if (readonly_p (var))
	    sh_readonly (name);
	  assign_error++;
	  NEXT_VARIABLE ();
	}

#if defined (ARRAY_VARS)
      /* Array variable error checking. */

      /* Cannot use declare +a name or declare +A name to remove an array variable. */
      if (((flags_off & att_array) && array_p (var)) || ((flags_off & att_assoc) && assoc_p (var)))
	{
	  builtin_error (_("%s: cannot destroy array variables in this way"), name);
	  any_failed++;
	  NEXT_VARIABLE ();
	}
      else if ((flags_on & att_array) && assoc_p (var))
	{
	  builtin_error (_("%s: cannot convert associative to indexed array"), name);
	  any_failed++;
	  NEXT_VARIABLE ();
	}
      else if ((flags_on & att_assoc) && array_p (var))
	{
	  builtin_error (_("%s: cannot convert indexed to associative array"), name);
	  any_failed++;
	  NEXT_VARIABLE ();
	}

      /* make declare A[2]=foo as similar to A[2]=foo as possible if A is
	 already an array or assoc variable. */
      if (array_subscript_assignment && array_exists && creating_array == 0)
	simple_array_assign = 1;
      else if ((making_array_special || creating_array || array_exists) && offset)
	{
	  int vlen;
	  vlen = STRLEN (value);
/*itrace("declare_builtin: name = %s value = %s flags = %d", name, value, wflags);*/

	  if (shell_compatibility_level > 43 && (wflags & W_COMPASSIGN) == 0 &&
		value[0] == '(' && value[vlen-1] == ')')
	    {
	      /* I don't believe this warning is printed any more.
		 We use creating_array to allow things like
		     declare -a foo$bar='(abc)'
		 to work as they have in the past. */
	      if (array_exists == 0 && creating_array == 0)
		internal_warning (_("%s: quoted compound array assignment deprecated"), list->word->word);
	      compound_array_assign = array_exists || creating_array;
	      simple_array_assign = making_array_special;
	    }
	  else if (value[0] == '(' && value[vlen-1] == ')' && (shell_compatibility_level < 44 || (wflags & W_COMPASSIGN)))
	    compound_array_assign = 1;
	  else
	    simple_array_assign = 1;
	}

      /* declare -A name[[n]] makes name an associative array variable. */
      if (flags_on & att_assoc)
	{
	  if (assoc_p (var) == 0)
	    var = convert_var_to_assoc (var);
	}
      /* declare -a name[[n]] or declare name[n] makes NAME an indexed
	 array variable. */
      else if ((making_array_special || (flags_on & att_array)) && array_p (var) == 0 && assoc_p (var) == 0)
	var = convert_var_to_array (var);
#endif /* ARRAY_VARS */

      /* ksh93 compat: turning on nameref attribute turns off -ilu */
      if (flags_on & att_nameref)
	VUNSETATTR (var, att_integer|att_uppercase|att_lowercase|att_capcase);

      /* XXX - we note that we are turning on nameref attribute and defer
	 setting it until the assignment has been made so we don't do an
	 inadvertent nameref lookup.  Might have to do the same thing for
	 flags_off&att_nameref. */
      /* XXX - ksh93 makes it an error to set a readonly nameref variable
	 using a single typeset command. */
      onref = (flags_on & att_nameref);
      flags_on &= ~att_nameref;
#if defined (ARRAY_VARS)
      /* I don't believe this condition ever tests true, but array variables
	 may not be namerefs */
      if (array_p (var) || assoc_p (var) || compound_array_assign || simple_array_assign)
	onref = 0;
#endif

      /* ksh93 seems to do this */
      offref = (flags_off & att_nameref);
      flags_off &= ~att_nameref;

      VSETATTR (var, flags_on);
      VUNSETATTR (var, flags_off);

#if defined (ARRAY_VARS)
      if (offset && compound_array_assign)
	assign_array_var_from_string (var, value, aflags|ASS_FORCE);
      else if (simple_array_assign && subscript_start)
	{
	  int local_aflags;

	  /* declare [-aA] name[N]=value */
	  *subscript_start = '[';	/* ] */
	  /* XXX - problem here with appending */
	  local_aflags = aflags&ASS_APPEND;
	  local_aflags |= assoc_noexpand ? ASS_NOEXPAND : 0;
	  local_aflags |= ASS_ALLOWALLSUB;		/* allow declare a[@]=at */
	  var = assign_array_element (name, value, local_aflags, (array_eltstate_t *)0);	/* XXX - not aflags */
	  *subscript_start = '\0';
	  if (var == 0)	/* some kind of assignment error */
	    {
	      assign_error++;
	      flags_on |= onref;
	      flags_off |= offref;
	      NEXT_VARIABLE ();
	    }
	}
      else if (simple_array_assign)
	{
	  /* let bind_{array,assoc}_variable take care of this. */
	  if (assoc_p (var))
	    bind_assoc_variable (var, name, savestring ("0"), value, aflags|ASS_FORCE);
	  else
	    bind_array_variable (name, 0, value, aflags|ASS_FORCE);
	}
      else
#endif
      /* XXX - no ASS_FORCE here */
      /* bind_variable_value duplicates the essential internals of bind_variable() */
      if (offset)
	{
	  if (onref || nameref_p (var))
	    aflags |= ASS_NAMEREF;
	  v = bind_variable_value (var, value, aflags);
	  if (v == 0 && (onref || nameref_p (var)))
	    {
	      if (valid_nameref_value (value, 1) == 0)
		sh_invalidid (value);
	      assign_error++;
	      /* XXX - unset this variable? or leave it as normal var? */
	      if (created_var)
		delete_var (name_cell (var), mkglobal ? global_variables : shell_variables);
	      flags_on |= onref;		/* undo change from above */
	      flags_off |= offref;
	      NEXT_VARIABLE ();
	    }
	}

      /* If we found this variable in the temporary environment, as with
	 `var=value declare -x var', make sure it is treated identically
	 to `var=value export var'.  Do the same for `declare -r' and
	 `readonly'.  Preserve the attributes, except for att_tempvar. */
      /* XXX -- should this create a variable in the global scope, or
	 modify the local variable flags?  ksh93 has it modify the
	 global scope.
	 Need to handle case like in set_var_attribute where a temporary
	 variable is in the same table as the function local vars. */
      if ((flags_on & (att_exported|att_readonly)) && tempvar_p (var))
	{
	  SHELL_VAR *tv;
	  char *tvalue;

	  tv = find_tempenv_variable (name_cell (var));
	  if (tv)
	    {
	      tvalue = var_isset (var) ? savestring (value_cell (var)) : savestring ("");
	      tv = bind_variable (name_cell (var), tvalue, 0);
	      if (tv)
		{
		  tv->attributes |= var->attributes & ~att_tempvar;
		  if (tv->context > 0)
		    VSETATTR (tv, att_propagate);
		}
	      free (tvalue);
	    }
	  VSETATTR (var, att_propagate);
	}

      /* Turn on nameref attribute we deferred above. */
      VSETATTR (var, onref);
      flags_on |= onref;
      VUNSETATTR (var, offref);
      flags_off |= offref;

      /* Yuck.  ksh93 compatibility.  XXX - need to investigate more but
	 definitely happens when turning off nameref attribute on nameref
	 (see comments above).  Under no circumstances allow this to turn
	 off readonly attribute on readonly nameref variable. */
      if (refvar)
	{
	  if (flags_off & att_readonly)
	    flags_off &= ~att_readonly;
 	  VUNSETATTR (refvar, flags_off);
	}

      stupidly_hack_special_variables (name);

      NEXT_VARIABLE ();
    }

  return (assign_error ? EX_BADASSIGN
		       : ((any_failed == 0) ? EXECUTION_SUCCESS
  					    : EXECUTION_FAILURE));
}
