/* externs.h -- extern function declarations which do not appear in their
   own header file. */

/* Copyright (C) 1993-2021 Free Software Foundation, Inc.

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

/* Make sure that this is included *after* config.h! */

#if !defined (_EXTERNS_H_)
#  define _EXTERNS_H_

#include "stdc.h"

/* Functions from expr.c. */
#define EXP_EXPANDED	0x01

extern intmax_t evalexp PARAMS((char *, int, int *));

/* Functions from print_cmd.c. */
#define FUNC_MULTILINE	0x01
#define FUNC_EXTERNAL	0x02

extern char *make_command_string PARAMS((COMMAND *));
extern char *print_comsub PARAMS((COMMAND *));
extern char *named_function_string PARAMS((char *, COMMAND *, int));

extern void print_command PARAMS((COMMAND *));
extern void print_simple_command PARAMS((SIMPLE_COM *));
extern void print_word_list PARAMS((WORD_LIST *, char *));

/* debugger support */
extern void print_for_command_head PARAMS((FOR_COM *));
#if defined (SELECT_COMMAND)
extern void print_select_command_head PARAMS((SELECT_COM *));
#endif
extern void print_case_command_head PARAMS((CASE_COM *));
#if defined (DPAREN_ARITHMETIC)
extern void print_arith_command PARAMS((WORD_LIST *));
#endif
#if defined (COND_COMMAND)
extern void print_cond_command PARAMS((COND_COM *));
#endif

/* set -x support */
extern void xtrace_init PARAMS((void));
#ifdef NEED_XTRACE_SET_DECL
extern void xtrace_set PARAMS((int, FILE *));
#endif
extern void xtrace_fdchk PARAMS((int));
extern void xtrace_reset PARAMS((void));
extern char *indirection_level_string PARAMS((void));
extern void xtrace_print_assignment PARAMS((char *, char *, int, int));
extern void xtrace_print_word_list PARAMS((WORD_LIST *, int));
extern void xtrace_print_for_command_head PARAMS((FOR_COM *));
#if defined (SELECT_COMMAND)
extern void xtrace_print_select_command_head PARAMS((SELECT_COM *));
#endif
extern void xtrace_print_case_command_head PARAMS((CASE_COM *));
#if defined (DPAREN_ARITHMETIC)
extern void xtrace_print_arith_cmd PARAMS((WORD_LIST *));
#endif
#if defined (COND_COMMAND)
extern void xtrace_print_cond_term PARAMS((int, int, WORD_DESC *, char *, char *));
#endif

/* Functions from shell.c. */
extern void exit_shell PARAMS((int)) __attribute__((__noreturn__));
extern void sh_exit PARAMS((int)) __attribute__((__noreturn__));
extern void subshell_exit PARAMS((int)) __attribute__((__noreturn__));
extern void set_exit_status PARAMS((int));
extern void disable_priv_mode PARAMS((void));
extern void unbind_args PARAMS((void));

#if defined (RESTRICTED_SHELL)
extern int shell_is_restricted PARAMS((char *));
extern int maybe_make_restricted PARAMS((char *));
#endif

extern void unset_bash_input PARAMS((int));
extern void get_current_user_info PARAMS((void));

/* Functions from eval.c. */
extern int reader_loop PARAMS((void));
extern int pretty_print_loop PARAMS((void));
extern int parse_command PARAMS((void));
extern int read_command PARAMS((void));

/* Functions from braces.c. */
#if defined (BRACE_EXPANSION)
extern char **brace_expand PARAMS((char *));
#endif

/* Miscellaneous functions from parse.y */
extern int yyparse PARAMS((void));
extern int return_EOF PARAMS((void));
extern void push_token PARAMS((int));
extern char *xparse_dolparen PARAMS((char *, char *, int *, int));
extern COMMAND *parse_string_to_command PARAMS((char *, int));
extern void reset_parser PARAMS((void));
extern void reset_readahead_token PARAMS((void));
extern WORD_LIST *parse_string_to_word_list PARAMS((char *, int, const char *));

extern int parser_will_prompt PARAMS((void));
extern int parser_in_command_position PARAMS((void));

extern void free_pushed_string_input PARAMS((void));

extern int parser_expanding_alias PARAMS((void));
extern void parser_save_alias PARAMS((void));
extern void parser_restore_alias PARAMS((void));

extern void clear_shell_input_line PARAMS((void));

extern char *decode_prompt_string PARAMS((char *));

extern int get_current_prompt_level PARAMS((void));
extern void set_current_prompt_level PARAMS((int));

#if defined (HISTORY)
extern char *history_delimiting_chars PARAMS((const char *));
#endif

