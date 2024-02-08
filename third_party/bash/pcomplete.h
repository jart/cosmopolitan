/* pcomplete.h - structure definitions and other stuff for programmable
		 completion. */

/* Copyright (C) 1999-2020 Free Software Foundation, Inc.

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

#if !defined (_PCOMPLETE_H_)
#  define _PCOMPLETE_H_

#include "stdc.h"
#include "hashlib.h"

typedef struct compspec {
  int refcount;
  unsigned long actions;
  unsigned long options;
  char *globpat;
  char *words;
  char *prefix;
  char *suffix;
  char *funcname;
  char *command;
  char *lcommand;
  char *filterpat;
} COMPSPEC;

/* Values for COMPSPEC actions.  These are things the shell knows how to
   build internally. */
#define CA_ALIAS	(1<<0)
#define CA_ARRAYVAR	(1<<1)
#define CA_BINDING	(1<<2)
#define CA_BUILTIN	(1<<3)
#define CA_COMMAND	(1<<4)
#define CA_DIRECTORY	(1<<5)
#define CA_DISABLED	(1<<6)
#define CA_ENABLED	(1<<7)
#define CA_EXPORT	(1<<8)
#define CA_FILE		(1<<9)
#define CA_FUNCTION	(1<<10)
#define CA_GROUP	(1<<11)
#define CA_HELPTOPIC	(1<<12)
#define CA_HOSTNAME	(1<<13)
#define CA_JOB		(1<<14)
#define CA_KEYWORD	(1<<15)
#define CA_RUNNING	(1<<16)
#define CA_SERVICE	(1<<17)
#define CA_SETOPT	(1<<18)
#define CA_SHOPT	(1<<19)
#define CA_SIGNAL	(1<<20)
#define CA_STOPPED	(1<<21)
#define CA_USER		(1<<22)
#define CA_VARIABLE	(1<<23)

/* Values for COMPSPEC options field. */
#define COPT_RESERVED	(1<<0)		/* reserved for other use */
#define COPT_DEFAULT	(1<<1)
#define COPT_FILENAMES	(1<<2)
#define COPT_DIRNAMES	(1<<3)
#define COPT_NOQUOTE	(1<<4)
#define COPT_NOSPACE	(1<<5)
#define COPT_BASHDEFAULT (1<<6)
#define COPT_PLUSDIRS	(1<<7)
#define COPT_NOSORT	(1<<8)

#define COPT_LASTUSER	COPT_NOSORT

#define PCOMP_RETRYFAIL (COPT_LASTUSER << 1)
#define PCOMP_NOTFOUND	(COPT_LASTUSER << 2)


/* List of items is used by the code that implements the programmable
   completions. */
typedef struct _list_of_items {
  int flags;
  int (*list_getter) PARAMS((struct _list_of_items *));	/* function to call to get the list */

  STRINGLIST *slist;

  /* These may or may not be used. */
  STRINGLIST *genlist;	/* for handing to the completion code one item at a time */
  int genindex;		/* index of item last handed to completion code */

} ITEMLIST;

/* Values for ITEMLIST -> flags */
#define LIST_DYNAMIC		0x001
#define LIST_DIRTY		0x002
#define LIST_INITIALIZED	0x004
#define LIST_MUSTSORT		0x008
#define LIST_DONTFREE		0x010
#define LIST_DONTFREEMEMBERS	0x020

#define EMPTYCMD	"_EmptycmD_"
#define DEFAULTCMD	"_DefaultCmD_"
#define INITIALWORD	"_InitialWorD_"

extern HASH_TABLE *prog_completes;

extern char *pcomp_line;
extern int pcomp_ind;

extern int prog_completion_enabled;
extern int progcomp_alias;

/* Not all of these are used yet. */
extern ITEMLIST it_aliases;
extern ITEMLIST it_arrayvars;
extern ITEMLIST it_bindings;
extern ITEMLIST it_builtins;
extern ITEMLIST it_commands;
extern ITEMLIST it_directories;
extern ITEMLIST it_disabled;
extern ITEMLIST it_enabled;
extern ITEMLIST it_exports;
extern ITEMLIST it_files;
extern ITEMLIST it_functions;
extern ITEMLIST it_groups;
extern ITEMLIST it_helptopics;
extern ITEMLIST it_hostnames;
extern ITEMLIST it_jobs;
extern ITEMLIST it_keywords;
extern ITEMLIST it_running;
extern ITEMLIST it_services;
extern ITEMLIST it_setopts;
extern ITEMLIST it_shopts;
extern ITEMLIST it_signals;
extern ITEMLIST it_stopped;
extern ITEMLIST it_users;
extern ITEMLIST it_variables;

extern COMPSPEC *pcomp_curcs;
extern const char *pcomp_curcmd;

/* Functions from pcomplib.c */
extern COMPSPEC *compspec_create PARAMS((void));
extern void compspec_dispose PARAMS((COMPSPEC *));
extern COMPSPEC *compspec_copy PARAMS((COMPSPEC *));

extern void progcomp_create PARAMS((void));
extern void progcomp_flush PARAMS((void));
extern void progcomp_dispose PARAMS((void));

extern int progcomp_size PARAMS((void));

extern int progcomp_insert PARAMS((char *, COMPSPEC *));
extern int progcomp_remove PARAMS((char *));

extern COMPSPEC *progcomp_search PARAMS((const char *));

extern void progcomp_walk PARAMS((hash_wfunc *));

/* Functions from pcomplete.c */
extern void set_itemlist_dirty PARAMS((ITEMLIST *));

extern STRINGLIST *completions_to_stringlist PARAMS((char **));

extern STRINGLIST *gen_compspec_completions PARAMS((COMPSPEC *, const char *, const char *, int, int, int *));
extern char **programmable_completions PARAMS((const char *, const char *, int, int, int *));

extern void pcomp_set_readline_variables PARAMS((int, int));
extern void pcomp_set_compspec_options PARAMS((COMPSPEC *, int, int));
#endif /* _PCOMPLETE_H_ */
