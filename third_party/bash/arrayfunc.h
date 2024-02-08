/* arrayfunc.h -- declarations for miscellaneous array functions in arrayfunc.c */

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

#if !defined (_ARRAYFUNC_H_)
#define _ARRAYFUNC_H_

/* Must include variables.h before including this file. */

/* An object to encapsulate the state of an array element. It can describe
   an array assignment A[KEY]=VALUE or a[IND]=VALUE depending on TYPE, or
   for passing array subscript references around, where VALUE would be
   ${a[IND]} or ${A[KEY]}.  This is not dependent on ARRAY_VARS so we can
   use it in function parameters. */

/* values for `type' field */
#define ARRAY_INVALID	-1
#define ARRAY_SCALAR	0
#define ARRAY_INDEXED	1
#define ARRAY_ASSOC	2

/* KEY will contain allocated memory if called through the assign_array_element
   code path because of how assoc_insert works. */
typedef struct element_state
{
  short type;			/* assoc or indexed, says which fields are valid */  
  short subtype;		/* `*', `@', or something else */
  arrayind_t ind;
  char *key;			/* can be allocated memory */
  char *value;
} array_eltstate_t;

#if defined (ARRAY_VARS)

/* This variable means to not expand associative array subscripts more than
   once, when performing variable expansion. */
extern int assoc_expand_once;

/* The analog for indexed array subscripts */
extern int array_expand_once;

/* Flags for array_value_internal and callers array_value/get_array_value; also
   used by array_variable_name and array_variable_part. */
#define AV_ALLOWALL	0x001	/* treat a[@] like $@ and a[*] like $* */
#define AV_QUOTED	0x002
#define AV_USEIND	0x004
#define AV_USEVAL	0x008	/* XXX - should move this */
#define AV_ASSIGNRHS	0x010	/* no splitting, special case ${a[@]} */
#define AV_NOEXPAND	0x020	/* don't run assoc subscripts through word expansion */
#define AV_ONEWORD	0x040	/* not used yet */
#define AV_ATSTARKEYS	0x080	/* accept a[@] and a[*] but use them as keys, not special values */

/* Flags for valid_array_reference. Value 1 is reserved for skipsubscript().
   Also used by unbind_array_element, which is currently the only function
   that uses VA_ALLOWALL. */
#define VA_NOEXPAND	0x001
#define VA_ONEWORD	0x002
#define VA_ALLOWALL	0x004	/* allow @ to mean all elements of the array */

extern SHELL_VAR *convert_var_to_array PARAMS((SHELL_VAR *));
extern SHELL_VAR *convert_var_to_assoc PARAMS((SHELL_VAR *));

extern char *make_array_variable_value PARAMS((SHELL_VAR *, arrayind_t, char *, char *, int));

extern SHELL_VAR *bind_array_variable PARAMS((char *, arrayind_t, char *, int));
extern SHELL_VAR *bind_array_element PARAMS((SHELL_VAR *, arrayind_t, char *, int));
extern SHELL_VAR *assign_array_element PARAMS((char *, char *, int, array_eltstate_t *));

extern SHELL_VAR *bind_assoc_variable PARAMS((SHELL_VAR *, char *, char *, char *, int));

extern SHELL_VAR *find_or_make_array_variable PARAMS((char *, int));

extern SHELL_VAR *assign_array_from_string  PARAMS((char *, char *, int));
extern SHELL_VAR *assign_array_var_from_word_list PARAMS((SHELL_VAR *, WORD_LIST *, int));

extern WORD_LIST *expand_compound_array_assignment PARAMS((SHELL_VAR *, char *, int));
extern void assign_compound_array_list PARAMS((SHELL_VAR *, WORD_LIST *, int));
extern SHELL_VAR *assign_array_var_from_string PARAMS((SHELL_VAR *, char *, int));

extern char *expand_and_quote_assoc_word PARAMS((char *, int));
extern void quote_compound_array_list PARAMS((WORD_LIST *, int));

extern int kvpair_assignment_p PARAMS((WORD_LIST *));
extern char *expand_and_quote_kvpair_word PARAMS((char *));

extern int unbind_array_element PARAMS((SHELL_VAR *, char *, int));
extern int skipsubscript PARAMS((const char *, int, int));

extern void print_array_assignment PARAMS((SHELL_VAR *, int));
extern void print_assoc_assignment PARAMS((SHELL_VAR *, int));

extern arrayind_t array_expand_index PARAMS((SHELL_VAR *, char *, int, int));
extern int valid_array_reference PARAMS((const char *, int));
extern int tokenize_array_reference PARAMS((char *, int, char **));

extern char *array_value PARAMS((const char *, int, int, array_eltstate_t *));
extern char *get_array_value PARAMS((const char *, int, array_eltstate_t *));

extern char *array_keys PARAMS((char *, int, int));

extern char *array_variable_name PARAMS((const char *, int, char **, int *));
extern SHELL_VAR *array_variable_part PARAMS((const char *, int, char **, int *));

extern void init_eltstate (array_eltstate_t *);
extern void flush_eltstate (array_eltstate_t *);

#else

#define AV_ALLOWALL	0
#define AV_QUOTED	0
#define AV_USEIND	0
#define AV_USEVAL	0
#define AV_ASSIGNRHS	0
#define AV_NOEXPAND	0
#define AV_ONEWORD	0
#define AV_ATSTARKEYS	0

#define VA_NOEXPAND	0
#define VA_ONEWORD	0
#define VA_ALLOWALL	0

#endif

#endif /* !_ARRAYFUNC_H_ */
