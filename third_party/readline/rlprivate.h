/* rlprivate.h -- functions and variables global to the readline library,
		  but not intended for use by applications. */

/* Copyright (C) 1999-2022 Free Software Foundation, Inc.

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

#if !defined (_RL_PRIVATE_H_)
#define _RL_PRIVATE_H_

#include "rlconf.h"	/* for VISIBLE_STATS */
#include "rlstdc.h"
#include "posixjmp.h"	/* defines procenv_t */
#include "rlmbutil.h"	/* for HANDLE_MULTIBYTE */

/*************************************************************************
 *									 *
 * Convenience definitions						 *
 *									 *
 *************************************************************************/

#define EMACS_MODE()		(rl_editing_mode == emacs_mode)
#define VI_COMMAND_MODE()	(rl_editing_mode == vi_mode && _rl_keymap == vi_movement_keymap)
#define VI_INSERT_MODE()	(rl_editing_mode == vi_mode && _rl_keymap == vi_insertion_keymap)

#define RL_CHECK_SIGNALS() \
	do { \
	  if (_rl_caught_signal) _rl_signal_handler (_rl_caught_signal); \
	} while (0)

#define RL_SIG_RECEIVED() (_rl_caught_signal != 0)
#define RL_SIGINT_RECEIVED() (_rl_caught_signal == SIGINT)
#define RL_SIGWINCH_RECEIVED() (_rl_caught_signal == SIGWINCH)

#define CUSTOM_REDISPLAY_FUNC() (rl_redisplay_function != rl_redisplay)
#define CUSTOM_INPUT_FUNC() (rl_getc_function != rl_getc)

/*************************************************************************
 *									 *
 * Global structs undocumented in texinfo manual and not in readline.h   *
 *									 *
 *************************************************************************/
/* search types */
#define RL_SEARCH_ISEARCH	0x01		/* incremental search */
#define RL_SEARCH_NSEARCH	0x02		/* non-incremental search */
#define RL_SEARCH_CSEARCH	0x04		/* intra-line char search */

/* search flags */
#define SF_REVERSE		0x01
#define SF_FOUND		0x02
#define SF_FAILED		0x04
#define SF_CHGKMAP		0x08
#define SF_PATTERN		0x10
#define SF_NOCASE		0x20		/* unused so far */

typedef struct  __rl_search_context
{
  int type;
  int sflags;

  char *search_string;
  int search_string_index;
  int search_string_size;

  char **lines;
  char *allocated_line;
  int hlen;
  int hindex;

  int save_point;
  int save_mark;
  int save_line;
  int last_found_line;
  char *prev_line_found;

  UNDO_LIST *save_undo_list;

  Keymap keymap;	/* used when dispatching commands in search string */
  Keymap okeymap;	/* original keymap */

  int history_pos;
  int direction;

  int prevc;
  int lastc;
#if defined (HANDLE_MULTIBYTE)
  char mb[MB_LEN_MAX];
  char pmb[MB_LEN_MAX];
#endif

  char *sline;
  int sline_len;
  int sline_index;

  char  *search_terminators;
} _rl_search_cxt;

struct _rl_cmd {
  Keymap map;
  int count;
  int key;
  rl_command_func_t *func;
};
extern struct _rl_cmd _rl_pending_command;
extern struct _rl_cmd *_rl_command_to_execute;

/* Callback data for reading numeric arguments */
#define NUM_SAWMINUS	0x01
#define NUM_SAWDIGITS	0x02
#define NUM_READONE	0x04

typedef int _rl_arg_cxt;

/* A context for reading key sequences longer than a single character when
   using the callback interface. */
#define KSEQ_DISPATCHED	0x01
#define KSEQ_SUBSEQ	0x02
#define KSEQ_RECURSIVE	0x04

typedef struct __rl_keyseq_context
{
  int flags;
  int subseq_arg;
  int subseq_retval;		/* XXX */
  int okey;

  Keymap dmap;
  Keymap oldmap;

  struct __rl_keyseq_context *ocxt;
  int childval;
} _rl_keyseq_cxt;

/* vi-mode commands that use result of motion command to define boundaries */
#define VIM_DELETE	0x01
#define VIM_CHANGE	0x02
#define VIM_YANK	0x04