/* Declarations for functions defined in locale.c */
extern void set_default_locale PARAMS((void));
extern void set_default_locale_vars PARAMS((void));
extern int set_locale_var PARAMS((char *, char *));
extern int set_lang PARAMS((char *, char *));
extern void set_default_lang PARAMS((void));
extern char *get_locale_var PARAMS((char *));
extern char *localetrans PARAMS((char *, int, int *));
extern char *mk_msgstr PARAMS((char *, int *));
extern char *locale_expand PARAMS((char *, int, int, int, int *));
#ifndef locale_decpoint
extern int locale_decpoint PARAMS((void));
#endif

/* Declarations for functions defined in list.c. */
extern void list_walk PARAMS((GENERIC_LIST *, sh_glist_func_t *));
extern void wlist_walk PARAMS((WORD_LIST *, sh_icpfunc_t *));
extern GENERIC_LIST *list_reverse ();
extern int list_length ();
extern GENERIC_LIST *list_append ();
extern GENERIC_LIST *list_remove ();

/* Declarations for functions defined in stringlib.c */
extern int find_string_in_alist PARAMS((char *, STRING_INT_ALIST *, int));
extern char *find_token_in_alist PARAMS((int, STRING_INT_ALIST *, int));
extern int find_index_in_alist PARAMS((char *, STRING_INT_ALIST *, int));

extern char *substring PARAMS((const char *, int, int));
extern char *strsub PARAMS((char *, char *, char *, int));
extern char *strcreplace PARAMS((char *, int, const char *, int));
extern void strip_leading PARAMS((char *));
extern void strip_trailing PARAMS((char *, int, int));
extern void xbcopy PARAMS((char *, char *, int));

/* Functions from version.c. */
extern char *shell_version_string PARAMS((void));
extern void show_shell_version PARAMS((int));

/* Functions from the bash library, lib/sh/libsh.a.  These should really
   go into a separate include file. */

/* declarations for functions defined in lib/sh/casemod.c */
extern char *sh_modcase PARAMS((const char *, char *, int));

/* Defines for flags argument to sh_modcase.  These need to agree with what's
   in lib/sh/casemode.c */
#define CASE_LOWER	0x0001
#define CASE_UPPER	0x0002
#define CASE_CAPITALIZE	0x0004
#define CASE_UNCAP	0x0008
#define CASE_TOGGLE	0x0010
#define CASE_TOGGLEALL	0x0020
#define CASE_UPFIRST	0x0040
#define CASE_LOWFIRST	0x0080

#define CASE_USEWORDS	0x1000

/* declarations for functions defined in lib/sh/clktck.c */
extern long get_clk_tck PARAMS((void));

/* declarations for functions defined in lib/sh/clock.c */
extern void clock_t_to_secs ();
extern void print_clock_t ();

/* Declarations for functions defined in lib/sh/dprintf.c */
#if !defined (HAVE_DPRINTF)
extern void dprintf PARAMS((int, const char *, ...))  __attribute__((__format__ (printf, 2, 3)));
#endif

/* Declarations for functions defined in lib/sh/fmtulong.c */
#define FL_PREFIX     0x01    /* add 0x, 0X, or 0 prefix as appropriate */
#define FL_ADDBASE    0x02    /* add base# prefix to converted value */
#define FL_HEXUPPER   0x04    /* use uppercase when converting to hex */
#define FL_UNSIGNED   0x08    /* don't add any sign */

extern char *fmtulong PARAMS((unsigned long int, int, char *, size_t, int));

/* Declarations for functions defined in lib/sh/fmtulong.c */
#if defined (HAVE_LONG_LONG_INT)
extern char *fmtullong PARAMS((unsigned long long int, int, char *, size_t, int));
#endif

/* Declarations for functions defined in lib/sh/fmtumax.c */
extern char *fmtumax PARAMS((uintmax_t, int, char *, size_t, int));

/* Declarations for functions defined in lib/sh/fnxform.c */
extern char *fnx_fromfs PARAMS((char *, size_t));
extern char *fnx_tofs PARAMS((char *, size_t));

/* Declarations for functions defined in lib/sh/fpurge.c */

#if defined NEED_FPURGE_DECL
#if !HAVE_DECL_FPURGE

#if HAVE_FPURGE
#  define fpurge _bash_fpurge
#endif
extern int fpurge PARAMS((FILE *stream));

#endif /* HAVE_DECL_FPURGE */
#endif /* NEED_FPURGE_DECL */

/* Declarations for functions defined in lib/sh/getcwd.c */
#if !defined (HAVE_GETCWD)
extern char *getcwd PARAMS((char *, size_t));
#endif

