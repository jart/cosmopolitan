/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/calls/calls.h"
#include "libc/calls/termios.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/macros.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/termios.h"
#include "third_party/editline/editline.h"
#include "third_party/editline/internal.h"

asm(".ident\t\"\\n\\n\
Cosmopolitan Linenoise (BSD-like license)\\n\
Copyright 2019 Justine Alexandra Roberts Tunney\\n\
Copyright 1992,1993 Simmule Turner and Rich Salz\\n\
All rights reserved.\\n\
\\n\
This software is not subject to any license of the American Telephone\\n\
and Telegraph Company or of the Regents of the University of California.\\n\
\\n\
Permission is granted to anyone to use this software for any purpose on\\n\
any computer system, and to alter it and redistribute it freely, subject\\n\
to the following restrictions:\\n\
1. The authors are not responsible for the consequences of use of this\\n\
   software, no matter how awful, even if they arise from flaws in it.\\n\
2. The origin of this software must not be misrepresented, either by\\n\
   explicit claim or by omission.  Since few users ever read sources,\\n\
   credits must appear in the documentation.\\n\
3. Altered versions must be plainly marked as such, and must not be\\n\
   misrepresented as being the original software.  Since few users\\n\
   ever read sources, credits must appear in the documentation.\\n\
4. This notice may not be removed or altered.\"");
asm(".include \"libc/disclaimer.inc\"");

#define SCREEN_COLS 80
#define SCREEN_ROWS 24
#define EL_STDIN    0
#define EL_STDOUT   1
#define NO_ARG      (-1)
#define DEL         127
#define SEPS        "\"#$&'()*:;<=>?[\\]^`{|}~\n\t "

enum EditlineCase { TOupper, TOlower, TOcapitalize };

struct EditlineKeymap {
  int Key;
  el_status_t (*Function)(void);
};

struct EditlineHistory {
  int Size;
  int Pos;
  char **Lines;
};

rl_getc_func_t *rl_getc_function = rl_getc;
rl_hook_func_t *rl_event_hook;
rl_vintfunc_t *rl_prep_term_function = rl_prep_terminal;
rl_voidfunc_t *rl_deprep_term_function = rl_deprep_terminal;

int rl_eof;
int rl_erase;
int rl_intr;
int rl_kill;
int rl_quit;
int rl_susp;
int el_hist_size = 15;

static struct EditlineHistory g_hist = {
    .Size = 0,
    .Pos = 0,
    .Lines = NULL,
};

static char NILSTR[] = "";
static const char *el_input = NILSTR;
static char *Yanked;
static char *Screen;
static char NEWLINE[] = "\r\n";
static char CLEAR[] = "\ec";
static const char *el_term = "dumb";
static int Repeat;
static int old_point;
static int el_push_back;
static int el_pushed;
static int el_intr_pending;
static int el_infd = EL_STDIN;
static int el_outfd = EL_STDOUT;
static struct EditlineKeymap Map[];
static struct EditlineKeymap MetaMap[];
static size_t Length = 0;
static size_t ScreenCount;
static size_t ScreenSize;
static char *backspace = "\b";
static char *old_search = NULL;
static int tty_cols = SCREEN_COLS;
static int tty_rows = SCREEN_ROWS;
static int Searching = 0;
static const char *(*search_move)(void);
static const char *old_prompt = NULL;
static rl_vcpfunc_t *line_handler = NULL;
static const char *rl_saved_prompt = NULL;

int el_no_echo = 0; /* e.g., under Emacs */
int el_no_hist = 0;
int rl_point;
int rl_mark;
int rl_end;
int rl_meta_chars = 0; /* 8-bit chars as actual char(0) or `M-x'(1)? */
int rl_inhibit_complete = 0;
char *rl_line_buffer = NULL;
const char *rl_prompt = NULL;
const char *rl_readline_name =
    NULL; /* Set by calling program, for conditional parsing of ~/.inputrc - Not
             supported yet! */
FILE *rl_instream = NULL; /* The stdio stream from which input is read. Defaults
                             to stdin if NULL */
FILE *rl_outstream = NULL; /* The stdio stream to which output is flushed.
                              Defaults to stdout if NULL */

/* Declarations. */
static char *editinput(int complete);
#ifdef CONFIG_USE_TERMCAP
extern char *tgetstr(const char *, char **);
extern int tgetent(char *, const char *);
extern int tgetnum(const char *);
#endif

/*
**  Misc. local helper functions.
*/
static int is_alpha_num(unsigned char c) {
  if (isalnum(c)) return 1;
  if (ISMETA(c)) return 1;
  if (ISCTL(c)) return 1;
  return 0;
}

/*
**  TTY input/output functions.
*/

static void tty_flush(void) {
  ssize_t res;
  if (!ScreenCount) return;
  if (!el_no_echo) {
    res = write(el_outfd, Screen, ScreenCount);
    if (res > 0) ScreenCount = 0;
  }
}

static void tty_put(const char c) {
  if (el_no_echo) return;
  Screen[ScreenCount] = c;
  if (++ScreenCount >= ScreenSize) {
    ScreenSize += SCREEN_INC;
    Screen = realloc(Screen, sizeof(char) * ScreenSize);
  }
}

static void tty_puts(const char *p) {
  while (*p) tty_put(*p++);
}

static void tty_show(unsigned char c) {
  if (c == DEL) {
    tty_put('^');
    tty_put('?');
  } else if (ISCTL(c)) {
    tty_put('^');
    tty_put(UNCTL(c));
  } else if (rl_meta_chars && ISMETA(c)) {
    tty_put('M');
    tty_put('-');
    tty_put(UNMETA(c));
  } else {
    tty_put(c);
  }
}

