/* builtins.c -- the built in shell commands. */

/* This file is manufactured by ./mkbuiltins, and should not be
   edited by hand.  See the source to mkbuiltins for details. */

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

/* The list of shell builtins.  Each element is name, function, flags,
   long-doc, short-doc.  The long-doc field contains a pointer to an array
   of help lines.  The function takes a WORD_LIST *; the first word in the
   list is the first arg to the command.  The list has already had word
   expansion performed.

   Functions which need to look at only the simple commands (e.g.
   the enable_builtin ()), should ignore entries where
   (array[i].function == (sh_builtin_func_t *)NULL).  Such entries are for
   the list of shell reserved control structures, like `if' and `while'.
   The end of the list is denoted with a NULL name field. */

/* TRANSLATORS: Please do not translate command names in descriptions */

#include "builtins.h"
#include "builtext.h"
#include "bashintl.h"

struct builtin static_shell_builtins[] = {
#if defined (ALIAS)
  { "alias", alias_builtin, BUILTIN_ENABLED | STATIC_BUILTIN | ASSIGNMENT_BUILTIN | POSIX_BUILTIN, alias_doc,
     N_("alias [-p] [name[=value] ... ]"), (char *)NULL },
#endif /* ALIAS */
#if defined (ALIAS)
  { "unalias", unalias_builtin, BUILTIN_ENABLED | STATIC_BUILTIN | POSIX_BUILTIN, unalias_doc,
     N_("unalias [-a] name [name ...]"), (char *)NULL },
#endif /* ALIAS */
#if defined (READLINE)
  { "bind", bind_builtin, BUILTIN_ENABLED | STATIC_BUILTIN, bind_doc,
     N_("bind [-lpsvPSVX] [-m keymap] [-f filename] [-q name] [-u name] [-r keyseq] [-x keyseq:shell-command] [keyseq:readline-function or readline-command]"), (char *)NULL },
#endif /* READLINE */
  { "break", break_builtin, BUILTIN_ENABLED | STATIC_BUILTIN | SPECIAL_BUILTIN, break_doc,
     N_("break [n]"), (char *)NULL },
  { "continue", continue_builtin, BUILTIN_ENABLED | STATIC_BUILTIN | SPECIAL_BUILTIN, continue_doc,
     N_("continue [n]"), (char *)NULL },
  { "builtin", builtin_builtin, BUILTIN_ENABLED | STATIC_BUILTIN, builtin_doc,
     N_("builtin [shell-builtin [arg ...]]"), (char *)NULL },
#if defined (DEBUGGER)
  { "caller", caller_builtin, BUILTIN_ENABLED | STATIC_BUILTIN, caller_doc,
     N_("caller [expr]"), (char *)NULL },
#endif /* DEBUGGER */
  { "cd", cd_builtin, BUILTIN_ENABLED | STATIC_BUILTIN | POSIX_BUILTIN, cd_doc,
     N_("cd [-L|[-P [-e]] [-@]] [dir]"), (char *)NULL },
  { "pwd", pwd_builtin, BUILTIN_ENABLED | STATIC_BUILTIN | POSIX_BUILTIN, pwd_doc,
     N_("pwd [-LP]"), (char *)NULL },
  { ":", colon_builtin, BUILTIN_ENABLED | STATIC_BUILTIN | SPECIAL_BUILTIN, colon_doc,
     ":", (char *)NULL },
  { "true", colon_builtin, BUILTIN_ENABLED | STATIC_BUILTIN | POSIX_BUILTIN, true_doc,
     "true", (char *)NULL },
  { "false", false_builtin, BUILTIN_ENABLED | STATIC_BUILTIN | POSIX_BUILTIN, false_doc,
     "false", (char *)NULL },
  { "command", command_builtin, BUILTIN_ENABLED | STATIC_BUILTIN | POSIX_BUILTIN, command_doc,
     N_("command [-pVv] command [arg ...]"), (char *)NULL },
  { "declare", declare_builtin, BUILTIN_ENABLED | STATIC_BUILTIN | ASSIGNMENT_BUILTIN | LOCALVAR_BUILTIN | ARRAYREF_BUILTIN, declare_doc,
     N_("declare [-aAfFgiIlnrtux] [name[=value] ...] or declare -p [-aAfFilnrtux] [name ...]"), (char *)NULL },
  { "typeset", declare_builtin, BUILTIN_ENABLED | STATIC_BUILTIN | ASSIGNMENT_BUILTIN | LOCALVAR_BUILTIN | ARRAYREF_BUILTIN, typeset_doc,
     N_("typeset [-aAfFgiIlnrtux] name[=value] ... or typeset -p [-aAfFilnrtux] [name ...]"), (char *)NULL },
  { "local", local_builtin, BUILTIN_ENABLED | STATIC_BUILTIN | ASSIGNMENT_BUILTIN | LOCALVAR_BUILTIN | ARRAYREF_BUILTIN, local_doc,
     N_("local [option] name[=value] ..."), (char *)NULL },
#if defined (V9_ECHO)
  { "echo", echo_builtin, BUILTIN_ENABLED | STATIC_BUILTIN, echo_doc,
     N_("echo [-neE] [arg ...]"), (char *)NULL },
#endif /* V9_ECHO */
#if !defined (V9_ECHO)
  { "echo", echo_builtin, BUILTIN_ENABLED | STATIC_BUILTIN, echo_doc,
     N_("echo [-n] [arg ...]"), (char *)NULL },
#endif /* !V9_ECHO */
  { "enable", enable_builtin, BUILTIN_ENABLED | STATIC_BUILTIN, enable_doc,
     N_("enable [-a] [-dnps] [-f filename] [name ...]"), (char *)NULL },
  { "eval", eval_builtin, BUILTIN_ENABLED | STATIC_BUILTIN | SPECIAL_BUILTIN, eval_doc,
     N_("eval [arg ...]"), (char *)NULL },
  { "getopts", getopts_builtin, BUILTIN_ENABLED | STATIC_BUILTIN | POSIX_BUILTIN, getopts_doc,
     N_("getopts optstring name [arg ...]"), (char *)NULL },
  { "exec", exec_builtin, BUILTIN_ENABLED | STATIC_BUILTIN | SPECIAL_BUILTIN, exec_doc,
     N_("exec [-cl] [-a name] [command [argument ...]] [redirection ...]"), (char *)NULL },
  { "exit", exit_builtin, BUILTIN_ENABLED | STATIC_BUILTIN | SPECIAL_BUILTIN, exit_doc,
     N_("exit [n]"), (char *)NULL },
  { "logout", logout_builtin, BUILTIN_ENABLED | STATIC_BUILTIN, logout_doc,
     N_("logout [n]"), (char *)NULL },
#if defined (HISTORY)
  { "fc", fc_builtin, BUILTIN_ENABLED | STATIC_BUILTIN | POSIX_BUILTIN, fc_doc,
     N_("fc [-e ename] [-lnr] [first] [last] or fc -s [pat=rep] [command]"), (char *)NULL },
#endif /* HISTORY */
#if defined (JOB_CONTROL)
  { "fg", fg_builtin, BUILTIN_ENABLED | STATIC_BUILTIN | POSIX_BUILTIN, fg_doc,
     N_("fg [job_spec]"), (char *)NULL },
#endif /* JOB_CONTROL */
#if defined (JOB_CONTROL)
  { "bg", bg_builtin, BUILTIN_ENABLED | STATIC_BUILTIN | POSIX_BUILTIN, bg_doc,
     N_("bg [job_spec ...]"), (char *)NULL },
#endif /* JOB_CONTROL */
  { "hash", hash_builtin, BUILTIN_ENABLED | STATIC_BUILTIN, hash_doc,
     N_("hash [-lr] [-p pathname] [-dt] [name ...]"), (char *)NULL },
#if defined (HELP_BUILTIN)
  { "help", help_builtin, BUILTIN_ENABLED | STATIC_BUILTIN, help_doc,
     N_("help [-dms] [pattern ...]"), (char *)NULL },
#endif /* HELP_BUILTIN */
#if defined (HISTORY)
  { "history", history_builtin, BUILTIN_ENABLED | STATIC_BUILTIN, history_doc,
     N_("history [-c] [-d offset] [n] or history -anrw [filename] or history -ps arg [arg...]"), (char *)NULL },
#endif /* HISTORY */
#if defined (JOB_CONTROL)
  { "jobs", jobs_builtin, BUILTIN_ENABLED | STATIC_BUILTIN | POSIX_BUILTIN, jobs_doc,
     N_("jobs [-lnprs] [jobspec ...] or jobs -x command [args]"), (char *)NULL },
#endif /* JOB_CONTROL */
#if defined (JOB_CONTROL)
  { "disown", disown_builtin, BUILTIN_ENABLED | STATIC_BUILTIN, disown_doc,
     N_("disown [-h] [-ar] [jobspec ... | pid ...]"), (char *)NULL },
#endif /* JOB_CONTROL */
  { "kill", kill_builtin, BUILTIN_ENABLED | STATIC_BUILTIN | POSIX_BUILTIN, kill_doc,
     N_("kill [-s sigspec | -n signum | -sigspec] pid | jobspec ... or kill -l [sigspec]"), (char *)NULL },
  { "let", let_builtin, BUILTIN_ENABLED | STATIC_BUILTIN | ARRAYREF_BUILTIN, let_doc,
     N_("let arg [arg ...]"), (char *)NULL },
  { "read", read_builtin, BUILTIN_ENABLED | STATIC_BUILTIN | POSIX_BUILTIN | ARRAYREF_BUILTIN, read_doc,
     N_("read [-ers] [-a array] [-d delim] [-i text] [-n nchars] [-N nchars] [-p prompt] [-t timeout] [-u fd] [name ...]"), (char *)NULL },
  { "return", return_builtin, BUILTIN_ENABLED | STATIC_BUILTIN | SPECIAL_BUILTIN, return_doc,
     N_("return [n]"), (char *)NULL },
  { "set", set_builtin, BUILTIN_ENABLED | STATIC_BUILTIN | SPECIAL_BUILTIN, set_doc,
     N_("set [-abefhkmnptuvxBCEHPT] [-o option-name] [--] [-] [arg ...]"), (char *)NULL },
  { "unset", unset_builtin, BUILTIN_ENABLED | STATIC_BUILTIN | SPECIAL_BUILTIN | ARRAYREF_BUILTIN, unset_doc,
     N_("unset [-f] [-v] [-n] [name ...]"), (char *)NULL },
  { "export", export_builtin, BUILTIN_ENABLED | STATIC_BUILTIN | SPECIAL_BUILTIN | ASSIGNMENT_BUILTIN, export_doc,
     N_("export [-fn] [name[=value] ...] or export -p"), (char *)NULL },
  { "readonly", readonly_builtin, BUILTIN_ENABLED | STATIC_BUILTIN | SPECIAL_BUILTIN | ASSIGNMENT_BUILTIN, readonly_doc,
     N_("readonly [-aAf] [name[=value] ...] or readonly -p"), (char *)NULL },
  { "shift", shift_builtin, BUILTIN_ENABLED | STATIC_BUILTIN | SPECIAL_BUILTIN, shift_doc,
     N_("shift [n]"), (char *)NULL },
  { "source", source_builtin, BUILTIN_ENABLED | STATIC_BUILTIN | SPECIAL_BUILTIN, source_doc,
     N_("source filename [arguments]"), (char *)NULL },
  { ".", source_builtin, BUILTIN_ENABLED | STATIC_BUILTIN | SPECIAL_BUILTIN, dot_doc,
     N_(". filename [arguments]"), (char *)NULL },
#if defined (JOB_CONTROL)
  { "suspend", suspend_builtin, BUILTIN_ENABLED | STATIC_BUILTIN, suspend_doc,
     N_("suspend [-f]"), (char *)NULL },
#endif /* JOB_CONTROL */
  { "test", test_builtin, BUILTIN_ENABLED | STATIC_BUILTIN | ARRAYREF_BUILTIN, test_doc,
     N_("test [expr]"), (char *)NULL },
  { "[", test_builtin, BUILTIN_ENABLED | STATIC_BUILTIN | ARRAYREF_BUILTIN, test_bracket_doc,
     N_("[ arg... ]"), (char *)NULL },
  { "times", times_builtin, BUILTIN_ENABLED | STATIC_BUILTIN | SPECIAL_BUILTIN, times_doc,
     "times", (char *)NULL },
  { "trap", trap_builtin, BUILTIN_ENABLED | STATIC_BUILTIN | SPECIAL_BUILTIN, trap_doc,
     N_("trap [-lp] [[arg] signal_spec ...]"), (char *)NULL },
  { "type", type_builtin, BUILTIN_ENABLED | STATIC_BUILTIN, type_doc,
     N_("type [-afptP] name [name ...]"), (char *)NULL },
#if !defined (_MINIX)
  { "ulimit", ulimit_builtin, BUILTIN_ENABLED | STATIC_BUILTIN, ulimit_doc,
     N_("ulimit [-SHabcdefiklmnpqrstuvxPRT] [limit]"), (char *)NULL },
#endif /* !_MINIX */
  { "umask", umask_builtin, BUILTIN_ENABLED | STATIC_BUILTIN | POSIX_BUILTIN, umask_doc,
     N_("umask [-p] [-S] [mode]"), (char *)NULL },
#if defined (JOB_CONTROL)
  { "wait", wait_builtin, BUILTIN_ENABLED | STATIC_BUILTIN | POSIX_BUILTIN | ARRAYREF_BUILTIN, wait_doc,
     N_("wait [-fn] [-p var] [id ...]"), (char *)NULL },
#endif /* JOB_CONTROL */
#if !defined (JOB_CONTROL)
  { "wait", wait_builtin, BUILTIN_ENABLED | STATIC_BUILTIN | POSIX_BUILTIN | ARRAYREF_BUILTIN, wait_doc,
     N_("wait [pid ...]"), (char *)NULL },
#endif /* !JOB_CONTROL */
  { "for", (sh_builtin_func_t *)0x0, BUILTIN_ENABLED | STATIC_BUILTIN, for_doc,
     N_("for NAME [in WORDS ... ] ; do COMMANDS; done"), (char *)NULL },
  { "for ((", (sh_builtin_func_t *)0x0, BUILTIN_ENABLED | STATIC_BUILTIN, arith_for_doc,
     N_("for (( exp1; exp2; exp3 )); do COMMANDS; done"), (char *)NULL },
  { "select", (sh_builtin_func_t *)0x0, BUILTIN_ENABLED | STATIC_BUILTIN, select_doc,
     N_("select NAME [in WORDS ... ;] do COMMANDS; done"), (char *)NULL },
  { "time", (sh_builtin_func_t *)0x0, BUILTIN_ENABLED | STATIC_BUILTIN, time_doc,
     N_("time [-p] pipeline"), (char *)NULL },
  { "case", (sh_builtin_func_t *)0x0, BUILTIN_ENABLED | STATIC_BUILTIN, case_doc,
     N_("case WORD in [PATTERN [| PATTERN]...) COMMANDS ;;]... esac"), (char *)NULL },
  { "if", (sh_builtin_func_t *)0x0, BUILTIN_ENABLED | STATIC_BUILTIN, if_doc,
     N_("if COMMANDS; then COMMANDS; [ elif COMMANDS; then COMMANDS; ]... [ else COMMANDS; ] fi"), (char *)NULL },
  { "while", (sh_builtin_func_t *)0x0, BUILTIN_ENABLED | STATIC_BUILTIN, while_doc,
     N_("while COMMANDS; do COMMANDS-2; done"), (char *)NULL },
  { "until", (sh_builtin_func_t *)0x0, BUILTIN_ENABLED | STATIC_BUILTIN, until_doc,
     N_("until COMMANDS; do COMMANDS-2; done"), (char *)NULL },
  { "coproc", (sh_builtin_func_t *)0x0, BUILTIN_ENABLED | STATIC_BUILTIN, coproc_doc,
     N_("coproc [NAME] command [redirections]"), (char *)NULL },
  { "function", (sh_builtin_func_t *)0x0, BUILTIN_ENABLED | STATIC_BUILTIN, function_doc,
     N_("function name { COMMANDS ; } or name () { COMMANDS ; }"), (char *)NULL },
  { "{ ... }", (sh_builtin_func_t *)0x0, BUILTIN_ENABLED | STATIC_BUILTIN, grouping_braces_doc,
     N_("{ COMMANDS ; }"), (char *)NULL },
  { "%", (sh_builtin_func_t *)0x0, BUILTIN_ENABLED | STATIC_BUILTIN, fg_percent_doc,
     N_("job_spec [&]"), (char *)NULL },
  { "(( ... ))", (sh_builtin_func_t *)0x0, BUILTIN_ENABLED | STATIC_BUILTIN, arith_doc,
     N_("(( expression ))"), (char *)NULL },
  { "[[ ... ]]", (sh_builtin_func_t *)0x0, BUILTIN_ENABLED | STATIC_BUILTIN, conditional_doc,
     N_("[[ expression ]]"), (char *)NULL },
  { "variables", (sh_builtin_func_t *)0x0, BUILTIN_ENABLED | STATIC_BUILTIN, variable_help_doc,
     N_("variables - Names and meanings of some shell variables"), (char *)NULL },
#if defined (PUSHD_AND_POPD)
  { "pushd", pushd_builtin, BUILTIN_ENABLED | STATIC_BUILTIN, pushd_doc,
     N_("pushd [-n] [+N | -N | dir]"), (char *)NULL },
#endif /* PUSHD_AND_POPD */
#if defined (PUSHD_AND_POPD)
  { "popd", popd_builtin, BUILTIN_ENABLED | STATIC_BUILTIN, popd_doc,
     N_("popd [-n] [+N | -N]"), (char *)NULL },
#endif /* PUSHD_AND_POPD */
#if defined (PUSHD_AND_POPD)
  { "dirs", dirs_builtin, BUILTIN_ENABLED | STATIC_BUILTIN, dirs_doc,
     N_("dirs [-clpv] [+N] [-N]"), (char *)NULL },
#endif /* PUSHD_AND_POPD */
  { "shopt", shopt_builtin, BUILTIN_ENABLED | STATIC_BUILTIN, shopt_doc,
     N_("shopt [-pqsu] [-o] [optname ...]"), (char *)NULL },
  { "printf", printf_builtin, BUILTIN_ENABLED | STATIC_BUILTIN | ARRAYREF_BUILTIN, printf_doc,
     N_("printf [-v var] format [arguments]"), (char *)NULL },
#if defined (PROGRAMMABLE_COMPLETION)
  { "complete", complete_builtin, BUILTIN_ENABLED | STATIC_BUILTIN, complete_doc,
     N_("complete [-abcdefgjksuv] [-pr] [-DEI] [-o option] [-A action] [-G globpat] [-W wordlist] [-F function] [-C command] [-X filterpat] [-P prefix] [-S suffix] [name ...]"), (char *)NULL },
#endif /* PROGRAMMABLE_COMPLETION */
#if defined (PROGRAMMABLE_COMPLETION)
  { "compgen", compgen_builtin, BUILTIN_ENABLED | STATIC_BUILTIN, compgen_doc,
     N_("compgen [-abcdefgjksuv] [-o option] [-A action] [-G globpat] [-W wordlist] [-F function] [-C command] [-X filterpat] [-P prefix] [-S suffix] [word]"), (char *)NULL },
#endif /* PROGRAMMABLE_COMPLETION */
#if defined (PROGRAMMABLE_COMPLETION)
  { "compopt", compopt_builtin, BUILTIN_ENABLED | STATIC_BUILTIN, compopt_doc,
     N_("compopt [-o|+o option] [-DEI] [name ...]"), (char *)NULL },
#endif /* PROGRAMMABLE_COMPLETION */
  { "mapfile", mapfile_builtin, BUILTIN_ENABLED | STATIC_BUILTIN, mapfile_doc,
     N_("mapfile [-d delim] [-n count] [-O origin] [-s count] [-t] [-u fd] [-C callback] [-c quantum] [array]"), (char *)NULL },
  { "readarray", mapfile_builtin, BUILTIN_ENABLED | STATIC_BUILTIN, readarray_doc,
     N_("readarray [-d delim] [-n count] [-O origin] [-s count] [-t] [-u fd] [-C callback] [-c quantum] [array]"), (char *)NULL },
  { (char *)0x0, (sh_builtin_func_t *)0x0, 0, (char **)0x0, (char *)0x0, (char *)0x0 }
};

