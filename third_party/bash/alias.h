/* alias.h -- structure definitions. */

/* Copyright (C) 1987-2020 Free Software Foundation, Inc.

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

#if !defined (_ALIAS_H_)
#define _ALIAS_H_

#include "stdc.h"

#include "hashlib.h"

typedef struct alias {
  char *name;
  char *value;
  char flags;
} alias_t;

/* Values for `flags' member of struct alias. */
#define AL_EXPANDNEXT		0x1
#define AL_BEINGEXPANDED	0x2

/* The list of known aliases. */
extern HASH_TABLE *aliases;

extern void initialize_aliases PARAMS((void));

/* Scan the list of aliases looking for one with NAME.  Return NULL
   if the alias doesn't exist, else a pointer to the alias. */
extern alias_t *find_alias PARAMS((char *));

/* Return the value of the alias for NAME, or NULL if there is none. */
extern char *get_alias_value PARAMS((char *));

/* Make a new alias from NAME and VALUE.  If NAME can be found,
   then replace its value. */
extern void add_alias PARAMS((char *, char *));

/* Remove the alias with name NAME from the alias list.  Returns
   the index of the removed alias, or -1 if the alias didn't exist. */
extern int remove_alias PARAMS((char *));

/* Remove all aliases. */
extern void delete_all_aliases PARAMS((void));

/* Return an array of all defined aliases. */
extern alias_t **all_aliases PARAMS((void));

/* Expand a single word for aliases. */
extern char *alias_expand_word PARAMS((char *));

/* Return a new line, with any aliases expanded. */
extern char *alias_expand PARAMS((char *));

/* Helper definition for the parser */
extern void clear_string_list_expander PARAMS((alias_t *));

#endif /* _ALIAS_H_ */