/* various states for vi-mode commands that use motion commands.  reflects
   RL_READLINE_STATE */
#define VMSTATE_READ	0x01
#define VMSTATE_NUMARG	0x02

typedef struct __rl_vimotion_context
{
  int op;
  int state;
  int flags;		/* reserved */
  _rl_arg_cxt ncxt;
  int numeric_arg;
  int start, end;	/* rl_point, rl_end */
  int key, motion;	/* initial key, motion command */
} _rl_vimotion_cxt;

/* fill in more as needed */
/* `Generic' callback data and functions */
typedef struct __rl_callback_generic_arg
{
  int count;
  int i1, i2;
  /* add here as needed */
} _rl_callback_generic_arg;

typedef int _rl_callback_func_t (_rl_callback_generic_arg *);

typedef void _rl_sigcleanup_func_t (int, void *);

/*************************************************************************
 *									 *
 * Global functions undocumented in texinfo manual and not in readline.h *
 *									 *
 *************************************************************************/

/*************************************************************************
 *									 *
 * Global variables undocumented in texinfo manual and not in readline.h *
 *									 *
 *************************************************************************/

/* complete.c */
extern int rl_complete_with_tilde_expansion;
#if defined (VISIBLE_STATS)
extern int rl_visible_stats;
#endif /* VISIBLE_STATS */
#if defined (COLOR_SUPPORT)
extern int _rl_colored_stats;
extern int _rl_colored_completion_prefix;
#endif

/* readline.c */
extern int rl_line_buffer_len;
extern int rl_arg_sign;
extern int rl_visible_prompt_length;
extern int rl_byte_oriented;

/* display.c */
extern int rl_display_fixed;

/* parens.c */
extern int rl_blink_matching_paren;

/*************************************************************************
 *									 *
 * Global functions and variables unused and undocumented		 *
 *									 *
 *************************************************************************/

/* kill.c */
extern int rl_set_retained_kills (int);

/* terminal.c */
extern void _rl_set_screen_size (int, int);

/* undo.c */
extern int _rl_fix_last_undo_of_type (int, int, int);

/* util.c */
extern char *_rl_savestring (const char *);

/*************************************************************************
 *									 *
 * Functions and variables private to the readline library		 *
 *									 *
 *************************************************************************/

/* NOTE: Functions and variables prefixed with `_rl_' are
   pseudo-global: they are global so they can be shared
   between files in the readline library, but are not intended
   to be visible to readline callers. */

/*************************************************************************
 * Undocumented private functions					 *
 *************************************************************************/

#if defined(READLINE_CALLBACKS)

/* readline.c */
extern void readline_internal_setup (void);
extern char *readline_internal_teardown (int);
extern int readline_internal_char (void);

extern _rl_keyseq_cxt *_rl_keyseq_cxt_alloc (void);
extern void _rl_keyseq_cxt_dispose (_rl_keyseq_cxt *);
extern void _rl_keyseq_chain_dispose (void);

extern int _rl_dispatch_callback (_rl_keyseq_cxt *);

/* callback.c */
extern _rl_callback_generic_arg *_rl_callback_data_alloc (int);
extern void _rl_callback_data_dispose (_rl_callback_generic_arg *);

#endif /* READLINE_CALLBACKS */

/* bind.c */
extern char *_rl_untranslate_macro_value (char *, int);

/* complete.c */
extern void _rl_reset_completion_state (void);
extern char _rl_find_completion_word (int *, int *);
extern void _rl_free_match_list (char **);

/* display.c */
extern char *_rl_strip_prompt (char *);
extern void _rl_reset_prompt (void);
extern void _rl_move_vert (int);
extern void _rl_save_prompt (void);
extern void _rl_restore_prompt (void);
extern char *_rl_make_prompt_for_search (int);
extern void _rl_erase_at_end_of_line (int);
extern void _rl_clear_to_eol (int);
extern void _rl_clear_screen (int);
extern void _rl_update_final (void);
extern void _rl_optimize_redisplay (void);
extern void _rl_redisplay_after_sigwinch (void);
extern void _rl_clean_up_for_exit (void);
extern void _rl_erase_entire_line (void);
extern int _rl_current_display_line (void);
extern void _rl_refresh_line (void);