struct builtin *shell_builtins = static_shell_builtins;
struct builtin *current_builtin;

int num_shell_builtins =
	sizeof (static_shell_builtins) / sizeof (struct builtin) - 1;
#if defined (ALIAS)
char * const alias_doc[] = {
#if defined (HELP_BUILTIN)
N_("Define or display aliases.\n\
    \n\
    Without arguments, `alias' prints the list of aliases in the reusable\n\
    form `alias NAME=VALUE' on standard output.\n\
    \n\
    Otherwise, an alias is defined for each NAME whose VALUE is given.\n\
    A trailing space in VALUE causes the next word to be checked for\n\
    alias substitution when the alias is expanded.\n\
    \n\
    Options:\n\
      -p	print all defined aliases in a reusable format\n\
    \n\
    Exit Status:\n\
    alias returns true unless a NAME is supplied for which no alias has been\n\
    defined."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
#endif /* ALIAS */
#if defined (ALIAS)
char * const unalias_doc[] = {
#if defined (HELP_BUILTIN)
N_("Remove each NAME from the list of defined aliases.\n\
    \n\
    Options:\n\
      -a	remove all alias definitions\n\
    \n\
    Return success unless a NAME is not an existing alias."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
#endif /* ALIAS */
#if defined (READLINE)
char * const bind_doc[] = {
#if defined (HELP_BUILTIN)
N_("Set Readline key bindings and variables.\n\
    \n\
    Bind a key sequence to a Readline function or a macro, or set a\n\
    Readline variable.  The non-option argument syntax is equivalent to\n\
    that found in ~/.inputrc, but must be passed as a single argument:\n\
    e.g., bind '\"\\C-x\\C-r\": re-read-init-file'.\n\
    \n\
    Options:\n\
      -m  keymap         Use KEYMAP as the keymap for the duration of this\n\
                         command.  Acceptable keymap names are emacs,\n\
                         emacs-standard, emacs-meta, emacs-ctlx, vi, vi-move,\n\
                         vi-command, and vi-insert.\n\
      -l                 List names of functions.\n\
      -P                 List function names and bindings.\n\
      -p                 List functions and bindings in a form that can be\n\
                         reused as input.\n\
      -S                 List key sequences that invoke macros and their values\n\
      -s                 List key sequences that invoke macros and their values\n\
                         in a form that can be reused as input.\n\
      -V                 List variable names and values\n\
      -v                 List variable names and values in a form that can\n\
                         be reused as input.\n\
      -q  function-name  Query about which keys invoke the named function.\n\
      -u  function-name  Unbind all keys which are bound to the named function.\n\
      -r  keyseq         Remove the binding for KEYSEQ.\n\
      -f  filename       Read key bindings from FILENAME.\n\
      -x  keyseq:shell-command	Cause SHELL-COMMAND to be executed when\n\
    				KEYSEQ is entered.\n\
      -X                 List key sequences bound with -x and associated commands\n\
                         in a form that can be reused as input.\n\
    \n\
    Exit Status:\n\
    bind returns 0 unless an unrecognized option is given or an error occurs."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
#endif /* READLINE */
char * const break_doc[] = {
#if defined (HELP_BUILTIN)
N_("Exit for, while, or until loops.\n\
    \n\
    Exit a FOR, WHILE or UNTIL loop.  If N is specified, break N enclosing\n\
    loops.\n\
    \n\
    Exit Status:\n\
    The exit status is 0 unless N is not greater than or equal to 1."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
char * const continue_doc[] = {
#if defined (HELP_BUILTIN)
N_("Resume for, while, or until loops.\n\
    \n\
    Resumes the next iteration of the enclosing FOR, WHILE or UNTIL loop.\n\
    If N is specified, resumes the Nth enclosing loop.\n\
    \n\
    Exit Status:\n\
    The exit status is 0 unless N is not greater than or equal to 1."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
char * const builtin_doc[] = {
#if defined (HELP_BUILTIN)
N_("Execute shell builtins.\n\
    \n\
    Execute SHELL-BUILTIN with arguments ARGs without performing command\n\
    lookup.  This is useful when you wish to reimplement a shell builtin\n\
    as a shell function, but need to execute the builtin within the function.\n\
    \n\
    Exit Status:\n\
    Returns the exit status of SHELL-BUILTIN, or false if SHELL-BUILTIN is\n\
    not a shell builtin."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
#if defined (DEBUGGER)
char * const caller_doc[] = {
#if defined (HELP_BUILTIN)
N_("Return the context of the current subroutine call.\n\
    \n\
    Without EXPR, returns \"$line $filename\".  With EXPR, returns\n\
    \"$line $subroutine $filename\"; this extra information can be used to\n\
    provide a stack trace.\n\
    \n\
    The value of EXPR indicates how many call frames to go back before the\n\
    current one; the top frame is frame 0.\n\
    \n\
    Exit Status:\n\
    Returns 0 unless the shell is not executing a shell function or EXPR\n\
    is invalid."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
#endif /* DEBUGGER */
char * const cd_doc[] = {
#if defined (HELP_BUILTIN)
N_("Change the shell working directory.\n\
    \n\
    Change the current directory to DIR.  The default DIR is the value of the\n\
    HOME shell variable. If DIR is \"-\", it is converted to $OLDPWD.\n\
    \n\
    The variable CDPATH defines the search path for the directory containing\n\
    DIR.  Alternative directory names in CDPATH are separated by a colon (:).\n\
    A null directory name is the same as the current directory.  If DIR begins\n\
    with a slash (/), then CDPATH is not used.\n\
    \n\
    If the directory is not found, and the shell option `cdable_vars' is set,\n\
    the word is assumed to be  a variable name.  If that variable has a value,\n\
    its value is used for DIR.\n\
    \n\
    Options:\n\
      -L	force symbolic links to be followed: resolve symbolic\n\
    		links in DIR after processing instances of `..'\n\
      -P	use the physical directory structure without following\n\
    		symbolic links: resolve symbolic links in DIR before\n\
    		processing instances of `..'\n\
      -e	if the -P option is supplied, and the current working\n\
    		directory cannot be determined successfully, exit with\n\
    		a non-zero status\n\
      -@	on systems that support it, present a file with extended\n\
    		attributes as a directory containing the file attributes\n\
    \n\
    The default is to follow symbolic links, as if `-L' were specified.\n\
    `..' is processed by removing the immediately previous pathname component\n\
    back to a slash or the beginning of DIR.\n\
    \n\
    Exit Status:\n\
    Returns 0 if the directory is changed, and if $PWD is set successfully when\n\
    -P is used; non-zero otherwise."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
char * const pwd_doc[] = {
#if defined (HELP_BUILTIN)
N_("Print the name of the current working directory.\n\
    \n\
    Options:\n\
      -L	print the value of $PWD if it names the current working\n\
    		directory\n\
      -P	print the physical directory, without any symbolic links\n\
    \n\
    By default, `pwd' behaves as if `-L' were specified.\n\
    \n\
    Exit Status:\n\
    Returns 0 unless an invalid option is given or the current directory\n\
    cannot be read."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
char * const colon_doc[] = {
#if defined (HELP_BUILTIN)
N_("Null command.\n\
    \n\
    No effect; the command does nothing.\n\
    \n\
    Exit Status:\n\
    Always succeeds."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
char * const true_doc[] = {
#if defined (HELP_BUILTIN)
N_("Return a successful result.\n\
    \n\
    Exit Status:\n\
    Always succeeds."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
char * const false_doc[] = {
#if defined (HELP_BUILTIN)
N_("Return an unsuccessful result.\n\
    \n\
    Exit Status:\n\
    Always fails."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
char * const command_doc[] = {
#if defined (HELP_BUILTIN)
N_("Execute a simple command or display information about commands.\n\
    \n\
    Runs COMMAND with ARGS suppressing  shell function lookup, or display\n\
    information about the specified COMMANDs.  Can be used to invoke commands\n\
    on disk when a function with the same name exists.\n\
    \n\
    Options:\n\
      -p    use a default value for PATH that is guaranteed to find all of\n\
            the standard utilities\n\
      -v    print a description of COMMAND similar to the `type' builtin\n\
      -V    print a more verbose description of each COMMAND\n\
    \n\
    Exit Status:\n\
    Returns exit status of COMMAND, or failure if COMMAND is not found."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
char * const declare_doc[] = {
#if defined (HELP_BUILTIN)
N_("Set variable values and attributes.\n\
    \n\
    Declare variables and give them attributes.  If no NAMEs are given,\n\
    display the attributes and values of all variables.\n\
    \n\
    Options:\n\
      -f	restrict action or display to function names and definitions\n\
      -F	restrict display to function names only (plus line number and\n\
    		source file when debugging)\n\
      -g	create global variables when used in a shell function; otherwise\n\
    		ignored\n\
      -I	if creating a local variable, inherit the attributes and value\n\
    		of a variable with the same name at a previous scope\n\
      -p	display the attributes and value of each NAME\n\
    \n\
    Options which set attributes:\n\
      -a	to make NAMEs indexed arrays (if supported)\n\
      -A	to make NAMEs associative arrays (if supported)\n\
      -i	to make NAMEs have the `integer' attribute\n\
      -l	to convert the value of each NAME to lower case on assignment\n\
      -n	make NAME a reference to the variable named by its value\n\
      -r	to make NAMEs readonly\n\
      -t	to make NAMEs have the `trace' attribute\n\
      -u	to convert the value of each NAME to upper case on assignment\n\
      -x	to make NAMEs export\n\
    \n\
    Using `+' instead of `-' turns off the given attribute.\n\
    \n\
    Variables with the integer attribute have arithmetic evaluation (see\n\
    the `let' command) performed when the variable is assigned a value.\n\
    \n\
    When used in a function, `declare' makes NAMEs local, as with the `local'\n\
    command.  The `-g' option suppresses this behavior.\n\
    \n\
    Exit Status:\n\
    Returns success unless an invalid option is supplied or a variable\n\
    assignment error occurs."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
char * const typeset_doc[] = {
#if defined (HELP_BUILTIN)
N_("Set variable values and attributes.\n\
    \n\
    A synonym for `declare'.  See `help declare'."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
char * const local_doc[] = {
#if defined (HELP_BUILTIN)
N_("Define local variables.\n\
    \n\
    Create a local variable called NAME, and give it VALUE.  OPTION can\n\
    be any option accepted by `declare'.\n\
    \n\
    Local variables can only be used within a function; they are visible\n\
    only to the function where they are defined and its children.\n\
    \n\
    Exit Status:\n\
    Returns success unless an invalid option is supplied, a variable\n\
    assignment error occurs, or the shell is not executing a function."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
#if defined (V9_ECHO)
char * const echo_doc[] = {
#if defined (HELP_BUILTIN)
N_("Write arguments to the standard output.\n\
    \n\
    Display the ARGs, separated by a single space character and followed by a\n\
    newline, on the standard output.\n\
    \n\
    Options:\n\
      -n	do not append a newline\n\
      -e	enable interpretation of the following backslash escapes\n\
      -E	explicitly suppress interpretation of backslash escapes\n\
    \n\
    `echo' interprets the following backslash-escaped characters:\n\
      \\a	alert (bell)\n\
      \\b	backspace\n\
      \\c	suppress further output\n\
      \\e	escape character\n\
      \\E	escape character\n\
      \\f	form feed\n\
      \\n	new line\n\
      \\r	carriage return\n\
      \\t	horizontal tab\n\
      \\v	vertical tab\n\
      \\\\	backslash\n\
      \\0nnn	the character whose ASCII code is NNN (octal).  NNN can be\n\
    		0 to 3 octal digits\n\
      \\xHH	the eight-bit character whose value is HH (hexadecimal).  HH\n\
    		can be one or two hex digits\n\
      \\uHHHH	the Unicode character whose value is the hexadecimal value HHHH.\n\
    		HHHH can be one to four hex digits.\n\
      \\UHHHHHHHH the Unicode character whose value is the hexadecimal value\n\
    		HHHHHHHH. HHHHHHHH can be one to eight hex digits.\n\
    \n\
    Exit Status:\n\
    Returns success unless a write error occurs."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
#endif /* V9_ECHO */
#if !defined (V9_ECHO)
char * const echo_doc[] = {
#if defined (HELP_BUILTIN)
N_("Write arguments to the standard output.\n\
    \n\
    Display the ARGs on the standard output followed by a newline.\n\
    \n\
    Options:\n\
      -n	do not append a newline\n\
    \n\
    Exit Status:\n\
    Returns success unless a write error occurs."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
#endif /* !V9_ECHO */
char * const enable_doc[] = {
#if defined (HELP_BUILTIN)
N_("Enable and disable shell builtins.\n\
    \n\
    Enables and disables builtin shell commands.  Disabling allows you to\n\
    execute a disk command which has the same name as a shell builtin\n\
    without using a full pathname.\n\
    \n\
    Options:\n\
      -a	print a list of builtins showing whether or not each is enabled\n\
      -n	disable each NAME or display a list of disabled builtins\n\
      -p	print the list of builtins in a reusable format\n\
      -s	print only the names of Posix `special' builtins\n\
    \n\
    Options controlling dynamic loading:\n\
      -f	Load builtin NAME from shared object FILENAME\n\
      -d	Remove a builtin loaded with -f\n\
    \n\
    Without options, each NAME is enabled.\n\
    \n\
    To use the `test' found in $PATH instead of the shell builtin\n\
    version, type `enable -n test'.\n\
    \n\
    Exit Status:\n\
    Returns success unless NAME is not a shell builtin or an error occurs."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
char * const eval_doc[] = {
#if defined (HELP_BUILTIN)
N_("Execute arguments as a shell command.\n\
    \n\
    Combine ARGs into a single string, use the result as input to the shell,\n\
    and execute the resulting commands.\n\
    \n\
    Exit Status:\n\
    Returns exit status of command or success if command is null."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
char * const getopts_doc[] = {
#if defined (HELP_BUILTIN)
N_("Parse option arguments.\n\
    \n\
    Getopts is used by shell procedures to parse positional parameters\n\
    as options.\n\
    \n\
    OPTSTRING contains the option letters to be recognized; if a letter\n\
    is followed by a colon, the option is expected to have an argument,\n\
    which should be separated from it by white space.\n\
    \n\
    Each time it is invoked, getopts will place the next option in the\n\
    shell variable $name, initializing name if it does not exist, and\n\
    the index of the next argument to be processed into the shell\n\
    variable OPTIND.  OPTIND is initialized to 1 each time the shell or\n\
    a shell script is invoked.  When an option requires an argument,\n\
    getopts places that argument into the shell variable OPTARG.\n\
    \n\
    getopts reports errors in one of two ways.  If the first character\n\
    of OPTSTRING is a colon, getopts uses silent error reporting.  In\n\
    this mode, no error messages are printed.  If an invalid option is\n\
    seen, getopts places the option character found into OPTARG.  If a\n\
    required argument is not found, getopts places a ':' into NAME and\n\
    sets OPTARG to the option character found.  If getopts is not in\n\
    silent mode, and an invalid option is seen, getopts places '?' into\n\
    NAME and unsets OPTARG.  If a required argument is not found, a '?'\n\
    is placed in NAME, OPTARG is unset, and a diagnostic message is\n\
    printed.\n\
    \n\
    If the shell variable OPTERR has the value 0, getopts disables the\n\
    printing of error messages, even if the first character of\n\
    OPTSTRING is not a colon.  OPTERR has the value 1 by default.\n\
    \n\
    Getopts normally parses the positional parameters, but if arguments\n\
    are supplied as ARG values, they are parsed instead.\n\
    \n\
    Exit Status:\n\
    Returns success if an option is found; fails if the end of options is\n\
    encountered or an error occurs."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
char * const exec_doc[] = {
#if defined (HELP_BUILTIN)
N_("Replace the shell with the given command.\n\
    \n\
    Execute COMMAND, replacing this shell with the specified program.\n\
    ARGUMENTS become the arguments to COMMAND.  If COMMAND is not specified,\n\
    any redirections take effect in the current shell.\n\
    \n\
    Options:\n\
      -a name	pass NAME as the zeroth argument to COMMAND\n\
      -c	execute COMMAND with an empty environment\n\
      -l	place a dash in the zeroth argument to COMMAND\n\
    \n\
    If the command cannot be executed, a non-interactive shell exits, unless\n\
    the shell option `execfail' is set.\n\
    \n\
    Exit Status:\n\
    Returns success unless COMMAND is not found or a redirection error occurs."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
char * const exit_doc[] = {
#if defined (HELP_BUILTIN)
N_("Exit the shell.\n\
    \n\
    Exits the shell with a status of N.  If N is omitted, the exit status\n\
    is that of the last command executed."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
char * const logout_doc[] = {
#if defined (HELP_BUILTIN)
N_("Exit a login shell.\n\
    \n\
    Exits a login shell with exit status N.  Returns an error if not executed\n\
    in a login shell."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
#if defined (HISTORY)
char * const fc_doc[] = {
#if defined (HELP_BUILTIN)
N_("Display or execute commands from the history list.\n\
    \n\
    fc is used to list or edit and re-execute commands from the history list.\n\
    FIRST and LAST can be numbers specifying the range, or FIRST can be a\n\
    string, which means the most recent command beginning with that\n\
    string.\n\
    \n\
    Options:\n\
      -e ENAME	select which editor to use.  Default is FCEDIT, then EDITOR,\n\
    		then vi\n\
      -l 	list lines instead of editing\n\
      -n	omit line numbers when listing\n\
      -r	reverse the order of the lines (newest listed first)\n\
    \n\
    With the `fc -s [pat=rep ...] [command]' format, COMMAND is\n\
    re-executed after the substitution OLD=NEW is performed.\n\
    \n\
    A useful alias to use with this is r='fc -s', so that typing `r cc'\n\
    runs the last command beginning with `cc' and typing `r' re-executes\n\
    the last command.\n\
    \n\
    Exit Status:\n\
    Returns success or status of executed command; non-zero if an error occurs."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
#endif /* HISTORY */
#if defined (JOB_CONTROL)
char * const fg_doc[] = {
#if defined (HELP_BUILTIN)
N_("Move job to the foreground.\n\
    \n\
    Place the job identified by JOB_SPEC in the foreground, making it the\n\
    current job.  If JOB_SPEC is not present, the shell's notion of the\n\
    current job is used.\n\
    \n\
    Exit Status:\n\
    Status of command placed in foreground, or failure if an error occurs."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
#endif /* JOB_CONTROL */
#if defined (JOB_CONTROL)
char * const bg_doc[] = {
#if defined (HELP_BUILTIN)
N_("Move jobs to the background.\n\
    \n\
    Place the jobs identified by each JOB_SPEC in the background, as if they\n\
    had been started with `&'.  If JOB_SPEC is not present, the shell's notion\n\
    of the current job is used.\n\
    \n\
    Exit Status:\n\
    Returns success unless job control is not enabled or an error occurs."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
#endif /* JOB_CONTROL */
char * const hash_doc[] = {
#if defined (HELP_BUILTIN)
N_("Remember or display program locations.\n\
    \n\
    Determine and remember the full pathname of each command NAME.  If\n\
    no arguments are given, information about remembered commands is displayed.\n\
    \n\
    Options:\n\
      -d	forget the remembered location of each NAME\n\
      -l	display in a format that may be reused as input\n\
      -p pathname	use PATHNAME as the full pathname of NAME\n\
      -r	forget all remembered locations\n\
      -t	print the remembered location of each NAME, preceding\n\
    		each location with the corresponding NAME if multiple\n\
    		NAMEs are given\n\
    Arguments:\n\
      NAME	Each NAME is searched for in $PATH and added to the list\n\
    		of remembered commands.\n\
    \n\
    Exit Status:\n\
    Returns success unless NAME is not found or an invalid option is given."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
#if defined (HELP_BUILTIN)
char * const help_doc[] = {
#if defined (HELP_BUILTIN)
N_("Display information about builtin commands.\n\
    \n\
    Displays brief summaries of builtin commands.  If PATTERN is\n\
    specified, gives detailed help on all commands matching PATTERN,\n\
    otherwise the list of help topics is printed.\n\
    \n\
    Options:\n\
      -d	output short description for each topic\n\
      -m	display usage in pseudo-manpage format\n\
      -s	output only a short usage synopsis for each topic matching\n\
    		PATTERN\n\
    \n\
    Arguments:\n\
      PATTERN	Pattern specifying a help topic\n\
    \n\
    Exit Status:\n\
    Returns success unless PATTERN is not found or an invalid option is given."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
#endif /* HELP_BUILTIN */
#if defined (HISTORY)
char * const history_doc[] = {
#if defined (HELP_BUILTIN)
N_("Display or manipulate the history list.\n\
    \n\
    Display the history list with line numbers, prefixing each modified\n\
    entry with a `*'.  An argument of N lists only the last N entries.\n\
    \n\
    Options:\n\
      -c	clear the history list by deleting all of the entries\n\
      -d offset	delete the history entry at position OFFSET. Negative\n\
    		offsets count back from the end of the history list\n\
    \n\
      -a	append history lines from this session to the history file\n\
      -n	read all history lines not already read from the history file\n\
    		and append them to the history list\n\
      -r	read the history file and append the contents to the history\n\
    		list\n\
      -w	write the current history to the history file\n\
    \n\
      -p	perform history expansion on each ARG and display the result\n\
    		without storing it in the history list\n\
      -s	append the ARGs to the history list as a single entry\n\
    \n\
    If FILENAME is given, it is used as the history file.  Otherwise,\n\
    if HISTFILE has a value, that is used, else ~/.bash_history.\n\
    \n\
    If the HISTTIMEFORMAT variable is set and not null, its value is used\n\
    as a format string for strftime(3) to print the time stamp associated\n\
    with each displayed history entry.  No time stamps are printed otherwise.\n\
    \n\
    Exit Status:\n\
    Returns success unless an invalid option is given or an error occurs."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
#endif /* HISTORY */
#if defined (JOB_CONTROL)
char * const jobs_doc[] = {
#if defined (HELP_BUILTIN)
N_("Display status of jobs.\n\
    \n\
    Lists the active jobs.  JOBSPEC restricts output to that job.\n\
    Without options, the status of all active jobs is displayed.\n\
    \n\
    Options:\n\
      -l	lists process IDs in addition to the normal information\n\
      -n	lists only processes that have changed status since the last\n\
    		notification\n\
      -p	lists process IDs only\n\
      -r	restrict output to running jobs\n\
      -s	restrict output to stopped jobs\n\
    \n\
    If -x is supplied, COMMAND is run after all job specifications that\n\
    appear in ARGS have been replaced with the process ID of that job's\n\
    process group leader.\n\
    \n\
    Exit Status:\n\
    Returns success unless an invalid option is given or an error occurs.\n\
    If -x is used, returns the exit status of COMMAND."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
#endif /* JOB_CONTROL */
#if defined (JOB_CONTROL)
char * const disown_doc[] = {
#if defined (HELP_BUILTIN)
N_("Remove jobs from current shell.\n\
    \n\
    Removes each JOBSPEC argument from the table of active jobs.  Without\n\
    any JOBSPECs, the shell uses its notion of the current job.\n\
    \n\
    Options:\n\
      -a	remove all jobs if JOBSPEC is not supplied\n\
      -h	mark each JOBSPEC so that SIGHUP is not sent to the job if the\n\
    		shell receives a SIGHUP\n\
      -r	remove only running jobs\n\
    \n\
    Exit Status:\n\
    Returns success unless an invalid option or JOBSPEC is given."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
#endif /* JOB_CONTROL */
char * const kill_doc[] = {
#if defined (HELP_BUILTIN)
N_("Send a signal to a job.\n\
    \n\
    Send the processes identified by PID or JOBSPEC the signal named by\n\
    SIGSPEC or SIGNUM.  If neither SIGSPEC nor SIGNUM is present, then\n\
    SIGTERM is assumed.\n\
    \n\
    Options:\n\
      -s sig	SIG is a signal name\n\
      -n sig	SIG is a signal number\n\
      -l	list the signal names; if arguments follow `-l' they are\n\
    		assumed to be signal numbers for which names should be listed\n\
      -L	synonym for -l\n\
    \n\
    Kill is a shell builtin for two reasons: it allows job IDs to be used\n\
    instead of process IDs, and allows processes to be killed if the limit\n\
    on processes that you can create is reached.\n\
    \n\
    Exit Status:\n\
    Returns success unless an invalid option is given or an error occurs."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
char * const let_doc[] = {
#if defined (HELP_BUILTIN)
N_("Evaluate arithmetic expressions.\n\
    \n\
    Evaluate each ARG as an arithmetic expression.  Evaluation is done in\n\
    fixed-width integers with no check for overflow, though division by 0\n\
    is trapped and flagged as an error.  The following list of operators is\n\
    grouped into levels of equal-precedence operators.  The levels are listed\n\
    in order of decreasing precedence.\n\
    \n\
    	id++, id--	variable post-increment, post-decrement\n\
    	++id, --id	variable pre-increment, pre-decrement\n\
    	-, +		unary minus, plus\n\
    	!, ~		logical and bitwise negation\n\
    	**		exponentiation\n\
    	*, /, %		multiplication, division, remainder\n\
    	+, -		addition, subtraction\n\
    	<<, >>		left and right bitwise shifts\n\
    	<=, >=, <, >	comparison\n\
    	==, !=		equality, inequality\n\
    	&		bitwise AND\n\
    	^		bitwise XOR\n\
    	|		bitwise OR\n\
    	&&		logical AND\n\
    	||		logical OR\n\
    	expr ? expr : expr\n\
    			conditional operator\n\
    	=, *=, /=, %=,\n\
    	+=, -=, <<=, >>=,\n\
    	&=, ^=, |=	assignment\n\
    \n\
    Shell variables are allowed as operands.  The name of the variable\n\
    is replaced by its value (coerced to a fixed-width integer) within\n\
    an expression.  The variable need not have its integer attribute\n\
    turned on to be used in an expression.\n\
    \n\
    Operators are evaluated in order of precedence.  Sub-expressions in\n\
    parentheses are evaluated first and may override the precedence\n\
    rules above.\n\
    \n\
    Exit Status:\n\
    If the last ARG evaluates to 0, let returns 1; let returns 0 otherwise."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
char * const read_doc[] = {
#if defined (HELP_BUILTIN)
N_("Read a line from the standard input and split it into fields.\n\
    \n\
    Reads a single line from the standard input, or from file descriptor FD\n\
    if the -u option is supplied.  The line is split into fields as with word\n\
    splitting, and the first word is assigned to the first NAME, the second\n\
    word to the second NAME, and so on, with any leftover words assigned to\n\
    the last NAME.  Only the characters found in $IFS are recognized as word\n\
    delimiters. By default, the backslash character escapes delimiter characters\n\
    and newline.\n\
    \n\
    If no NAMEs are supplied, the line read is stored in the REPLY variable.\n\
    \n\
    Options:\n\
      -a array	assign the words read to sequential indices of the array\n\
    		variable ARRAY, starting at zero\n\
      -d delim	continue until the first character of DELIM is read, rather\n\
    		than newline\n\
      -e	use Readline to obtain the line\n\
      -i text	use TEXT as the initial text for Readline\n\
      -n nchars	return after reading NCHARS characters rather than waiting\n\
    		for a newline, but honor a delimiter if fewer than\n\
    		NCHARS characters are read before the delimiter\n\
      -N nchars	return only after reading exactly NCHARS characters, unless\n\
    		EOF is encountered or read times out, ignoring any\n\
    		delimiter\n\
      -p prompt	output the string PROMPT without a trailing newline before\n\
    		attempting to read\n\
      -r	do not allow backslashes to escape any characters\n\
      -s	do not echo input coming from a terminal\n\
      -t timeout	time out and return failure if a complete line of\n\
    		input is not read within TIMEOUT seconds.  The value of the\n\
    		TMOUT variable is the default timeout.  TIMEOUT may be a\n\
    		fractional number.  If TIMEOUT is 0, read returns\n\
    		immediately, without trying to read any data, returning\n\
    		success only if input is available on the specified\n\
    		file descriptor.  The exit status is greater than 128\n\
    		if the timeout is exceeded\n\
      -u fd	read from file descriptor FD instead of the standard input\n\
    \n\
    Exit Status:\n\
    The return code is zero, unless end-of-file is encountered, read times out\n\
    (in which case it's greater than 128), a variable assignment error occurs,\n\
    or an invalid file descriptor is supplied as the argument to -u."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
char * const return_doc[] = {
#if defined (HELP_BUILTIN)
N_("Return from a shell function.\n\
    \n\
    Causes a function or sourced script to exit with the return value\n\
    specified by N.  If N is omitted, the return status is that of the\n\
    last command executed within the function or script.\n\
    \n\
    Exit Status:\n\
    Returns N, or failure if the shell is not executing a function or script."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
char * const set_doc[] = {
#if defined (HELP_BUILTIN)
N_("Set or unset values of shell options and positional parameters.\n\
    \n\
    Change the value of shell attributes and positional parameters, or\n\
    display the names and values of shell variables.\n\
    \n\
    Options:\n\
      -a  Mark variables which are modified or created for export.\n\
      -b  Notify of job termination immediately.\n\
      -e  Exit immediately if a command exits with a non-zero status.\n\
      -f  Disable file name generation (globbing).\n\
      -h  Remember the location of commands as they are looked up.\n\
      -k  All assignment arguments are placed in the environment for a\n\
          command, not just those that precede the command name.\n\
      -m  Job control is enabled.\n\
      -n  Read commands but do not execute them.\n\
      -o option-name\n\
          Set the variable corresponding to option-name:\n\
              allexport    same as -a\n\
              braceexpand  same as -B\n\
              emacs        use an emacs-style line editing interface\n\
              errexit      same as -e\n\
              errtrace     same as -E\n\
              functrace    same as -T\n\
              hashall      same as -h\n\
              histexpand   same as -H\n\
              history      enable command history\n\
              ignoreeof    the shell will not exit upon reading EOF\n\
              interactive-comments\n\
                           allow comments to appear in interactive commands\n\
              keyword      same as -k\n\
              monitor      same as -m\n\
              noclobber    same as -C\n\
              noexec       same as -n\n\
              noglob       same as -f\n\
              nolog        currently accepted but ignored\n\
              notify       same as -b\n\
              nounset      same as -u\n\
              onecmd       same as -t\n\
              physical     same as -P\n\
              pipefail     the return value of a pipeline is the status of\n\
                           the last command to exit with a non-zero status,\n\
                           or zero if no command exited with a non-zero status\n\
              posix        change the behavior of bash where the default\n\
                           operation differs from the Posix standard to\n\
                           match the standard\n\
              privileged   same as -p\n\
              verbose      same as -v\n\
              vi           use a vi-style line editing interface\n\
              xtrace       same as -x\n\
      -p  Turned on whenever the real and effective user ids do not match.\n\
          Disables processing of the $ENV file and importing of shell\n\
          functions.  Turning this option off causes the effective uid and\n\
          gid to be set to the real uid and gid.\n\
      -t  Exit after reading and executing one command.\n\
      -u  Treat unset variables as an error when substituting.\n\
      -v  Print shell input lines as they are read.\n\
      -x  Print commands and their arguments as they are executed.\n\
      -B  the shell will perform brace expansion\n\
      -C  If set, disallow existing regular files to be overwritten\n\
          by redirection of output.\n\
      -E  If set, the ERR trap is inherited by shell functions.\n\
      -H  Enable ! style history substitution.  This flag is on\n\
          by default when the shell is interactive.\n\
      -P  If set, do not resolve symbolic links when executing commands\n\
          such as cd which change the current directory.\n\
      -T  If set, the DEBUG and RETURN traps are inherited by shell functions.\n\
      --  Assign any remaining arguments to the positional parameters.\n\
          If there are no remaining arguments, the positional parameters\n\
          are unset.\n\
      -   Assign any remaining arguments to the positional parameters.\n\
          The -x and -v options are turned off.\n\
    \n\
    Using + rather than - causes these flags to be turned off.  The\n\
    flags can also be used upon invocation of the shell.  The current\n\
    set of flags may be found in $-.  The remaining n ARGs are positional\n\
    parameters and are assigned, in order, to $1, $2, .. $n.  If no\n\
    ARGs are given, all shell variables are printed.\n\
    \n\
    Exit Status:\n\
    Returns success unless an invalid option is given."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
char * const unset_doc[] = {
#if defined (HELP_BUILTIN)
N_("Unset values and attributes of shell variables and functions.\n\
    \n\
    For each NAME, remove the corresponding variable or function.\n\
    \n\
    Options:\n\
      -f	treat each NAME as a shell function\n\
      -v	treat each NAME as a shell variable\n\
      -n	treat each NAME as a name reference and unset the variable itself\n\
    		rather than the variable it references\n\
    \n\
    Without options, unset first tries to unset a variable, and if that fails,\n\
    tries to unset a function.\n\
    \n\
    Some variables cannot be unset; also see `readonly'.\n\
    \n\
    Exit Status:\n\
    Returns success unless an invalid option is given or a NAME is read-only."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
char * const export_doc[] = {
#if defined (HELP_BUILTIN)
N_("Set export attribute for shell variables.\n\
    \n\
    Marks each NAME for automatic export to the environment of subsequently\n\
    executed commands.  If VALUE is supplied, assign VALUE before exporting.\n\
    \n\
    Options:\n\
      -f	refer to shell functions\n\
      -n	remove the export property from each NAME\n\
      -p	display a list of all exported variables and functions\n\
    \n\
    An argument of `--' disables further option processing.\n\
    \n\
    Exit Status:\n\
    Returns success unless an invalid option is given or NAME is invalid."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
char * const readonly_doc[] = {
#if defined (HELP_BUILTIN)
N_("Mark shell variables as unchangeable.\n\
    \n\
    Mark each NAME as read-only; the values of these NAMEs may not be\n\
    changed by subsequent assignment.  If VALUE is supplied, assign VALUE\n\
    before marking as read-only.\n\
    \n\
    Options:\n\
      -a	refer to indexed array variables\n\
      -A	refer to associative array variables\n\
      -f	refer to shell functions\n\
      -p	display a list of all readonly variables or functions,\n\
    		depending on whether or not the -f option is given\n\
    \n\
    An argument of `--' disables further option processing.\n\
    \n\
    Exit Status:\n\
    Returns success unless an invalid option is given or NAME is invalid."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
char * const shift_doc[] = {
#if defined (HELP_BUILTIN)
N_("Shift positional parameters.\n\
    \n\
    Rename the positional parameters $N+1,$N+2 ... to $1,$2 ...  If N is\n\
    not given, it is assumed to be 1.\n\
    \n\
    Exit Status:\n\
    Returns success unless N is negative or greater than $#."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
char * const source_doc[] = {
#if defined (HELP_BUILTIN)
N_("Execute commands from a file in the current shell.\n\
    \n\
    Read and execute commands from FILENAME in the current shell.  The\n\
    entries in $PATH are used to find the directory containing FILENAME.\n\
    If any ARGUMENTS are supplied, they become the positional parameters\n\
    when FILENAME is executed.\n\
    \n\
    Exit Status:\n\
    Returns the status of the last command executed in FILENAME; fails if\n\
    FILENAME cannot be read."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
char * const dot_doc[] = {
#if defined (HELP_BUILTIN)
N_("Execute commands from a file in the current shell.\n\
    \n\
    Read and execute commands from FILENAME in the current shell.  The\n\
    entries in $PATH are used to find the directory containing FILENAME.\n\
    If any ARGUMENTS are supplied, they become the positional parameters\n\
    when FILENAME is executed.\n\
    \n\
    Exit Status:\n\
    Returns the status of the last command executed in FILENAME; fails if\n\
    FILENAME cannot be read."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
#if defined (JOB_CONTROL)
char * const suspend_doc[] = {
#if defined (HELP_BUILTIN)
N_("Suspend shell execution.\n\
    \n\
    Suspend the execution of this shell until it receives a SIGCONT signal.\n\
    Unless forced, login shells and shells without job control cannot be\n\
    suspended.\n\
    \n\
    Options:\n\
      -f	force the suspend, even if the shell is a login shell or job\n\
    		control is not enabled.\n\
    \n\
    Exit Status:\n\
    Returns success unless job control is not enabled or an error occurs."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
#endif /* JOB_CONTROL */
char * const test_doc[] = {
#if defined (HELP_BUILTIN)
N_("Evaluate conditional expression.\n\
    \n\
    Exits with a status of 0 (true) or 1 (false) depending on\n\
    the evaluation of EXPR.  Expressions may be unary or binary.  Unary\n\
    expressions are often used to examine the status of a file.  There\n\
    are string operators and numeric comparison operators as well.\n\
    \n\
    The behavior of test depends on the number of arguments.  Read the\n\
    bash manual page for the complete specification.\n\
    \n\
    File operators:\n\
    \n\
      -a FILE        True if file exists.\n\
      -b FILE        True if file is block special.\n\
      -c FILE        True if file is character special.\n\
      -d FILE        True if file is a directory.\n\
      -e FILE        True if file exists.\n\
      -f FILE        True if file exists and is a regular file.\n\
      -g FILE        True if file is set-group-id.\n\
      -h FILE        True if file is a symbolic link.\n\
      -L FILE        True if file is a symbolic link.\n\
      -k FILE        True if file has its `sticky' bit set.\n\
      -p FILE        True if file is a named pipe.\n\
      -r FILE        True if file is readable by you.\n\
      -s FILE        True if file exists and is not empty.\n\
      -S FILE        True if file is a socket.\n\
      -t FD          True if FD is opened on a terminal.\n\
      -u FILE        True if the file is set-user-id.\n\
      -w FILE        True if the file is writable by you.\n\
      -x FILE        True if the file is executable by you.\n\
      -O FILE        True if the file is effectively owned by you.\n\
      -G FILE        True if the file is effectively owned by your group.\n\
      -N FILE        True if the file has been modified since it was last read.\n\
    \n\
      FILE1 -nt FILE2  True if file1 is newer than file2 (according to\n\
                       modification date).\n\
    \n\
      FILE1 -ot FILE2  True if file1 is older than file2.\n\
    \n\
      FILE1 -ef FILE2  True if file1 is a hard link to file2.\n\
    \n\
    String operators:\n\
    \n\
      -z STRING      True if string is empty.\n\
    \n\
      -n STRING\n\
         STRING      True if string is not empty.\n\
    \n\
      STRING1 = STRING2\n\
                     True if the strings are equal.\n\
      STRING1 != STRING2\n\
                     True if the strings are not equal.\n\
      STRING1 < STRING2\n\
                     True if STRING1 sorts before STRING2 lexicographically.\n\
      STRING1 > STRING2\n\
                     True if STRING1 sorts after STRING2 lexicographically.\n\
    \n\
    Other operators:\n\
    \n\
      -o OPTION      True if the shell option OPTION is enabled.\n\
      -v VAR         True if the shell variable VAR is set.\n\
      -R VAR         True if the shell variable VAR is set and is a name\n\
                     reference.\n\
      ! EXPR         True if expr is false.\n\
      EXPR1 -a EXPR2 True if both expr1 AND expr2 are true.\n\
      EXPR1 -o EXPR2 True if either expr1 OR expr2 is true.\n\
    \n\
      arg1 OP arg2   Arithmetic tests.  OP is one of -eq, -ne,\n\
                     -lt, -le, -gt, or -ge.\n\
    \n\
    Arithmetic binary operators return true if ARG1 is equal, not-equal,\n\
    less-than, less-than-or-equal, greater-than, or greater-than-or-equal\n\
    than ARG2.\n\
    \n\
    Exit Status:\n\
    Returns success if EXPR evaluates to true; fails if EXPR evaluates to\n\
    false or an invalid argument is given."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
char * const test_bracket_doc[] = {
#if defined (HELP_BUILTIN)
N_("Evaluate conditional expression.\n\
    \n\
    This is a synonym for the \"test\" builtin, but the last argument must\n\
    be a literal `]', to match the opening `['."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
char * const times_doc[] = {
#if defined (HELP_BUILTIN)
N_("Display process times.\n\
    \n\
    Prints the accumulated user and system times for the shell and all of its\n\
    child processes.\n\
    \n\
    Exit Status:\n\
    Always succeeds."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
char * const trap_doc[] = {
#if defined (HELP_BUILTIN)
N_("Trap signals and other events.\n\
    \n\
    Defines and activates handlers to be run when the shell receives signals\n\
    or other conditions.\n\
    \n\
    ARG is a command to be read and executed when the shell receives the\n\
    signal(s) SIGNAL_SPEC.  If ARG is absent (and a single SIGNAL_SPEC\n\
    is supplied) or `-', each specified signal is reset to its original\n\
    value.  If ARG is the null string each SIGNAL_SPEC is ignored by the\n\
    shell and by the commands it invokes.\n\
    \n\
    If a SIGNAL_SPEC is EXIT (0) ARG is executed on exit from the shell.  If\n\
    a SIGNAL_SPEC is DEBUG, ARG is executed before every simple command.  If\n\
    a SIGNAL_SPEC is RETURN, ARG is executed each time a shell function or a\n\
    script run by the . or source builtins finishes executing.  A SIGNAL_SPEC\n\
    of ERR means to execute ARG each time a command's failure would cause the\n\
    shell to exit when the -e option is enabled.\n\
    \n\
    If no arguments are supplied, trap prints the list of commands associated\n\
    with each signal.\n\
    \n\
    Options:\n\
      -l	print a list of signal names and their corresponding numbers\n\
      -p	display the trap commands associated with each SIGNAL_SPEC\n\
    \n\
    Each SIGNAL_SPEC is either a signal name in <signal.h> or a signal number.\n\
    Signal names are case insensitive and the SIG prefix is optional.  A\n\
    signal may be sent to the shell with \"kill -signal $$\".\n\
    \n\
    Exit Status:\n\
    Returns success unless a SIGSPEC is invalid or an invalid option is given."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
char * const type_doc[] = {
#if defined (HELP_BUILTIN)
N_("Display information about command type.\n\
    \n\
    For each NAME, indicate how it would be interpreted if used as a\n\
    command name.\n\
    \n\
    Options:\n\
      -a	display all locations containing an executable named NAME;\n\
    		includes aliases, builtins, and functions, if and only if\n\
    		the `-p' option is not also used\n\
      -f	suppress shell function lookup\n\
      -P	force a PATH search for each NAME, even if it is an alias,\n\
    		builtin, or function, and returns the name of the disk file\n\
    		that would be executed\n\
      -p	returns either the name of the disk file that would be executed,\n\
    		or nothing if `type -t NAME' would not return `file'\n\
      -t	output a single word which is one of `alias', `keyword',\n\
    		`function', `builtin', `file' or `', if NAME is an alias,\n\
    		shell reserved word, shell function, shell builtin, disk file,\n\
    		or not found, respectively\n\
    \n\
    Arguments:\n\
      NAME	Command name to be interpreted.\n\
    \n\
    Exit Status:\n\
    Returns success if all of the NAMEs are found; fails if any are not found."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
#if !defined (_MINIX)
char * const ulimit_doc[] = {
#if defined (HELP_BUILTIN)
N_("Modify shell resource limits.\n\
    \n\
    Provides control over the resources available to the shell and processes\n\
    it creates, on systems that allow such control.\n\
    \n\
    Options:\n\
      -S	use the `soft' resource limit\n\
      -H	use the `hard' resource limit\n\
      -a	all current limits are reported\n\
      -b	the socket buffer size\n\
      -c	the maximum size of core files created\n\
      -d	the maximum size of a process's data segment\n\
      -e	the maximum scheduling priority (`nice')\n\
      -f	the maximum size of files written by the shell and its children\n\
      -i	the maximum number of pending signals\n\
      -k	the maximum number of kqueues allocated for this process\n\
      -l	the maximum size a process may lock into memory\n\
      -m	the maximum resident set size\n\
      -n	the maximum number of open file descriptors\n\
      -p	the pipe buffer size\n\
      -q	the maximum number of bytes in POSIX message queues\n\
      -r	the maximum real-time scheduling priority\n\
      -s	the maximum stack size\n\
      -t	the maximum amount of cpu time in seconds\n\
      -u	the maximum number of user processes\n\
      -v	the size of virtual memory\n\
      -x	the maximum number of file locks\n\
      -P	the maximum number of pseudoterminals\n\
      -R	the maximum time a real-time process can run before blocking\n\
      -T	the maximum number of threads\n\
    \n\
    Not all options are available on all platforms.\n\
    \n\
    If LIMIT is given, it is the new value of the specified resource; the\n\
    special LIMIT values `soft', `hard', and `unlimited' stand for the\n\
    current soft limit, the current hard limit, and no limit, respectively.\n\
    Otherwise, the current value of the specified resource is printed.  If\n\
    no option is given, then -f is assumed.\n\
    \n\
    Values are in 1024-byte increments, except for -t, which is in seconds,\n\
    -p, which is in increments of 512 bytes, and -u, which is an unscaled\n\
    number of processes.\n\
    \n\
    Exit Status:\n\
    Returns success unless an invalid option is supplied or an error occurs."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
#endif /* !_MINIX */
char * const umask_doc[] = {
#if defined (HELP_BUILTIN)
N_("Display or set file mode mask.\n\
    \n\
    Sets the user file-creation mask to MODE.  If MODE is omitted, prints\n\
    the current value of the mask.\n\
    \n\
    If MODE begins with a digit, it is interpreted as an octal number;\n\
    otherwise it is a symbolic mode string like that accepted by chmod(1).\n\
    \n\
    Options:\n\
      -p	if MODE is omitted, output in a form that may be reused as input\n\
      -S	makes the output symbolic; otherwise an octal number is output\n\
    \n\
    Exit Status:\n\
    Returns success unless MODE is invalid or an invalid option is given."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
#if defined (JOB_CONTROL)
char * const wait_doc[] = {
#if defined (HELP_BUILTIN)
N_("Wait for job completion and return exit status.\n\
    \n\
    Waits for each process identified by an ID, which may be a process ID or a\n\
    job specification, and reports its termination status.  If ID is not\n\
    given, waits for all currently active child processes, and the return\n\
    status is zero.  If ID is a job specification, waits for all processes\n\
    in that job's pipeline.\n\
    \n\
    If the -n option is supplied, waits for a single job from the list of IDs,\n\
    or, if no IDs are supplied, for the next job to complete and returns its\n\
    exit status.\n\
    \n\
    If the -p option is supplied, the process or job identifier of the job\n\
    for which the exit status is returned is assigned to the variable VAR\n\
    named by the option argument. The variable will be unset initially, before\n\
    any assignment. This is useful only when the -n option is supplied.\n\
    \n\
    If the -f option is supplied, and job control is enabled, waits for the\n\
    specified ID to terminate, instead of waiting for it to change status.\n\
    \n\
    Exit Status:\n\
    Returns the status of the last ID; fails if ID is invalid or an invalid\n\
    option is given, or if -n is supplied and the shell has no unwaited-for\n\
    children."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
#endif /* JOB_CONTROL */
#if !defined (JOB_CONTROL)
char * const wait_doc[] = {
#if defined (HELP_BUILTIN)
N_("Wait for process completion and return exit status.\n\
    \n\
    Waits for each process specified by a PID and reports its termination status.\n\
    If PID is not given, waits for all currently active child processes,\n\
    and the return status is zero.  PID must be a process ID.\n\
    \n\
    Exit Status:\n\
    Returns the status of the last PID; fails if PID is invalid or an invalid\n\
    option is given."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
#endif /* !JOB_CONTROL */
char * const for_doc[] = {
#if defined (HELP_BUILTIN)
N_("Execute commands for each member in a list.\n\
    \n\
    The `for' loop executes a sequence of commands for each member in a\n\
    list of items.  If `in WORDS ...;' is not present, then `in \"$@\"' is\n\
    assumed.  For each element in WORDS, NAME is set to that element, and\n\
    the COMMANDS are executed.\n\
    \n\
    Exit Status:\n\
    Returns the status of the last command executed."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
char * const arith_for_doc[] = {
#if defined (HELP_BUILTIN)
N_("Arithmetic for loop.\n\
    \n\
    Equivalent to\n\
    	(( EXP1 ))\n\
    	while (( EXP2 )); do\n\
    		COMMANDS\n\
    		(( EXP3 ))\n\
    	done\n\
    EXP1, EXP2, and EXP3 are arithmetic expressions.  If any expression is\n\
    omitted, it behaves as if it evaluates to 1.\n\
    \n\
    Exit Status:\n\
    Returns the status of the last command executed."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
char * const select_doc[] = {
#if defined (HELP_BUILTIN)
N_("Select words from a list and execute commands.\n\
    \n\
    The WORDS are expanded, generating a list of words.  The\n\
    set of expanded words is printed on the standard error, each\n\
    preceded by a number.  If `in WORDS' is not present, `in \"$@\"'\n\
    is assumed.  The PS3 prompt is then displayed and a line read\n\
    from the standard input.  If the line consists of the number\n\
    corresponding to one of the displayed words, then NAME is set\n\
    to that word.  If the line is empty, WORDS and the prompt are\n\
    redisplayed.  If EOF is read, the command completes.  Any other\n\
    value read causes NAME to be set to null.  The line read is saved\n\
    in the variable REPLY.  COMMANDS are executed after each selection\n\
    until a break command is executed.\n\
    \n\
    Exit Status:\n\
    Returns the status of the last command executed."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
char * const time_doc[] = {
#if defined (HELP_BUILTIN)
N_("Report time consumed by pipeline's execution.\n\
    \n\
    Execute PIPELINE and print a summary of the real time, user CPU time,\n\
    and system CPU time spent executing PIPELINE when it terminates.\n\
    \n\
    Options:\n\
      -p	print the timing summary in the portable Posix format\n\
    \n\
    The value of the TIMEFORMAT variable is used as the output format.\n\
    \n\
    Exit Status:\n\
    The return status is the return status of PIPELINE."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
char * const case_doc[] = {
#if defined (HELP_BUILTIN)
N_("Execute commands based on pattern matching.\n\
    \n\
    Selectively execute COMMANDS based upon WORD matching PATTERN.  The\n\
    `|' is used to separate multiple patterns.\n\
    \n\
    Exit Status:\n\
    Returns the status of the last command executed."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
char * const if_doc[] = {
#if defined (HELP_BUILTIN)
N_("Execute commands based on conditional.\n\
    \n\
    The `if COMMANDS' list is executed.  If its exit status is zero, then the\n\
    `then COMMANDS' list is executed.  Otherwise, each `elif COMMANDS' list is\n\
    executed in turn, and if its exit status is zero, the corresponding\n\
    `then COMMANDS' list is executed and the if command completes.  Otherwise,\n\
    the `else COMMANDS' list is executed, if present.  The exit status of the\n\
    entire construct is the exit status of the last command executed, or zero\n\
    if no condition tested true.\n\
    \n\
    Exit Status:\n\
    Returns the status of the last command executed."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
char * const while_doc[] = {
#if defined (HELP_BUILTIN)
N_("Execute commands as long as a test succeeds.\n\
    \n\
    Expand and execute COMMANDS-2 as long as the final command in COMMANDS has\n\
    an exit status of zero.\n\
    \n\
    Exit Status:\n\
    Returns the status of the last command executed."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
char * const until_doc[] = {
#if defined (HELP_BUILTIN)
N_("Execute commands as long as a test does not succeed.\n\
    \n\
    Expand and execute COMMANDS-2 as long as the final command in COMMANDS has\n\
    an exit status which is not zero.\n\
    \n\
    Exit Status:\n\
    Returns the status of the last command executed."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
char * const coproc_doc[] = {
#if defined (HELP_BUILTIN)
N_("Create a coprocess named NAME.\n\
    \n\
    Execute COMMAND asynchronously, with the standard output and standard\n\
    input of the command connected via a pipe to file descriptors assigned\n\
    to indices 0 and 1 of an array variable NAME in the executing shell.\n\
    The default NAME is \"COPROC\".\n\
    \n\
    Exit Status:\n\
    The coproc command returns an exit status of 0."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
char * const function_doc[] = {
#if defined (HELP_BUILTIN)
N_("Define shell function.\n\
    \n\
    Create a shell function named NAME.  When invoked as a simple command,\n\
    NAME runs COMMANDs in the calling shell's context.  When NAME is invoked,\n\
    the arguments are passed to the function as $1...$n, and the function's\n\
    name is in $FUNCNAME.\n\
    \n\
    Exit Status:\n\
    Returns success unless NAME is readonly."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
char * const grouping_braces_doc[] = {
#if defined (HELP_BUILTIN)
N_("Group commands as a unit.\n\
    \n\
    Run a set of commands in a group.  This is one way to redirect an\n\
    entire set of commands.\n\
    \n\
    Exit Status:\n\
    Returns the status of the last command executed."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
char * const fg_percent_doc[] = {
#if defined (HELP_BUILTIN)
N_("Resume job in foreground.\n\
    \n\
    Equivalent to the JOB_SPEC argument to the `fg' command.  Resume a\n\
    stopped or background job.  JOB_SPEC can specify either a job name\n\
    or a job number.  Following JOB_SPEC with a `&' places the job in\n\
    the background, as if the job specification had been supplied as an\n\
    argument to `bg'.\n\
    \n\
    Exit Status:\n\
    Returns the status of the resumed job."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
char * const arith_doc[] = {
#if defined (HELP_BUILTIN)
N_("Evaluate arithmetic expression.\n\
    \n\
    The EXPRESSION is evaluated according to the rules for arithmetic\n\
    evaluation.  Equivalent to `let \"EXPRESSION\"'.\n\
    \n\
    Exit Status:\n\
    Returns 1 if EXPRESSION evaluates to 0; returns 0 otherwise."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
char * const conditional_doc[] = {
#if defined (HELP_BUILTIN)
N_("Execute conditional command.\n\
    \n\
    Returns a status of 0 or 1 depending on the evaluation of the conditional\n\
    expression EXPRESSION.  Expressions are composed of the same primaries used\n\
    by the `test' builtin, and may be combined using the following operators:\n\
    \n\
      ( EXPRESSION )	Returns the value of EXPRESSION\n\
      ! EXPRESSION		True if EXPRESSION is false; else false\n\
      EXPR1 && EXPR2	True if both EXPR1 and EXPR2 are true; else false\n\
      EXPR1 || EXPR2	True if either EXPR1 or EXPR2 is true; else false\n\
    \n\
    When the `==' and `!=' operators are used, the string to the right of\n\
    the operator is used as a pattern and pattern matching is performed.\n\
    When the `=~' operator is used, the string to the right of the operator\n\
    is matched as a regular expression.\n\
    \n\
    The && and || operators do not evaluate EXPR2 if EXPR1 is sufficient to\n\
    determine the expression's value.\n\
    \n\
    Exit Status:\n\
    0 or 1 depending on value of EXPRESSION."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
char * const variable_help_doc[] = {
#if defined (HELP_BUILTIN)
N_("Common shell variable names and usage.\n\
    \n\
    BASH_VERSION	Version information for this Bash.\n\
    CDPATH	A colon-separated list of directories to search\n\
    		for directories given as arguments to `cd'.\n\
    GLOBIGNORE	A colon-separated list of patterns describing filenames to\n\
    		be ignored by pathname expansion.\n\
    HISTFILE	The name of the file where your command history is stored.\n\
    HISTFILESIZE	The maximum number of lines this file can contain.\n\
    HISTSIZE	The maximum number of history lines that a running\n\
    		shell can access.\n\
    HOME	The complete pathname to your login directory.\n\
    HOSTNAME	The name of the current host.\n\
    HOSTTYPE	The type of CPU this version of Bash is running under.\n\
    IGNOREEOF	Controls the action of the shell on receipt of an EOF\n\
    		character as the sole input.  If set, then the value\n\
    		of it is the number of EOF characters that can be seen\n\
    		in a row on an empty line before the shell will exit\n\
    		(default 10).  When unset, EOF signifies the end of input.\n\
    MACHTYPE	A string describing the current system Bash is running on.\n\
    MAILCHECK	How often, in seconds, Bash checks for new mail.\n\
    MAILPATH	A colon-separated list of filenames which Bash checks\n\
    		for new mail.\n\
    OSTYPE	The version of Unix this version of Bash is running on.\n\
    PATH	A colon-separated list of directories to search when\n\
    		looking for commands.\n\
    PROMPT_COMMAND	A command to be executed before the printing of each\n\
    		primary prompt.\n\
    PS1		The primary prompt string.\n\
    PS2		The secondary prompt string.\n\
    PWD		The full pathname of the current directory.\n\
    SHELLOPTS	A colon-separated list of enabled shell options.\n\
    TERM	The name of the current terminal type.\n\
    TIMEFORMAT	The output format for timing statistics displayed by the\n\
    		`time' reserved word.\n\
    auto_resume	Non-null means a command word appearing on a line by\n\
    		itself is first looked for in the list of currently\n\
    		stopped jobs.  If found there, that job is foregrounded.\n\
    		A value of `exact' means that the command word must\n\
    		exactly match a command in the list of stopped jobs.  A\n\
    		value of `substring' means that the command word must\n\
    		match a substring of the job.  Any other value means that\n\
    		the command must be a prefix of a stopped job.\n\
    histchars	Characters controlling history expansion and quick\n\
    		substitution.  The first character is the history\n\
    		substitution character, usually `!'.  The second is\n\
    		the `quick substitution' character, usually `^'.  The\n\
    		third is the `history comment' character, usually `#'.\n\
    HISTIGNORE	A colon-separated list of patterns used to decide which\n\
    		commands should be saved on the history list.\n\
"),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
#if defined (PUSHD_AND_POPD)
char * const pushd_doc[] = {
#if defined (HELP_BUILTIN)
N_("Add directories to stack.\n\
    \n\
    Adds a directory to the top of the directory stack, or rotates\n\
    the stack, making the new top of the stack the current working\n\
    directory.  With no arguments, exchanges the top two directories.\n\
    \n\
    Options:\n\
      -n	Suppresses the normal change of directory when adding\n\
    		directories to the stack, so only the stack is manipulated.\n\
    \n\
    Arguments:\n\
      +N	Rotates the stack so that the Nth directory (counting\n\
    		from the left of the list shown by `dirs', starting with\n\
    		zero) is at the top.\n\
    \n\
      -N	Rotates the stack so that the Nth directory (counting\n\
    		from the right of the list shown by `dirs', starting with\n\
    		zero) is at the top.\n\
    \n\
      dir	Adds DIR to the directory stack at the top, making it the\n\
    		new current working directory.\n\
    \n\
    The `dirs' builtin displays the directory stack.\n\
    \n\
    Exit Status:\n\
    Returns success unless an invalid argument is supplied or the directory\n\
    change fails."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
#endif /* PUSHD_AND_POPD */
#if defined (PUSHD_AND_POPD)
char * const popd_doc[] = {
#if defined (HELP_BUILTIN)
N_("Remove directories from stack.\n\
    \n\
    Removes entries from the directory stack.  With no arguments, removes\n\
    the top directory from the stack, and changes to the new top directory.\n\
    \n\
    Options:\n\
      -n	Suppresses the normal change of directory when removing\n\
    		directories from the stack, so only the stack is manipulated.\n\
    \n\
    Arguments:\n\
      +N	Removes the Nth entry counting from the left of the list\n\
    		shown by `dirs', starting with zero.  For example: `popd +0'\n\
    		removes the first directory, `popd +1' the second.\n\
    \n\
      -N	Removes the Nth entry counting from the right of the list\n\
    		shown by `dirs', starting with zero.  For example: `popd -0'\n\
    		removes the last directory, `popd -1' the next to last.\n\
    \n\
    The `dirs' builtin displays the directory stack.\n\
    \n\
    Exit Status:\n\
    Returns success unless an invalid argument is supplied or the directory\n\
    change fails."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
#endif /* PUSHD_AND_POPD */
#if defined (PUSHD_AND_POPD)
char * const dirs_doc[] = {
#if defined (HELP_BUILTIN)
N_("Display directory stack.\n\
    \n\
    Display the list of currently remembered directories.  Directories\n\
    find their way onto the list with the `pushd' command; you can get\n\
    back up through the list with the `popd' command.\n\
    \n\
    Options:\n\
      -c	clear the directory stack by deleting all of the elements\n\
      -l	do not print tilde-prefixed versions of directories relative\n\
    		to your home directory\n\
      -p	print the directory stack with one entry per line\n\
      -v	print the directory stack with one entry per line prefixed\n\
    		with its position in the stack\n\
    \n\
    Arguments:\n\
      +N	Displays the Nth entry counting from the left of the list\n\
    		shown by dirs when invoked without options, starting with\n\
    		zero.\n\
    \n\
      -N	Displays the Nth entry counting from the right of the list\n\
    		shown by dirs when invoked without options, starting with\n\
    		zero.\n\
    \n\
    Exit Status:\n\
    Returns success unless an invalid option is supplied or an error occurs."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
#endif /* PUSHD_AND_POPD */
char * const shopt_doc[] = {
#if defined (HELP_BUILTIN)
N_("Set and unset shell options.\n\
    \n\
    Change the setting of each shell option OPTNAME.  Without any option\n\
    arguments, list each supplied OPTNAME, or all shell options if no\n\
    OPTNAMEs are given, with an indication of whether or not each is set.\n\
    \n\
    Options:\n\
      -o	restrict OPTNAMEs to those defined for use with `set -o'\n\
      -p	print each shell option with an indication of its status\n\
      -q	suppress output\n\
      -s	enable (set) each OPTNAME\n\
      -u	disable (unset) each OPTNAME\n\
    \n\
    Exit Status:\n\
    Returns success if OPTNAME is enabled; fails if an invalid option is\n\
    given or OPTNAME is disabled."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
char * const printf_doc[] = {
#if defined (HELP_BUILTIN)
N_("Formats and prints ARGUMENTS under control of the FORMAT.\n\
    \n\
    Options:\n\
      -v var	assign the output to shell variable VAR rather than\n\
    		display it on the standard output\n\
    \n\
    FORMAT is a character string which contains three types of objects: plain\n\
    characters, which are simply copied to standard output; character escape\n\
    sequences, which are converted and copied to the standard output; and\n\
    format specifications, each of which causes printing of the next successive\n\
    argument.\n\
    \n\
    In addition to the standard format specifications described in printf(1),\n\
    printf interprets:\n\
    \n\
      %b	expand backslash escape sequences in the corresponding argument\n\
      %q	quote the argument in a way that can be reused as shell input\n\
      %Q	like %q, but apply any precision to the unquoted argument before\n\
    		quoting\n\
      %(fmt)T	output the date-time string resulting from using FMT as a format\n\
    	        string for strftime(3)\n\
    \n\
    The format is re-used as necessary to consume all of the arguments.  If\n\
    there are fewer arguments than the format requires,  extra format\n\
    specifications behave as if a zero value or null string, as appropriate,\n\
    had been supplied.\n\
    \n\
    Exit Status:\n\
    Returns success unless an invalid option is given or a write or assignment\n\
    error occurs."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
#if defined (PROGRAMMABLE_COMPLETION)
char * const complete_doc[] = {
#if defined (HELP_BUILTIN)
N_("Specify how arguments are to be completed by Readline.\n\
    \n\
    For each NAME, specify how arguments are to be completed.  If no options\n\
    are supplied, existing completion specifications are printed in a way that\n\
    allows them to be reused as input.\n\
    \n\
    Options:\n\
      -p	print existing completion specifications in a reusable format\n\
      -r	remove a completion specification for each NAME, or, if no\n\
    		NAMEs are supplied, all completion specifications\n\
      -D	apply the completions and actions as the default for commands\n\
    		without any specific completion defined\n\
      -E	apply the completions and actions to \"empty\" commands --\n\
    		completion attempted on a blank line\n\
      -I	apply the completions and actions to the initial (usually the\n\
    		command) word\n\
    \n\
    When completion is attempted, the actions are applied in the order the\n\
    uppercase-letter options are listed above. If multiple options are supplied,\n\
    the -D option takes precedence over -E, and both take precedence over -I.\n\
    \n\
    Exit Status:\n\
    Returns success unless an invalid option is supplied or an error occurs."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
#endif /* PROGRAMMABLE_COMPLETION */
#if defined (PROGRAMMABLE_COMPLETION)
char * const compgen_doc[] = {
#if defined (HELP_BUILTIN)
N_("Display possible completions depending on the options.\n\
    \n\
    Intended to be used from within a shell function generating possible\n\
    completions.  If the optional WORD argument is supplied, matches against\n\
    WORD are generated.\n\
    \n\
    Exit Status:\n\
    Returns success unless an invalid option is supplied or an error occurs."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
#endif /* PROGRAMMABLE_COMPLETION */
#if defined (PROGRAMMABLE_COMPLETION)
char * const compopt_doc[] = {
#if defined (HELP_BUILTIN)
N_("Modify or display completion options.\n\
    \n\
    Modify the completion options for each NAME, or, if no NAMEs are supplied,\n\
    the completion currently being executed.  If no OPTIONs are given, print\n\
    the completion options for each NAME or the current completion specification.\n\
    \n\
    Options:\n\
    	-o option	Set completion option OPTION for each NAME\n\
    	-D		Change options for the \"default\" command completion\n\
    	-E		Change options for the \"empty\" command completion\n\
    	-I		Change options for completion on the initial word\n\
    \n\
    Using `+o' instead of `-o' turns off the specified option.\n\
    \n\
    Arguments:\n\
    \n\
    Each NAME refers to a command for which a completion specification must\n\
    have previously been defined using the `complete' builtin.  If no NAMEs\n\
    are supplied, compopt must be called by a function currently generating\n\
    completions, and the options for that currently-executing completion\n\
    generator are modified.\n\
    \n\
    Exit Status:\n\
    Returns success unless an invalid option is supplied or NAME does not\n\
    have a completion specification defined."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
#endif /* PROGRAMMABLE_COMPLETION */
char * const mapfile_doc[] = {
#if defined (HELP_BUILTIN)
N_("Read lines from the standard input into an indexed array variable.\n\
    \n\
    Read lines from the standard input into the indexed array variable ARRAY, or\n\
    from file descriptor FD if the -u option is supplied.  The variable MAPFILE\n\
    is the default ARRAY.\n\
    \n\
    Options:\n\
      -d delim	Use DELIM to terminate lines, instead of newline\n\
      -n count	Copy at most COUNT lines.  If COUNT is 0, all lines are copied\n\
      -O origin	Begin assigning to ARRAY at index ORIGIN.  The default index is 0\n\
      -s count	Discard the first COUNT lines read\n\
      -t	Remove a trailing DELIM from each line read (default newline)\n\
      -u fd	Read lines from file descriptor FD instead of the standard input\n\
      -C callback	Evaluate CALLBACK each time QUANTUM lines are read\n\
      -c quantum	Specify the number of lines read between each call to\n\
    			CALLBACK\n\
    \n\
    Arguments:\n\
      ARRAY	Array variable name to use for file data\n\
    \n\
    If -C is supplied without -c, the default quantum is 5000.  When\n\
    CALLBACK is evaluated, it is supplied the index of the next array\n\
    element to be assigned and the line to be assigned to that element\n\
    as additional arguments.\n\
    \n\
    If not supplied with an explicit origin, mapfile will clear ARRAY before\n\
    assigning to it.\n\
    \n\
    Exit Status:\n\
    Returns success unless an invalid option is given or ARRAY is readonly or\n\
    not an indexed array."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
char * const readarray_doc[] = {
#if defined (HELP_BUILTIN)
N_("Read lines from a file into an array variable.\n\
    \n\
    A synonym for `mapfile'."),
#endif /* HELP_BUILTIN */
  (char *)NULL
};
