/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│                                                                              │
│  Kilo ── A very simple editor in less than 1-kilo lines of code (as          │
│          counted by "cloc"). Does not depend on libcurses, directly          │
│          emits VT100 escapes on the terminal.                                │
│                                                                              │
╞══════════════════════════════════════════════════════════════════════════════╡
│  Copyright 2016 Salvatore Sanfilippo <antirez@gmail.com>                     │
│                                                                              │
│  Redistribution and use in source and binary forms, with or without          │
│  modification, are permitted provided that the following conditions are      │
│  met:                                                                        │
│                                                                              │
│   *  Redistributions of source code must retain the above copyright          │
│      notice, this list of conditions and the following disclaimer.           │
│                                                                              │
│   *  Redistributions in binary form must reproduce the above copyright       │
│      notice, this list of conditions and the following disclaimer in the     │
│      documentation and/or other materials provided with the distribution.    │
│                                                                              │
│  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS         │
│  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT           │
│  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR       │
│  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT        │
│  HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,      │
│  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT            │
│  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,       │
│  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY       │
│  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT         │
│  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE       │
│  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.        │
╚─────────────────────────────────────────────────────────────────────────────*/

asm(".ident \"\n\
Kilo ─ A very simple editor (BSD-2)\n\
Copyright 2016 Salvatore Sanfilippo\n\
Contact: antirez@gmail.com\"\n\
.include \"libc/disclaimer.inc\"");

/*
 * This software has been modified by Justine Tunney to:
 *
 *   1. Have Emacs keybindings.
 *
 *   2. Be capable of editing files with ANSI color codes, in such a way
 *      that the ANSI color codes are displayed 'as is' (since that's
 *      something no other editor can quite do). The kinda buggy syntax
 *      highlighting code needed to be commented out, to do this.
 */

#define KILO_VERSION "0.0.1"
#define SYNTAX       1

#ifndef _BSD_SOURCE
#define _BSD_SOURCE
#endif
#define _GNU_SOURCE

#include "libc/calls/calls.h"
#include "libc/calls/termios.h"
#include "libc/calls/weirdtypes.h"
#include "libc/errno.h"
#include "libc/log/log.h"
#include "libc/mem/alg.h"
#include "libc/mem/arraylist2.internal.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/termios.h"
#include "libc/time/time.h"

/* Syntax highlight types */
#define HL_NORMAL    0
#define HL_NONPRINT  1
#define HL_COMMENT   2 /* Single line comment. */
#define HL_MLCOMMENT 3 /* Multi-line comment. */
#define HL_KEYWORD1  4
#define HL_KEYWORD2  5
#define HL_STRING    6
#define HL_NUMBER    7
#define HL_MATCH     8 /* Search match. */

#define HL_HIGHLIGHT_STRINGS (1 << 0)
#define HL_HIGHLIGHT_NUMBERS (1 << 1)

struct editorSyntax {
  const char *const *filematch;
  const char *const *keywords;
  char singleline_comment_start[2];
  char multiline_comment_start[3];
  char multiline_comment_end[3];
  int flags;
};

/* This structure represents a single line of the file we are editing. */
typedef struct erow {
  int idx;           /* Row index in the file, zero-based. */
  int size;          /* Size of the row, excluding the null term. */
  int rsize;         /* Size of the rendered row. */
  char *chars;       /* Row content. */
  char *render;      /* Row content "rendered" for screen (for TABs). */
  unsigned char *hl; /* Syntax highlight type for each character in render.*/
  int hl_oc;         /* Row had open comment at end in last syntax highlight
                        check. */
} erow;

typedef struct hlcolor {
  int r, g, b;
} hlcolor;

struct editorConfig {
  int cx, cy;     /* Cursor x and y position in characters */
  int rowoff;     /* Offset of row displayed. */
  int coloff;     /* Offset of column displayed. */
  int screenrows; /* Number of rows that we can show */
  int screencols; /* Number of cols that we can show */
  int numrows;    /* Number of rows */
  int rawmode;    /* Is terminal raw mode enabled? */
  erow *row;      /* Rows */
  int dirty;      /* File modified but not saved. */
  char *filename; /* Currently open filename */
  char statusmsg[80];
  time_t statusmsg_time;
  const struct editorSyntax *syntax; /* Current syntax highlight, or NULL. */
};

static struct editorConfig E;

#define CTRL(C) ((C) ^ 0b01000000) /* where ^W etc. codes come from */

enum KEY_ACTION {
  ARROW_LEFT = 1000,
  ARROW_RIGHT,
  ARROW_UP,
  ARROW_DOWN,
  DEL_KEY,
  HOME_KEY,
  END_KEY,
  PAGE_UP,
  PAGE_DOWN
};

void editorSetStatusMessage(const char *fmt, ...);

/* =========================== Syntax highlights DB =========================
 *
 * In order to add a new syntax, define two arrays with a list of file name
 * matches and keywords. The file name matches are used in order to match
 * a given syntax with a given file name: if a match pattern starts with a
 * dot, it is matched as the last past of the filename, for example ".c".
 * Otherwise the pattern is just searched inside the filename, like "Makefile").
 *
 * The list of keywords to highlight is just a list of words, however if they
 * a trailing '|' character is added at the end, they are highlighted in
 * a different color, so that you can have two different sets of keywords.
 *
 * Finally add a stanza in the HLDB global variable with two two arrays
 * of strings, and a set of flags in order to enable highlighting of
 * comments and numbers.
 *
 * The characters for single and multi line comments must be exactly two
 * and must be provided as well (see the C language example).
 *
 * There is no support to highlight patterns currently. */