static void tty_string(char *p) {
  while (*p) tty_show(*p++);
}

static void tty_push(int c) {
  el_pushed = 1;
  el_push_back = c;
}

int rl_getc(void) {
  int r;
  char c;
  do {
    r = read(el_infd, &c, 1);
  } while (r == -1 && errno == EINTR);
  return r == 1 ? c : EOF;
}

static int tty_get(void) {
  tty_flush();
  if (el_pushed) {
    el_pushed = 0;
    return el_push_back;
  }
  if (*el_input) return *el_input++;
  return rl_getc_function();
}

#define tty_back() tty_puts(backspace)

static void tty_backn(int n) {
  while (--n >= 0) tty_back();
}

static void tty_info(void) {
  rl_reset_terminal(NULL);
}

/*
**  Glue routines to rl_ttyset()
*/
void rl_prep_terminal(int meta_flag) {
  rl_meta_chars = !meta_flag;
  rl_ttyset(0);
}

void rl_deprep_terminal(void) {
  rl_ttyset(1);
}

/*
**  Print an array of words in columns.
*/
void el_print_columns(int ac, char **av) {
  char *p;
  int i;
  int j;
  int k;
  int len;
  int skip;
  int longest;
  int cols;
  int colwidth;
  /* Find longest name, determine column count from that. */
  for (longest = 0, i = 0; i < ac; i++) {
    if ((j = strlen((char *)av[i])) > longest) longest = j;
  }
  colwidth = longest + 3;
  if (colwidth > tty_cols) colwidth = tty_cols;
  cols = tty_cols / colwidth;
  tty_puts(NEWLINE);
  for (skip = ac / cols + 1, i = 0; i < skip; i++) {
    for (j = i; j < ac; j += skip) {
      for (p = av[j], len = strlen((char *)p), k = len; --k >= 0; p++)
        tty_put(*p);
      if (j + skip < ac) {
        while (++len < colwidth) tty_put(' ');
      }
    }
    tty_puts(NEWLINE);
  }
}

static void reposition(void) {
  int i;
  tty_put('\r');
  tty_puts(rl_prompt);
  for (i = 0; i < rl_point; i++) tty_show(rl_line_buffer[i]);
}

static void left(el_status_t Change) {
  if (rl_point) {
    tty_back();
    if (ISMETA(rl_line_buffer[rl_point - 1])) {
      if (rl_meta_chars) {
        tty_back();
        tty_back();
      }
    } else if (ISCTL(rl_line_buffer[rl_point - 1])) {
      tty_back();
    }
  }
  if (Change == CSmove) rl_point--;
}

static void right(el_status_t Change) {
  tty_show(rl_line_buffer[rl_point]);
  if (Change == CSmove) rl_point++;
}

el_status_t el_ring_bell(void) {
  tty_put('\07');
  tty_flush();
  return CSstay;
}

static el_status_t do_macro(int c) {
  char name[4];
  name[0] = '_';
  name[1] = c;
  name[2] = '_';
  name[3] = '\0';
  if ((el_input = (char *)getenv((char *)name)) == NULL) {
    el_input = NILSTR;
    return el_ring_bell();
  }
  return CSstay;
}

/* Skip forward to start of next word. If @move is set we also move the cursor.
 */
static el_status_t do_forward(el_status_t move) {
  int i;
  char *p;
  i = 0;
  do {
    p = &rl_line_buffer[rl_point];
    /* Skip leading whitespace, like FSF Readline */
    for (; rl_point < rl_end && (p[0] == ' ' || !is_alpha_num(p[0]));
         rl_point++, p++) {
      if (move == CSmove) right(CSstay);
    }
    /* Skip to end of word, if inside a word. */
    for (; rl_point < rl_end && is_alpha_num(p[0]); rl_point++, p++) {
      if (move == CSmove) right(CSstay);
    }
    /* Skip to next word, or skip leading white space if outside a word. */
    for (; rl_point < rl_end && (p[0] == ' ' || !is_alpha_num(p[0]));
         rl_point++, p++) {
      if (move == CSmove) right(CSstay);
    }
    if (rl_point == rl_end) break;
  } while (++i < Repeat);
  return CSstay;
}

static el_status_t do_case(enum EditlineCase type) {
  int i;
  int end;
  int count;
  char *p;
  do_forward(CSstay);
  if (old_point != rl_point) {
    if ((count = rl_point - old_point) < 0) count = -count;
    rl_point = old_point;
    if ((end = rl_point + count) > rl_end) end = rl_end;
    for (i = rl_point, p = &rl_line_buffer[i]; rl_point < end; p++) {
      if ((type == TOupper) || (type == TOcapitalize && rl_point == i)) {
        if (islower(*p)) *p = toupper(*p);
      } else if (isupper(*p)) {
        *p = tolower(*p);
      }
      right(CSmove);
    }
  }
  return CSstay;
}

static el_status_t case_down_word(void) {
  return do_case(TOlower);
}

static el_status_t case_up_word(void) {
  return do_case(TOupper);
}

static el_status_t case_cap_word(void) {
  return do_case(TOcapitalize);
}

