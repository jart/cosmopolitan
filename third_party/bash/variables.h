/* variables.h -- data structures for shell variables. */

/* Copyright (C) 1987-2022 Free Software Foundation, Inc.

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

#if !defined (_VARIABLES_H_)
#define _VARIABLES_H_

#include "stdc.h"
#include "array.h"
#include "assoc.h"

/* Shell variables and functions are stored in hash tables. */
#include "hashlib.h"

#include "conftypes.h"

/* A variable context. */
typedef struct var_context {
  char *name;		/* empty or NULL means global context */
  int scope;		/* 0 means global context */
  int flags;
  struct var_context *up;	/* previous function calls */
  struct var_context *down;	/* down towards global context */
  HASH_TABLE *table;		/* variables at this scope */
} VAR_CONTEXT;

/* Flags for var_context->flags */
#define VC_HASLOCAL	0x01
#define VC_HASTMPVAR	0x02
#define VC_FUNCENV	0x04	/* also function if name != NULL */
#define VC_BLTNENV	0x08	/* builtin_env */
#define VC_TEMPENV	0x10	/* temporary_env */

#define VC_TEMPFLAGS	(VC_FUNCENV|VC_BLTNENV|VC_TEMPENV)

/* Accessing macros */
#define vc_isfuncenv(vc)	(((vc)->flags & VC_FUNCENV) != 0)
#define vc_isbltnenv(vc)	(((vc)->flags & VC_BLTNENV) != 0)
#define vc_istempenv(vc)	(((vc)->flags & (VC_TEMPFLAGS)) == VC_TEMPENV)

#define vc_istempscope(vc)	(((vc)->flags & (VC_TEMPENV|VC_BLTNENV)) != 0)

#define vc_haslocals(vc)	(((vc)->flags & VC_HASLOCAL) != 0)
#define vc_hastmpvars(vc)	(((vc)->flags & VC_HASTMPVAR) != 0)

/* What a shell variable looks like. */

typedef struct variable *sh_var_value_func_t PARAMS((struct variable *));
typedef struct variable *sh_var_assign_func_t PARAMS((struct variable *, char *, arrayind_t, char *));

/* For the future */
union _value {
  char *s;			/* string value */
  intmax_t i;			/* int value */
  COMMAND *f;			/* function */
  ARRAY *a;			/* array */
  HASH_TABLE *h;		/* associative array */
  double d;			/* floating point number */
#if defined (HAVE_LONG_DOUBLE)
  long double ld;		/* long double */
#endif
  struct variable *v;		/* possible indirect variable use */
  void *opaque;			/* opaque data for future use */
};

typedef struct variable {
  char *name;			/* Symbol that the user types. */
  char *value;			/* Value that is returned. */
  char *exportstr;		/* String for the environment. */
  sh_var_value_func_t *dynamic_value;	/* Function called to return a `dynamic'
				   value for a variable, like $SECONDS
				   or $RANDOM. */
  sh_var_assign_func_t *assign_func; /* Function called when this `special
				   variable' is assigned a value in
				   bind_variable. */
  int attributes;		/* export, readonly, array, invisible... */
  int context;			/* Which context this variable belongs to. */
} SHELL_VAR;

typedef struct _vlist {
  SHELL_VAR **list;
  int list_size;	/* allocated size */
  int list_len;		/* current number of entries */
} VARLIST;

/* The various attributes that a given variable can have. */
/* First, the user-visible attributes */
#define att_exported	0x0000001	/* export to environment */
#define att_readonly	0x0000002	/* cannot change */
#define att_array	0x0000004	/* value is an array */
#define att_function	0x0000008	/* value is a function */
#define att_integer	0x0000010	/* internal representation is int */
#define att_local	0x0000020	/* variable is local to a function */
#define att_assoc	0x0000040	/* variable is an associative array */
#define att_trace	0x0000080	/* function is traced with DEBUG trap */
#define att_uppercase	0x0000100	/* word converted to uppercase on assignment */
#define att_lowercase	0x0000200	/* word converted to lowercase on assignment */
#define att_capcase	0x0000400	/* word capitalized on assignment */
#define att_nameref	0x0000800	/* word is a name reference */