/* C / C++ */
const char *const C_HL_extensions[] = {".c", ".h", ".cpp", NULL};
const char *const C_HL_keywords[] = {
    /* A few C / C++ keywords */
    "switch", "if", "while", "for", "break", "continue", "return", "else",
    "struct", "union", "typedef", "static", "enum", "class",
    /* C types */
    "int|", "long|", "double|", "float|", "char|", "unsigned|", "signed|",
    "void|", "const|", "size_t|", "ssize_t|", "uint8_t|", "int8_t|",
    "uint16_t|", "int16_t|", "uint32_t|", "int32_t|", "uint64_t|", "int64_t|",
    NULL};

/* Here we define an array of syntax highlights by extensions, keywords,
 * comments delimiters and flags. */
const struct editorSyntax HLDB[] = {
    {/* C / C++ */
     C_HL_extensions, C_HL_keywords, "//", "/*", "*/",
     HL_HIGHLIGHT_STRINGS | HL_HIGHLIGHT_NUMBERS}};

#define HLDB_ENTRIES (sizeof(HLDB) / sizeof(HLDB[0]))

/* ======================= Low level terminal handling ====================== */

static struct termios orig_termios; /* In order to restore at exit.*/

void disableRawMode(int64_t fd) {
  /* Don't even check the return value as it's too late. */
  if (E.rawmode) {
    tcsetattr(fd, TCSAFLUSH, &orig_termios);
    E.rawmode = 0;
  }
}

/* Called at exit to avoid remaining in raw mode. */
void editorAtExit(void) {
  char buf[64];
  disableRawMode(STDIN_FILENO);
  write(STDOUT_FILENO, buf,
        sprintf(buf, "\e[%d;%dH\r\n\r\n\r\n", E.screenrows, E.screencols));
}

/* Raw mode: 1960 magic shit. */
int enableRawMode(int64_t fd) {
  struct termios raw;

  if (E.rawmode) return 0; /* Already enabled. */
  if (!isatty(STDIN_FILENO)) goto fatal;
  atexit(editorAtExit);
  if (tcgetattr(fd, &orig_termios) == -1) goto fatal;

  raw = orig_termios; /* modify the original mode */
  /* input modes: no break, no CR to NL, no parity check, no strip char,
   * no start/stop output control. */
  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  /* output modes - disable post processing */
  raw.c_oflag &= ~(OPOST);
  /* control modes - set 8 bit chars */
  raw.c_cflag |= (CS8);
  /* local modes - choing off, canonical off, no extended functions,
   * no signal chars (^Z,^C) */
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
  /* control chars - set return condition: min number of bytes and timer. */
  raw.c_cc[VMIN] = 0;  /* Return each byte, or zero for timeout. */
  raw.c_cc[VTIME] = 1; /* 100 ms timeout (unit is tens of second). */

  /* put terminal in raw mode after flushing */
  if (tcsetattr(fd, TCSAFLUSH, &raw) < 0) goto fatal;
  E.rawmode = 1;
  return 0;

fatal:
  errno = ENOTTY;
  return -1;
}

/* Read a key from the terminal put in raw mode, trying to handle
 * escape sequences. */
int editorReadKey(int64_t fd) {
  int nread;
  char c, seq[3];
  do {
    nread = read(fd, &c, 1);
    if (nread == -1) exit(1);
  } while (!nread);

  while (1) {
    switch (c) {
      case CTRL('J'): /* newline */
        return CTRL('M');
      case CTRL('V'):
        return PAGE_DOWN;
      case '\e': /* escape sequence */
        /* If this is just an ESC, we'll timeout here. */
        if (read(fd, seq, 1) == 0) return CTRL('[');
        if (seq[0] == '[') {
          if (read(fd, seq + 1, 1) == 0) return CTRL('[');
          if (seq[1] >= '0' && seq[1] <= '9') {
            /* Extended escape, read additional byte. */
            if (read(fd, seq + 2, 1) == 0) return CTRL('[');
            if (seq[2] == '~') {
              switch (seq[1]) {
                case '1':
                  return HOME_KEY;
                case '3':
                  return DEL_KEY;
                case '4':
                  return END_KEY;
                case '5':
                  return PAGE_UP;
                case '6':
                  return PAGE_DOWN;
              }
            }
          } else {
            /* Arrow Keys
             *
             *   KEY   CODE     FN  SHIFT OPTION
             * ───── ────── ────── ────── ──────
             *    UP    ←[A   ←[5~    ←[A    ←[A
             *  DOWN    ←[B   ←[6~    ←[B    ←[B
             * RIGHT    ←[C   ←[4~ ←[1;2C    ←[f
             *  LEFT    ←[D   ←[1~ ←[1;2C    ←[b
             */
            switch (seq[1]) {
              case 'A':
                return ARROW_UP;
              case 'B':
                return ARROW_DOWN;
              case 'C':
                return ARROW_RIGHT;
              case 'D':
                return ARROW_LEFT;
              case 'H':
                return HOME_KEY;
              case 'F':
                return END_KEY;
            }
          }
        } else if (seq[0] == 'v') {
          return PAGE_UP;
        } else if (seq[0] == 'O') {
          if (read(fd, seq + 1, 1) == 0) return CTRL('[');
          /* ESC O sequences. */
          switch (seq[1]) {
            case 'H':
              return HOME_KEY;
            case 'F':
              return END_KEY;
          }
        }
        break;
      default:
        return c;
    }
  }
}

/* Use the ESC [6n escape sequence to query the horizontal cursor position
 * and return it. On error -1 is returned, on success the position of the
 * cursor is stored at *rows and *cols and 0 is returned. */