static void ceol(void) {
  int extras = 0;
  int i;
  char *p;
  while (rl_point < 0) {
    tty_put(' ');
    rl_point++;
    extras++;
  }
  for (i = rl_point, p = &rl_line_buffer[i]; i <= rl_end; i++, p++) {
    tty_put(' ');
    if (ISMETA(*p)) {
      if (rl_meta_chars) {
        tty_put(' ');
        tty_put(' ');
        extras += 2;
      }
    } else if (ISCTL(*p)) {
      tty_put(' ');
      extras++;
    }
  }
  for (i += extras; i > rl_point; i--) tty_back();
}

static void clear_line(void) {
  rl_point = -(int)strlen(rl_prompt);
  tty_put('\r');
  ceol();
  rl_point = 0;
  rl_end = 0;
  rl_line_buffer[0] = '\0';
}

static el_status_t insert_string(const char *p) {
  size_t len;
  int i;
  char *line;
  char *q;
  len = strlen(p);
  if (rl_end + len >= Length) {
    line = malloc(sizeof(char) * (Length + len + MEM_INC));
    if (!line) return CSstay;
    if (Length) {
      memcpy(line, rl_line_buffer, Length);
      free(rl_line_buffer);
    }
    rl_line_buffer = line;
    Length += len + MEM_INC;
  }
  for (q = &rl_line_buffer[rl_point], i = rl_end - rl_point; --i >= 0;)
    q[len + i] = q[i];
  memcpy(&rl_line_buffer[rl_point], p, len);
  rl_end += len;
  rl_line_buffer[rl_end] = '\0';
  tty_string(&rl_line_buffer[rl_point]);
  rl_point += len;
  return rl_point == rl_end ? CSstay : CSmove;
}

int rl_insert_text(const char *text) {
  int mark = rl_point;
  insert_string(text);
  ceol();
  return rl_point - mark;
}

static el_status_t redisplay(int cls) {
  if (cls && rl_point == 0 && rl_end == 0)
    tty_puts(CLEAR);
  else
    tty_puts("\r\e[K");
  tty_puts(rl_prompt);
  tty_string(rl_line_buffer);
  return CSmove;
}

static el_status_t refresh(void) {
  return redisplay(1);
}

int rl_refresh_line(int ignore1 __attribute__((unused)),
                    int ignore2 __attribute__((unused))) {
  redisplay(0);
  return 0;
}

static el_status_t toggle_meta_mode(void) {
  rl_meta_chars = !rl_meta_chars;
  return redisplay(0);
}

const char *el_next_hist(void) {
  return g_hist.Pos >= g_hist.Size - 1 ? NULL : g_hist.Lines[++g_hist.Pos];
}

const char *el_prev_hist(void) {
  return g_hist.Pos == 0 ? NULL : g_hist.Lines[--g_hist.Pos];
}

static el_status_t do_insert_hist(const char *p) {
  if (p == NULL) return el_ring_bell();
  clear_line();
  rl_point = 0;
  reposition();
  rl_end = 0;
  return insert_string(p);
}

static el_status_t do_hist(const char *(*move)(void)) {
  const char *p;
  int i = 0;
  do {
    if ((p = move()) == NULL) return el_ring_bell();
  } while (++i < Repeat);
  return do_insert_hist(p);
}

static el_status_t h_next(void) {
  if (el_no_hist) return CSstay;
  return do_hist(el_next_hist);
}

static el_status_t h_prev(void) {
  if (el_no_hist) return CSstay;
  return do_hist(el_prev_hist);
}

static el_status_t h_first(void) {
  return do_insert_hist(g_hist.Lines[g_hist.Pos = 0]);
}

static el_status_t h_last(void) {
  return do_insert_hist(g_hist.Lines[g_hist.Pos = g_hist.Size - 1]);
}

/*
**  Return zero if pat appears as a substring in text.
*/
static int substrcmp(const char *text, const char *pat, size_t len) {
  char c;
  if ((c = *pat) == '\0') return *text == '\0';
  for (; *text; text++) {
    if (*text == c && strncmp(text, pat, len) == 0) return 0;
  }
  return 1;
}

static const char *search_hist(const char *search, const char *(*move)(void)) {
  int len;
  int pos;
  int (*match)(const char *s1, const char *s2, size_t n);
  const char *pat;
  /* Save or get remembered search pattern. */
  if (search && *search) {
    if (old_search) free(old_search);
    old_search = strdup(search);
  } else {
    if (old_search == NULL || *old_search == '\0') return NULL;
    search = old_search;
  }
  /* Set up pattern-finder. */
  if (*search == '^') {
    match = strncmp;
    pat = search + 1;
  } else {
    match = substrcmp;
    pat = search;
  }
  len = strlen(pat);
  pos = g_hist.Pos; /* Save g_hist.Pos */
  while (move()) {
    if (match(g_hist.Lines[g_hist.Pos], pat, len) == 0)
      return g_hist.Lines[g_hist.Pos];
  }
  g_hist.Pos = pos; /* Restore g_hist.Pos */
  return NULL;
}

static el_status_t h_search_end(const char *p) {
  rl_prompt = old_prompt;
  Searching = 0;
  if (el_intr_pending > 0) {
    el_intr_pending = 0;
    clear_line();
    return redisplay(0);
  }
  p = search_hist(p, search_move);
  if (p == NULL) {
    el_ring_bell();
    clear_line();
    return redisplay(0);
  }
  return do_insert_hist(p);
}

static el_status_t h_search(void) {
  if (Searching) return el_ring_bell();
  Searching = 1;
  clear_line();
  old_prompt = rl_prompt;
  rl_prompt = "Search: ";
  tty_puts(rl_prompt);
  search_move = Repeat == NO_ARG ? el_prev_hist : el_next_hist;
  if (line_handler) {
    editinput(0);
    return CSstay;
  }
  return h_search_end(editinput(1));
}

