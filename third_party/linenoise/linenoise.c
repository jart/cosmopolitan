/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│                                                                              │
│ Cosmopolitan Linenoise ── guerrilla line editing library against the         │
│ notion that a library for user-friendly pseudoteletypewriter command         │
│ sessions using ANSI standard X3.64 control sequences must have 100k+         │
│ lines of GPL workarounds to 300 baud and bygone commercial rivalries         │
│                                                                              │
│ CHANGES                                                                      │
│                                                                              │
│   - Remove bell                                                              │
│   - Add kill ring                                                            │
│   - Add UTF-8 editing                                                        │
│   - Add CTRL-R search                                                        │
│   - React to terminal resizing                                               │
│   - Don't generate .data section                                             │
│   - Support terminal flow control                                            │
│   - Make history loading 10x faster                                          │
│   - Make multiline mode the only mode                                        │
│   - Restore raw mode on process foregrounding                                │
│   - Make source code compatible with C++ compilers                           │
│   - Fix corruption issues by using generalized parsing                       │
│   - Implement nearly all GNU readline editing shortcuts                      │
│   - Remove heavyweight dependencies like printf/sprintf                      │
│   - Remove ISIG→^C→EAGAIN hack and catch signals properly                    │
│   - Support running on Windows in MinTTY or CMD.EXE on Win10+                │
│   - Support diacratics, русский, Ελληνικά, 中国人, 한국인, 日本              │
│                                                                              │
│ SHORTCUTS                                                                    │
│                                                                              │
│   CTRL-E         END                                                         │
│   CTRL-A         START                                                       │
│   CTRL-B         BACK                                                        │
│   CTRL-F         FORWARD                                                     │
│   CTRL-L         CLEAR                                                       │
│   CTRL-H         BACKSPACE                                                   │
│   CTRL-D         DELETE                                                      │
│   CTRL-D         EOF (IF EMPTY)                                              │
│   CTRL-N         NEXT HISTORY                                                │
│   CTRL-P         PREVIOUS HISTORY                                            │
│   CTRL-R         SEARCH HISTORY                                              │
│   CTRL-G         CANCEL SEARCH                                               │
│   ALT-<          BEGINNING OF HISTORY                                        │
│   ALT->          END OF HISTORY                                              │
│   ALT-F          FORWARD WORD                                                │
│   ALT-B          BACKWARD WORD                                               │
│   CTRL-K         KILL LINE FORWARDS                                          │
│   CTRL-U         KILL LINE BACKWARDS                                         │
│   ALT-H          KILL WORD BACKWARDS                                         │
│   CTRL-W         KILL WORD BACKWARDS                                         │
│   CTRL-ALT-H     KILL WORD BACKWARDS                                         │
│   ALT-D          KILL WORD FORWARDS                                          │
│   CTRL-Y         YANK                                                        │
│   ALT-Y          ROTATE KILL RING AND YANK AGAIN                             │
│   CTRL-T         TRANSPOSE                                                   │
│   ALT-T          TRANSPOSE WORD                                              │
│   ALT-U          UPPERCASE WORD                                              │
│   ALT-L          LOWERCASE WORD                                              │
│   ALT-C          CAPITALIZE WORD                                             │
│   CTRL-SPACE     SET MARK                                                    │
│   CTRL-X CTRL-X  GOTO MARK                                                   │
│   CTRL-S         PAUSE OUTPUT                                                │
│   CTRL-Q         RESUME OUTPUT                                               │
│   CTRL-Z         SUSPEND PROCESS                                             │
│                                                                              │
│ EXAMPLE                                                                      │
│                                                                              │
│   // should be ~80kb statically linked                                       │
│   // will save history to ~/.foo_history                                     │
│   // cc -fno-jump-tables -Os -o foo foo.c linenoise.c                        │
│   main() {                                                                   │
│       char *line;                                                            │
│       while ((line = ezlinenoise("IN> ", "foo"))) {                          │
│           fputs("OUT> ", stdout);                                            │
│           fputs(line, stdout);                                               │
│           fputs("\n", stdout);                                               │
│           free(line);                                                        │
│       }                                                                      │
│   }                                                                          │
│                                                                              │
╞══════════════════════════════════════════════════════════════════════════════╡
│                                                                              │
│ Copyright 2018-2021 Justine Tunney <jtunney@gmail.com>                       │
│ Copyright 2010-2016 Salvatore Sanfilippo <antirez@gmail.com>                 │
│ Copyright 2010-2013 Pieter Noordhuis <pcnoordhuis@gmail.com>                 │
│                                                                              │
│ All rights reserved.                                                         │
│                                                                              │
│ Redistribution and use in source and binary forms, with or without           │
│ modification, are permitted provided that the following conditions are       │
│ met:                                                                         │
│                                                                              │
│  *  Redistributions of source code must retain the above copyright           │
│     notice, this list of conditions and the following disclaimer.            │
│                                                                              │
│  *  Redistributions in binary form must reproduce the above copyright        │
│     notice, this list of conditions and the following disclaimer in the      │
│     documentation and/or other materials provided with the distribution.     │
│                                                                              │
│ THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS          │
│ "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT            │
│ LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR        │
│ A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT         │
│ HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,       │
│ SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT             │
│ LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,        │
│ DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY        │
│ THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT          │
│ (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE        │
│ OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.         │
│                                                                              │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/assert.h"
#include "libc/bits/bits.h"
#include "libc/calls/calls.h"
#include "libc/calls/sigbits.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/termios.h"
#include "libc/calls/ttydefaults.h"
#include "libc/calls/weirdtypes.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/bsr.h"
#include "libc/nexgen32e/rdtsc.h"
#include "libc/sock/sock.h"
#include "libc/stdio/append.internal.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/str/tpenc.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/poll.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sig.h"
#include "third_party/linenoise/linenoise.h"
#include "tool/build/lib/case.h"

asm(".ident\t\"\\n\\n\
Cosmopolitan Linenoise (BSD-2)\\n\
Copyright 2018-2020 Justine Tunney <jtunney@gmail.com>\\n\
Copyright 2010-2016 Salvatore Sanfilippo <antirez@gmail.com>\\n\
Copyright 2010-2013 Pieter Noordhuis <pcnoordhuis@gmail.com>\"");

#define LINENOISE_MAX_RING    8
#define LINENOISE_MAX_DEBUG   16
#define LINENOISE_MAX_HISTORY 1024
#define LINENOISE_MAX_LINE    4096

#define LINENOISE_HISTORY_FIRST +LINENOISE_MAX_HISTORY
#define LINENOISE_HISTORY_PREV  +1
#define LINENOISE_HISTORY_NEXT  -1
#define LINENOISE_HISTORY_LAST  -LINENOISE_MAX_HISTORY

#if 0
#define DEBUG(L, ...) linenoiseDebug(L, __VA_ARGS__)
#else
#define DEBUG(L, ...) (void)0
#endif

struct abuf {
  char *b;
  int len;
};

struct rune {
  unsigned c;
  unsigned n;
};

struct linenoiseRing {
  unsigned i;
  char *p[LINENOISE_MAX_RING];
};

/* The linenoiseState structure represents the state during line editing.
 * We pass this state to functions implementing specific editing
 * functionalities. */
struct linenoiseState {
  int ifd;            /* Terminal stdin file descriptor */
  int ofd;            /* Terminal stdout file descriptor */
  struct winsize ws;  /* Rows and columns in terminal */
  char *buf;          /* Edited line buffer */
  const char *prompt; /* Prompt to display */
  int hindex;         /* history index */
  unsigned buflen;    /* Edited line buffer size */
  unsigned pos;       /* Current cursor position */
  unsigned oldpos;    /* Previous refresh cursor position */
  unsigned len;       /* Current edited line length */
  unsigned maxrows;   /* Maximum num of rows used so far */
  unsigned mark;      /* Saved cursor position */
  unsigned yi, yj;    /* Boundaries of last yank */
  unsigned debugrow;  /* Debug log row offset */
  char seq[2][16];    /* Keystroke history for yanking code */
};

static const char *const kUnsupported[] = {"dumb", "cons25", "emacs"};

static jmp_buf jraw;
static char rawmode;
static char gotcont;
static char gotwinch;
static char maskmode;
static char iscapital;
static int historylen;
static struct linenoiseRing ring;
static struct sigaction orig_int;
static struct sigaction orig_quit;
static struct sigaction orig_cont;
static struct sigaction orig_winch;
static struct termios orig_termios;
static char *history[LINENOISE_MAX_HISTORY];
static linenoiseHintsCallback *hintsCallback;
static linenoiseFreeHintsCallback *freeHintsCallback;
static linenoiseCompletionCallback *completionCallback;

static void linenoiseAtExit(void);
static void linenoiseRefreshLine(struct linenoiseState *);

static int notwseparator(wint_t c) {
  return !iswseparator(c);
}

static wint_t Capitalize(wint_t c) {
  if (!iscapital) {
    c = towupper(c);
    iscapital = 1;
  }
  return c;
}

static struct rune DecodeUtf8(int c) {
  struct rune r;
  if (c < 252) {
    r.n = bsr(255 & ~c);
    r.c = c & (((1 << r.n) - 1) | 3);
    r.n = 6 - r.n;
  } else {
    r.c = c & 3;
    r.n = 5;
  }
  return r;
}

static size_t GetFdSize(int fd) {
  struct stat st;
  st.st_size = 0;
  fstat(fd, &st);
  return st.st_size;
}

static char *GetLine(FILE *f) {
  ssize_t rc;
  char *p = 0;
  size_t n, c = 0;
  if ((rc = getdelim(&p, &c, '\n', f)) != EOF) {
    for (n = rc; n; --n) {
      if (p[n - 1] == '\r' || p[n - 1] == '\n') {
        p[n - 1] = 0;
      } else {
        break;
      }
    }
    return p;
  } else {
    free(p);
    return 0;
  }
}

static const char *FindSubstringReverse(const char *p, size_t n, const char *q,
                                        size_t m) {
  size_t i;
  if (m <= n) {
    for (n -= m; n; --n) {
      for (i = 0; i < m; ++i) {
        if (p[n + i] != q[i]) {
          break;
        }
      }
      if (i == m) {
        return p + n;
      }
    }
  }
  return 0;
}

static int ParseUnsigned(const char *s, void *e) {
  int c, x;
  for (x = 0; (c = *s++);) {
    if ('0' <= c && c <= '9') {
      x = MIN(c - '0' + x * 10, 32767);
    } else {
      break;
    }
  }
  if (e) *(const char **)e = s;
  return x;
}

static char *FormatUnsigned(char *p, unsigned x) {
  char t;
  size_t i, a, b;
  i = 0;
  do {
    p[i++] = x % 10 + '0';
    x = x / 10;
  } while (x > 0);
  p[i] = '\0';
  if (i) {
    for (a = 0, b = i - 1; a < b; ++a, --b) {
      t = p[a];
      p[a] = p[b];
      p[b] = t;
    }
  }
  return p + i;
}

static size_t GetMonospaceWidth(const char *p, size_t n) {
  int c;
  size_t i, w;
  struct rune r;
  enum { kAscii, kUtf8, kEsc, kCsi1, kCsi2, kSs, kNf, kStr, kStr2 } t;
  for (r.c = 0, r.n = 0, t = kAscii, w = i = 0; i < n; ++i) {
    c = p[i] & 255;
    switch (t) {
    Whoopsie:
      t = kAscii;
        /* fallthrough */
      case kAscii:
        if (c < 0200) {
          if (c == 033) {
            t = kEsc;
          } else {
            ++w;
          }
        } else if (c >= 0300) {
          t = kUtf8;
          r = DecodeUtf8(c);
        }
        break;
      case kUtf8:
        if ((c & 0300) == 0200) {
          r.c <<= 6;
          r.c |= c & 077;
          if (!--r.n) {
            switch (r.c) {
              case 033:
                t = kEsc;
                break;
              case 0x9b:
                t = kCsi1;
                break;
              case 0x8e:
              case 0x8f:
                t = kSs;
                break;
              case 0x90:
              case 0x98:
              case 0x9d:
              case 0x9e:
              case 0x9f:
                t = kStr;
                break;
              default:
                w += wcwidth(r.c);
                t = kAscii;
                break;
            }
          }
        } else {
          goto Whoopsie;
        }
        break;
      case kEsc:
        if (0x20 <= c && c <= 0x2f) {
          t = kNf;
        } else if (0x30 <= c && c <= 0x3f) {
          t = kAscii;
        } else if (0x20 <= c && c <= 0x5F) {
          switch (c) {
            case '[':
              t = kCsi1;
              break;
            case 'N':
            case 'O':
              t = kSs;
              break;
            case 'P':
            case 'X':
            case ']':
            case '^':
            case '_':
              t = kStr;
              break;
            case '\\':
              goto Whoopsie;
            default:
              t = kAscii;
              break;
          }
        } else if (0x60 <= c && c <= 0x7e) {
          t = kAscii;
        } else if (c == 033) {
          if (i == 3) t = kAscii;
        } else {
          t = kAscii;
        }
        break;
      case kSs:
        t = kAscii;
        break;
      case kNf:
        if (0x30 <= c && c <= 0x7e) {
          t = kAscii;
        } else if (!(0x20 <= c && c <= 0x2f)) {
          goto Whoopsie;
        }
        break;
      case kCsi1:
        if (0x20 <= c && c <= 0x2f) {
          t = kCsi2;
        } else if (c == '[' && i == 3) {
          /* linux function keys */
        } else if (0x40 <= c && c <= 0x7e) {
          t = kAscii;
        } else if (!(0x30 <= c && c <= 0x3f)) {
          goto Whoopsie;
        }
        break;
      case kCsi2:
        if (0x40 <= c && c <= 0x7e) {
          t = kAscii;
        } else if (!(0x20 <= c && c <= 0x2f)) {
          goto Whoopsie;
        }
        break;
      case kStr:
        switch (c) {
          case '\a':
            t = kAscii;
            break;
          case 0033:
          case 0302:
            t = kStr2;
            break;
          default:
            break;
        }
        break;
      case kStr2:
        switch (c) {
          case '\a':
          case '\\':
          case 0234:
            t = kAscii;
            break;
          default:
            t = kStr;
            break;
        }
        break;
      default:
        assert(0);
    }
  }
  return w;
}

static void abInit(struct abuf *ab) {
  ab->b = malloc(1);
  ab->len = 0;
  ab->b[0] = 0;
}

static void abAppend(struct abuf *ab, const char *s, int len) {
  char *p;
  if (!(p = realloc(ab->b, ab->len + len + 1))) return;
  memcpy(p + ab->len, s, len);
  p[ab->len + len] = 0;
  ab->b = p;
  ab->len += len;
}

static void abAppends(struct abuf *ab, const char *s) {
  abAppend(ab, s, strlen(s));
}

static void abAppendu(struct abuf *ab, unsigned u) {
  char b[11];
  abAppend(ab, b, FormatUnsigned(b, u) - b);
}

static void abAppendw(struct abuf *ab, unsigned long long w) {
  char b[8];
  unsigned n = 0;
  do b[n++] = w;
  while ((w >>= 8));
  abAppend(ab, b, n);
}

static void abFree(struct abuf *ab) {
  free(ab->b);
}

/**
 * Enables "mask mode".
 *
 * When it is enabled, instead of the input that the user is typing, the
 * terminal will just display a corresponding number of asterisks, like
 * "****". This is useful for passwords and other secrets that should
 * not be displayed.
 */
void linenoiseMaskModeEnable(void) {
  maskmode = 1;
}

void linenoiseMaskModeDisable(void) {
  maskmode = 0;
}

static void linenoiseOnCont(int sig) {
  gotcont = 1;
}

static void linenoiseOnWinch(int sig) {
  gotwinch = 1;
}

static void linenoiseOnInt(int sig) {
  longjmp(jraw, sig);
}

static void linenoiseOnQuit(int sig) {
  longjmp(jraw, sig);
}

static int linenoiseIsUnsupportedTerm(void) {
  int i;
  char *term;
  static char once, res;
  if (!once) {
    if ((term = getenv("TERM"))) {
      for (i = 0; i < sizeof(kUnsupported) / sizeof(*kUnsupported); i++) {
        if (!strcasecmp(term, kUnsupported[i])) {
          res = 1;
          break;
        }
      }
    }
    once = 1;
  }
  return res;
}

static int enableRawMode(int fd) {
  struct termios raw;
  struct sigaction sa;
  if (tcgetattr(fd, &orig_termios) != -1) {
    raw = orig_termios;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN);
    raw.c_oflag &= ~OPOST;
    raw.c_iflag |= IUTF8;
    raw.c_cflag |= CS8;
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;
    if (tcsetattr(fd, TCSANOW, &raw) != -1) {
      sa.sa_flags = 0;
      sa.sa_handler = linenoiseOnCont;
      sigemptyset(&sa.sa_mask);
      sigaction(SIGCONT, &sa, &orig_cont);
      sa.sa_handler = linenoiseOnWinch;
      sigaction(SIGWINCH, &sa, &orig_winch);
      rawmode = fd;
      gotwinch = 0;
      gotcont = 0;
      return 0;
    }
  }
  errno = ENOTTY;
  return -1;
}