/* Declarations for functions defined in lib/sh/input_avail.c */
extern int input_avail PARAMS((int));

/* Declarations for functions defined in lib/sh/itos.c */
extern char *inttostr PARAMS((intmax_t, char *, size_t));
extern char *itos PARAMS((intmax_t));
extern char *mitos PARAMS((intmax_t));
extern char *uinttostr PARAMS((uintmax_t, char *, size_t));
extern char *uitos PARAMS((uintmax_t));

/* declarations for functions defined in lib/sh/makepath.c */
#define MP_DOTILDE	0x01
#define MP_DOCWD	0x02
#define MP_RMDOT	0x04
#define MP_IGNDOT	0x08

extern char *sh_makepath PARAMS((const char *, const char *, int));

/* declarations for functions defined in lib/sh/mbscasecmp.c */
#if !defined (HAVE_MBSCASECMP)
extern char *mbscasecmp PARAMS((const char *, const char *));
#endif

/* declarations for functions defined in lib/sh/mbschr.c */
#if !defined (HAVE_MBSCHR)
extern char *mbschr PARAMS((const char *, int));
#endif

/* declarations for functions defined in lib/sh/mbscmp.c */
#if !defined (HAVE_MBSCMP)
extern char *mbscmp PARAMS((const char *, const char *));
#endif

/* declarations for functions defined in lib/sh/netconn.c */
extern int isnetconn PARAMS((int));

/* declarations for functions defined in lib/sh/netopen.c */
extern int netopen PARAMS((char *));

/* Declarations for  functions defined in lib/sh/oslib.c */

#if !defined (HAVE_DUP2) || defined (DUP2_BROKEN)
extern int dup2 PARAMS((int, int));
#endif

#if !defined (HAVE_GETDTABLESIZE)
extern int getdtablesize PARAMS((void));
#endif /* !HAVE_GETDTABLESIZE */

#if !defined (HAVE_GETHOSTNAME)
extern int gethostname PARAMS((char *, int));
#endif /* !HAVE_GETHOSTNAME */

extern int getmaxgroups PARAMS((void));
extern long getmaxchild PARAMS((void));

/* declarations for functions defined in lib/sh/pathcanon.c */
#define PATH_CHECKDOTDOT	0x0001
#define PATH_CHECKEXISTS	0x0002
#define PATH_HARDPATH		0x0004
#define PATH_NOALLOC		0x0008

extern char *sh_canonpath PARAMS((char *, int));

/* declarations for functions defined in lib/sh/pathphys.c */
extern char *sh_physpath PARAMS((char *, int));
extern char *sh_realpath PARAMS((const char *, char *));

/* declarations for functions defined in lib/sh/random.c */
extern int brand PARAMS((void));
extern void sbrand PARAMS((unsigned long));		/* set bash random number generator. */
extern void seedrand PARAMS((void));			/* seed generator randomly */
extern void seedrand32 PARAMS((void));
extern u_bits32_t get_urandom32 PARAMS((void));

/* declarations for functions defined in lib/sh/setlinebuf.c */
#ifdef NEED_SH_SETLINEBUF_DECL
extern int sh_setlinebuf PARAMS((FILE *));
#endif

/* declarations for functions defined in lib/sh/shaccess.c */
extern int sh_eaccess PARAMS((const char *, int));

/* declarations for functions defined in lib/sh/shmatch.c */
extern int sh_regmatch PARAMS((const char *, const char *, int));

/* defines for flags argument to sh_regmatch. */
#define SHMAT_SUBEXP		0x001	/* save subexpressions in SH_REMATCH */
#define SHMAT_PWARN		0x002	/* print a warning message on invalid regexp */

/* declarations for functions defined in lib/sh/shmbchar.c */
extern size_t mbstrlen PARAMS((const char *));
extern char *mbsmbchar PARAMS((const char *));
extern int sh_mbsnlen PARAMS((const char *, size_t, int));

/* declarations for functions defined in lib/sh/shquote.c */
extern char *sh_single_quote PARAMS((const char *));
extern char *sh_double_quote PARAMS((const char *));
extern char *sh_mkdoublequoted PARAMS((const char *, int, int));
extern char *sh_un_double_quote PARAMS((char *));
extern char *sh_backslash_quote PARAMS((char *, const char *, int));
extern char *sh_backslash_quote_for_double_quotes PARAMS((char *, int));
extern char *sh_quote_reusable PARAMS((char *, int));
extern int sh_contains_shell_metas PARAMS((const char *));
extern int sh_contains_quotes PARAMS((const char *));