static el_status_t fd_char(void) {
  int i = 0;
  do {
    if (rl_point >= rl_end) break;
    right(CSmove);
  } while (++i < Repeat);
  return CSstay;
}

static void save_yank(int begin, int i) {
  if (Yanked) {
    free(Yanked);
    Yanked = NULL;
  }
  if (i < 1) return;
  Yanked = malloc(sizeof(char) * (i + 1));
  if (Yanked) {
    memcpy(Yanked, &rl_line_buffer[begin], i);
    Yanked[i] = '\0';
  }
}

static el_status_t delete_string(int count) {
  int i;
  char *p;
  if (count <= 0 || rl_end == rl_point) return el_ring_bell();
  if (count == 1 && rl_point == rl_end - 1) {
    /* Optimize common case of delete at end of line. */
    rl_end--;
    p = &rl_line_buffer[rl_point];
    i = 1;
    tty_put(' ');
    if (ISCTL(*p)) {
      i = 2;
      tty_put(' ');
    } else if (rl_meta_chars && ISMETA(*p)) {
      i = 3;
      tty_put(' ');
      tty_put(' ');
    }
    tty_backn(i);
    *p = '\0';
    return CSmove;
  }
  if (rl_point + count > rl_end && (count = rl_end - rl_point) <= 0)
    return CSstay;
  if (count > 1) save_yank(rl_point, count);
  for (p = &rl_line_buffer[rl_point], i = rl_end - (rl_point + count) + 1;
       --i >= 0; p++)
    p[0] = p[count];
  ceol();
  rl_end -= count;
  tty_string(&rl_line_buffer[rl_point]);
  return CSmove;
}

static el_status_t bk_char(void) {
  int i = 0;
  do {
    if (rl_point == 0) break;
    left(CSmove);
  } while (++i < Repeat);
  return CSstay;
}

static el_status_t bk_del_char(void) {
  int i = 0;
  do {
    if (rl_point == 0) break;
    left(CSmove);
  } while (++i < Repeat);
  return delete_string(i);
}

static el_status_t kill_line(void) {
  int i;
  if (Repeat != NO_ARG) {
    if (Repeat < rl_point) {
      i = rl_point;
      rl_point = Repeat;
      reposition();
      delete_string(i - rl_point);
    } else if (Repeat > rl_point) {
      right(CSmove);
      delete_string(Repeat - rl_point - 1);
    }
    return CSmove;
  }
  save_yank(rl_point, rl_end - rl_point);
  rl_line_buffer[rl_point] = '\0';
  ceol();
  rl_end = rl_point;
  return CSstay;
}

static el_status_t insert_char(int c) {
  el_status_t s;
  char buff[2];
  char *p;
  char *q;
  int i;
  if (Repeat == NO_ARG || Repeat < 2) {
    buff[0] = c;
    buff[1] = '\0';
    return insert_string(buff);
  }
  p = malloc(sizeof(char) * (Repeat + 1));
  if (!p) return CSstay;
  for (i = Repeat, q = p; --i >= 0;) *q++ = c;
  *q = '\0';
  Repeat = 0;
  s = insert_string(p);
  free(p);
  return s;
}

static el_status_t beg_line(void) {
  if (rl_point) {
    rl_point = 0;
    return CSmove;
  }
  return CSstay;
}

static el_status_t end_line(void) {
  if (rl_point != rl_end) {
    rl_point = rl_end;
    return CSmove;
  }
  return CSstay;
}

static el_status_t del_char(void) {
  return delete_string(Repeat == NO_ARG ? CSeof : Repeat);
}

el_status_t el_del_char(void) {
  return del_char();
}

static el_status_t fd_word(void) {
  return do_forward(CSmove);
}

static el_status_t bk_word(void) {
  int i;
  char *p;
  i = 0;
  do {
    for (p = &rl_line_buffer[rl_point];
         p > rl_line_buffer && !is_alpha_num(p[-1]); p--)
      left(CSmove);
    for (; p > rl_line_buffer && !isblank(p[-1]) && is_alpha_num(p[-1]); p--)
      left(CSmove);
    if (rl_point == 0) break;
  } while (++i < Repeat);
  return CSstay;
}

static el_status_t meta(void) {
  int c;
  struct EditlineKeymap *kp;
  if ((c = tty_get()) == EOF) return CSeof;
#ifdef CONFIG_ANSI_ARROWS
  /* Also include VT-100 arrows. */
  if (c == '[' || c == 'O') {
    switch (tty_get()) {
      case EOF:
        return CSeof;
      case '1': {
        char seq[4] = {0};
        for (c = 0; c < 3; c++) seq[c] = tty_get();
        if (!strncmp(seq, ";5C", 3)) return fd_word(); /* Ctrl+Right */
        if (!strncmp(seq, ";5D", 3)) return bk_word(); /* Ctrl+Left */
        break;
      }
      case '2':
        tty_get();
        return CSstay; /* Insert */
      case '3':
        tty_get();
        return del_char(); /* Delete */
      case '5':
        tty_get();
        return CSstay; /* PgUp */
      case '6':
        tty_get();
        return CSstay; /* PgDn */
      case 'A':
        return h_prev(); /* Up */
      case 'B':
        return h_next(); /* Down */
      case 'C':
        return fd_char(); /* Left */
      case 'D':
        return bk_char(); /* Right */
      case 'F':
        return end_line(); /* End */
      case 'H':
        return beg_line(); /* Home */
      default:             /* Fall through */
        break;
    }
    return el_ring_bell();
  }
#endif /* CONFIG_ANSI_ARROWS */
  if (isdigit(c)) {
    for (Repeat = c - '0'; (c = tty_get()) != EOF && isdigit(c);)
      Repeat = Repeat * 10 + c - '0';
    tty_push(c);
    return CSstay;
  }
  if (isupper(c)) return do_macro(c);
  for (kp = MetaMap; kp->Function; kp++) {
    if (kp->Key == c) return kp->Function();
  }
  return el_ring_bell();
}