#define user_attrs	(att_exported|att_readonly|att_integer|att_local|att_trace|att_uppercase|att_lowercase|att_capcase|att_nameref)

#define attmask_user	0x0000fff

/* Internal attributes used for bookkeeping */
#define att_invisible	0x0001000	/* cannot see */
#define att_nounset	0x0002000	/* cannot unset */
#define att_noassign	0x0004000	/* assignment not allowed */
#define att_imported	0x0008000	/* came from environment */
#define att_special	0x0010000	/* requires special handling */
#define att_nofree	0x0020000	/* do not free value on unset */
#define att_regenerate	0x0040000	/* regenerate when exported */

#define	attmask_int	0x00ff000

/* Internal attributes used for variable scoping. */
#define att_tempvar	0x0100000	/* variable came from the temp environment */
#define att_propagate	0x0200000	/* propagate to previous scope */

#define attmask_scope	0x0f00000

#define exported_p(var)		((((var)->attributes) & (att_exported)))
#define readonly_p(var)		((((var)->attributes) & (att_readonly)))
#define array_p(var)		((((var)->attributes) & (att_array)))
#define function_p(var)		((((var)->attributes) & (att_function)))
#define integer_p(var)		((((var)->attributes) & (att_integer)))
#define local_p(var)		((((var)->attributes) & (att_local)))
#define assoc_p(var)		((((var)->attributes) & (att_assoc)))
#define trace_p(var)		((((var)->attributes) & (att_trace)))
#define uppercase_p(var)	((((var)->attributes) & (att_uppercase)))
#define lowercase_p(var)	((((var)->attributes) & (att_lowercase)))
#define capcase_p(var)		((((var)->attributes) & (att_capcase)))
#define nameref_p(var)		((((var)->attributes) & (att_nameref)))

#define invisible_p(var)	((((var)->attributes) & (att_invisible)))
#define non_unsettable_p(var)	((((var)->attributes) & (att_nounset)))
#define noassign_p(var)		((((var)->attributes) & (att_noassign)))
#define imported_p(var)		((((var)->attributes) & (att_imported)))
#define specialvar_p(var)	((((var)->attributes) & (att_special)))
#define nofree_p(var)		((((var)->attributes) & (att_nofree)))
#define regen_p(var)		((((var)->attributes) & (att_regenerate)))

#define tempvar_p(var)		((((var)->attributes) & (att_tempvar)))
#define propagate_p(var)	((((var)->attributes) & (att_propagate)))

/* Variable names: lvalues */
#define name_cell(var)		((var)->name)

/* Accessing variable values: rvalues */
#define value_cell(var)		((var)->value)
#define function_cell(var)	(COMMAND *)((var)->value)
#define array_cell(var)		(ARRAY *)((var)->value)
#define assoc_cell(var)		(HASH_TABLE *)((var)->value)
#define nameref_cell(var)	((var)->value)		/* so it can change later */

#define NAMEREF_MAX	8	/* only 8 levels of nameref indirection */

#define var_isset(var)		((var)->value != 0)
#define var_isunset(var)	((var)->value == 0)
#define var_isnull(var)		((var)->value && *(var)->value == 0)

/* Assigning variable values: lvalues */
#define var_setvalue(var, str)	((var)->value = (str))
#define var_setfunc(var, func)	((var)->value = (char *)(func))
#define var_setarray(var, arr)	((var)->value = (char *)(arr))
#define var_setassoc(var, arr)	((var)->value = (char *)(arr))
#define var_setref(var, str)	((var)->value = (str))

/* Make VAR be auto-exported. */
#define set_auto_export(var) \
  do { (var)->attributes |= att_exported; array_needs_making = 1; } while (0)

#define SETVARATTR(var, attr, undo) \
	((undo == 0) ? ((var)->attributes |= (attr)) \
		     : ((var)->attributes &= ~(attr)))

#define VSETATTR(var, attr)	((var)->attributes |= (attr))
#define VUNSETATTR(var, attr)	((var)->attributes &= ~(attr))