int getCursorPosition(int64_t ifd, int64_t ofd, int *rows, int *cols) {
  char buf[32];
  unsigned i = 0;

  /* Report cursor location */
  if (write(ofd, "\e[6n", 4) != 4) return -1;

  /* Read the response: ESC [ rows ; cols R */
  while (i < sizeof(buf) - 1) {
    if (read(ifd, buf + i, 1) != 1) break;
    if (buf[i] == 'R') break;
    i++;
  }
  buf[i] = '\0';

  /* Parse it. */
  if (buf[0] != CTRL('[') || buf[1] != '[') return -1;
  if (sscanf(buf + 2, "%d;%d", rows, cols) != 2) return -1;
  return 0;
}

/* Try to get the number of columns in the current terminal. If the ioctl()
 * call fails the function will try to query the terminal itself.
 * Returns 0 on success, -1 on error. */
int getWindowSize(int64_t ifd, int64_t ofd, int *rows, int *cols) {
  struct winsize ws;
  if (tcgetwinsize(1, &ws) == -1 || ws.ws_col == 0) {
    /* ioctl() failed. Try to query the terminal itself. */
    int orig_row, orig_col, retval;

    /* Get the initial position so we can restore it later. */
    retval = getCursorPosition(ifd, ofd, &orig_row, &orig_col);
    if (retval == -1) goto failed;

    /* Go to right/bottom margin and get position. */
    if (write(ofd, "\e[999C\e[999B", 12) != 12) goto failed;
    retval = getCursorPosition(ifd, ofd, rows, cols);
    if (retval == -1) goto failed;

    /* Restore position. */
    char seq[32];
    snprintf(seq, 32, "\e[%d;%dH", orig_row, orig_col);
    if (write(ofd, seq, strlen(seq)) == -1) {
      /* Can't recover... */
    }
    return 0;
  } else {
    *cols = ws.ws_col;
    *rows = ws.ws_row;
    return 0;
  }

failed:
  return -1;
}

/* ====================== Syntax highlight color scheme  ==================== */

int is_separator(int c) {
  return c == '\0' || isspace(c) || strchr(",.()+-/*=~%[];", c) != NULL;
}

/* Return true if the specified row last char is part of a multi line comment
 * that starts at this row or at one before, and does not end at the end
 * of the row but spans to the next row. */
int editorRowHasOpenComment(erow *row) {
  if (row->hl && row->rsize && row->hl[row->rsize - 1] == HL_MLCOMMENT &&
      (row->rsize < 2 || (row->render[row->rsize - 2] != '*' ||
                          row->render[row->rsize - 1] != '/')))
    return 1;
  return 0;
}

/* Set every byte of row->hl (that corresponds to every character in the line)
 * to the right syntax highlight type (HL_* defines). */
void editorUpdateSyntax(erow *row) {
  row->hl = realloc(row->hl, row->rsize);
  memset(row->hl, HL_NORMAL, row->rsize);

  if (E.syntax == NULL) return; /* No syntax, everything is HL_NORMAL. */

  int i, prev_sep, in_string, in_comment;
  char *p;
  const char *const *keywords = E.syntax->keywords;
  const char *scs = E.syntax->singleline_comment_start;
  const char *mcs = E.syntax->multiline_comment_start;
  const char *mce = E.syntax->multiline_comment_end;

  /* Point to the first non-space char. */
  p = row->render;
  i = 0; /* Current char offset */
  while (*p && isspace(*p)) {
    p++;
    i++;
  }
  prev_sep = 1;   /* Tell the parser if 'i' points to start of word. */
  in_string = 0;  /* Are we inside "" or '' ? */
  in_comment = 0; /* Are we inside multi-line comment? */

  /* If the previous line has an open comment, this line starts
   * with an open comment state. */
  if (row->idx > 0 && editorRowHasOpenComment(&E.row[row->idx - 1]))
    in_comment = 1;

  while (*p) {
    /* Handle // comments. */
    if (prev_sep && *p == scs[0] && *(p + 1) == scs[1]) {
      /* From here to end is a comment */
      memset(row->hl + i, HL_COMMENT, row->size - i);
      return;
    }

    /* Handle multi line comments. */
    if (in_comment) {
      row->hl[i] = HL_MLCOMMENT;
      if (*p == mce[0] && *(p + 1) == mce[1]) {
        row->hl[i + 1] = HL_MLCOMMENT;
        p += 2;
        i += 2;
        in_comment = 0;
        prev_sep = 1;
        continue;
      } else {
        prev_sep = 0;
        p++;
        i++;
        continue;
      }
    } else if (*p == mcs[0] && *(p + 1) == mcs[1]) {
      row->hl[i] = HL_MLCOMMENT;
      row->hl[i + 1] = HL_MLCOMMENT;
      p += 2;
      i += 2;
      in_comment = 1;
      prev_sep = 0;
      continue;
    }

    /* Handle "" and '' */
    if (in_string) {
      row->hl[i] = HL_STRING;
      if (*p == '\\') {
        row->hl[i + 1] = HL_STRING;
        p += 2;
        i += 2;
        prev_sep = 0;
        continue;
      }
      if (*p == in_string) in_string = 0;
      p++;
      i++;
      continue;
    } else {
      if (*p == '"' || *p == '\'') {
        in_string = *p;
        row->hl[i] = HL_STRING;
        p++;
        i++;
        prev_sep = 0;
        continue;
      }
    }

    /* Handle non printable chars. */
    if (!isprint(*p)) {
      row->hl[i] = HL_NONPRINT;
      p++;
      i++;
      prev_sep = 0;
      continue;
    }

    /* Handle numbers */
    if ((isdigit(*p) && (prev_sep || row->hl[i - 1] == HL_NUMBER)) ||
        (*p == '.' && i > 0 && row->hl[i - 1] == HL_NUMBER)) {
      row->hl[i] = HL_NUMBER;
      p++;
      i++;
      prev_sep = 0;
      continue;
    }

    /* Handle keywords and lib calls */
    if (prev_sep) {
      int j;
      for (j = 0; keywords[j]; j++) {
        int klen = strlen(keywords[j]);
        int kw2 = keywords[j][klen - 1] == '|';
        if (kw2) klen--;

        if (!memcmp(p, keywords[j], klen) && is_separator(*(p + klen))) {
          /* Keyword */
          memset(row->hl + i, kw2 ? HL_KEYWORD2 : HL_KEYWORD1, klen);
          p += klen;
          i += klen;
          break;
        }
      }
      if (keywords[j] != NULL) {
        prev_sep = 0;
        continue; /* We had a keyword match */
      }
    }

    /* Not special chars */
    prev_sep = is_separator(*p);
    p++;
    i++;
  }

  /* Propagate syntax change to the next row if the open comment
   * state changed. This may recursively affect all the following rows
   * in the file. */
  int oc = editorRowHasOpenComment(row);
#if SYNTAX
  if (row->hl_oc != oc && row->idx + 1 < E.numrows)
    editorUpdateSyntax(&E.row[row->idx + 1]);
#endif
  row->hl_oc = oc;
}