void linenoiseDisableRawMode(void) {
  if (rawmode != -1) {
    sigaction(SIGCONT, &orig_cont, 0);
    sigaction(SIGWINCH, &orig_winch, 0);
    tcsetattr(rawmode, TCSAFLUSH, &orig_termios);
    rawmode = -1;
  }
}

static int linenoiseWrite(int fd, const void *p, size_t n) {
  ssize_t rc;
  size_t wrote;
  do {
    for (;;) {
      rc = write(fd, p, n);
      if (rc == -1 && errno == EINTR) {
        continue;
      } else {
        break;
      }
    }
    if (rc != -1) {
      wrote = rc;
      n -= wrote;
      p = (char *)p + wrote;
    } else {
      return -1;
    }
  } while (n);
  return 0;
}

static int linenoiseWriteStr(int fd, const char *p) {
  return linenoiseWrite(fd, p, strlen(p));
}

static void linenoiseDebug(struct linenoiseState *l, const char *fmt, ...) {
  va_list va;
  char *msg = 0;
  char *ansi = 0;
  int x, y, n, dy, xn;
  va_start(va, fmt);
  (vappendf)(&msg, fmt, va);
  va_end(va);
  xn = l->ws.ws_col;
  xn = MAX(xn, 80);
  y = l->debugrow;
  n = GetMonospaceWidth(msg, strlen(msg));
  x = MAX(xn - n, 0);
  (appendf)(&ansi, "\e7\e[%u;%uH\e[1K%s\e8", y + 1, x + 1, msg);
  linenoiseWrite(l->ofd, ansi, appendz(ansi).i);
  y = (y + (n + (xn - 1)) / xn) % LINENOISE_MAX_DEBUG;
  l->debugrow = y;
  free(ansi);
  free(msg);
}