/* input.c */
extern int _rl_any_typein (void);
extern int _rl_input_available (void);
extern int _rl_nchars_available (void);
extern int _rl_input_queued (int);
extern void _rl_insert_typein (int);
extern int _rl_unget_char (int);
extern int _rl_pushed_input_available (void);

extern int _rl_timeout_init (void);
extern int _rl_timeout_handle_sigalrm (void);
#if defined (_POSIXSELECT_H_)
/* use as a sentinel for fd_set, struct timeval,  and sigset_t definitions */
extern int _rl_timeout_select (int, fd_set *, fd_set *, fd_set *, const struct timeval *, const sigset_t *);
#endif

/* isearch.c */
extern _rl_search_cxt *_rl_scxt_alloc (int, int);
extern void _rl_scxt_dispose (_rl_search_cxt *, int);

extern int _rl_isearch_dispatch (_rl_search_cxt *, int);
extern int _rl_isearch_callback (_rl_search_cxt *);
extern int _rl_isearch_cleanup (_rl_search_cxt *, int);

extern int _rl_search_getchar (_rl_search_cxt *);

/* kill.c */
#ifndef BRACKETED_PASTE_DEFAULT
#  define BRACKETED_PASTE_DEFAULT	1	/* XXX - for now */
#endif

#define BRACK_PASTE_PREF	"\033[200~"
#define BRACK_PASTE_SUFF	"\033[201~"

#define BRACK_PASTE_LAST	'~'
#define BRACK_PASTE_SLEN	6

#define BRACK_PASTE_INIT	"\033[?2004h"
#define BRACK_PASTE_FINI	"\033[?2004l\r"

extern int _rl_read_bracketed_paste_prefix (int);
extern char *_rl_bracketed_text (size_t *);
extern int _rl_bracketed_read_key (void);
extern int _rl_bracketed_read_mbstring (char *, int);

/* macro.c */
extern void _rl_with_macro_input (char *);
extern int _rl_peek_macro_key (void);
extern int _rl_next_macro_key (void);
extern int _rl_prev_macro_key (void);
extern void _rl_push_executing_macro (void);
extern void _rl_pop_executing_macro (void);
extern void _rl_add_macro_char (int);
extern void _rl_kill_kbd_macro (void);

/* misc.c */
extern int _rl_arg_overflow (void);
extern void _rl_arg_init (void);
extern int _rl_arg_getchar (void);
extern int _rl_arg_callback (_rl_arg_cxt);
extern void _rl_reset_argument (void);

extern void _rl_start_using_history (void);
extern int _rl_free_saved_history_line (void);
extern void _rl_set_insert_mode (int, int);

extern void _rl_revert_previous_lines (void);
extern void _rl_revert_all_lines (void);

/* nls.c */
extern char *_rl_init_locale (void);
extern int _rl_init_eightbit (void);
extern void _rl_reset_locale (void);

/* parens.c */
extern void _rl_enable_paren_matching (int);

/* readline.c */
extern void _rl_init_line_state (void);
extern void _rl_set_the_line (void);
extern int _rl_dispatch (int, Keymap);
extern int _rl_dispatch_subseq (int, Keymap, int);
extern void _rl_internal_char_cleanup (void);

extern void _rl_init_executing_keyseq (void);
extern void _rl_term_executing_keyseq (void);
extern void _rl_end_executing_keyseq (void);
extern void _rl_add_executing_keyseq (int);
extern void _rl_del_executing_keyseq (void);

extern rl_command_func_t *_rl_executing_func;

/* rltty.c */
extern int _rl_disable_tty_signals (void);
extern int _rl_restore_tty_signals (void);

/* search.c */
extern int _rl_nsearch_callback (_rl_search_cxt *);
extern int _rl_nsearch_cleanup (_rl_search_cxt *, int);

/* signals.c */
extern void _rl_signal_handler (int);

extern void _rl_block_sigint (void);
extern void _rl_release_sigint (void);
extern void _rl_block_sigwinch (void);
extern void _rl_release_sigwinch (void);