/* Maps syntax highlight token types to terminal colors. */
int editorSyntaxToColor(int hl) {
  switch (hl) {
    case HL_COMMENT:
    case HL_MLCOMMENT:
      return 36; /* cyan */
    case HL_KEYWORD1:
      return 33; /* yellow */
    case HL_KEYWORD2:
      return 32; /* green */
    case HL_STRING:
      return 35; /* magenta */
    case HL_NUMBER:
      return 31; /* red */
    case HL_MATCH:
      return 34; /* blu */
    default:
      return 37; /* white */
  }
}

/* Select the syntax highlight scheme depending on the filename,
 * setting it in the global state E.syntax. */
void editorSelectSyntaxHighlight(char *filename) {
  for (unsigned j = 0; j < HLDB_ENTRIES; j++) {
    const struct editorSyntax *s = HLDB + j;
    unsigned i = 0;
    while (s->filematch[i]) {
      char *p;
      int patlen = strlen(s->filematch[i]);
      if ((p = strstr(filename, s->filematch[i])) != NULL) {
        if (s->filematch[i][0] != '.' || p[patlen] == '\0') {
          E.syntax = s;
          return;
        }
      }
      i++;
    }
  }
}

/* ======================= Editor rows implementation ======================= */

/* Update the rendered version and the syntax highlight of a row. */
void editorUpdateRow(erow *row) {
  int tabs = 0, nonprint = 0, j, idx;

  /* Create a version of the row we can directly print on the screen,
   * respecting tabs, substituting non printable characters with '?'. */
  free(row->render);
  for (j = 0; j < row->size; j++) {
    if (row->chars[j] == '\t') tabs++;
  }

  row->render = malloc(row->size + tabs * 8 + nonprint * 9 + 1);
  idx = 0;
  for (j = 0; j < row->size; j++) {
    if (row->chars[j] == '\t') {
      row->render[idx++] = ' ';
      while (idx % 8 != 0) {
        row->render[idx++] = ' ';
      }
    } else {
      row->render[idx++] = row->chars[j];
    }
  }
  row->rsize = idx;
  row->render[idx] = '\0';

  /* Update the syntax highlighting attributes of the row. */
#if SYNTAX
  editorUpdateSyntax(row);
#endif
}

/* Insert a row at the specified position, shifting the other rows on the bottom
 * if required. */
void editorInsertRow(int at, char *s, size_t len) {
  if (at > E.numrows) return;
  E.row = realloc(E.row, sizeof(erow) * (E.numrows + 1));
  if (at != E.numrows) {
    memmove(E.row + at + 1, E.row + at, sizeof(E.row[0]) * (E.numrows - at));
    for (int j = at + 1; j <= E.numrows; j++) E.row[j].idx++;
  }
  E.row[at].size = len;
  E.row[at].chars = malloc(len + 1);
  memcpy(E.row[at].chars, s, len + 1);
  E.row[at].hl = NULL;
  E.row[at].hl_oc = 0;
  E.row[at].render = NULL;
  E.row[at].rsize = 0;
  E.row[at].idx = at;
  editorUpdateRow(E.row + at);
  E.numrows++;
  E.dirty++;
}

/* Free row's heap allocated stuff. */
void editorFreeRow(erow *row) {
  free(row->render);
  free(row->chars);
  free(row->hl);
}

/* Remove the row at the specified position, shifting the remaining on the
 * top. */
void editorDelRow(int at) {
  erow *row;

  if (at >= E.numrows) return;
  row = E.row + at;
  editorFreeRow(row);
  memmove(E.row + at, E.row + at + 1, sizeof(E.row[0]) * (E.numrows - at - 1));
  for (int j = at; j < E.numrows - 1; j++) E.row[j].idx++;
  E.numrows--;
  E.dirty++;
}

/* Turn the editor rows into a single heap-allocated string.
 * Returns the pointer to the heap-allocated string and populate the
 * integer pointed by 'buflen' with the size of the string, excluding
 * the final nulterm. */