static ssize_t linenoiseRead(int fd, char *buf, size_t size,
                             struct linenoiseState *l) {
  ssize_t rc;
  int refreshme;
  do {
    refreshme = 0;
    if (gotcont && rawmode != -1) {
      enableRawMode(rawmode);
      if (l) refreshme = 1;
    }
    if (l && gotwinch) refreshme = 1;
    if (refreshme) linenoiseRefreshLine(l);
    rc = readansi(fd, buf, size);
  } while (rc == -1 && errno == EINTR);
  if (l && rc > 0) {
    memcpy(l->seq[1], l->seq[0], sizeof(l->seq[0]));
    memset(l->seq[0], 0, sizeof(l->seq[0]));
    memcpy(l->seq[0], buf, MIN(MIN(size, rc), sizeof(l->seq[0]) - 1));
  }
  return rc;
}

/**
 * Returns number of columns in current terminal.
 *
 * 1. Checks COLUMNS environment variable (set by Emacs)
 * 2. Tries asking termios (works for pseudoteletypewriters)
 * 3. Falls back to inband signalling (works w/ pipe or serial)
 * 4. Otherwise we conservatively assume 80 columns
 *
 * @param ws should be initialized by caller to zero before first call
 * @param ifd is input file descriptor
 * @param ofd is output file descriptor
 * @return window size
 */
static struct winsize GetTerminalSize(struct winsize ws, int ifd, int ofd) {
  int x;
  ssize_t n;
  char *p, *s, b[16];
  ioctl(ofd, TIOCGWINSZ, &ws);
  if ((!ws.ws_row && (s = getenv("ROWS")) && (x = ParseUnsigned(s, 0)))) {
    ws.ws_row = x;
  }
  if ((!ws.ws_col && (s = getenv("COLUMNS")) && (x = ParseUnsigned(s, 0)))) {
    ws.ws_col = x;
  }
  if (((!ws.ws_col || !ws.ws_row) && linenoiseRead(ifd, 0, 0, 0) != -1 &&
       linenoiseWriteStr(
           ofd, "\0337"           /* save position */
                "\033[9979;9979H" /* move cursor to bottom right corner */
                "\033[6n"         /* report position */
                "\0338") != -1 && /* restore position */
       (n = linenoiseRead(ifd, b, sizeof(b), 0)) != -1 &&
       n && b[0] == 033 && b[1] == '[' && b[n - 1] == 'R')) {
    p = b + 2;
    if ((x = ParseUnsigned(p, &p))) ws.ws_row = x;
    if (*p++ == ';' && (x = ParseUnsigned(p, 0))) ws.ws_col = x;
  }
  if (!ws.ws_col) ws.ws_col = 80;
  if (!ws.ws_row) ws.ws_row = 24;
  return ws;
}

/* Clear the screen. Used to handle ctrl+l */
void linenoiseClearScreen(int fd) {
  linenoiseWriteStr(fd, "\033[H"    /* move cursor to top left corner */
                        "\033[2J"); /* erase display */
}

static void linenoiseBeep(void) {
  /* THE TERMINAL BELL IS DEAD - HISTORY HAS KILLED IT */
}

