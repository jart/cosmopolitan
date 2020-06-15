#ifndef COSMOPOLITAN_THIRD_PARTY_EDITLINE_EDITLINE_H_
#define COSMOPOLITAN_THIRD_PARTY_EDITLINE_EDITLINE_H_
#include "libc/stdio/stdio.h"

#define CTL(x)    ((x)&0x1F)
#define ISCTL(x)  ((x) && (x) < ' ')
#define UNCTL(x)  ((x) + 64)
#define META(x)   ((x) | 0x80)
#define ISMETA(x) ((x)&0x80)
#define UNMETA(x) ((x)&0x7F)

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

typedef enum {
  CSdone = 0, /* OK */
  CSeof,      /* Error, or EOF */
  CSmove,
  CSdispatch,
  CSstay,
  CSsignal
} el_status_t;

typedef int rl_list_possib_func_t(char *, char ***);
typedef el_status_t el_keymap_func_t(void);
typedef int rl_hook_func_t(void);
typedef int rl_getc_func_t(void);
typedef void rl_voidfunc_t(void);
typedef void rl_vintfunc_t(int);
typedef void rl_vcpfunc_t(char *);
compatfn typedef char *rl_complete_func_t(char *, int *);
compatfn typedef char *rl_compentry_func_t(const char *, int);
compatfn typedef char **rl_completion_func_t(const char *, int, int);

extern int rl_point;
extern int rl_mark;
extern int rl_end;
extern int rl_inhibit_complete;
extern char *rl_line_buffer;
extern const char *rl_readline_name;
extern FILE *rl_instream; /* The stdio stream from which input is read. Defaults
                             to stdin if NULL - Not supported yet! */
extern FILE *rl_outstream; /* The stdio stream to which output is flushed.
                              Defaults to stdout if NULL - Not supported yet! */
extern int el_no_echo;     /* E.g under emacs, don't echo except prompt */
extern int el_no_hist; /* Disable auto-save of and access to history -- e.g. for
                          password prompts or wizards */
extern int el_hist_size;  /* size of history scrollback buffer, default: 15 */
extern int rl_meta_chars; /* Display 8-bit chars "as-is" or as `M-x'? Toggle
                             with M-m. (Default:0 - "as-is") */
extern rl_completion_func_t *rl_attempted_completion_function;

char **rl_completion_matches(const char *, rl_compentry_func_t *);
char *el_find_word(void);
char *readline(const char *);
char *rl_complete(char *, int *);
char *rl_filename_completion_function(const char *, int);
const char *el_next_hist(void);
const char *el_prev_hist(void);
el_status_t el_bind_key(int, el_keymap_func_t);
el_status_t el_bind_key_in_metamap(int, el_keymap_func_t);
el_status_t el_del_char(void);
el_status_t el_ring_bell(void);
int read_history(const char *);
int rl_getc(void);
int rl_insert_text(const char *);
int rl_list_possib(char *, char ***);
int rl_refresh_line(int, int);
int write_history(const char *);
rl_complete_func_t *rl_set_complete_func(rl_complete_func_t *);
rl_getc_func_t *rl_set_getc_func(rl_getc_func_t *);
rl_list_possib_func_t *rl_set_list_possib_func(rl_list_possib_func_t *);
void add_history(const char *);
void el_print_columns(int, char **);
void rl_callback_handler_install(const char *, rl_vcpfunc_t *);
void rl_callback_handler_remove(void);
void rl_callback_read_char(void);
void rl_clear_message(void);
void rl_deprep_terminal(void);
void rl_forced_update_display(void);
void rl_initialize(void);
void rl_prep_terminal(int);
void rl_reset_terminal(const char *);
void rl_restore_prompt(void);
void rl_save_prompt(void);
void rl_set_prompt(const char *);
void rl_uninitialize(void);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_THIRD_PARTY_EDITLINE_EDITLINE_H_ */