char *editorRowsToString(int *buflen) {
  char *buf = NULL, *p;
  int totlen = 0;
  int j;

  /* Compute count of bytes */
  for (j = 0; j < E.numrows; j++) {
    totlen += E.row[j].size + 1; /* +1 is for "\n" at end of every row */
  }
  *buflen = totlen;
  totlen++; /* Also make space for nulterm */

  p = buf = malloc(totlen);
  for (j = 0; j < E.numrows; j++) {
    memcpy(p, E.row[j].chars, E.row[j].size);
    p += E.row[j].size;
    *p = '\n';
    p++;
  }
  *p = '\0';
  return buf;
}

/* Insert a character at the specified position in a row, moving the remaining
 * chars on the right if needed. */
void editorRowInsertChar(erow *row, int at, int c) {
  if (at > row->size) {
    /* Pad the string with spaces if the insert location is outside the
     * current length by more than a single character. */
    int padlen = at - row->size;
    /* In the next line +2 means: new char and null term. */
    row->chars = realloc(row->chars, row->size + padlen + 2);
    memset(row->chars + row->size, ' ', padlen);
    row->chars[row->size + padlen + 1] = '\0';
    row->size += padlen + 1;
  } else {
    /* If we are in the middle of the string just make space for 1 new
     * char plus the (already existing) null term. */
    row->chars = realloc(row->chars, row->size + 2);
    memmove(row->chars + at + 1, row->chars + at, row->size - at + 1);
    row->size++;
  }
  row->chars[at] = c;
  editorUpdateRow(row);
  E.dirty++;
}

/* Append the string 's' at the end of a row */
void editorRowAppendString(erow *row, char *s, size_t len) {
  row->chars = realloc(row->chars, row->size + len + 1);
  memcpy(row->chars + row->size, s, len);
  row->size += len;
  row->chars[row->size] = '\0';
  editorUpdateRow(row);
  E.dirty++;
}

/* Delete the character at offset 'at' from the specified row. */
void editorRowDelChar(erow *row, int at) {
  if (row->size <= at) return;
  memmove(row->chars + at, row->chars + at + 1, row->size - at);
  editorUpdateRow(row);
  row->size--;
  E.dirty++;
}

/* Insert the specified char at the current prompt position. */
void editorInsertChar(int c) {
  int filerow = E.rowoff + E.cy;
  int filecol = E.coloff + E.cx;
  erow *row = (filerow >= E.numrows) ? NULL : &E.row[filerow];

  /* If the row where the cursor is currently located does not exist in our
   * logical representation of the file, add enough empty rows as needed. */
  if (!row) {
    while (E.numrows <= filerow) editorInsertRow(E.numrows, "", 0);
  }
  row = &E.row[filerow];
  editorRowInsertChar(row, filecol, c);
  if (E.cx == E.screencols - 1) {
    E.coloff++;
  } else {
    E.cx++;
  }
  E.dirty++;
}

/* Inserting a newline is slightly complex as we have to handle inserting a
 * newline in the middle of a line, splitting the line as needed. */
void editorInsertNewline(void) {
  int filerow = E.rowoff + E.cy;
  int filecol = E.coloff + E.cx;
  erow *row = (filerow >= E.numrows) ? NULL : &E.row[filerow];

  if (!row) {
    if (filerow == E.numrows) {
      editorInsertRow(filerow, "", 0);
      goto fixcursor;
    }
    return;
  }
  /* If the cursor is over the current line size, we want to conceptually
   * think it's just over the last character. */
  if (filecol >= row->size) filecol = row->size;
  if (filecol == 0) {
    editorInsertRow(filerow, "", 0);
  } else {
    /* We are in the middle of a line. Split it between two rows. */
    editorInsertRow(filerow + 1, row->chars + filecol, row->size - filecol);
    row = &E.row[filerow];
    row->chars[filecol] = '\0';
    row->size = filecol;
    editorUpdateRow(row);
  }
fixcursor:
  if (E.cy == E.screenrows - 1) {
    E.rowoff++;
  } else {
    E.cy++;
  }
  E.cx = 0;
  E.coloff = 0;
}

/* Delete the char at the current prompt position. */
void editorDelChar(void) {
  int filerow = E.rowoff + E.cy;
  int filecol = E.coloff + E.cx;
  erow *row = (filerow >= E.numrows) ? NULL : &E.row[filerow];

  if (!row || (filecol == 0 && filerow == 0)) return;
  if (filecol == 0) {
    /* Handle the case of column 0, we need to move the current line
     * on the right of the previous one. */
    filecol = E.row[filerow - 1].size;
    editorRowAppendString(&E.row[filerow - 1], row->chars, row->size);
    editorDelRow(filerow);
    row = NULL;
    if (E.cy == 0)
      E.rowoff--;
    else
      E.cy--;
    E.cx = filecol;
    if (E.cx >= E.screencols) {
      int shift = (E.screencols - E.cx) + 1;
      E.cx -= shift;
      E.coloff += shift;
    }
  } else {
    editorRowDelChar(row, filecol - 1);
    if (E.cx == 0 && E.coloff)
      E.coloff--;
    else
      E.cx--;
  }
  if (row) editorUpdateRow(row);
  E.dirty++;
}

/* Load the specified program in the editor memory and returns 0 on success
 * or 1 on error. */
int editorOpen(char *filename) {
  FILE *fp;

  E.dirty = 0;
  free(E.filename);
  E.filename = strdup(filename);

  fp = fopen(filename, "r");
  if (!fp) {
    if (errno != ENOENT) {
      perror("Opening file");
      exit(1);
    }
    return 1;
  }

  char *line = NULL;
  size_t linecap = 0;
  ssize_t linelen;
  while ((linelen = getline(&line, &linecap, fp)) != -1) {
    if (linelen && (line[linelen - 1] == '\n' || line[linelen - 1] == '\r'))
      line[--linelen] = '\0';
    editorInsertRow(E.numrows, line, linelen);
  }
  free(line);
  fclose(fp);
  E.dirty = 0;
  return 0;
}