/* Free a list of completion option populated by linenoiseAddCompletion(). */
void linenoiseFreeCompletions(linenoiseCompletions *lc) {
  size_t i;
  for (i = 0; i < lc->len; i++) free(lc->cvec[i]);
  if (lc->cvec) free(lc->cvec);
}

/* This is an helper function for linenoiseEdit() and is called when the
 * user types the <tab> key in order to complete the string currently in the
 * input.
 *
 * The state of the editing is encapsulated into the pointed linenoiseState
 * structure as described in the structure definition. */
static int linenoiseCompleteLine(struct linenoiseState *ls, char *seq,
                                 int size) {
  size_t i, n, stop;
  int nwritten, nread;
  linenoiseCompletions lc;
  struct linenoiseState saved;
  nread = 0;
  memset(&lc, 0, sizeof(lc));
  completionCallback(ls->buf, &lc);
  if (!lc.len) {
    linenoiseBeep();
  } else {
    i = 0;
    stop = 0;
    while (!stop) {
      /* Show completion or original buffer */
      if (i < lc.len) {
        saved = *ls;
        ls->len = ls->pos = strlen(lc.cvec[i]);
        ls->buf = lc.cvec[i];
        linenoiseRefreshLine(ls);
        ls->len = saved.len;
        ls->pos = saved.pos;
        ls->buf = saved.buf;
      } else {
        linenoiseRefreshLine(ls);
      }
      nread = linenoiseRead(ls->ifd, seq, size, ls);
      if (nread <= 0) {
        linenoiseFreeCompletions(&lc);
        return -1;
      }
      switch (seq[0]) {
        case '\t':
          i = (i + 1) % (lc.len + 1);
          if (i == lc.len) {
            linenoiseBeep();
          }
          break;
        default:
          if (i < lc.len) {
            n = strlen(lc.cvec[i]);
            nwritten = MIN(n, ls->buflen);
            memcpy(ls->buf, lc.cvec[i], nwritten + 1);
            ls->len = ls->pos = nwritten;
          }
          stop = 1;
          break;
      }
    }
  }
  linenoiseFreeCompletions(&lc);
  return nread;
}

static void linenoiseEditHistoryGoto(struct linenoiseState *l, int i) {
  if (historylen <= 1) return;
  i = MAX(MIN(i, historylen - 1), 0);
  free(history[historylen - 1 - l->hindex]);
  history[historylen - 1 - l->hindex] = strdup(l->buf);
  l->hindex = i;
  if (!memccpy(l->buf, history[historylen - 1 - l->hindex], 0, l->buflen)) {
    l->buf[l->buflen - 1] = 0;
  }
  l->len = l->pos = strlen(l->buf);
  linenoiseRefreshLine(l);
}

static void linenoiseEditHistoryMove(struct linenoiseState *l, int dx) {
  linenoiseEditHistoryGoto(l, l->hindex + dx);
}

static char *linenoiseMakeSearchPrompt(struct abuf *ab, int fail, const char *s,
                                       int n) {
  ab->len = 0;
  abAppendw(ab, '(');
  if (fail) abAppends(ab, "failed ");
  abAppends(ab, "reverse-i-search `\e[4m");
  abAppend(ab, s, n);
  abAppends(ab, "\033[24m");
  abAppends(ab, s + n);
  abAppendw(ab, READ32LE("') "));
  return ab->b;
}

static int linenoiseSearch(struct linenoiseState *l, char *seq, int size) {
  char *p;
  struct abuf ab;
  struct abuf prompt;
  const char *oldprompt, *q;
  int i, j, k, rc, fail, added, oldpos, matlen, oldindex;
  if (historylen <= 1) return 0;
  abInit(&ab);
  abInit(&prompt);
  oldpos = l->pos;
  oldprompt = l->prompt;
  oldindex = l->hindex;
  for (fail = matlen = 0;;) {
    l->prompt = linenoiseMakeSearchPrompt(&prompt, fail, ab.b, matlen);
    linenoiseRefreshLine(l);
    fail = 1;
    added = 0;
    j = l->pos;
    i = l->hindex;
    rc = linenoiseRead(l->ifd, seq, size, l);
    if (rc > 0) {
      if (seq[0] == CTRL('?') || seq[0] == CTRL('H')) {
        if (ab.len) {
          --ab.len;
          matlen = MIN(matlen, ab.len);
        }
      } else if (seq[0] == CTRL('R')) {
        if (j) {
          --j;
        } else if (i + 1 < historylen) {
          ++i;
          j = strlen(history[historylen - 1 - i]);
        }
      } else if (seq[0] == CTRL('G')) {
        linenoiseEditHistoryGoto(l, oldindex);
        l->pos = oldpos;
        rc = 0;
        break;
      } else if (iswcntrl(seq[0])) { /* only sees canonical c0 */
        break;
      } else {
        abAppend(&ab, seq, rc);
        added = rc;
      }
    } else {
      break;
    }
    while (i < historylen) {
      p = history[historylen - 1 - i];
      k = strlen(p);
      j = MIN(k, j + ab.len);
      if ((q = FindSubstringReverse(p, j, ab.b, ab.len))) {
        linenoiseEditHistoryGoto(l, i);
        l->pos = q - p;
        fail = 0;
        if (added) {
          matlen += added;
          added = 0;
        }
        break;
      } else {
        i = i + 1;
        j = LINENOISE_MAX_LINE;
      }
    }
  }
  l->prompt = oldprompt;
  linenoiseRefreshLine(l);
  abFree(&prompt);
  abFree(&ab);
  linenoiseRefreshLine(l);
  return rc;
}

/* Register a callback function to be called for tab-completion. */
void linenoiseSetCompletionCallback(linenoiseCompletionCallback *fn) {
  completionCallback = fn;
}

/* Register a hits function to be called to show hits to the user at the
 * right of the prompt. */
void linenoiseSetHintsCallback(linenoiseHintsCallback *fn) {
  hintsCallback = fn;
}

/* Register a function to free the hints returned by the hints callback
 * registered with linenoiseSetHintsCallback(). */
void linenoiseSetFreeHintsCallback(linenoiseFreeHintsCallback *fn) {
  freeHintsCallback = fn;
}

/* This function is used by the callback function registered by the user
 * in order to add completion options given the input string when the
 * user typed <tab>. See the example.c source code for a very easy to
 * understand example. */
void linenoiseAddCompletion(linenoiseCompletions *lc, const char *str) {
  size_t len;
  char *copy, **cvec;
  if ((copy = malloc((len = strlen(str)) + 1))) {
    memcpy(copy, str, len + 1);
    if ((cvec =
             (char **)realloc(lc->cvec, (lc->len + 1) * sizeof(*lc->cvec)))) {
      lc->cvec = cvec;
      lc->cvec[lc->len++] = copy;
    } else {
      free(copy);
    }
  }
}

static void linenoiseRingFree(void) {
  size_t i;
  for (i = 0; i < LINENOISE_MAX_RING; ++i) {
    if (ring.p[i]) {
      free(ring.p[i]);
      ring.p[i] = 0;
    }
  }
}

static void linenoiseRingPush(const char *p, size_t n) {
  if (LINENOISE_MAX_RING && n) {
    ring.i = (ring.i + 1) % LINENOISE_MAX_RING;
    free(ring.p[ring.i]);
    ring.p[ring.i] = strndup(p, n);
  }
}