#define VGETFLAGS(var)		((var)->attributes)

#define VSETFLAGS(var, flags)	((var)->attributes = (flags))
#define VCLRFLAGS(var)		((var)->attributes = 0)

/* Macros to perform various operations on `exportstr' member of a SHELL_VAR. */
#define CLEAR_EXPORTSTR(var)	(var)->exportstr = (char *)NULL
#define COPY_EXPORTSTR(var)	((var)->exportstr) ? savestring ((var)->exportstr) : (char *)NULL
#define SET_EXPORTSTR(var, value)  (var)->exportstr = (value)
#define SAVE_EXPORTSTR(var, value) (var)->exportstr = (value) ? savestring (value) : (char *)NULL

#define FREE_EXPORTSTR(var) \
	do { if ((var)->exportstr) free ((var)->exportstr); } while (0)

#define CACHE_IMPORTSTR(var, value) \
	(var)->exportstr = savestring (value)

#define INVALIDATE_EXPORTSTR(var) \
	do { \
	  if ((var)->exportstr) \
	    { \
	      free ((var)->exportstr); \
	      (var)->exportstr = (char *)NULL; \
	    } \
	} while (0)

#define ifsname(s)	((s)[0] == 'I' && (s)[1] == 'F' && (s)[2] == 'S' && (s)[3] == '\0')

/* Flag values for make_local_variable and its array counterparts */
#define MKLOC_ASSOCOK		0x01
#define MKLOC_ARRAYOK		0x02
#define MKLOC_INHERIT		0x04

/* Special value for nameref with invalid value for creation or assignment */
extern SHELL_VAR nameref_invalid_value;
#define INVALID_NAMEREF_VALUE	(void *)&nameref_invalid_value
	
/* Stuff for hacking variables. */
typedef int sh_var_map_func_t PARAMS((SHELL_VAR *));

/* Where we keep the variables and functions */
extern VAR_CONTEXT *global_variables;
extern VAR_CONTEXT *shell_variables;

extern HASH_TABLE *shell_functions;
extern HASH_TABLE *temporary_env;

extern int variable_context;
extern char *dollar_vars[];
extern char **export_env;

extern int tempenv_assign_error;
extern int array_needs_making;
extern int shell_level;

/* XXX */
extern WORD_LIST *rest_of_args;
extern int posparam_count;
extern pid_t dollar_dollar_pid;

extern int localvar_inherit;		/* declared in variables.c */

extern void initialize_shell_variables PARAMS((char **, int));

extern int validate_inherited_value PARAMS((SHELL_VAR *, int));

extern SHELL_VAR *set_if_not PARAMS((char *, char *));

extern void sh_set_lines_and_columns PARAMS((int, int));
extern void set_pwd PARAMS((void));
extern void set_ppid PARAMS((void));
extern void make_funcname_visible PARAMS((int));

extern SHELL_VAR *var_lookup PARAMS((const char *, VAR_CONTEXT *));

extern SHELL_VAR *find_function PARAMS((const char *));
extern FUNCTION_DEF *find_function_def PARAMS((const char *));
extern SHELL_VAR *find_variable PARAMS((const char *));
extern SHELL_VAR *find_variable_noref PARAMS((const char *));
extern SHELL_VAR *find_variable_last_nameref PARAMS((const char *, int));
extern SHELL_VAR *find_global_variable_last_nameref PARAMS((const char *, int));
extern SHELL_VAR *find_variable_nameref PARAMS((SHELL_VAR *));
extern SHELL_VAR *find_variable_nameref_for_create PARAMS((const char *, int));
extern SHELL_VAR *find_variable_nameref_for_assignment PARAMS((const char *, int));
/*extern SHELL_VAR *find_variable_internal PARAMS((const char *, int));*/
extern SHELL_VAR *find_variable_tempenv PARAMS((const char *));
extern SHELL_VAR *find_variable_notempenv PARAMS((const char *));
extern SHELL_VAR *find_global_variable PARAMS((const char *));
extern SHELL_VAR *find_global_variable_noref PARAMS((const char *));
extern SHELL_VAR *find_shell_variable PARAMS((const char *));
extern SHELL_VAR *find_tempenv_variable PARAMS((const char *));
extern SHELL_VAR *find_variable_no_invisible PARAMS((const char *));
extern SHELL_VAR *find_variable_for_assignment PARAMS((const char *));
extern char *nameref_transform_name PARAMS((char *, int));
extern SHELL_VAR *copy_variable PARAMS((SHELL_VAR *));
extern SHELL_VAR *make_local_variable PARAMS((const char *, int));
extern SHELL_VAR *bind_variable PARAMS((const char *, char *, int));
extern SHELL_VAR *bind_global_variable PARAMS((const char *, char *, int));
extern SHELL_VAR *bind_function PARAMS((const char *, COMMAND *));