static el_status_t emacs(int c) {
  el_status_t s;
  struct EditlineKeymap *kp;
  /* Save point before interpreting input character 'c'. */
  old_point = rl_point;
  if (rl_meta_chars && ISMETA(c)) {
    tty_push(UNMETA(c));
    return meta();
  }
  for (kp = Map; kp->Function; kp++) {
    if (kp->Key == c) break;
  }
  if (kp->Function) {
    s = kp->Function();
    if (s == CSdispatch) /* If Function is inhibited. */
      s = insert_char(c);
  } else {
    s = insert_char(c);
  }
  if (!el_pushed) {
    /* No pushback means no repeat count; hacky, but true. */
    Repeat = NO_ARG;
  }
  return s;
}

static el_status_t tty_special(int c) {
#ifdef CONFIG_SIGINT
  if (c == rl_intr) {
    el_intr_pending = SIGINT;
    return CSsignal;
  }
#endif
  if (c == rl_quit) {
    el_intr_pending = SIGQUIT;
    return CSeof;
  }
#ifdef CONFIG_SIGSTOP
  if (c == rl_susp) {
    el_intr_pending = SIGTSTP;
    return CSsignal;
  }
#endif
  if (rl_meta_chars && ISMETA(c)) return CSdispatch;
  if (c == rl_erase || c == DEL) return bk_del_char();
  if (c == rl_kill) {
    if (rl_point != 0) {
      rl_point = 0;
      reposition();
    }
    Repeat = NO_ARG;
    return kill_line();
  }
#ifdef CONFIG_EOF
  if (c == rl_eof && rl_point == 0 && rl_end == 0) return CSeof;
#endif
  return CSdispatch;
}

static char *editinput(int complete) {
  int c;
  do {
    c = tty_get();
    if (c == EOF) break;
    switch (tty_special(c)) {
      case CSdone:
        return rl_line_buffer;
      case CSeof:
        return NULL;
      case CSsignal:
        return (char *)"";
      case CSmove:
        reposition();
        break;
      case CSdispatch:
        switch (emacs(c)) {
          case CSdone:
            return rl_line_buffer;
          case CSeof:
            return NULL;
          case CSsignal:
            return (char *)"";
          case CSmove:
            reposition();
            break;
          case CSdispatch:
          case CSstay:
            break;
        }
        break;
      case CSstay:
        break;
    }
  } while (complete);
  return NULL;
}

static void hist_alloc(void) {
  if (!g_hist.Lines) g_hist.Lines = calloc(el_hist_size, sizeof(char *));
}

static void hist_add(const char *p) {
  int i;
  char *s;
#ifdef CONFIG_UNIQUE_HISTORY
  if (g_hist.Size && strcmp(p, g_hist.Lines[g_hist.Size - 1]) == 0) return;
#endif
  s = strdup(p);
  if (s == NULL) return;
  if (g_hist.Size < el_hist_size) {
    g_hist.Lines[g_hist.Size++] = s;
  } else {
    free(g_hist.Lines[0]);
    for (i = 0; i < el_hist_size - 1; i++)
      g_hist.Lines[i] = g_hist.Lines[i + 1];
    g_hist.Lines[i] = s;
  }
  g_hist.Pos = g_hist.Size - 1;
}

static char *read_redirected(void) {
  int size = MEM_INC;
  char *p;
  char *line;
  char *end;
  p = line = malloc(sizeof(char) * size);
  if (!p) return NULL;
  end = p + size;
  while (1) {
    if (p == end) {
      int oldpos = end - line;
      size += MEM_INC;
      p = line = realloc(line, sizeof(char) * size);
      if (!p) return NULL;
      end = p + size;
      p += oldpos; /* Continue where we left off... */
    }
    if (read(el_infd, p, 1) <= 0) {
      /* Ignore "incomplete" lines at EOF, just like we do for a tty. */
      free(line);
      return NULL;
    }
    if (*p == '\n') break;
    p++;
  }
  *p = '\0';
  return line;
}

/* For compatibility with FSF readline. */
void rl_reset_terminal(const char *terminal_name) {
#ifdef CONFIG_USE_TERMCAP
  char buf[1024];
  char *bp;
#endif
#ifdef TIOCGWINSZ
  struct winsize W;
#endif
  if (terminal_name) {
    el_term = terminal_name;
  } else if ((el_term = getenv("TERM")) == NULL) {
    el_term = "dumb";
  }
  /* Initialize to faulty values to trigger fallback if nothing else works. */
  tty_cols = tty_rows = -1;
#ifdef CONFIG_USE_TERMCAP
  bp = buf;
  if (-1 != tgetent(buf, el_term)) {
    if ((backspace = tgetstr("le", &bp)) != NULL) backspace = strdup(backspace);
    tty_cols = tgetnum("co");
    tty_rows = tgetnum("li");
  }
  /* Make sure to check width & rows and fallback to TIOCGWINSZ if available. */
#endif
  if (tty_cols <= 0 || tty_rows <= 0) {
#ifdef TIOCGWINSZ
    if (ioctl(el_outfd, TIOCGWINSZ, &W) >= 0 && W.ws_col > 0 && W.ws_row > 0) {
      tty_cols = (int)W.ws_col;
      tty_rows = (int)W.ws_row;
      return;
    }
#endif
    tty_cols = SCREEN_COLS;
    tty_rows = SCREEN_ROWS;
  }
}