static void linenoiseRingRotate(void) {
  size_t i;
  for (i = 0; i < LINENOISE_MAX_RING; ++i) {
    ring.i = (ring.i - 1) % LINENOISE_MAX_RING;
    if (ring.p[ring.i]) {
      break;
    }
  }
}

static void linenoiseRefreshHints(struct abuf *ab, struct linenoiseState *l) {
  char *hint;
  const char *ansi1, *ansi2;
  if (!hintsCallback) return;
  ansi1 = "\033[90m";
  ansi2 = "\033[39m";
  if (!(hint = hintsCallback(l->buf, &ansi1, &ansi2))) return;
  if (ansi1) abAppends(ab, ansi1);
  abAppends(ab, hint);
  if (ansi2) abAppends(ab, ansi2);
  if (freeHintsCallback) freeHintsCallback(hint);
}

static void linenoiseRefreshLine(struct linenoiseState *l) {
  struct abuf ab;
  int i, j, fd, plen, pwidth, rows, rpos, rpos2, col, old_rows;
  if (gotwinch && rawmode != -1) {
    l->ws = GetTerminalSize(l->ws, l->ifd, l->ofd);
    gotwinch = 0;
  }
  fd = l->ofd;
  old_rows = l->maxrows;
  plen = strlen(l->prompt);
  pwidth = GetMonospaceWidth(l->prompt, plen);
  /* cursor relative row */
  rpos = (pwidth + l->oldpos + l->ws.ws_col) / l->ws.ws_col;
  /* rows used by current buf */
  rows = (pwidth + GetMonospaceWidth(l->buf, l->len) + l->ws.ws_col - 1) /
         l->ws.ws_col;
  if (rows > (int)l->maxrows) l->maxrows = rows;
  /* First step: clear all the lines used before.
   * To do so start by going to the last row. */
  abInit(&ab);
  if (old_rows - rpos > 0) {
    abAppendw(&ab, READ32LE("\033[\0"));
    abAppendu(&ab, old_rows - rpos);
    abAppendw(&ab, 'B');
  }
  /* Now for every row clear it, go up. */
  for (j = 0; j < old_rows - 1; j++) {
    abAppends(&ab, "\r\033[K\033[A");
  }
  abAppendw(&ab, READ32LE("\r\033[K"));
  abAppends(&ab, l->prompt);
  if (maskmode) {
    for (i = 0; i < l->len; i++) {
      abAppendw(&ab, '*');
    }
  } else {
    abAppend(&ab, l->buf, l->len);
  }
  linenoiseRefreshHints(&ab, l);
  /* If we are at the very end of the screen with our prompt, we need to
   * emit a newline and move the prompt to the first column. */
  if ((l->pos && l->pos == l->len &&
       !((pwidth + GetMonospaceWidth(l->buf, l->pos)) % l->ws.ws_col))) {
    abAppendw(&ab, READ32LE("\n\r\0"));
    if (++rows > (int)l->maxrows) {
      l->maxrows = rows;
    }
  }
  /* move cursor to right position */
  /* get current cursor relative row */
  rpos2 = (pwidth + GetMonospaceWidth(l->buf, l->pos) + l->ws.ws_col) /
          l->ws.ws_col;
  /* go up till we reach expected positon */
  if (rows - rpos2 > 0) {
    abAppendw(&ab, READ32LE("\033[\0"));
    abAppendu(&ab, rows - rpos2);
    abAppendw(&ab, 'A');
  }
  /* set column */
  col = (pwidth + (int)GetMonospaceWidth(l->buf, l->pos)) % (int)l->ws.ws_col;
  if (col) {
    abAppendw(&ab, READ32LE("\r\033["));
    abAppendu(&ab, col);
    abAppendw(&ab, 'C');
  } else {
    abAppendw(&ab, '\r');
  }
  l->oldpos = l->pos;
  linenoiseWrite(fd, ab.b, ab.len);
  abFree(&ab);
}

static int linenoiseEditInsert(struct linenoiseState *l, const char *p,
                               size_t n) {
  char d;
  if (l->len + n < l->buflen) {
    memmove(l->buf + l->pos + n, l->buf + l->pos, l->len - l->pos);
    memcpy(l->buf + l->pos, p, n);
    l->pos += n;
    l->len += n;
    l->buf[l->len] = 0;
    linenoiseRefreshLine(l);
  }
  return 0;
}

static void linenoiseEditHome(struct linenoiseState *l) {
  l->pos = 0;
  linenoiseRefreshLine(l);
}

static void linenoiseEditEnd(struct linenoiseState *l) {
  l->pos = l->len;
  linenoiseRefreshLine(l);
}

static void linenoiseEditUp(struct linenoiseState *l) {
  linenoiseEditHistoryMove(l, LINENOISE_HISTORY_PREV);
}

static void linenoiseEditDown(struct linenoiseState *l) {
  linenoiseEditHistoryMove(l, LINENOISE_HISTORY_NEXT);
}

static void linenoiseEditBof(struct linenoiseState *l) {
  linenoiseEditHistoryMove(l, LINENOISE_HISTORY_FIRST);
}

static void linenoiseEditEof(struct linenoiseState *l) {
  linenoiseEditHistoryMove(l, LINENOISE_HISTORY_LAST);
}

static void linenoiseEditRefresh(struct linenoiseState *l) {
  linenoiseClearScreen(l->ofd);
  linenoiseRefreshLine(l);
}

static struct rune GetUtf8(const char *p, size_t n) {
  struct rune r;
  if ((r.n = r.c = 0) < n && (r.c = p[r.n++] & 255) >= 0300) {
    r.c = DecodeUtf8(r.c).c;
    while (r.n < n && (p[r.n] & 0300) == 0200) {
      r.c = r.c << 6 | p[r.n++] & 077;
    }
  }
  return r;
}

static size_t Forward(struct linenoiseState *l, size_t pos) {
  return pos + GetUtf8(l->buf + pos, l->len - pos).n;
}

static size_t Backward(struct linenoiseState *l, size_t pos) {
  if (pos) {
    do --pos;
    while (pos && (l->buf[pos] & 0300) == 0200);
  }
  return pos;
}

static size_t Backwards(struct linenoiseState *l, size_t pos,
                        int pred(wint_t)) {
  size_t i;
  struct rune r;
  while (pos) {
    i = Backward(l, pos);
    r = GetUtf8(l->buf + i, l->len - i);
    if (pred(r.c)) {
      pos = i;
    } else {
      break;
    }
  }
  return pos;
}

static size_t Forwards(struct linenoiseState *l, size_t pos, int pred(wint_t)) {
  struct rune r;
  while (pos < l->len) {
    r = GetUtf8(l->buf + pos, l->len - pos);
    if (pred(r.c)) {
      pos += r.n;
    } else {
      break;
    }
  }
  return pos;
}

static size_t ForwardWord(struct linenoiseState *l, size_t pos) {
  pos = Forwards(l, pos, iswseparator);
  pos = Forwards(l, pos, notwseparator);
  return pos;
}

static size_t BackwardWord(struct linenoiseState *l, size_t pos) {
  pos = Backwards(l, pos, iswseparator);
  pos = Backwards(l, pos, notwseparator);
  return pos;
}

static size_t EscapeWord(struct linenoiseState *l) {
  size_t i, j;
  struct rune r;
  for (i = l->pos; i && i < l->len; i += r.n) {
    if (i < l->len) {
      r = GetUtf8(l->buf + i, l->len - i);
      if (iswseparator(r.c)) break;
    }
    if ((j = i)) {
      do --j;
      while (j && (l->buf[j] & 0300) == 0200);
      r = GetUtf8(l->buf + j, l->len - j);
      if (iswseparator(r.c)) break;
    }
  }
  return i;
}