extern void bind_function_def PARAMS((const char *, FUNCTION_DEF *, int));

extern SHELL_VAR **map_over PARAMS((sh_var_map_func_t *, VAR_CONTEXT *));
SHELL_VAR **map_over_funcs PARAMS((sh_var_map_func_t *));
     
extern SHELL_VAR **all_shell_variables PARAMS((void));
extern SHELL_VAR **all_shell_functions PARAMS((void));
extern SHELL_VAR **all_visible_variables PARAMS((void));
extern SHELL_VAR **all_visible_functions PARAMS((void));
extern SHELL_VAR **all_exported_variables PARAMS((void));
extern SHELL_VAR **local_exported_variables PARAMS((void));
extern SHELL_VAR **all_local_variables PARAMS((int));
#if defined (ARRAY_VARS)
extern SHELL_VAR **all_array_variables PARAMS((void));
#endif
extern char **all_variables_matching_prefix PARAMS((const char *));

extern char **make_var_array PARAMS((HASH_TABLE *));
extern char **add_or_supercede_exported_var PARAMS((char *, int));

extern char *get_variable_value PARAMS((SHELL_VAR *));
extern char *get_string_value PARAMS((const char *));
extern char *sh_get_env_value PARAMS((const char *));
extern char *make_variable_value PARAMS((SHELL_VAR *, char *, int));

extern SHELL_VAR *bind_variable_value PARAMS((SHELL_VAR *, char *, int));
extern SHELL_VAR *bind_int_variable PARAMS((char *, char *, int));
extern SHELL_VAR *bind_var_to_int PARAMS((char *, intmax_t, int));

extern int assign_in_env PARAMS((WORD_DESC *, int));

extern int unbind_variable PARAMS((const char *));
extern int check_unbind_variable PARAMS((const char *));
extern int unbind_nameref PARAMS((const char *));
extern int unbind_variable_noref PARAMS((const char *));
extern int unbind_global_variable PARAMS((const char *));
extern int unbind_global_variable_noref PARAMS((const char *));
extern int unbind_func PARAMS((const char *));
extern int unbind_function_def PARAMS((const char *));
extern int delete_var PARAMS((const char *, VAR_CONTEXT *));
extern int makunbound PARAMS((const char *, VAR_CONTEXT *));
extern int kill_local_variable PARAMS((const char *));

extern void delete_all_variables PARAMS((HASH_TABLE *));
extern void delete_all_contexts PARAMS((VAR_CONTEXT *));
extern void reset_local_contexts PARAMS((void));

extern VAR_CONTEXT *new_var_context PARAMS((char *, int));
extern void dispose_var_context PARAMS((VAR_CONTEXT *));
extern VAR_CONTEXT *push_var_context PARAMS((char *, int, HASH_TABLE *));
extern void pop_var_context PARAMS((void));
extern VAR_CONTEXT *push_scope PARAMS((int, HASH_TABLE *));
extern void pop_scope PARAMS((int));

extern void clear_dollar_vars PARAMS((void));

extern void push_context PARAMS((char *, int, HASH_TABLE *));
extern void pop_context PARAMS((void));
extern void push_dollar_vars PARAMS((void));
extern void pop_dollar_vars PARAMS((void));
extern void dispose_saved_dollar_vars PARAMS((void));

extern void init_bash_argv PARAMS((void));
extern void save_bash_argv PARAMS((void));
extern void push_args PARAMS((WORD_LIST *));
extern void pop_args PARAMS((void));