/* terminal.c */
extern void _rl_get_screen_size (int, int);
extern void _rl_sigwinch_resize_terminal (void);
extern int _rl_init_terminal_io (const char *);
#ifdef _MINIX
extern void _rl_output_character_function (int);
#else
extern int _rl_output_character_function (int);
#endif
extern void _rl_cr (void);
extern void _rl_output_some_chars (const char *, int);
extern int _rl_backspace (int);
extern void _rl_enable_meta_key (void);
extern void _rl_disable_meta_key (void);
extern void _rl_control_keypad (int);
extern void _rl_set_cursor (int, int);
extern void _rl_standout_on (void);
extern void _rl_standout_off (void);
extern int _rl_reset_region_color (int, const char *);
extern void _rl_region_color_on (void);
extern void _rl_region_color_off (void);

/* text.c */
extern void _rl_fix_point (int);
extern void _rl_fix_mark (void);
extern int _rl_replace_text (const char *, int, int);
extern int _rl_forward_char_internal (int);
extern int _rl_backward_char_internal (int);
extern int _rl_insert_char (int, int);
extern int _rl_overwrite_char (int, int);
extern int _rl_overwrite_rubout (int, int);
extern int _rl_rubout_char (int, int);
#if defined (HANDLE_MULTIBYTE)
extern int _rl_char_search_internal (int, int, char *, int);
#else
extern int _rl_char_search_internal (int, int, int);
#endif
extern int _rl_set_mark_at_pos (int);

/* undo.c */
extern UNDO_LIST *_rl_copy_undo_entry (UNDO_LIST *);
extern UNDO_LIST *_rl_copy_undo_list (UNDO_LIST *);
extern void _rl_free_undo_list (UNDO_LIST *);

/* util.c */
#if defined (USE_VARARGS) && defined (PREFER_STDARG)
extern void _rl_ttymsg (const char *, ...)  __attribute__((__format__ (printf, 1, 2)));
extern void _rl_errmsg (const char *, ...)  __attribute__((__format__ (printf, 1, 2)));
extern void _rl_trace (const char *, ...)  __attribute__((__format__ (printf, 1, 2)));
#else
extern void _rl_ttymsg ();
extern void _rl_errmsg ();
extern void _rl_trace ();
#endif
extern void _rl_audit_tty (char *);

extern int _rl_tropen (void);

extern int _rl_abort_internal (void);
extern int _rl_null_function (int, int);
extern char *_rl_strindex (const char *, const char *);
extern int _rl_qsort_string_compare (char **, char **);
extern int (_rl_uppercase_p) (int);
extern int (_rl_lowercase_p) (int);
extern int (_rl_pure_alphabetic) (int);
extern int (_rl_digit_p) (int);
extern int (_rl_to_lower) (int);
extern int (_rl_to_upper) (int);
extern int (_rl_digit_value) (int);

/* vi_mode.c */
extern void _rl_vi_initialize_line (void);
extern void _rl_vi_reset_last (void);
extern void _rl_vi_set_last (int, int, int);
extern int _rl_vi_textmod_command (int);
extern int _rl_vi_motion_command (int);
extern void _rl_vi_done_inserting (void);
extern int _rl_vi_domove_callback (_rl_vimotion_cxt *);
extern int _rl_vi_domove_motion_cleanup (int, _rl_vimotion_cxt *);

/* Use HS_HISTORY_VERSION as the sentinel to see if we've included history.h
   and so can use HIST_ENTRY */
#if defined (HS_HISTORY_VERSION)
extern void _rl_free_history_entry (HIST_ENTRY *);
#endif

/*************************************************************************
 * Undocumented private variables					 *
 *************************************************************************/

/* bind.c */
extern const char * const _rl_possible_control_prefixes[];
extern const char * const _rl_possible_meta_prefixes[];

/* callback.c */
extern _rl_callback_func_t *_rl_callback_func;
extern _rl_callback_generic_arg *_rl_callback_data;