static void linenoiseEditLeft(struct linenoiseState *l) {
  l->pos = Backward(l, l->pos);
  linenoiseRefreshLine(l);
}

static void linenoiseEditRight(struct linenoiseState *l) {
  if (l->pos == l->len) return;
  do l->pos++;
  while (l->pos < l->len && (l->buf[l->pos] & 0300) == 0200);
  linenoiseRefreshLine(l);
}

static void linenoiseEditLeftWord(struct linenoiseState *l) {
  l->pos = BackwardWord(l, l->pos);
  linenoiseRefreshLine(l);
}

static void linenoiseEditRightWord(struct linenoiseState *l) {
  l->pos = ForwardWord(l, l->pos);
  linenoiseRefreshLine(l);
}

static void linenoiseEditDelete(struct linenoiseState *l) {
  size_t i;
  if (l->pos == l->len) return;
  i = Forward(l, l->pos);
  memmove(l->buf + l->pos, l->buf + i, l->len - i + 1);
  l->len -= i - l->pos;
  linenoiseRefreshLine(l);
}

static void linenoiseEditRubout(struct linenoiseState *l) {
  size_t i;
  if (!l->pos) return;
  i = Backward(l, l->pos);
  memmove(l->buf + i, l->buf + l->pos, l->len - l->pos + 1);
  l->len -= l->pos - i;
  l->pos = i;
  linenoiseRefreshLine(l);
}

static void linenoiseEditDeleteWord(struct linenoiseState *l) {
  size_t i;
  if (l->pos == l->len) return;
  i = ForwardWord(l, l->pos);
  linenoiseRingPush(l->buf + l->pos, i - l->len);
  memmove(l->buf + l->pos, l->buf + i, l->len - i + 1);
  l->len -= i - l->pos;
  linenoiseRefreshLine(l);
}

static void linenoiseEditRuboutWord(struct linenoiseState *l) {
  size_t i;
  if (!l->pos) return;
  i = BackwardWord(l, l->pos);
  linenoiseRingPush(l->buf + i, l->pos - i);
  memmove(l->buf + i, l->buf + l->pos, l->len - l->pos + 1);
  l->len -= l->pos - i;
  l->pos = i;
  linenoiseRefreshLine(l);
}

static void linenoiseEditXlatWord(struct linenoiseState *l,
                                  wint_t xlat(wint_t)) {
  int c;
  struct rune r;
  struct abuf ab;
  size_t i, j, p;
  abInit(&ab);
  i = Forwards(l, l->pos, iswseparator);
  for (j = i; j < l->len; j += r.n) {
    r = GetUtf8(l->buf + j, l->len - j);
    if (iswseparator(r.c)) break;
    if ((c = xlat(r.c)) != r.c) {
      abAppendw(&ab, tpenc(c));
    } else { /* avoid canonicalization */
      abAppend(&ab, l->buf + j, r.n);
    }
  }
  if (ab.len && i + ab.len + l->len - j < l->buflen) {
    l->pos = i + ab.len;
    abAppend(&ab, l->buf + j, l->len - j);
    l->len = i + ab.len;
    memcpy(l->buf + i, ab.b, ab.len + 1);
    linenoiseRefreshLine(l);
  }
  abFree(&ab);
}

static void linenoiseEditLowercaseWord(struct linenoiseState *l) {
  linenoiseEditXlatWord(l, towlower);
}

static void linenoiseEditUppercaseWord(struct linenoiseState *l) {
  linenoiseEditXlatWord(l, towupper);
}

static void linenoiseEditCapitalizeWord(struct linenoiseState *l) {
  iscapital = 0;
  linenoiseEditXlatWord(l, Capitalize);
}

static void linenoiseEditKillLeft(struct linenoiseState *l) {
  size_t diff, old_pos;
  linenoiseRingPush(l->buf, l->pos);
  old_pos = l->pos;
  l->pos = 0;
  diff = old_pos - l->pos;
  memmove(l->buf + l->pos, l->buf + old_pos, l->len - old_pos + 1);
  l->len -= diff;
  linenoiseRefreshLine(l);
}

static void linenoiseEditKillRight(struct linenoiseState *l) {
  linenoiseRingPush(l->buf + l->pos, l->len - l->pos);
  l->buf[l->pos] = '\0';
  l->len = l->pos;
  linenoiseRefreshLine(l);
}

static void linenoiseEditYank(struct linenoiseState *l) {
  char *p;
  size_t n;
  if (!ring.p[ring.i]) return;
  n = strlen(ring.p[ring.i]);
  if (l->len + n < l->buflen) {
    p = malloc(l->len - l->pos + 1);
    memcpy(p, l->buf + l->pos, l->len - l->pos + 1);
    memcpy(l->buf + l->pos, ring.p[ring.i], n);
    memcpy(l->buf + l->pos + n, p, l->len - l->pos + 1);
    l->yi = l->pos;
    l->yj = l->pos + n;
    l->pos += n;
    l->len += n;
    free(p);
    linenoiseRefreshLine(l);
  }
}

static void linenoiseEditRotate(struct linenoiseState *l) {
  if ((l->seq[1][0] == CTRL('Y') ||
       (l->seq[1][0] == 033 && l->seq[1][1] == 'y'))) {
    if (l->yi < l->len && l->yj <= l->len) {
      memmove(l->buf + l->yi, l->buf + l->yj, l->len - l->yj + 1);
      l->len -= l->yj - l->yi;
      l->pos -= l->yj - l->yi;
    }
    linenoiseRingRotate();
    linenoiseEditYank(l);
  }
}

static void linenoiseEditTranspose(struct linenoiseState *l) {
  char *q, *p;
  size_t a, b, c;
  b = l->pos;
  a = Backward(l, b);
  c = Forward(l, b);
  if (!(a < b && b < c)) return;
  p = q = malloc(c - a);
  p = mempcpy(p, l->buf + b, c - b);
  p = mempcpy(p, l->buf + a, b - a);
  assert(p - q == c - a);
  memcpy(l->buf + a, q, p - q);
  l->pos = c;
  free(q);
  linenoiseRefreshLine(l);
}

static void linenoiseEditTransposeWords(struct linenoiseState *l) {
  char *q, *p;
  struct rune r;
  size_t pi, xi, xj, yi, yj;
  pi = EscapeWord(l);
  xj = Backwards(l, pi, iswseparator);
  xi = Backwards(l, xj, notwseparator);
  yi = Forwards(l, pi, iswseparator);
  yj = Forwards(l, yi, notwseparator);
  if (!(xi < xj && xj < yi && yi < yj)) return;
  p = q = malloc(yj - xi);
  p = mempcpy(p, l->buf + yi, yj - yi);
  p = mempcpy(p, l->buf + xj, yi - xj);
  p = mempcpy(p, l->buf + xi, xj - xi);
  assert(p - q == yj - xi);
  memcpy(l->buf + xi, q, p - q);
  l->pos = yj;
  free(q);
  linenoiseRefreshLine(l);
}

static void linenoiseEditSqueeze(struct linenoiseState *l) {
  size_t i, j;
  i = Backwards(l, l->pos, iswseparator);
  j = Forwards(l, l->pos, iswseparator);
  if (!(i < j)) return;
  memmove(l->buf + i, l->buf + j, l->len - j + 1);
  l->len -= j - i;
  l->pos = i;
  linenoiseRefreshLine(l);
}