#define UNSAFE_SAVES 1

/* Save the current file on disk. Return 0 on success, 1 on error. */
int editorSave(void) {
  int len;
  char *buf = editorRowsToString(&len);
  int64_t fd = open(E.filename, O_RDWR | O_CREAT, 0644);
  if (fd == -1) goto writeerr;

  /* Use truncate + a single write(2) call in order to make saving
   * a bit safer, under the limits of what we can do in a small editor. */
  if (ftruncate(fd, len) == -1) goto writeerr;
  if (write(fd, buf, len) != len) goto writeerr;

  close(fd);
  free(buf);
  E.dirty = 0;
  editorSetStatusMessage("%d bytes written on disk", len);
  return 0;

writeerr:
  free(buf);
  if (fd != -1) close(fd);
  editorSetStatusMessage("Can't save! I/O error: %s", strerror(errno));
  return 1;
}

/* ============================= Terminal update ============================ */

struct abuf {
  size_t i, n;
  char *p;
};

static void abAppend(struct abuf *ab, const char *s, int len) {
  CONCAT(&ab->p, &ab->i, &ab->n, (void *)s, len);
}

/* This function writes the whole screen using VT100 escape characters
 * starting from the logical state of the editor in the global state 'E'. */
void editorRefreshScreen(void) {
  int y;
  erow *r;
  char buf[32];
  struct abuf ab;

  memset(&ab, 0, sizeof(ab));
  abAppend(&ab, "\e[?25l", 6); /* Hide cursor. */
  abAppend(&ab, "\e[H", 3);    /* Go home. */
  for (y = 0; y < E.screenrows; y++) {
    int filerow = E.rowoff + y;

    if (filerow >= E.numrows) {
      if (E.numrows == 0 && y == E.screenrows / 3) {
        char welcome[80];
        int welcomelen =
            snprintf(welcome, sizeof(welcome),
                     "Kilo editor -- version %s\e[0K\r\n", KILO_VERSION);
        int padding = (E.screencols - welcomelen) / 2;
        if (padding) {
          abAppend(&ab, "~", 1);
          padding--;
        }
        while (padding--) abAppend(&ab, " ", 1);
        abAppend(&ab, welcome, welcomelen);
      } else {
        abAppend(&ab, "~\e[0K\r\n", 7);
      }
      continue;
    }

    r = &E.row[filerow];

    int len = r->rsize - E.coloff;
#if SYNTAX
    int current_color = -1;
#endif
    if (len > 0) {
      if (len > E.screencols) len = E.screencols;
      char *c = r->render + E.coloff;
#if SYNTAX
      unsigned char *hl = r->hl + E.coloff;
#endif
      int j;
      for (j = 0; j < len; j++) {
#if SYNTAX
        if (hl[j] == HL_NONPRINT) {
          char sym;
          abAppend(&ab, "\e[7m", 4);
          if (c[j] <= 26)
            sym = '@' + c[j];
          else
            sym = '?';
          abAppend(&ab, &sym, 1);
          abAppend(&ab, "\e[0m", 4);
        } else if (hl[j] == HL_NORMAL) {
          if (current_color != -1) {
            abAppend(&ab, "\e[39m", 5);
            current_color = -1;
          }
#endif
          abAppend(&ab, c + j, 1);
#if SYNTAX
        } else {
          int color = editorSyntaxToColor(hl[j]);
          if (color != current_color) {
            char buf_[16];
            int clen = snprintf(buf_, sizeof(buf_), "\e[%dm", color);
            current_color = color;
            abAppend(&ab, buf_, clen);
          }
          abAppend(&ab, c + j, 1);
        }
#endif
      }
    }
    abAppend(&ab, "\e[39m", 5);
    abAppend(&ab, "\e[0K", 4);
    abAppend(&ab, "\r\n", 2);
  }

  /* Create a two rows status. First row: */
  abAppend(&ab, "\e[0K", 4);
  abAppend(&ab, "\e[7m", 4);
  char status[80], rstatus[80];
  int len = snprintf(status, sizeof(status), "%.20s - %d lines %s", E.filename,
                     E.numrows, E.dirty ? "(modified)" : "");
  int rlen = snprintf(rstatus, sizeof(rstatus), "%d/%d", E.rowoff + E.cy + 1,
                      E.numrows);
  if (len > E.screencols) len = E.screencols;
  abAppend(&ab, status, len);
  while (len < E.screencols) {
    if (E.screencols - len == rlen) {
      abAppend(&ab, rstatus, rlen);
      break;
    } else {
      abAppend(&ab, " ", 1);
      len++;
    }
  }
  abAppend(&ab, "\e[0m\r\n", 6);

  /* Second row depends on E.statusmsg and the status message update time. */
  abAppend(&ab, "\e[0K", 4);
  int msglen = strlen(E.statusmsg);
  if (msglen && time(NULL) - E.statusmsg_time < 5)
    abAppend(&ab, E.statusmsg, msglen <= E.screencols ? msglen : E.screencols);

  /* Put cursor at its current position. Note that the horizontal position
   * at which the cursor is displayed may be different compared to 'E.cx'
   * because of TABs. */
  int j;
  int cx = 1;
  int filerow = E.rowoff + E.cy;
  erow *row = (filerow >= E.numrows) ? NULL : &E.row[filerow];
  if (row) {
    for (j = E.coloff; j < (E.cx + E.coloff); j++) {
      if (j < row->size && row->chars[j] == CTRL('I')) cx += 7 - ((cx) % 8);
      cx++;
    }
  }
  snprintf(buf, sizeof(buf), "\e[%d;%dH", E.cy + 1, cx);
  abAppend(&ab, buf, strlen(buf));
  abAppend(&ab, "\e[?25h", 6); /* Show cursor. */
  write(STDOUT_FILENO, ab.p, ab.i);
  free(ab.p);
}