void rl_initialize(void) {
  if (!rl_prompt) rl_prompt = "? ";
  hist_alloc();
  /* Setup I/O descriptors */
  if (!rl_instream)
    el_infd = EL_STDIN;
  else
    el_infd = fileno(rl_instream);
  if (el_infd < 0) el_infd = EL_STDIN;
  if (!rl_outstream)
    el_outfd = EL_STDOUT;
  else
    el_outfd = fileno(rl_outstream);
  if (el_outfd < 0) el_outfd = EL_STDOUT;
}

void rl_uninitialize(void) {
  int i;
  /* Uninitialize the history */
  if (g_hist.Lines) {
    for (i = 0; i < el_hist_size; i++) {
      if (g_hist.Lines[i]) free(g_hist.Lines[i]);
      g_hist.Lines[i] = NULL;
    }
    free(g_hist.Lines);
    g_hist.Lines = NULL;
  }
  g_hist.Size = 0;
  g_hist.Pos = 0;
  if (old_search) free(old_search);
  old_search = NULL;
  /* Uninitialize the line buffer */
  if (rl_line_buffer) free(rl_line_buffer);
  rl_line_buffer = NULL;
  Length = 0;
}

void rl_save_prompt(void) {
  rl_saved_prompt = rl_prompt;
}

void rl_restore_prompt(void) {
  if (rl_saved_prompt) rl_prompt = rl_saved_prompt;
}

void rl_set_prompt(const char *prompt) {
  rl_prompt = prompt;
}

void rl_clear_message(void) { /* Nothing to do atm. */
}

void rl_forced_update_display() {
  redisplay(0);
  tty_flush();
}

static int el_prep(const char *prompt) {
  rl_initialize();
  if (!rl_line_buffer) {
    Length = MEM_INC;
    rl_line_buffer = malloc(sizeof(char) * Length);
    if (!rl_line_buffer) return -1;
  }
  tty_info();
  rl_prep_term_function(!rl_meta_chars);
  hist_add(NILSTR);
  ScreenSize = SCREEN_INC;
  Screen = malloc(sizeof(char) * ScreenSize);
  if (!Screen) return -1;
  rl_prompt = prompt ? prompt : NILSTR;
  if (el_no_echo) {
    int old = el_no_echo;
    el_no_echo = 0;
    tty_puts(rl_prompt);
    tty_flush();
    el_no_echo = old;
  } else {
    tty_puts(rl_prompt);
  }
  Repeat = NO_ARG;
  old_point = rl_point = rl_mark = rl_end = 0;
  rl_line_buffer[0] = '\0';
  el_intr_pending = -1;
  return 0;
}

static char *el_deprep(char *line) {
  if (line) {
    line = strdup(line);
    tty_puts(NEWLINE);
    tty_flush();
  }
  rl_deprep_term_function();
  if (Screen) {
    free(Screen);
    Screen = NULL;
  }
  free(g_hist.Lines[--g_hist.Size]);
  g_hist.Lines[g_hist.Size] = NULL;
  /* Add to history, unless no-echo or no-history mode ... */
  if (!el_no_echo && !el_no_hist) {
    if (line != NULL && *line != '\0') hist_add(line);
  }
  if (el_intr_pending > 0) {
    int signo = el_intr_pending;
    el_intr_pending = 0;
    kill(getpid(), signo);
  }
  return line;
}

void rl_callback_handler_install(const char *prompt, rl_vcpfunc_t *lhandler) {
  if (!lhandler) return;
  line_handler = lhandler;
  /*
   * Any error from el_prep() is handled by the lhandler callbck as
   * soon as the user calls rl_callback_read_char().
   */
  el_prep(prompt);
  tty_flush();
}

/*
 * Reads one character at a time, when a complete line has been received
 * the lhandler from rl_callback_handler_install() is called with the
 * line as argument.
 *
 * If the callback returns the terminal is prepped for reading a new
 * line.
 *
 * If any error occurs, either in the _install() phase, or while reading
 * one character, this function restores the terminal and calls lhandler
 * with a NULL argument.
 */
void rl_callback_read_char(void) {
  char *line;
  if (!line_handler) {
    errno = EINVAL;
    return;
  }
  /*
   * Check if rl_callback_handler_install() failed
   * This is the only point where we can tell user
   */
  if (!Screen || !rl_line_buffer) {
    errno = ENOMEM;
    line_handler(el_deprep(NULL));
    return;
  }
  line = editinput(0);
  if (line) {
    char *l;
    if (Searching) {
      h_search_end(line);
      tty_flush();
      return;
    }
    l = el_deprep(line);
    line_handler(l);
    if (el_prep(rl_prompt)) line_handler(NULL);
  }
  tty_flush();
}

void rl_callback_handler_remove(void) {
  if (!line_handler) return;
  el_deprep(NULL);
  line_handler = NULL;
}