static void linenoiseEditMark(struct linenoiseState *l) {
  l->mark = l->pos;
}

static void linenoiseEditGoto(struct linenoiseState *l) {
  if (l->mark > l->len) return;
  l->pos = MIN(l->mark, l->len);
  linenoiseRefreshLine(l);
}

/**
 * Runs linenoise engine.
 *
 * This function is the core of the line editing capability of linenoise.
 * It expects 'fd' to be already in "raw mode" so that every key pressed
 * will be returned ASAP to read().
 *
 * The resulting string is put into 'buf' when the user type enter, or
 * when ctrl+d is typed.
 *
 * Returns chomped character count in buf >=0 or -1 on eof / error
 */
static ssize_t linenoiseEdit(int stdin_fd, int stdout_fd, char *buf,
                             size_t buflen, const char *prompt) {
  ssize_t rc;
  size_t nread;
  char seq[16];
  struct linenoiseState l;
  linenoiseHintsCallback *hc;
  memset(&l, 0, sizeof(l));
  buf[0] = 0;
  l.buf = buf;
  l.ifd = stdin_fd;
  l.ofd = stdout_fd;
  l.buflen = buflen - 1;
  l.prompt = prompt ? prompt : "";
  l.ws = GetTerminalSize(l.ws, l.ifd, l.ofd);
  linenoiseHistoryAdd("");
  linenoiseWriteStr(l.ofd, l.prompt);
  while (1) {
    rc = linenoiseRead(l.ifd, seq, sizeof(seq), &l);
    if (rc > 0) {
      if (seq[0] == CTRL('R')) {
        rc = linenoiseSearch(&l, seq, sizeof(seq));
        if (!rc) continue;
      } else if (seq[0] == '\t' && completionCallback) {
        rc = linenoiseCompleteLine(&l, seq, sizeof(seq));
        if (!rc) continue;
      }
    }
    if (rc > 0) {
      nread = rc;
    } else if (!rc && l.len) {
      nread = 1;
      seq[0] = '\r';
      seq[1] = 0;
    } else {
      historylen--;
      free(history[historylen]);
      history[historylen] = 0;
      return -1;
    }
    switch (seq[0]) {
      CASE(CTRL('P'), linenoiseEditUp(&l));
      CASE(CTRL('E'), linenoiseEditEnd(&l));
      CASE(CTRL('N'), linenoiseEditDown(&l));
      CASE(CTRL('A'), linenoiseEditHome(&l));
      CASE(CTRL('B'), linenoiseEditLeft(&l));
      CASE(CTRL('@'), linenoiseEditMark(&l));
      CASE(CTRL('Y'), linenoiseEditYank(&l));
      CASE(CTRL('F'), linenoiseEditRight(&l));
      CASE(CTRL('?'), linenoiseEditRubout(&l));
      CASE(CTRL('H'), linenoiseEditRubout(&l));
      CASE(CTRL('L'), linenoiseEditRefresh(&l));
      CASE(CTRL('U'), linenoiseEditKillLeft(&l));
      CASE(CTRL('T'), linenoiseEditTranspose(&l));
      CASE(CTRL('K'), linenoiseEditKillRight(&l));
      CASE(CTRL('W'), linenoiseEditRuboutWord(&l));
      case CTRL('X'):
        if (l.seq[1][0] == CTRL('X')) {
          linenoiseEditGoto(&l);
        }
        break;
      case CTRL('D'):
        if (l.len) {
          linenoiseEditDelete(&l);
        } else {
          free(history[--historylen]);
          history[historylen] = 0;
          return -1;
        }
        break;
      case '\r':
        free(history[--historylen]);
        history[historylen] = 0;
        linenoiseEditEnd(&l);
        if (hintsCallback) {
          /* Force a refresh without hints to leave the previous
           * line as the user typed it after a newline. */
          hc = hintsCallback;
          hintsCallback = 0;
          linenoiseRefreshLine(&l);
          hintsCallback = hc;
        }
        DEBUG(&l, "%`'s", buf);
        return l.len;
      case 033:
        if (nread < 2) break;
        switch (seq[1]) {
          CASE('<', linenoiseEditBof(&l));
          CASE('>', linenoiseEditEof(&l));
          CASE('y', linenoiseEditRotate(&l));
          CASE('\\', linenoiseEditSqueeze(&l));
          CASE('b', linenoiseEditLeftWord(&l));
          CASE('f', linenoiseEditRightWord(&l));
          CASE('h', linenoiseEditRuboutWord(&l));
          CASE('d', linenoiseEditDeleteWord(&l));
          CASE('l', linenoiseEditLowercaseWord(&l));
          CASE('u', linenoiseEditUppercaseWord(&l));
          CASE('c', linenoiseEditCapitalizeWord(&l));
          CASE('t', linenoiseEditTransposeWords(&l));
          CASE(CTRL('H'), linenoiseEditRuboutWord(&l));
          case '[':
            if (nread < 3) break;
            if (seq[2] >= '0' && seq[2] <= '9') {
              if (nread < 4) break;
              if (seq[3] == '~') {
                switch (seq[2]) {
                  CASE('1', linenoiseEditHome(&l));   /* \e[1~ */
                  CASE('3', linenoiseEditDelete(&l)); /* \e[3~ */
                  CASE('4', linenoiseEditEnd(&l));    /* \e[4~ */
                  default:
                    break;
                }
              }
            } else {
              switch (seq[2]) {
                CASE('A', linenoiseEditUp(&l));
                CASE('B', linenoiseEditDown(&l));
                CASE('C', linenoiseEditRight(&l));
                CASE('D', linenoiseEditLeft(&l));
                CASE('H', linenoiseEditHome(&l));
                CASE('F', linenoiseEditEnd(&l));
                default:
                  break;
              }
            }
            break;
          case 'O':
            if (nread < 3) break;
            switch (seq[2]) {
              CASE('H', linenoiseEditHome(&l));
              CASE('F', linenoiseEditEnd(&l));
              default:
                break;
            }
            break;
          default:
            break;
        }
        break;
      default:
        if (!iswcntrl(seq[0])) { /* only sees canonical c0 */
          linenoiseEditInsert(&l, seq, nread);
        }
        break;
    }
  }
}

void linenoiseFree(void *ptr) {
  free(ptr);
}

void linenoiseHistoryFree(void) {
  size_t i;
  for (i = 0; i < LINENOISE_MAX_HISTORY; i++) {
    if (history[i]) {
      free(history[i]);
      history[i] = 0;
    }
  }
  historylen = 0;
}

static void linenoiseAtExit(void) {
  linenoiseDisableRawMode();
  linenoiseHistoryFree();
  linenoiseRingFree();
}

int linenoiseHistoryAdd(const char *line) {
  char *linecopy;
  if (!LINENOISE_MAX_HISTORY) return 0;
  if (historylen && !strcmp(history[historylen - 1], line)) return 0;
  if (!(linecopy = strdup(line))) return 0;
  if (historylen == LINENOISE_MAX_HISTORY) {
    free(history[0]);
    memmove(history, history + 1, sizeof(char *) * (LINENOISE_MAX_HISTORY - 1));
    historylen--;
  }
  history[historylen++] = linecopy;
  return 1;
}

/**
 * Saves line editing history to file.
 *
 * @return 0 on success, or -1 w/ errno
 */
