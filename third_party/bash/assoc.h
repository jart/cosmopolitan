/* assoc.h -- definitions for the interface exported by assoc.c that allows
   the rest of the shell to manipulate associative array variables. */

/* Copyright (C) 2008,2009-2021 Free Software Foundation, Inc.

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

#ifndef _ASSOC_H_
#define _ASSOC_H_

#include "stdc.h"
#include "hashlib.h"

#define ASSOC_HASH_BUCKETS	1024

#define assoc_empty(h)		((h)->nentries == 0)
#define assoc_num_elements(h)	((h)->nentries)

#define assoc_create(n)		(hash_create((n)))

#define assoc_copy(h)		(hash_copy((h), 0))

#define assoc_walk(h, f)	(hash_walk((h), (f))

extern void assoc_dispose PARAMS((HASH_TABLE *));
extern void assoc_flush PARAMS((HASH_TABLE *));

extern int assoc_insert PARAMS((HASH_TABLE *, char *, char *));
extern PTR_T assoc_replace PARAMS((HASH_TABLE *, char *, char *));
extern void assoc_remove PARAMS((HASH_TABLE *, char *));

extern char *assoc_reference PARAMS((HASH_TABLE *, char *));

extern char *assoc_subrange PARAMS((HASH_TABLE *, arrayind_t, arrayind_t, int, int, int));
extern char *assoc_patsub PARAMS((HASH_TABLE *, char *, char *, int));
extern char *assoc_modcase PARAMS((HASH_TABLE *, char *, int, int));

extern HASH_TABLE *assoc_quote PARAMS((HASH_TABLE *));
extern HASH_TABLE *assoc_quote_escapes PARAMS((HASH_TABLE *));
extern HASH_TABLE *assoc_dequote PARAMS((HASH_TABLE *));
extern HASH_TABLE *assoc_dequote_escapes PARAMS((HASH_TABLE *));
extern HASH_TABLE *assoc_remove_quoted_nulls PARAMS((HASH_TABLE *));

extern char *assoc_to_kvpair PARAMS((HASH_TABLE *, int));
extern char *assoc_to_assign PARAMS((HASH_TABLE *, int));

extern WORD_LIST *assoc_to_word_list PARAMS((HASH_TABLE *));
extern WORD_LIST *assoc_keys_to_word_list PARAMS((HASH_TABLE *));
extern WORD_LIST *assoc_to_kvpair_list PARAMS((HASH_TABLE *));

extern char *assoc_to_string PARAMS((HASH_TABLE *, char *, int));
#endif /* _ASSOC_H_ */