char *readline(const char *prompt) {
  /* Unless called by the user already. */
  rl_initialize();
  if (!isatty(el_infd)) {
    tty_flush();
    return read_redirected();
  }
  if (el_prep(prompt)) return NULL;
  return el_deprep(editinput(1));
}

/*
 * Even though readline() itself adds history automatically, the user
 * can also add lines.  This is for compatibility with GNU Readline.
 */
void add_history(const char *p) {
  if (p == NULL || *p == '\0') return;
  hist_add(p);
}

int read_history(const char *filename) {
  FILE *fp;
  char buf[SCREEN_INC];
  hist_alloc();
  fp = fopen(filename, "r");
  if (fp) {
    g_hist.Size = 0;
    while (g_hist.Size < el_hist_size) {
      if (!fgets(buf, SCREEN_INC, fp)) break;
      buf[strlen(buf) - 1] = 0; /* Remove '\n' */
      add_history(buf);
    }
    return fclose(fp);
  }
  return errno;
}

int write_history(const char *filename) {
  FILE *fp;
  hist_alloc();
  fp = fopen(filename, "w");
  if (fp) {
    int i = 0;
    while (i < g_hist.Size) fprintf(fp, "%s\n", g_hist.Lines[i++]);
    return fclose(fp);
  }
  return errno;
}

/*
**  Move back to the beginning of the current word and return an
**  allocated copy of it.
*/
char *el_find_word(void) {
  char *p, *q;
  char *word;
  size_t len;
  p = &rl_line_buffer[rl_point];
  while (p > rl_line_buffer) {
    p--;
    if (p > rl_line_buffer && p[-1] == '\\') {
      p--;
    } else {
      if (strchr(SEPS, (char)*p) != NULL) {
        p++;
        break;
      }
    }
  }
  len = rl_point - (p - rl_line_buffer) + 1;
  word = malloc(sizeof(char) * len);
  if (!word) return NULL;
  q = word;
  while (p < &rl_line_buffer[rl_point]) {
    if (*p == '\\') {
      if (++p == &rl_line_buffer[rl_point]) break;
    }
    *q++ = *p++;
  }
  *q = '\0';
  return word;
}

static el_status_t c_possible(void) {
  char **av;
  char *word;
  int ac;
  word = el_find_word();
  ac = rl_list_possib(word, &av);
  if (word) free(word);
  if (ac) {
    el_print_columns(ac, av);
    while (--ac >= 0) free(av[ac]);
    free(av);
    return CSmove;
  }
  return el_ring_bell();
}

static el_status_t c_complete(void) {
  char *p, *q;
  char *word, *string;
  size_t len;
  int unique;
  el_status_t s = CSdone;
  if (rl_inhibit_complete) return CSdispatch;
  word = el_find_word();
  p = rl_complete(word, &unique);
  if (word) free(word);
  if (p) {
    len = strlen(p);
    word = p;
    string = q = malloc(sizeof(char) * (2 * len + 1));
    if (!string) {
      free(word);
      return CSstay;
    }
    while (*p) {
      if ((*p < ' ' || strchr(SEPS, *p) != NULL) && (!unique || p[1] != 0)) {
        *q++ = '\\';
      }
      *q++ = *p++;
    }
    *q = '\0';
    free(word);
    if (len > 0) {
      s = insert_string(string);
#ifdef CONFIG_TERMINAL_BELL
      if (!unique) el_ring_bell();
#endif
    }
    free(string);
    if (len > 0) return s;
  }
  return c_possible();
}

static el_status_t accept_line(void) {
  rl_line_buffer[rl_end] = '\0';
  return CSdone;
}

static el_status_t end_of_input(void) {
  rl_line_buffer[rl_end] = '\0';
  return CSeof;
}

static el_status_t ctrl_z(void) {
  if (IsWindows()) {
    return end_of_input();
  } else {
    return el_ring_bell();
  }
}

static el_status_t transpose_(void) {
  char c;
  if (rl_point) {
    if (rl_point == rl_end) left(CSmove);
    c = rl_line_buffer[rl_point - 1];
    left(CSstay);
    rl_line_buffer[rl_point - 1] = rl_line_buffer[rl_point];
    tty_show(rl_line_buffer[rl_point - 1]);
    rl_line_buffer[rl_point++] = c;
    tty_show(c);
  }
  return CSstay;
}

static el_status_t quote(void) {
  int c;
  return (c = tty_get()) == EOF ? CSeof : insert_char((int)c);
}

static el_status_t mk_set(void) {
  rl_mark = rl_point;
  return CSstay;
}

static el_status_t exchange(void) {
  int c;
  if ((c = tty_get()) != CTL('X')) return c == EOF ? CSeof : el_ring_bell();
  if ((c = rl_mark) <= rl_end) {
    rl_mark = rl_point;
    rl_point = c;
    return CSmove;
  }
  return CSstay;
}

static el_status_t yank(void) {
  if (Yanked && *Yanked) return insert_string(Yanked);
  return CSstay;
}

static el_status_t copy_region(void) {
  if (rl_mark > rl_end) return el_ring_bell();
  if (rl_point > rl_mark)
    save_yank(rl_mark, rl_point - rl_mark);
  else
    save_yank(rl_point, rl_mark - rl_point);
  return CSstay;
}

static el_status_t move_to_char(void) {
  int i, c;
  char *p;
  if ((c = tty_get()) == EOF) return CSeof;
  for (i = rl_point + 1, p = &rl_line_buffer[i]; i < rl_end; i++, p++) {
    if (*p == c) {
      rl_point = i;
      return CSmove;
    }
  }
  return CSstay;
}