int linenoiseHistorySave(const char *filename) {
  int j;
  FILE *fp;
  mode_t old_umask;
  old_umask = umask(S_IXUSR | S_IRWXG | S_IRWXO);
  fp = fopen(filename, "w");
  umask(old_umask);
  if (!fp) return -1;
  chmod(filename, S_IRUSR | S_IWUSR);
  for (j = 0; j < historylen; j++) {
    fputs(history[j], fp);
    fputc('\n', fp);
  }
  fclose(fp);
  return 0;
}

/**
 * Loads history from the specified file.
 *
 * If the file doesn't exist, zero is returned and this will do nothing.
 * If the file does exists and the operation succeeded zero is returned
 * otherwise on error -1 is returned.
 *
 * @return 0 on success, or -1 w/ errno
 */
int linenoiseHistoryLoad(const char *filename) {
  char **h;
  int rc, fd, err;
  size_t i, j, k, n;
  char *m, *e, *p, *q, *f, *s;
  err = errno, rc = 0;
  if (!LINENOISE_MAX_HISTORY) return 0;
  if (!(h = (char **)calloc(2 * LINENOISE_MAX_HISTORY, sizeof(char *))))
    return -1;
  if ((fd = open(filename, O_RDONLY)) != -1) {
    if ((n = GetFdSize(fd))) {
      if ((m = (char *)mmap(0, n, PROT_READ, MAP_SHARED, fd, 0)) !=
          MAP_FAILED) {
        for (i = 0, e = (p = m) + n; p < e; p = f + 1) {
          if (!(q = (char *)memchr(p, '\n', e - p))) q = e;
          for (f = q; q > p; --q) {
            if (q[-1] != '\n' && q[-1] != '\r') break;
          }
          if (q > p) {
            h[i * 2 + 0] = p;
            h[i * 2 + 1] = q;
            i = (i + 1) % LINENOISE_MAX_HISTORY;
          }
        }
        linenoiseHistoryFree();
        for (j = 0; j < LINENOISE_MAX_HISTORY; ++j) {
          if (h[(k = (i + j) % LINENOISE_MAX_HISTORY) * 2]) {
            if ((s = malloc((n = h[k * 2 + 1] - h[k * 2]) + 1))) {
              memcpy(s, h[k * 2], n), s[n] = 0;
              history[historylen++] = s;
            }
          }
        }
        munmap(m, n);
      } else {
        rc = -1;
      }
    }
    close(fd);
  } else if (errno == ENOENT) {
    errno = err;
  } else {
    rc = -1;
  }
  free(h);
  return rc;
}

/**
 * Reads line interactively.
 *
 * This function can be used instead of linenoise() in cases where we
 * know for certain we're dealing with a terminal, which means we can
 * avoid linking any stdio code.
 *
 * @return chomped allocated string of read line or null on eof/error
 */
char *linenoiseRaw(const char *prompt, int infd, int outfd) {
  int sig;
  ssize_t rc;
  size_t nread;
  char *buf, *p;
  sigset_t omask;
  static char once;
  struct sigaction sa;
  if (!once) atexit(linenoiseAtExit), once = 1;
  if (!(buf = malloc(LINENOISE_MAX_LINE))) return 0;
  if (enableRawMode(infd) == -1) return 0;
  sigemptyset(&sa.sa_mask);
  sigaddset(&sa.sa_mask, SIGINT);
  sigaddset(&sa.sa_mask, SIGQUIT);
  sigprocmask(SIG_BLOCK, &sa.sa_mask, &omask);
  sa.sa_flags = SA_NODEFER;
  sa.sa_handler = linenoiseOnInt;
  sigaction(SIGINT, &sa, &orig_int);
  sa.sa_handler = linenoiseOnQuit;
  sigaction(SIGQUIT, &sa, &orig_quit);
  if (!(sig = setjmp(jraw))) {
    sigprocmask(SIG_UNBLOCK, &sa.sa_mask, 0);
    rc = linenoiseEdit(infd, outfd, buf, LINENOISE_MAX_LINE, prompt);
  } else {
    rc = -1;
  }
  linenoiseDisableRawMode();
  sigaction(SIGINT, &orig_int, 0);
  sigaction(SIGQUIT, &orig_quit, 0);
  sigprocmask(SIG_SETMASK, &omask, 0);
  if (sig) raise(sig);
  if (rc != -1) {
    nread = rc;
    linenoiseWriteStr(outfd, "\n");
    if ((p = realloc(buf, nread + 1))) buf = p;
    return buf;
  } else {
    free(buf);
    return 0;
  }
}

/**
 * Reads line intelligently.
 *
 * The high level function that is the main API of the linenoise library.
 * This function checks if the terminal has basic capabilities, just checking
 * for a blacklist of inarticulate terminals, and later either calls the line
 * editing function or uses dummy fgets() so that you will be able to type
 * something even in the most desperate of the conditions.
 *
 * @param prompt is printed before asking for input if we have a term
 *     and this may be set to empty or null to disable and prompt may
 *     contain ansi escape sequences, color, utf8, etc.
 * @return chomped allocated string of read line or null on eof/error
 */
char *linenoise(const char *prompt) {
  if ((!isatty(fileno(stdin)) || !isatty(fileno(stdout)))) {
    return GetLine(stdin);
  } else if (linenoiseIsUnsupportedTerm()) {
    if (prompt && *prompt) {
      fputs(prompt, stdout);
      fflush(stdout);
    }
    return GetLine(stdin);
  } else {
    fflush(stdout);
    return linenoiseRaw(prompt, fileno(stdin), fileno(stdout));
  }
}

/**
 * Reads line intelligently w/ history, e.g.
 *
 *     // see ~/.foo_history
 *     main() {
 *         char *line;
 *         while ((line = ezlinenoise("IN> ", "foo"))) {
 *             printf("OUT> %s\n", line);
 *             free(line);
 *         }
 *     }
 *
 * @param prompt is printed before asking for input if we have a term
 *     and this may be set to empty or null to disable and prompt may
 *     contain ansi escape sequences, color, utf8, etc.
 * @param prog is name of your app, used to generate history filename
 *     however if it contains a slash / dot then we'll assume prog is
 *     the history filename which as determined by the caller
 * @return chomped allocated string of read line or null on eof/error
 */
char *ezlinenoise(const char *prompt, const char *prog) {
  char *line;
  struct abuf path;
  const char *a, *b;
  abInit(&path);
  if (prog) {
    if (strchr(prog, '/') || strchr(prog, '.')) {
      abAppends(&path, prog);
    } else {
      b = "";
      if (!(a = getenv("HOME"))) {
        if (!(a = getenv("HOMEDRIVE")) || !(b = getenv("HOMEPATH"))) {
          a = "";
        }
      }
      if (*a) {
        abAppends(&path, a);
        abAppends(&path, b);
        abAppendw(&path, '/');
      }
      abAppendw(&path, '.');
      abAppends(&path, prog);
      abAppends(&path, "_history");
    }
  }
  if (path.len) {
    linenoiseHistoryLoad(path.b);
  }
  line = linenoise(prompt);
  if (path.len && line && *line) {
    /* history here is inefficient but helpful when the user has multiple
     * repls open at the same time, so history propagates between them */
    linenoiseHistoryLoad(path.b);
    linenoiseHistoryAdd(line);
    linenoiseHistorySave(path.b);
  }
  abFree(&path);
  return line;
}