/* declarations for functions defined in lib/sh/spell.c */
extern int spname PARAMS((char *, char *));
extern char *dirspell PARAMS((char *));

/* declarations for functions defined in lib/sh/strcasecmp.c */
#if !defined (HAVE_STRCASECMP)
extern int strncasecmp PARAMS((const char *, const char *, size_t));
extern int strcasecmp PARAMS((const char *, const char *));
#endif /* HAVE_STRCASECMP */

/* declarations for functions defined in lib/sh/strcasestr.c */
#if ! HAVE_STRCASESTR
extern char *strcasestr PARAMS((const char *, const char *));
#endif

/* declarations for functions defined in lib/sh/strchrnul.c */
#if ! HAVE_STRCHRNUL
extern char *strchrnul PARAMS((const char *, int));
#endif

/* declarations for functions defined in lib/sh/strerror.c */
#if !defined (HAVE_STRERROR) && !defined (strerror)
extern char *strerror PARAMS((int));
#endif

/* declarations for functions defined in lib/sh/strftime.c */
#if !defined (HAVE_STRFTIME) && defined (NEED_STRFTIME_DECL)
extern size_t strftime PARAMS((char *, size_t, const char *, const struct tm *));
#endif

/* declarations for functions and structures defined in lib/sh/stringlist.c */

/* This is a general-purpose argv-style array struct. */
typedef struct _list_of_strings {
  char **list;
  int list_size;
  int list_len;
} STRINGLIST;

typedef int sh_strlist_map_func_t PARAMS((char *));

extern STRINGLIST *strlist_create PARAMS((int));
extern STRINGLIST *strlist_resize PARAMS((STRINGLIST *, int));
extern void strlist_flush PARAMS((STRINGLIST *));
extern void strlist_dispose PARAMS((STRINGLIST *));
extern int strlist_remove PARAMS((STRINGLIST *, char *));
extern STRINGLIST *strlist_copy PARAMS((STRINGLIST *));
extern STRINGLIST *strlist_merge PARAMS((STRINGLIST *, STRINGLIST *));
extern STRINGLIST *strlist_append PARAMS((STRINGLIST *, STRINGLIST *));
extern STRINGLIST *strlist_prefix_suffix PARAMS((STRINGLIST *, char *, char *));
extern void strlist_print PARAMS((STRINGLIST *, char *));
extern void strlist_walk PARAMS((STRINGLIST *, sh_strlist_map_func_t *));
extern void strlist_sort PARAMS((STRINGLIST *));

/* declarations for functions defined in lib/sh/stringvec.c */

extern char **strvec_create PARAMS((int));
extern char **strvec_resize PARAMS((char **, int));
extern char **strvec_mcreate PARAMS((int));
extern char **strvec_mresize PARAMS((char **, int));
extern void strvec_flush PARAMS((char **));
extern void strvec_dispose PARAMS((char **));
extern int strvec_remove PARAMS((char **, char *));
extern int strvec_len PARAMS((char **));
extern int strvec_search PARAMS((char **, char *));
extern char **strvec_copy PARAMS((char **));
extern int strvec_posixcmp PARAMS((char **, char **));
extern int strvec_strcmp PARAMS((char **, char **));
extern void strvec_sort PARAMS((char **, int));

extern char **strvec_from_word_list PARAMS((WORD_LIST *, int, int, int *));
extern WORD_LIST *strvec_to_word_list PARAMS((char **, int, int));

/* declarations for functions defined in lib/sh/strnlen.c */
#if !defined (HAVE_STRNLEN)
extern size_t strnlen PARAMS((const char *, size_t));
#endif

/* declarations for functions defined in lib/sh/strpbrk.c */
#if !defined (HAVE_STRPBRK)
extern char *strpbrk PARAMS((const char *, const char *));
#endif

/* declarations for functions defined in lib/sh/strtod.c */
#if !defined (HAVE_STRTOD)
extern double strtod PARAMS((const char *, char **));
#endif

/* declarations for functions defined in lib/sh/strtol.c */
#if !HAVE_DECL_STRTOL
extern long strtol PARAMS((const char *, char **, int));
#endif

/* declarations for functions defined in lib/sh/strtoll.c */
#if defined (HAVE_LONG_LONG_INT) && !HAVE_DECL_STRTOLL
extern long long strtoll PARAMS((const char *, char **, int));
#endif

/* declarations for functions defined in lib/sh/strtoul.c */
#if !HAVE_DECL_STRTOUL
extern unsigned long strtoul PARAMS((const char *, char **, int));
#endif