/* Set an editor status message for the second line of the status, at the
 * end of the screen. */
void editorSetStatusMessage(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(E.statusmsg, sizeof(E.statusmsg), fmt, ap);
  va_end(ap);
  E.statusmsg_time = time(NULL);
}

/* =============================== Find mode ================================ */

#define KILO_QUERY_LEN 256

void editorFind(int64_t fd) {
  char query[KILO_QUERY_LEN + 1] = {0};
  int qlen = 0;
  int last_match = -1;    /* Last line where a match was found. -1 for none. */
  int find_next = 0;      /* if 1 search next, if -1 search prev. */
  int saved_hl_line = -1; /* No saved HL */
  char *saved_hl = NULL;

#define FIND_RESTORE_HL                                                      \
  do {                                                                       \
    if (saved_hl) {                                                          \
      memcpy(E.row[saved_hl_line].hl, saved_hl, E.row[saved_hl_line].rsize); \
      saved_hl = NULL;                                                       \
    }                                                                        \
  } while (0)

  /* Save the cursor position in order to restore it later. */
  int saved_cx = E.cx, saved_cy = E.cy;
  int saved_coloff = E.coloff, saved_rowoff = E.rowoff;

  while (1) {
    editorSetStatusMessage("Search: %s (Use ESC/Arrows/Enter)", query);
    editorRefreshScreen();

    int c = editorReadKey(fd);
    if (c == DEL_KEY || c == CTRL('H') || c == CTRL('?')) {
      if (qlen != 0) query[--qlen] = '\0';
      last_match = -1;
    } else if (c == CTRL('G')) {
      break;
    } else if (c == CTRL('[') || c == CTRL('M')) {
      if (c == CTRL('[')) {
        E.cx = saved_cx;
        E.cy = saved_cy;
        E.coloff = saved_coloff;
        E.rowoff = saved_rowoff;
      }
      FIND_RESTORE_HL;
      editorSetStatusMessage("");
      return;
    } else if (c == ARROW_RIGHT || c == ARROW_DOWN) {
      find_next = 1;
    } else if (c == ARROW_LEFT || c == ARROW_UP) {
      find_next = -1;
    } else if (isprint(c)) {
      if (qlen < KILO_QUERY_LEN) {
        query[qlen++] = c;
        query[qlen] = '\0';
        last_match = -1;
      }
    }

    /* Search occurrence. */
    if (last_match == -1) find_next = 1;
    if (find_next) {
      char *match = NULL;
      int match_offset = 0;
      int i, current = last_match;

      for (i = 0; i < E.numrows; i++) {
        current += find_next;
        if (current == -1)
          current = E.numrows - 1;
        else if (current == E.numrows)
          current = 0;
        match = strstr(E.row[current].render, query);
        if (match) {
          match_offset = match - E.row[current].render;
          break;
        }
      }
      find_next = 0;

      /* Highlight */
      FIND_RESTORE_HL;

      if (match) {
        erow *row = &E.row[current];
        last_match = current;
        if (row->hl) {
          saved_hl_line = current;
          saved_hl = malloc(row->rsize);
          memcpy(saved_hl, row->hl, row->rsize);
          memset(row->hl + match_offset, HL_MATCH, qlen);
        }
        E.cy = 0;
        E.cx = match_offset;
        E.rowoff = current;
        E.coloff = 0;
        /* Scroll horizontally as needed. */
        if (E.cx > E.screencols) {
          int diff = E.cx - E.screencols;
          E.cx -= diff;
          E.coloff += diff;
        }
      }
    }
  }
}

/* ========================= Editor events handling  ======================== */

/* Handle cursor position change because arrow keys were pressed. */
void editorMoveCursor(int key) {
  int filerow = E.rowoff + E.cy;
  int filecol = E.coloff + E.cx;
  int rowlen;
  erow *row = (filerow >= E.numrows) ? NULL : &E.row[filerow];

  switch (key) {
    case ARROW_LEFT:
      if (E.cx == 0) {
        if (E.coloff) {
          E.coloff--;
        } else {
          if (filerow > 0) {
            E.cy--;
            E.cx = E.row[filerow - 1].size;
            if (E.cx > E.screencols - 1) {
              E.coloff = E.cx - E.screencols + 1;
              E.cx = E.screencols - 1;
            }
          }
        }
      } else {
        E.cx -= 1;
      }
      break;
    case ARROW_RIGHT:
      if (row && filecol < row->size) {
        if (E.cx == E.screencols - 1) {
          E.coloff++;
        } else {
          E.cx += 1;
        }
      } else if (row && filecol == row->size) {
        E.cx = 0;
        E.coloff = 0;
        if (E.cy == E.screenrows - 1) {
          E.rowoff++;
        } else {
          E.cy += 1;
        }
      }
      break;
    case ARROW_UP:
      if (E.cy == 0) {
        if (E.rowoff) E.rowoff--;
      } else {
        E.cy -= 1;
      }
      break;
    case ARROW_DOWN:
      if (filerow < E.numrows) {
        if (E.cy == E.screenrows - 1) {
          E.rowoff++;
        } else {
          E.cy += 1;
        }
      }
      break;
  }
  /* Fix cx if the current line has not enough chars. */
  filerow = E.rowoff + E.cy;
  filecol = E.coloff + E.cx;
  row = (filerow >= E.numrows) ? NULL : &E.row[filerow];
  rowlen = row ? row->size : 0;
  if (filecol > rowlen) {
    E.cx -= filecol - rowlen;
    if (E.cx < 0) {
      E.coloff += E.cx;
      E.cx = 0;
    }
  }
}

