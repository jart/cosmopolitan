/* builtext.h - The list of builtins found in libbuiltins.a. */
#if defined (ALIAS)
extern int alias_builtin PARAMS((WORD_LIST *));
extern char * const alias_doc[];
#endif /* ALIAS */
#if defined (ALIAS)
extern int unalias_builtin PARAMS((WORD_LIST *));
extern char * const unalias_doc[];
#endif /* ALIAS */
#if defined (READLINE)
extern int bind_builtin PARAMS((WORD_LIST *));
extern char * const bind_doc[];
#endif /* READLINE */
extern int break_builtin PARAMS((WORD_LIST *));
extern char * const break_doc[];
extern int continue_builtin PARAMS((WORD_LIST *));
extern char * const continue_doc[];
extern int builtin_builtin PARAMS((WORD_LIST *));
extern char * const builtin_doc[];
#if defined (DEBUGGER)
extern int caller_builtin PARAMS((WORD_LIST *));
extern char * const caller_doc[];
#endif /* DEBUGGER */
extern int cd_builtin PARAMS((WORD_LIST *));
extern char * const cd_doc[];
extern int pwd_builtin PARAMS((WORD_LIST *));
extern char * const pwd_doc[];
extern int colon_builtin PARAMS((WORD_LIST *));
extern char * const colon_doc[];
extern int colon_builtin PARAMS((WORD_LIST *));
extern char * const true_doc[];
extern int false_builtin PARAMS((WORD_LIST *));
extern char * const false_doc[];
extern int command_builtin PARAMS((WORD_LIST *));
extern char * const command_doc[];
extern int declare_builtin PARAMS((WORD_LIST *));
extern char * const declare_doc[];
extern int declare_builtin PARAMS((WORD_LIST *));
extern char * const typeset_doc[];
extern int local_builtin PARAMS((WORD_LIST *));
extern char * const local_doc[];
#if defined (V9_ECHO)
extern int echo_builtin PARAMS((WORD_LIST *));
extern char * const echo_doc[];
#endif /* V9_ECHO */
#if !defined (V9_ECHO)
extern int echo_builtin PARAMS((WORD_LIST *));
extern char * const echo_doc[];
#endif /* !V9_ECHO */
extern int enable_builtin PARAMS((WORD_LIST *));
extern char * const enable_doc[];
extern int eval_builtin PARAMS((WORD_LIST *));
extern char * const eval_doc[];
extern int getopts_builtin PARAMS((WORD_LIST *));
extern char * const getopts_doc[];
extern int exec_builtin PARAMS((WORD_LIST *));
extern char * const exec_doc[];
extern int exit_builtin PARAMS((WORD_LIST *));
extern char * const exit_doc[];
extern int logout_builtin PARAMS((WORD_LIST *));
extern char * const logout_doc[];
#if defined (HISTORY)
extern int fc_builtin PARAMS((WORD_LIST *));
extern char * const fc_doc[];
#endif /* HISTORY */
#if defined (JOB_CONTROL)
extern int fg_builtin PARAMS((WORD_LIST *));
extern char * const fg_doc[];
#endif /* JOB_CONTROL */
#if defined (JOB_CONTROL)
extern int bg_builtin PARAMS((WORD_LIST *));
extern char * const bg_doc[];
#endif /* JOB_CONTROL */
extern int hash_builtin PARAMS((WORD_LIST *));
extern char * const hash_doc[];
#if defined (HELP_BUILTIN)
extern int help_builtin PARAMS((WORD_LIST *));
extern char * const help_doc[];
#endif /* HELP_BUILTIN */
#if defined (HISTORY)
extern int history_builtin PARAMS((WORD_LIST *));
extern char * const history_doc[];
#endif /* HISTORY */
#if defined (JOB_CONTROL)
extern int jobs_builtin PARAMS((WORD_LIST *));
extern char * const jobs_doc[];
#endif /* JOB_CONTROL */
#if defined (JOB_CONTROL)
extern int disown_builtin PARAMS((WORD_LIST *));
extern char * const disown_doc[];
#endif /* JOB_CONTROL */
extern int kill_builtin PARAMS((WORD_LIST *));
extern char * const kill_doc[];
extern int let_builtin PARAMS((WORD_LIST *));
extern char * const let_doc[];
extern int read_builtin PARAMS((WORD_LIST *));
extern char * const read_doc[];
extern int return_builtin PARAMS((WORD_LIST *));
extern char * const return_doc[];
extern int set_builtin PARAMS((WORD_LIST *));
extern char * const set_doc[];
extern int unset_builtin PARAMS((WORD_LIST *));
extern char * const unset_doc[];
extern int export_builtin PARAMS((WORD_LIST *));
extern char * const export_doc[];
extern int readonly_builtin PARAMS((WORD_LIST *));
extern char * const readonly_doc[];
extern int shift_builtin PARAMS((WORD_LIST *));
extern char * const shift_doc[];
extern int source_builtin PARAMS((WORD_LIST *));
extern char * const source_doc[];
extern int source_builtin PARAMS((WORD_LIST *));
extern char * const dot_doc[];
#if defined (JOB_CONTROL)
extern int suspend_builtin PARAMS((WORD_LIST *));
extern char * const suspend_doc[];
#endif /* JOB_CONTROL */
extern int test_builtin PARAMS((WORD_LIST *));
extern char * const test_doc[];
extern int test_builtin PARAMS((WORD_LIST *));
extern char * const test_bracket_doc[];
extern int times_builtin PARAMS((WORD_LIST *));
extern char * const times_doc[];
extern int trap_builtin PARAMS((WORD_LIST *));
extern char * const trap_doc[];
extern int type_builtin PARAMS((WORD_LIST *));
extern char * const type_doc[];
#if !defined (_MINIX)
extern int ulimit_builtin PARAMS((WORD_LIST *));
extern char * const ulimit_doc[];
#endif /* !_MINIX */
extern int umask_builtin PARAMS((WORD_LIST *));
extern char * const umask_doc[];
#if defined (JOB_CONTROL)
extern int wait_builtin PARAMS((WORD_LIST *));
extern char * const wait_doc[];
#endif /* JOB_CONTROL */
#if !defined (JOB_CONTROL)
extern int wait_builtin PARAMS((WORD_LIST *));
extern char * const wait_doc[];
#endif /* !JOB_CONTROL */
extern char * const for_doc[];
extern char * const arith_for_doc[];
extern char * const select_doc[];
extern char * const time_doc[];
extern char * const case_doc[];
extern char * const if_doc[];
extern char * const while_doc[];
extern char * const until_doc[];
extern char * const coproc_doc[];
extern char * const function_doc[];
extern char * const grouping_braces_doc[];
extern char * const fg_percent_doc[];
extern char * const arith_doc[];
extern char * const conditional_doc[];
extern char * const variable_help_doc[];
#if defined (PUSHD_AND_POPD)
extern int pushd_builtin PARAMS((WORD_LIST *));
extern char * const pushd_doc[];
#endif /* PUSHD_AND_POPD */
#if defined (PUSHD_AND_POPD)
extern int popd_builtin PARAMS((WORD_LIST *));
extern char * const popd_doc[];
#endif /* PUSHD_AND_POPD */
#if defined (PUSHD_AND_POPD)
extern int dirs_builtin PARAMS((WORD_LIST *));
extern char * const dirs_doc[];
#endif /* PUSHD_AND_POPD */
extern int shopt_builtin PARAMS((WORD_LIST *));
extern char * const shopt_doc[];
extern int printf_builtin PARAMS((WORD_LIST *));
extern char * const printf_doc[];
#if defined (PROGRAMMABLE_COMPLETION)
extern int complete_builtin PARAMS((WORD_LIST *));
extern char * const complete_doc[];
#endif /* PROGRAMMABLE_COMPLETION */
#if defined (PROGRAMMABLE_COMPLETION)
extern int compgen_builtin PARAMS((WORD_LIST *));
extern char * const compgen_doc[];
#endif /* PROGRAMMABLE_COMPLETION */
#if defined (PROGRAMMABLE_COMPLETION)
extern int compopt_builtin PARAMS((WORD_LIST *));
extern char * const compopt_doc[];
#endif /* PROGRAMMABLE_COMPLETION */
extern int mapfile_builtin PARAMS((WORD_LIST *));
extern char * const mapfile_doc[];
extern int mapfile_builtin PARAMS((WORD_LIST *));
extern char * const readarray_doc[];