/* declarations for functions defined in lib/sh/strtoull.c */
#if defined (HAVE_UNSIGNED_LONG_LONG_INT) && !HAVE_DECL_STRTOULL
extern unsigned long long strtoull PARAMS((const char *, char **, int));
#endif

/* declarations for functions defined in lib/sh/strimax.c */
#if !HAVE_DECL_STRTOIMAX
extern intmax_t strtoimax PARAMS((const char *, char **, int));
#endif

/* declarations for functions defined in lib/sh/strumax.c */
#if !HAVE_DECL_STRTOUMAX
extern uintmax_t strtoumax PARAMS((const char *, char **, int));
#endif

/* declarations for functions defined in lib/sh/strtrans.c */
extern char *ansicstr PARAMS((char *, int, int, int *, int *));
extern char *ansic_quote PARAMS((char *, int, int *));
extern int ansic_shouldquote PARAMS((const char *));
extern char *ansiexpand PARAMS((char *, int, int, int *));

/* declarations for functions defined in lib/sh/strvis.c */
extern int sh_charvis PARAMS((const char *, size_t *, size_t, char *, size_t *));
extern char *sh_strvis PARAMS((const char *));

/* declarations for functions defined in lib/sh/timeval.c.  No prototypes
   so we don't have to count on having a definition of struct timeval in
   scope when this file is included. */
extern void timeval_to_secs ();
extern void print_timeval ();

/* declarations for functions defined in lib/sh/tmpfile.c */
#define MT_USETMPDIR		0x0001
#define MT_READWRITE		0x0002
#define MT_USERANDOM		0x0004
#define MT_TEMPLATE		0x0008

extern char *sh_mktmpname PARAMS((char *, int));
extern int sh_mktmpfd PARAMS((char *, int, char **));
/* extern FILE *sh_mktmpfp PARAMS((char *, int, char **)); */
extern char *sh_mktmpdir PARAMS((char *, int));

/* declarations for functions defined in lib/sh/uconvert.c */
extern int uconvert PARAMS((char *, long *, long *, char **));

/* declarations for functions defined in lib/sh/ufuncs.c */
extern unsigned int falarm PARAMS((unsigned int, unsigned int));
extern unsigned int fsleep PARAMS((unsigned int, unsigned int));

/* declarations for functions defined in lib/sh/unicode.c */
extern int u32cconv PARAMS((unsigned long, char *));
extern void u32reset PARAMS((void));

/* declarations for functions defined in lib/sh/utf8.c */
extern char *utf8_mbschr PARAMS((const char *, int));
extern int utf8_mbscmp PARAMS((const char *, const char *));
extern char *utf8_mbsmbchar PARAMS((const char *));
extern int utf8_mbsnlen PARAMS((const char *, size_t, int));
extern int utf8_mblen PARAMS((const char *, size_t));
extern size_t utf8_mbstrlen PARAMS((const char *));

/* declarations for functions defined in lib/sh/wcsnwidth.c */
#if defined (HANDLE_MULTIBYTE)
extern int wcsnwidth PARAMS((const wchar_t *, size_t, int));
#endif

/* declarations for functions defined in lib/sh/winsize.c */
extern void get_new_window_size PARAMS((int, int *, int *));

/* declarations for functions defined in lib/sh/zcatfd.c */
extern int zcatfd PARAMS((int, int, char *));

/* declarations for functions defined in lib/sh/zgetline.c */
extern ssize_t zgetline PARAMS((int, char **, size_t *, int, int));

/* declarations for functions defined in lib/sh/zmapfd.c */
extern int zmapfd PARAMS((int, char **, char *));

/* declarations for functions defined in lib/sh/zread.c */
extern ssize_t zread PARAMS((int, char *, size_t));
extern ssize_t zreadretry PARAMS((int, char *, size_t));
extern ssize_t zreadintr PARAMS((int, char *, size_t));
extern ssize_t zreadc PARAMS((int, char *));
extern ssize_t zreadcintr PARAMS((int, char *));
extern ssize_t zreadn PARAMS((int, char *, size_t));
extern void zreset PARAMS((void));
extern void zsyncfd PARAMS((int));

/* declarations for functions defined in lib/sh/zwrite.c */
extern int zwrite PARAMS((int, char *, size_t));

/* declarations for functions defined in lib/glob/gmisc.c */
extern int match_pattern_char PARAMS((char *, char *, int));
extern int umatchlen PARAMS((char *, size_t));

#if defined (HANDLE_MULTIBYTE)
extern int match_pattern_wchar PARAMS((wchar_t *, wchar_t *, int));
extern int wmatchlen PARAMS((wchar_t *, size_t));
#endif

#endif /* _EXTERNS_H_ */