static el_status_t fd_kill_word(void) {
  int i;
  do_forward(CSstay);
  if (old_point != rl_point) {
    i = rl_point - old_point - 1;
    rl_point = old_point;
    return delete_string(i);
  }
  return CSstay;
}

static el_status_t bk_kill_word(void) {
  bk_word();
  if (old_point != rl_point) return delete_string(old_point - rl_point);
  return CSstay;
}

static int argify(char *line, char ***avp) {
  char *c;
  char **p;
  char **arg;
  int ac;
  int i;
  i = MEM_INC;
  *avp = p = malloc(sizeof(char *) * i);
  if (!p) return 0;
  for (c = line; isspace(*c); c++) continue;
  if (*c == '\n' || *c == '\0') return 0;
  for (ac = 0, p[ac++] = c; *c && *c != '\n';) {
    if (!isspace(*c)) {
      c++;
      continue;
    }
    *c++ = '\0';
    if (*c && *c != '\n') {
      if (ac + 1 == i) {
        arg = malloc(sizeof(char *) * (i + MEM_INC));
        if (!arg) {
          p[ac] = NULL;
          return ac;
        }
        memcpy(arg, p, i * sizeof(char *));
        i += MEM_INC;
        free(p);
        *avp = p = arg;
      }
      p[ac++] = c;
    }
  }
  *c = '\0';
  p[ac] = NULL;
  return ac;
}

static el_status_t last_argument(void) {
  char **av = NULL;
  char *p;
  el_status_t s;
  int ac;
  if (g_hist.Size == 1 || (p = (char *)g_hist.Lines[g_hist.Size - 2]) == NULL)
    return el_ring_bell();
  p = strdup(p);
  if (!p) return CSstay;
  ac = argify(p, &av);
  if (Repeat != NO_ARG)
    s = Repeat < ac ? insert_string(av[Repeat]) : el_ring_bell();
  else
    s = ac ? insert_string(av[ac - 1]) : CSstay;
  if (av) free(av);
  free(p);
  return s;
}

static struct EditlineKeymap MetaMap[64] = {
    {CTL('H'), bk_kill_word}, {DEL, bk_kill_word}, {' ', mk_set},
    {'.', last_argument},     {'<', h_first},      {'>', h_last},
    {'?', c_possible},        {'b', bk_word},      {'c', case_cap_word},
    {'d', fd_kill_word},      {'f', fd_word},      {'l', case_down_word},
    {'m', toggle_meta_mode},  {'u', case_up_word}, {'y', yank},
    {'w', copy_region},       {0, NULL},
};

static size_t find_key_in_map(int key, struct EditlineKeymap map[],
                              size_t mapsz) {
  size_t i;
  for (i = 0; map[i].Function && i < mapsz; i++) {
    if (map[i].Key == key) return i;
  }
  if (i < mapsz) return i;
  return mapsz;
}

static el_status_t el_bind_key_in_map(int key, el_keymap_func_t function,
                                      struct EditlineKeymap map[],
                                      size_t mapsz) {
  size_t creat, pos = find_key_in_map(key, map, mapsz);
  /* Must check that pos is not the next to last array position,
   * otherwise we will write out-of-bounds to terminate the list. */
  if (pos + 1 >= mapsz) {
    errno = ENOMEM;
    return CSeof;
  }
  /* Add at end, create new? */
  creat = map[pos].Function == NULL;
  /* A new key so have to add it to end */
  map[pos].Key = key;
  map[pos].Function = function;
  /* Terminate list */
  if (creat) {
    map[pos + 1].Key = 0;
    map[pos + 1].Function = NULL;
  }
  return CSdone;
}

static struct EditlineKeymap Map[64] = {
    {CTL('@'), mk_set},
    {CTL('A'), beg_line},
    {CTL('B'), bk_char},
    {CTL('D'), del_char},
    {CTL('E'), end_line},
    {CTL('F'), fd_char},
    {CTL('G'), el_ring_bell},
    {CTL('H'), bk_del_char},
    {CTL('I'), c_complete},
    {CTL('J'), accept_line},
    {CTL('K'), kill_line},
    {CTL('L'), refresh},
    {CTL('M'), accept_line},
    {CTL('N'), h_next},
    {CTL('O'), el_ring_bell},
    {CTL('P'), h_prev},
    {CTL('Q'), el_ring_bell},
    {CTL('R'), h_search},
    {CTL('S'), el_ring_bell},
    {CTL('T'), transpose_},
    {CTL('U'), el_ring_bell},
    {CTL('V'), quote},
    {CTL('W'), bk_kill_word},
    {CTL('X'), exchange},
    {CTL('Y'), yank},
    {CTL('Z'), ctrl_z},
    {CTL('['), meta},
    {CTL(']'), move_to_char},
    {CTL('^'), el_ring_bell},
    {CTL('_'), el_ring_bell},
    {0, NULL},
};

el_status_t el_bind_key(int key, el_keymap_func_t function) {
  return el_bind_key_in_map(key, function, Map, ARRAYLEN(Map));
}

el_status_t el_bind_key_in_metamap(int key, el_keymap_func_t function) {
  return el_bind_key_in_map(key, function, MetaMap, ARRAYLEN(MetaMap));
}

rl_getc_func_t *rl_set_getc_func(rl_getc_func_t *func) {
  rl_getc_func_t *old = rl_getc_function;
  rl_getc_function = func;
  return old;
}