extern void adjust_shell_level PARAMS((int));
extern void non_unsettable PARAMS((char *));
extern void dispose_variable PARAMS((SHELL_VAR *));
extern void dispose_used_env_vars PARAMS((void));
extern void dispose_function_env PARAMS((void));
extern void dispose_builtin_env PARAMS((void));
extern void merge_temporary_env PARAMS((void));
extern void flush_temporary_env PARAMS((void));
extern void merge_builtin_env PARAMS((void));
extern void kill_all_local_variables PARAMS((void));

extern void set_var_read_only PARAMS((char *));
extern void set_func_read_only PARAMS((const char *));
extern void set_var_auto_export PARAMS((char *));
extern void set_func_auto_export PARAMS((const char *));

extern void sort_variables PARAMS((SHELL_VAR **));

extern int chkexport PARAMS((char *));
extern void maybe_make_export_env PARAMS((void));
extern void update_export_env_inplace PARAMS((char *, int, char *));
extern void put_command_name_into_env PARAMS((char *));
extern void put_gnu_argv_flags_into_env PARAMS((intmax_t, char *));

extern void print_var_list PARAMS((SHELL_VAR **));
extern void print_func_list PARAMS((SHELL_VAR **));
extern void print_assignment PARAMS((SHELL_VAR *));
extern void print_var_value PARAMS((SHELL_VAR *, int));
extern void print_var_function PARAMS((SHELL_VAR *));

#if defined (ARRAY_VARS)
extern SHELL_VAR *make_new_array_variable PARAMS((char *));
extern SHELL_VAR *make_local_array_variable PARAMS((char *, int));

extern SHELL_VAR *make_new_assoc_variable PARAMS((char *));
extern SHELL_VAR *make_local_assoc_variable PARAMS((char *, int));

extern void set_pipestatus_array PARAMS((int *, int));
extern ARRAY *save_pipestatus_array PARAMS((void));
extern void restore_pipestatus_array PARAMS((ARRAY *));
#endif

extern void set_pipestatus_from_exit PARAMS((int));

/* The variable in NAME has just had its state changed.  Check to see if it
   is one of the special ones where something special happens. */
extern void stupidly_hack_special_variables PARAMS((char *));

/* Reinitialize some special variables that have external effects upon unset
   when the shell reinitializes itself. */
extern void reinit_special_variables PARAMS((void));

extern int get_random_number PARAMS((void));

/* The `special variable' functions that get called when a particular
   variable is set. */
extern void sv_ifs PARAMS((char *));
extern void sv_path PARAMS((char *));
extern void sv_mail PARAMS((char *));
extern void sv_funcnest PARAMS((char *));
extern void sv_execignore PARAMS((char *));
extern void sv_globignore PARAMS((char *));
extern void sv_ignoreeof PARAMS((char *));
extern void sv_strict_posix PARAMS((char *));
extern void sv_optind PARAMS((char *));
extern void sv_opterr PARAMS((char *));
extern void sv_locale PARAMS((char *));
extern void sv_xtracefd PARAMS((char *));
extern void sv_shcompat PARAMS((char *));

#if defined (READLINE)
extern void sv_comp_wordbreaks PARAMS((char *));
extern void sv_terminal PARAMS((char *));
extern void sv_hostfile PARAMS((char *));
extern void sv_winsize PARAMS((char *));
#endif

#if defined (__CYGWIN__)
extern void sv_home PARAMS((char *));
#endif

#if defined (HISTORY)
extern void sv_histsize PARAMS((char *));
extern void sv_histignore PARAMS((char *));
extern void sv_history_control PARAMS((char *));
#  if defined (BANG_HISTORY)
extern void sv_histchars PARAMS((char *));
#  endif
extern void sv_histtimefmt PARAMS((char *));
#endif /* HISTORY */

#if defined (HAVE_TZSET)
extern void sv_tz PARAMS((char *));
#endif

#if defined (JOB_CONTROL)
extern void sv_childmax PARAMS((char *));
#endif

#endif /* !_VARIABLES_H_ */