/* complete.c */
extern int _rl_complete_show_all;
extern int _rl_complete_show_unmodified;
extern int _rl_complete_mark_directories;
extern int _rl_complete_mark_symlink_dirs;
extern int _rl_completion_prefix_display_length;
extern int _rl_completion_columns;
extern int _rl_print_completions_horizontally;
extern int _rl_completion_case_fold;
extern int _rl_completion_case_map;
extern int _rl_match_hidden_files;
extern int _rl_page_completions;
extern int _rl_skip_completed_text;
extern int _rl_menu_complete_prefix_first;

/* display.c */
extern int _rl_vis_botlin;
extern int _rl_last_c_pos;
extern int _rl_suppress_redisplay;
extern int _rl_want_redisplay;

extern char *_rl_emacs_mode_str;
extern int _rl_emacs_modestr_len;
extern char *_rl_vi_ins_mode_str;
extern int _rl_vi_ins_modestr_len;
extern char *_rl_vi_cmd_mode_str;
extern int _rl_vi_cmd_modestr_len;

/* isearch.c */
extern char *_rl_isearch_terminators;

extern _rl_search_cxt *_rl_iscxt;

/* macro.c */
extern char *_rl_executing_macro;

/* misc.c */
extern int _rl_history_preserve_point;
extern int _rl_history_saved_point;

extern _rl_arg_cxt _rl_argcxt;

/* nls.c */
extern int _rl_utf8locale;

/* readline.c */
extern int _rl_echoing_p;
extern int _rl_horizontal_scroll_mode;
extern int _rl_mark_modified_lines;
extern int _rl_bell_preference;
extern int _rl_meta_flag;
extern int _rl_convert_meta_chars_to_ascii;
extern int _rl_output_meta_chars;
extern int _rl_bind_stty_chars;
extern int _rl_revert_all_at_newline;
extern int _rl_echo_control_chars;
extern int _rl_show_mode_in_prompt;
extern int _rl_enable_bracketed_paste;
extern int _rl_enable_active_region;
extern char *_rl_active_region_start_color;
extern char *_rl_active_region_end_color;
extern char *_rl_comment_begin;
extern unsigned char _rl_parsing_conditionalized_out;
extern Keymap _rl_keymap;
extern FILE *_rl_in_stream;
extern FILE *_rl_out_stream;
extern int _rl_last_command_was_kill;
extern int _rl_eof_char;
extern procenv_t _rl_top_level;
extern _rl_keyseq_cxt *_rl_kscxt;
extern int _rl_keyseq_timeout;

extern int _rl_executing_keyseq_size;

extern rl_hook_func_t *_rl_internal_startup_hook;

/* search.c */
extern _rl_search_cxt *_rl_nscxt;
extern int _rl_history_search_pos;

/* signals.c */
extern int volatile _rl_caught_signal;

extern _rl_sigcleanup_func_t *_rl_sigcleanup;
extern void *_rl_sigcleanarg;

extern int _rl_echoctl;

extern int _rl_intr_char;
extern int _rl_quit_char;
extern int _rl_susp_char;

/* terminal.c */
extern int _rl_enable_keypad;
extern int _rl_enable_meta;
extern char *_rl_term_clreol;
extern char *_rl_term_clrpag;
extern char *_rl_term_clrscroll;
extern char *_rl_term_im;
extern char *_rl_term_ic;
extern char *_rl_term_ei;
extern char *_rl_term_DC;
extern char *_rl_term_up;
extern char *_rl_term_dc;
extern char *_rl_term_cr;
extern char *_rl_term_IC;
extern char *_rl_term_forward_char;
extern int _rl_screenheight;
extern int _rl_screenwidth;
extern int _rl_screenchars;
extern int _rl_terminal_can_insert;
extern int _rl_term_autowrap;

/* text.c */
extern int _rl_optimize_typeahead;
extern int _rl_keep_mark_active;

/* undo.c */
extern int _rl_doing_an_undo;
extern int _rl_undo_group_level;

/* vi_mode.c */
extern int _rl_vi_last_command;
extern int _rl_vi_redoing;
extern _rl_vimotion_cxt *_rl_vimvcxt;

/* Use HS_HISTORY_VERSION as the sentinel to see if we've included history.h
   and so can use HIST_ENTRY */
#if defined (HS_HISTORY_VERSION)
extern HIST_ENTRY *_rl_saved_line_for_history;
#endif

#endif /* _RL_PRIVATE_H_ */