/* Process events arriving from the standard input, which is, the user
 * is typing stuff on the terminal. */
#define KILO_QUIT_TIMES 3
void editorProcessKeypress(int64_t fd) {
  /* When the file is modified, requires Ctrl-q to be pressed N times
   * before actually quitting. */
  static int quit_times;
  int c, c2, times, oldcx;

  c = editorReadKey(fd);
  switch (c) {
    case CTRL('M'): /* Enter */
      editorInsertNewline();
      break;
    case CTRL('C'): /* Ctrl-c */
      /* We ignore ctrl-c, it can't be so simple to lose the changes
       * to the edited file. */
      break;
    case CTRL('Q'): /* Ctrl-q */
      /* Quit if the file was already saved. */
      if (E.dirty && quit_times < KILO_QUIT_TIMES) {
        editorSetStatusMessage("WARNING!!! File has unsaved changes. "
                               "Press Ctrl-Q %d more times to quit.",
                               KILO_QUIT_TIMES - quit_times);
        quit_times++;
        return;
      }
      exit(0);
      break;

    case CTRL('U'):
    case CTRL('X'): {
      c2 = editorReadKey(fd);
      switch (c2) {
        case CTRL('S'):
          editorSave();
          break;
        case CTRL('C'): {
          /* write(STDOUT_FILENO, "\r\e[0J", 5); */
          exit(0);
          break;
        }
        default: /* ignore */
          break;
      }
      break;
    }

    case CTRL('S'):
      editorFind(fd);
      break;
    case CTRL('D'): /* Delete */
    case DEL_KEY:
      editorMoveCursor(ARROW_RIGHT);
    case CTRL('?'): /* Backspace */
    case CTRL('H'):
      editorDelChar();
      break;

    case CTRL('K'): { /* Kill Line */
      oldcx = E.cx;
      do {
        editorMoveCursor(ARROW_RIGHT);
      } while (E.cx);
      editorMoveCursor(ARROW_LEFT);
      if (E.cx) {
        /* non-empty line: preserve row */
        while (E.cx > oldcx) {
          editorDelChar();
        }
      } else {
        /* empty line: remove row */
        editorMoveCursor(ARROW_RIGHT);
        editorDelChar();
      }
      break;
    }

    case CTRL('L'):
      times = E.screenrows / 2;
      while (times--) editorMoveCursor(c == PAGE_UP ? ARROW_UP : ARROW_DOWN);
      times = E.screenrows / 2;
      while (times--) editorMoveCursor(c == PAGE_UP ? ARROW_DOWN : ARROW_UP);
      break;

    case PAGE_UP:
    case PAGE_DOWN:
      if (c == PAGE_UP && E.cy != 0) {
        E.cy = 0;
      } else if (c == PAGE_DOWN && E.cy != E.screenrows - 1) {
        E.cy = E.screenrows - 1;
      }
      times = E.screenrows;
      while (times--) editorMoveCursor(c == PAGE_UP ? ARROW_UP : ARROW_DOWN);
      times = E.screenrows / 2;
      while (times--) editorMoveCursor(c == PAGE_UP ? ARROW_DOWN : ARROW_UP);
      break;

    case HOME_KEY:
    case CTRL('A'):
      while (E.cx || E.coloff) editorMoveCursor(ARROW_LEFT);
      break;
    case END_KEY:
    case CTRL('E'):
      do {
        editorMoveCursor(ARROW_RIGHT);
      } while (E.cx);
      editorMoveCursor(ARROW_LEFT);
      break;

    case CTRL('P'):
      editorMoveCursor(ARROW_UP);
      break;
    case CTRL('N'):
      editorMoveCursor(ARROW_DOWN);
      break;
    case CTRL('B'):
      editorMoveCursor(ARROW_LEFT);
      break;
    case CTRL('F'):
      editorMoveCursor(ARROW_RIGHT);
      break;

    case ARROW_UP:
    case ARROW_DOWN:
    case ARROW_LEFT:
    case ARROW_RIGHT:
      editorMoveCursor(c);
      break;

    case CTRL('['):
      /* Nothing to do for ESC in this mode. */
      break;
    default:
      editorInsertChar(c);
      break;
  }

  quit_times = 0; /* Reset it to the original value. */
}

int editorFileWasModified(void) {
  return E.dirty;
}

void initEditor(void) {
  E.cx = 0;
  E.cy = 0;
  E.rowoff = 0;
  E.coloff = 0;
  E.numrows = 0;
  E.row = NULL;
  E.dirty = 0;
  E.filename = NULL;
  E.syntax = NULL;
  if (getWindowSize(STDIN_FILENO, STDOUT_FILENO, &E.screenrows,
                    &E.screencols) == -1) {
    perror("Unable to query the screen for size (columns / rows)");
    exit(1);
  }
  E.screenrows -= 2; /* Get room for status bar. */
}

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "Usage: kilo <filename>\n");
    exit(1);
  }

  initEditor();
  editorSelectSyntaxHighlight(argv[1]);
  editorOpen(argv[1]);
  enableRawMode(STDIN_FILENO);
  editorSetStatusMessage("HELP: Ctrl-S = save | Ctrl-Q = quit | Ctrl-F = find");
  while (1) {
    editorRefreshScreen();
    editorProcessKeypress(STDIN_FILENO);
  }
  return 0;
}
