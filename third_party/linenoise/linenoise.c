/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=4 sts=4 sw=4 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ linenoise.c -- guerrilla line editing library against the idea that a        │
│ line editing lib needs to be 20,000 lines of C code.                         │
│                                                                              │
│ You can find the latest source code at:                                      │
│                                                                              │
│   http://github.com/antirez/linenoise                                        │
│                                                                              │
│ Does a number of crazy assumptions that happen to be true in 99.9999% of     │
│ the 2010 UNIX computers around.                                              │
│                                                                              │
╞══════════════════════════════════════════════════════════════════════════════╡
│                                                                              │
│ Copyright (c) 2010-2016, Salvatore Sanfilippo <antirez at gmail dot com>     │
│ Copyright (c) 2010-2013, Pieter Noordhuis <pcnoordhuis at gmail dot com>     │
│ Copyright (c) 2018-2020, Justine Tunney <jtunney at gmail dot com>           │
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
╞══════════════════════════════════════════════════════════════════════════════╡
│                                                                              │
│ CHANGES                                                                      │
│                                                                              │
│   - Remove bell                                                              │
│   - Windows support                                                          │
│   - Filter out unsupported control sequences                                 │
│   - Filter out Thompson-Pike input sequences                                 │
│   - Make behavior much more like GNU readline                                │
│                                                                              │
│ TODO                                                                         │
│                                                                              │
│   - Undo                                                                     │
│   - Kill Ring                                                                │
│   - Ctrl+R search                                                            │
│   - Thompson-Pike Encoding                                                   │
│                                                                              │
│ REFERENCE                                                                    │
│                                                                              │
│   The big scary coding you 𝘮𝘶𝘴𝘵 use curses to abstract.                      │
│                                                                              │
│   \t                                  TAB                                    │
│   \a                                  BELL                                   │
│   \r                                  CURSOR START                           │
│   \b                                  CURSOR LEFT                            │
│   \177                                CURSOR LEFT                            │
│   \n                                  CURSOR DOWN AND START IF OPOST         │
│   \f                                  CURSOR DOWN AND START IF OPOST         │
│   \v                                  CURSOR DOWN AND START IF OPOST         │
│   \eD                                 CURSOR DOWN AND START                  │
│   \eE                                 CURSOR DOWN                            │
│   \eM                                 CURSOR UP                              │
│   \ec                                 FULL RESET                             │
│   \e7                                 SAVE CURSOR POSITION                   │
│   \e8                                 RESTORE CURSOR POSITION                │
│   \e(0                                DEC SPECIAL GRAPHICS                   │
│   \e(B                                USAS X3.4-1967                         │
│   \e#5                                SINGLE WIDTH                           │
│   \e#6                                DOUBLE WIDTH                           │
│   \e#8                                SO MANY E                              │
│   \eZ → \e/Z                          REPORT IDENTITY                        │
│   \e[𝑛A                               CURSOR UP             [CLAMPED]        │
│   \e[𝑛B                               CURSOR DOWN           [CLAMPED]        │
│   \e[𝑛C                               CURSOR RIGHT          [CLAMPED]        │
│   \e[𝑛D                               CURSOR LEFT           [CLAMPED]        │
│   \e[𝑦;𝑥H                             SET CURSOR POSITION   [CLAMPED]        │
│   \e[𝑥G                               SET CURSOR COLUMN     [CLAMPED]        │
│   \e[𝑦d                               SET CURSOR ROW        [CLAMPED]        │
│   \e[𝑛E                               CURSOR DOWN AND START [CLAMPED]        │
│   \e[𝑛F                               CURSOR UP AND START   [CLAMPED]        │
│   \e[𝑛S                               SCROLL UP                              │
│   \e[𝑛T                               SCROLL DOWN                            │
│   \e[𝑛@                               INSERT CELLS                           │
│   \e[𝑛P                               DELETE CELLS                           │
│   \e[𝑛L                               INSERT LINES                           │
│   \e[𝑛M                               DELETE LINES                           │
│   \e[J                                ERASE DISPLAY FORWARDS                 │
│   \e[1J                               ERASE DISPLAY BACKWARDS                │
│   \e[2J                               ERASE DISPLAY                          │
│   \e[K                                ERASE LINE FORWARD                     │
│   \e[1K                               ERASE LINE BACKWARD                    │
│   \e[2K                               ERASE LINE                             │
│   \e[𝑛X                               ERASE CELLS                            │
│   \e[0m                               RESET                                  │
│   \e[1m                               BOLD                                   │
│   \e[2m                               FAINT                                  │
│   \e[3m                               ITALIC                                 │
│   \e[4m                               UNDER                                  │
│   \e[5m                               BLINK                                  │
│   \e[7m                               INVERT                                 │
│   \e[8m                               CONCEAL                                │
│   \e[9m                               STRIKE                                 │
│   \e[20m                              FRAKTUR                                │
│   \e[21m                              DUNDER                                 │
│   \e[22m                              RESET BOLD & FAINT                     │
│   \e[23m                              RESET ITALIC & FRAKTUR                 │
│   \e[24m                              RESET UNDER & DUNDER                   │
│   \e[25m                              RESET BLINK                            │
│   \e[27m                              RESET INVERT                           │
│   \e[28m                              RESET CONCEAL                          │
│   \e[29m                              RESET STRIKE                           │
│   \e[39m                              RESET FOREGROUND                       │
│   \e[49m                              RESET BACKGROUND                       │
│   \e[30m                              BLACK FOREGROUND                       │
│   \e[31m                              RED FOREGROUND                         │
│   \e[32m                              GREEN FOREGROUND                       │
│   \e[33m                              YELLOW FOREGROUND                      │
│   \e[34m                              BLUE FOREGROUND                        │
│   \e[35m                              MAGENTA FOREGROUND                     │
│   \e[36m                              CYAN FOREGROUND                        │
│   \e[37m                              WHITE FOREGROUND                       │
│   \e[40m                              BLACK BACKGROUND                       │
│   \e[41m                              RED BACKGROUND                         │
│   \e[42m                              GREEN BACKGROUND                       │
│   \e[44m                              YELLOW BACKGROUND                      │
│   \e[44m                              BLUE BACKGROUND                        │
│   \e[45m                              MAGENTA BACKGROUND                     │
│   \e[46m                              CYAN BACKGROUND                        │
│   \e[47m                              WHITE BACKGROUND                       │
│   \e[90m                              BRIGHT BLACK FOREGROUND                │
│   \e[91m                              BRIGHT RED FOREGROUND                  │
│   \e[92m                              BRIGHT GREEN FOREGROUND                │
│   \e[93m                              BRIGHT YELLOW FOREGROUND               │
│   \e[94m                              BRIGHT BLUE FOREGROUND                 │
│   \e[95m                              BRIGHT MAGENTA FOREGROUND              │
│   \e[96m                              BRIGHT CYAN FOREGROUND                 │
│   \e[97m                              BRIGHT WHITE FOREGROUND                │
│   \e[100m                             BRIGHT BLACK BACKGROUND                │
│   \e[101m                             BRIGHT RED BACKGROUND                  │
│   \e[102m                             BRIGHT GREEN BACKGROUND                │
│   \e[103m                             BRIGHT YELLOW BACKGROUND               │
│   \e[104m                             BRIGHT BLUE BACKGROUND                 │
│   \e[105m                             BRIGHT MAGENTA BACKGROUND              │
│   \e[106m                             BRIGHT CYAN BACKGROUND                 │
│   \e[107m                             BRIGHT WHITE BACKGROUND                │
│   \e[38;5;𝑥m                          XTERM256 FOREGROUND                    │
│   \e[48;5;𝑥m                          XTERM256 BACKGROUND                    │
│   \e[38;2;𝑟;𝑔;𝑏m                      RGB FOREGROUND                         │
│   \e[48;2;𝑟;𝑔;𝑏m                      RGB BACKGROUND                         │
│   \e[?25h                             SHOW CURSOR                            │
│   \e[?25l                             HIDE CURSOR                            │
│   \e[s                                SAVE CURSOR POSITION                   │
│   \e[u                                RESTORE CURSOR POSITION                │
│   \e[?5h ... \e[?5l                   REVERSE VIDEO EPILEPSY                 │
│   \e[0q                               RESET LEDS                             │
│   \e[1q                               TURN ON FIRST LED                      │
│   \e[2q                               TURN ON SECOND LED                     │
│   \e[3q                               TURN ON THIRD LED                      │
│   \e[4q                               TURN ON FOURTH LED                     │
│   \e[c → \e[?1;0c                     REPORT DEVICE TYPE                     │
│   \e[5n → \e[0n                       REPORT DEVICE STATUS                   │
│   \e[6n → \e[𝑦;𝑥R                     REPORT CURSOR POSITION                 │
│   \e7\e[9979;9979H\e[6n\e8 → \e[𝑦;𝑥R  REPORT DISPLAY DIMENSIONS              │
│                                                                              │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/calls/calls.h"
#include "libc/calls/termios.h"
#include "libc/calls/ttydefaults.h"
#include "libc/calls/weirdtypes.h"
#include "libc/errno.h"
#include "libc/fmt/fmt.h"
#include "libc/runtime/gc.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/x/x.h"
#include "third_party/linenoise/linenoise.h"
/* clang-format off */

#define LINENOISE_MAX_LINE 4096
#define LINENOISE_DEFAULT_HISTORY_MAX_LEN 256
#define LINENOISE_HISTORY_NEXT 0
#define LINENOISE_HISTORY_PREV 1

static char *unsupported_term[] = {"dumb","cons25","emacs",NULL};
static linenoiseCompletionCallback *completionCallback = NULL;
static linenoiseHintsCallback *hintsCallback = NULL;
static linenoiseFreeHintsCallback *freeHintsCallback = NULL;

static struct termios orig_termios; /* In order to restore at exit.*/
static int maskmode; /* Show "***" instead of input. For passwords. */
static int rawmode; /* For atexit() function to check if restore is needed*/
static int mlmode;  /* Multi line mode. Default is single line. */
static int atexit_registered; /* Register atexit just 1 time. */
static int history_max_len = LINENOISE_DEFAULT_HISTORY_MAX_LEN;
static int history_len;
static char **history;

/* The linenoiseState structure represents the state during line editing.
 * We pass this state to functions implementing specific editing
 * functionalities. */
struct linenoiseState {
    int ifd;            /* Terminal stdin file descriptor. */
    int ofd;            /* Terminal stdout file descriptor. */
    char *buf;          /* Edited line buffer. */
    size_t buflen;      /* Edited line buffer size. */
    const char *prompt; /* Prompt to display. */
    size_t plen;        /* Prompt length. */
    size_t pos;         /* Current cursor position. */
    size_t oldpos;      /* Previous refresh cursor position. */
    size_t len;         /* Current edited line length. */
    size_t cols;        /* Number of columns in terminal. */
    size_t maxrows;     /* Maximum num of rows used so far (multiline mode) */
    int history_index;  /* The history index we are currently editing. */
    char *killed;
};

static void linenoiseAtExit(void);
static void refreshLine(struct linenoiseState *);

/* Debugging macro. */
#if 0
FILE *lndebug_fp = NULL;
#define lndebug(...) \
    do { \
        if (lndebug_fp == NULL) { \
            lndebug_fp = fopen("/tmp/lndebug.txt","a"); \
            fprintf(lndebug_fp, \
            "[%d %d %d] p: %d, rows: %d, rpos: %d, max: %d, oldmax: %d\n", \
            (int)l->len,(int)l->pos,(int)l->oldpos,plen,rows,rpos, \
            (int)l->maxrows,old_rows); \
        } \
        fprintf(lndebug_fp, ", " __VA_ARGS__); \
        fflush(lndebug_fp); \
    } while (0)
#else
#define lndebug(fmt, ...)
#endif

/* ======================= Low level terminal handling ====================== */

/* Enable "mask mode". When it is enabled, instead of the input that
 * the user is typing, the terminal will just display a corresponding
 * number of asterisks, like "****". This is useful for passwords and other
 * secrets that should not be displayed. */
void linenoiseMaskModeEnable(void) {
    maskmode = 1;
}

/* Disable mask mode. */
void linenoiseMaskModeDisable(void) {
    maskmode = 0;
}

/* Set if to use or not the multi line mode. */
void linenoiseSetMultiLine(int ml) {
    mlmode = ml;
}

/* Return true if the terminal name is in the list of terminals we know are
 * not able to understand basic escape sequences. */
static int isUnsupportedTerm(void) {
    int j;
    char *term = getenv("TERM");
    if (term == NULL) return 0;
    for (j = 0; unsupported_term[j]; j++) {
        if (!strcasecmp(term,unsupported_term[j])) {
            return 1;
        }
    }
    return 0;
}

/* Raw mode: 1960's magic. */
static int enableRawMode(int fd) {
    struct termios raw;
    if (!isatty(fileno(stdin))) goto fatal;
    if (!atexit_registered) {
        atexit(linenoiseAtExit);
        atexit_registered = 1;
    }
    if (tcgetattr(fd,&orig_termios) == -1) goto fatal;
    raw = orig_termios;  /* modify the original mode */
    /* input modes: no break, no CR to NL, no parity check, no strip char,
     * no start/stop output control. */
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    /* output modes - disable post processing */
    raw.c_oflag &= ~(OPOST);
    /* control modes - set 8 bit chars */
    raw.c_cflag |= (CS8);
    /* local modes - echoing off, canonical off, no extended functions,
     * no signal chars (^Z,^C) */
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN);
    /* control chars - set return condition: min number of bytes and timer.
     * We want read to return every single byte, without timeout. */
    raw.c_cc[VMIN] = 1; raw.c_cc[VTIME] = 0; /* 1 byte, no timer */
    /* put terminal in raw mode */
    if (tcsetattr(fd,TCSANOW,&raw) < 0) goto fatal;
    rawmode = 1;
    return 0;
fatal:
    errno = ENOTTY;
    return -1;
}

void linenoiseDisableRawMode(int fd) {
    /* Don't even check the return value as it's too late. */
    if (!isatty(fileno(stdin))) return;
    if (rawmode && tcsetattr(fd,TCSAFLUSH,&orig_termios) != -1)
        rawmode = 0;
}

/* Use the ESC [6n escape sequence to query the horizontal cursor position
 * and return it. On error -1 is returned, on success the position of the
 * cursor. */
static int getCursorPosition(int ifd, int ofd) {
    char buf[32];
    int cols, rows;
    unsigned int i = 0;
    /* Report cursor location */
    if (write(ofd, "\e[6n", 4) != 4) return -1;
    /* Read the response: ESC [ rows ; cols R */
    while (i < sizeof(buf)-1) {
        if (read(ifd,buf+i,1) != 1) break;
        if (buf[i] == 'R') break;
        i++;
    }
    buf[i] = '\0';
    /* Parse it. */
    if (buf[0] != '\e' || buf[1] != '[') return -1;
    if (sscanf(buf+2,"%d;%d",&rows,&cols) != 2) return -1;
    return cols;
}

/* Try to get the number of columns in the current terminal, or assume 80
 * if it fails. */
static int getColumns(int ifd, int ofd) {
    struct winsize ws;
    if (ioctl(1, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        /* ioctl() failed. Try to query the terminal itself. */
        int start, cols;
        /* Get the initial position so we can restore it later. */
        start = getCursorPosition(ifd,ofd);
        if (start == -1) goto failed;
        /* Go to right margin and get position. */
        if (write(ofd,"\e[999C",6) != 6) goto failed;
        cols = getCursorPosition(ifd,ofd);
        if (cols == -1) goto failed;
        /* Restore position. */
        if (cols > start) {
            char seq[32];
            snprintf(seq,32,"\e[%dD",cols-start);
            if (write(ofd,seq,strlen(seq)) == -1) {
                /* Can't recover... */
            }
        }
        return cols;
    } else {
        return ws.ws_col;
    }
failed:
    return 80;
}

/* Clear the screen. Used to handle ctrl+l */
void linenoiseClearScreen(void) {
    if (write(fileno(stdout),"\e[H\e[2J",7) <= 0) {
        /* nothing to do, just to avoid warning. */
    }
}

/* Beep, used for completion when there is nothing to complete or when all
 * the choices were already shown. */
static void linenoiseBeep(void) {
    /* NOOOO */
    /* fprintf(stderr, "\x7"); */
    fflush(stderr);
}

/* ============================== Completion ================================ */

/* Free a list of completion option populated by linenoiseAddCompletion(). */
static void freeCompletions(linenoiseCompletions *lc) {
    size_t i;
    for (i = 0; i < lc->len; i++)
        free(lc->cvec[i]);
    if (lc->cvec != NULL)
        free(lc->cvec);
}

/* This is an helper function for linenoiseEdit() and is called when the
 * user types the <tab> key in order to complete the string currently in the
 * input.
 *
 * The state of the editing is encapsulated into the pointed linenoiseState
 * structure as described in the structure definition. */
static int completeLine(struct linenoiseState *ls, char *seq, int size) {
    linenoiseCompletions lc = { 0, NULL };
    int nwritten, nread = 0;
    completionCallback(ls->buf,&lc);
    if (!lc.len) {
        linenoiseBeep();
    } else {
        size_t stop = 0, i = 0;
        while(!stop) {
            /* Show completion or original buffer */
            if (i < lc.len) {
                struct linenoiseState saved = *ls;
                ls->len = ls->pos = strlen(lc.cvec[i]);
                ls->buf = lc.cvec[i];
                refreshLine(ls);
                ls->len = saved.len;
                ls->pos = saved.pos;
                ls->buf = saved.buf;
            } else {
                refreshLine(ls);
            }
            nread = readansi(ls->ifd,seq,size);
            if (nread <= 0) {
                freeCompletions(&lc);
                return -1;
            }
            switch (seq[0]) {
                case '\t':
                    i = (i+1) % (lc.len+1);
                    if (i == lc.len) linenoiseBeep();
                    break;
                default:
                    /* Update buffer and return */
                    if (i < lc.len) {
                        nwritten = snprintf(ls->buf,ls->buflen,"%s",lc.cvec[i]);
                        ls->len = ls->pos = nwritten;
                    }
                    stop = 1;
                    break;
            }
        }
    }
    freeCompletions(&lc);
    return nread;
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
    size_t len = strlen(str);
    char *copy, **cvec;
    copy = malloc(len+1);
    if (copy == NULL) return;
    memcpy(copy,str,len+1);
    cvec = realloc(lc->cvec,sizeof(char*)*(lc->len+1));
    if (cvec == NULL) {
        free(copy);
        return;
    }
    lc->cvec = cvec;
    lc->cvec[lc->len++] = copy;
}

/* =========================== Line editing ================================= */

/* We define a very simple "append buffer" structure, that is an heap
 * allocated string where we can append to. This is useful in order to
 * write all the escape sequences in a buffer and flush them to the standard
 * output in a single call, to avoid flickering effects. */
struct abuf{
    char *b;
    int len;
};

static void abInit(struct abuf *ab) {
    ab->b = NULL;
    ab->len = 0;
}

static void abAppend(struct abuf *ab, const char *s, int len) {
    char *new = realloc(ab->b,ab->len+len);
    if (new == NULL) return;
    memcpy(new+ab->len,s,len);
    ab->b = new;
    ab->len += len;
}

static void abFree(struct abuf *ab) {
    free(ab->b);
}

/* Helper of refreshSingleLine() and refreshMultiLine() to show hints
 * to the right of the prompt. */
static void refreshShowHints(struct abuf *ab, struct linenoiseState *l, int plen) {
    char seq[64];
    if (hintsCallback && plen+l->len < l->cols) {
        int color = -1, bold = 0;
        char *hint = hintsCallback(l->buf,&color,&bold);
        if (hint) {
            int hintlen = strlen(hint);
            int hintmaxlen = l->cols-(plen+l->len);
            if (hintlen > hintmaxlen) hintlen = hintmaxlen;
            if (bold == 1 && color == -1) color = 37;
            if (color != -1 || bold != 0)
                snprintf(seq,64,"\e[%d;%d;49m",bold,color);
            else
                seq[0] = '\0';
            abAppend(ab,seq,strlen(seq));
            abAppend(ab,hint,hintlen);
            if (color != -1 || bold != 0)
                abAppend(ab,"\e[0m",4);
            /* Call the function to free the hint returned. */
            if (freeHintsCallback) freeHintsCallback(hint);
        }
    }
}

/* Single line low level line refresh.
 *
 * Rewrite the currently edited line accordingly to the buffer content,
 * cursor position, and number of columns of the terminal. */
static void refreshSingleLine(struct linenoiseState *l) {
    char seq[64];
    size_t plen = strlen(l->prompt);
    int fd = l->ofd;
    char *buf = l->buf;
    size_t len = l->len;
    size_t pos = l->pos;
    struct abuf ab;
    while((plen+pos) >= l->cols) {
        buf++;
        len--;
        pos--;
    }
    while (plen+len > l->cols) {
        len--;
    }
    abInit(&ab);
    /* Cursor to left edge */
    snprintf(seq,64,"\r");
    abAppend(&ab,seq,strlen(seq));
    /* Write the prompt and the current buffer content */
    abAppend(&ab,l->prompt,strlen(l->prompt));
    if (maskmode == 1) {
        while (len--) abAppend(&ab,"*",1);
    } else {
        abAppend(&ab,buf,len);
    }
    /* Show hits if any. */
    refreshShowHints(&ab,l,plen);
    /* Erase to right */
    snprintf(seq,64,"\e[0K");
    abAppend(&ab,seq,strlen(seq));
    /* Move cursor to original position. */
    snprintf(seq,64,"\r\e[%dC", (int)(pos+plen));
    abAppend(&ab,seq,strlen(seq));
    if (write(fd,ab.b,ab.len) == -1) {} /* Can't recover from write error. */
    abFree(&ab);
}

/* Multi line low level line refresh.
 *
 * Rewrite the currently edited line accordingly to the buffer content,
 * cursor position, and number of columns of the terminal. */
static void refreshMultiLine(struct linenoiseState *l) {
    char seq[64];
    int plen = strlen(l->prompt);
    int rows = (plen+l->len+l->cols-1)/l->cols; /* rows used by current buf. */
    int rpos = (plen+l->oldpos+l->cols)/l->cols; /* cursor relative row. */
    int rpos2; /* rpos after refresh. */
    int col; /* colum position, zero-based. */
    int old_rows = l->maxrows;
    int fd = l->ofd, j;
    struct abuf ab;
    /* Update maxrows if needed. */
    if (rows > (int)l->maxrows) l->maxrows = rows;
    /* First step: clear all the lines used before. To do so start by
     * going to the last row. */
    abInit(&ab);
    if (old_rows-rpos > 0) {
        lndebug("go down %d", old_rows-rpos);
        snprintf(seq,64,"\e[%dB", old_rows-rpos);
        abAppend(&ab,seq,strlen(seq));
    }
    /* Now for every row clear it, go up. */
    for (j = 0; j < old_rows-1; j++) {
        lndebug("clear+up");
        snprintf(seq,64,"\r\e[0K\e[1A");
        abAppend(&ab,seq,strlen(seq));
    }
    /* Clean the top line. */
    lndebug("clear");
    snprintf(seq,64,"\r\e[0K");
    abAppend(&ab,seq,strlen(seq));
    /* Write the prompt and the current buffer content */
    abAppend(&ab,l->prompt,strlen(l->prompt));
    if (maskmode == 1) {
        unsigned int i;
        for (i = 0; i < l->len; i++) abAppend(&ab,"*",1);
    } else {
        abAppend(&ab,l->buf,l->len);
    }
    /* Show hits if any. */
    refreshShowHints(&ab,l,plen);
    /* If we are at the very end of the screen with our prompt, we need to
     * emit a newline and move the prompt to the first column. */
    if (l->pos &&
        l->pos == l->len &&
        (l->pos+plen) % l->cols == 0)
    {
        lndebug("<newline>");
        abAppend(&ab,"\n",1);
        snprintf(seq,64,"\r");
        abAppend(&ab,seq,strlen(seq));
        rows++;
        if (rows > (int)l->maxrows) l->maxrows = rows;
    }
    /* Move cursor to right position. */
    rpos2 = (plen+l->pos+l->cols)/l->cols; /* current cursor relative row. */
    lndebug("rpos2 %d", rpos2);
    /* Go up till we reach the expected positon. */
    if (rows-rpos2 > 0) {
        lndebug("go-up %d", rows-rpos2);
        snprintf(seq,64,"\e[%dA", rows-rpos2);
        abAppend(&ab,seq,strlen(seq));
    }
    /* Set column. */
    col = (plen+(int)l->pos) % (int)l->cols;
    lndebug("set col %d", 1+col);
    if (col)
        snprintf(seq,64,"\r\e[%dC", col);
    else
        snprintf(seq,64,"\r");
    abAppend(&ab,seq,strlen(seq));
    lndebug("\n");
    l->oldpos = l->pos;
    if (write(fd,ab.b,ab.len) == -1) {} /* Can't recover from write error. */
    abFree(&ab);
}

/* Calls the two low level functions refreshSingleLine() or
 * refreshMultiLine() according to the selected mode. */
static void refreshLine(struct linenoiseState *l) {
    if (mlmode)
        refreshMultiLine(l);
    else
        refreshSingleLine(l);
}

/* Insert the character 'c' at cursor current position.
 *
 * On error writing to the terminal -1 is returned, otherwise 0. */
static int linenoiseEditInsert(struct linenoiseState *l, char c) {
    if (l->len < l->buflen) {
        if (l->len == l->pos) {
            l->buf[l->pos] = c;
            l->pos++;
            l->len++;
            l->buf[l->len] = '\0';
            if ((!mlmode && l->plen+l->len < l->cols && !hintsCallback)) {
                /* Avoid a full update of the line in the
                 * trivial case. */
                char d = (maskmode==1) ? '*' : c;
                if (write(l->ofd,&d,1) == -1) return -1;
            } else {
                refreshLine(l);
            }
        } else {
            memmove(l->buf+l->pos+1,l->buf+l->pos,l->len-l->pos);
            l->buf[l->pos] = c;
            l->len++;
            l->pos++;
            l->buf[l->len] = '\0';
            refreshLine(l);
        }
    }
    return 0;
}

/* Move cursor on the left. */
static void linenoiseEditMoveLeft(struct linenoiseState *l) {
    if (l->pos > 0) {
        l->pos--;
        refreshLine(l);
    }
}

static bool IsSeparator(int c) {
    return !(isalnum(c) || c >= 128);
}

/* Move cursor on the left. */
static void linenoiseEditMoveLeftWord(struct linenoiseState *l) {
    if (l->pos > 0) {
        while (l->pos > 0 && IsSeparator(l->buf[l->pos-1])) l->pos--;
        while (l->pos > 0 && !IsSeparator(l->buf[l->pos-1])) l->pos--;
        refreshLine(l);
    }
}

/* Move cursor on the right. */
static void linenoiseEditMoveRightWord(struct linenoiseState *l) {
    if (l->pos != l->len) {
        while (l->pos < l->len && IsSeparator(l->buf[l->pos])) l->pos++;
        while (l->pos < l->len && !IsSeparator(l->buf[l->pos])) l->pos++;
        refreshLine(l);
    }
}

/* Move cursor on the right. */
static void linenoiseEditMoveRight(struct linenoiseState *l) {
    if (l->pos != l->len) {
        l->pos++;
        refreshLine(l);
    }
}

/* Move cursor to the start of the line. */
static void linenoiseEditMoveHome(struct linenoiseState *l) {
    if (l->pos != 0) {
        l->pos = 0;
        refreshLine(l);
    }
}

/* Move cursor to the end of the line. */
static void linenoiseEditMoveEnd(struct linenoiseState *l) {
    if (l->pos != l->len) {
        l->pos = l->len;
        refreshLine(l);
    }
}

/* Substitute the currently edited line with the next or previous history
 * entry as specified by 'dir'. */
static void linenoiseEditHistoryNext(struct linenoiseState *l, int dir) {
    if (history_len > 1) {
        /* Update the current history entry before to
         * overwrite it with the next one. */
        free(history[history_len - 1 - l->history_index]);
        history[history_len - 1 - l->history_index] = strdup(l->buf);
        /* Show the new entry */
        l->history_index += (dir == LINENOISE_HISTORY_PREV) ? 1 : -1;
        if (l->history_index < 0) {
            l->history_index = 0;
            return;
        } else if (l->history_index >= history_len) {
            l->history_index = history_len-1;
            return;
        }
        strncpy(l->buf,history[history_len - 1 - l->history_index],l->buflen);
        l->buf[l->buflen-1] = '\0';
        l->len = l->pos = strlen(l->buf);
        refreshLine(l);
    }
}

/* Delete the character at the right of the cursor without altering the cursor
 * position. Basically this is what happens with the "Delete" keyboard key. */
static void linenoiseEditDelete(struct linenoiseState *l) {
    if (l->len > 0 && l->pos < l->len) {
        memmove(l->buf+l->pos,l->buf+l->pos+1,l->len-l->pos-1);
        l->len--;
        l->buf[l->len] = '\0';
        refreshLine(l);
    }
}

/* Backspace implementation. */
static void linenoiseEditBackspace(struct linenoiseState *l) {
    if (l->pos > 0 && l->len > 0) {
        memmove(l->buf+l->pos-1,l->buf+l->pos,l->len-l->pos);
        l->pos--;
        l->len--;
        l->buf[l->len] = '\0';
        refreshLine(l);
    }
}

static void linenoiseEditDeleteNextWord(struct linenoiseState *l) {
    size_t i = l->pos;
    while (i != l->len && IsSeparator(l->buf[i])) i++;
    while (i != l->len && !IsSeparator(l->buf[i])) i++;
    memmove(l->buf+l->pos,l->buf+i,l->len-i);
    l->len -= i - l->pos;
    refreshLine(l);
}

/* Delete the previous word, maintaining the cursor at the start of the
 * current word. */
static void linenoiseEditDeletePrevWord(struct linenoiseState *l) {
    size_t diff;
    size_t old_pos = l->pos;
    while (l->pos > 0 && IsSeparator(l->buf[l->pos-1])) l->pos--;
    while (l->pos > 0 && !IsSeparator(l->buf[l->pos-1])) l->pos--;
    diff = old_pos - l->pos;
    memmove(l->buf+l->pos,l->buf+old_pos,l->len-old_pos+1);
    l->len -= diff;
    refreshLine(l);
}

static void linenoiseKill(struct linenoiseState *l, size_t i, size_t n) {
    free(l->killed);
    l->killed = memcpy(malloc(n + 1), l->buf + i, n);
    l->killed[n] = '\0';
}

static void linenoiseEditKillLeft(struct linenoiseState *l) {
    size_t diff;
    size_t old_pos = l->pos;
    l->pos = 0;
    diff = old_pos - l->pos;
    memmove(l->buf+l->pos,l->buf+old_pos,l->len-old_pos+1);
    l->len -= diff;
    refreshLine(l);
}

static void linenoiseEditKillRight(struct linenoiseState *l) {
    l->buf[l->pos] = '\0';
    l->len = l->pos;
    refreshLine(l);
}

/* This function is the core of the line editing capability of linenoise.
 * It expects 'fd' to be already in "raw mode" so that every key pressed
 * will be returned ASAP to read().
 *
 * The resulting string is put into 'buf' when the user type enter, or
 * when ctrl+d is typed.
 *
 * The function returns the length of the current buffer. */
static int linenoiseEdit(int stdin_fd, int stdout_fd, char *buf, size_t buflen, const char *prompt) {
    struct linenoiseState l;
    /* Populate the linenoise state that we pass to functions implementing
     * specific editing functionalities. */
    l.ifd = stdin_fd;
    l.ofd = stdout_fd;
    l.buf = buf;
    l.buflen = buflen;
    l.prompt = prompt;
    l.plen = strlen(prompt);
    l.oldpos = l.pos = 0;
    l.len = 0;
    l.cols = getColumns(stdin_fd, stdout_fd);
    l.maxrows = 0;
    l.history_index = 0;
    /* Buffer starts empty. */
    l.buf[0] = '\0';
    l.buflen--; /* Make sure there is always space for the nulterm */
    /* The latest history entry is always our current buffer, that
     * initially is just an empty string. */
    linenoiseHistoryAdd("");
    if (write(l.ofd,prompt,l.plen) == -1) return -1;
    while(1) {
        int i;
        int nread;
        char seq[32];
        nread = readansi(l.ifd,seq,sizeof(seq));
        if (nread <= 0) return l.len;
        /* Only autocomplete when the callback is set. It returns < 0 when
         * there was an error reading from fd. Otherwise it will return the
         * character that should be handled next. */
        if (seq[0] == '\t' && completionCallback) {
            nread = completeLine(&l, seq, sizeof(seq));
            if (nread < 0) return l.len;
            if (!nread) continue;
        }
        switch(seq[0]) {
        case '\r':    /* enter */
            history_len--;
            free(history[history_len]);
            if (mlmode) linenoiseEditMoveEnd(&l);
            if (hintsCallback) {
                /* Force a refresh without hints to leave the previous
                 * line as the user typed it after a newline. */
                linenoiseHintsCallback *hc = hintsCallback;
                hintsCallback = NULL;
                refreshLine(&l);
                hintsCallback = hc;
            }
            return (int)l.len;
        case CTRL('C'):
            errno = EAGAIN;
            return -1;
        case CTRL('?'): /* backspace a.k.a. 0177 a.k.a. 127 */
        case CTRL('H'):
            linenoiseEditBackspace(&l);
            break;
        case CTRL('D'):     /* remove char at right of cursor, or if the
                               line is empty, act as end-of-file. */
            if (l.len > 0) {
                linenoiseEditDelete(&l);
            } else {
                history_len--;
                free(history[history_len]);
                return -1;
            }
            break;
        case CTRL('T'):    /* swaps current character with previous. */
            if (l.pos > 0 && l.pos < l.len) {
                int aux = buf[l.pos-1];
                buf[l.pos-1] = buf[l.pos];
                buf[l.pos] = aux;
                if (l.pos != l.len-1) l.pos++;
                refreshLine(&l);
            }
            break;
        case CTRL('B'):
            linenoiseEditMoveLeft(&l);
            break;
        case CTRL('F'):
            linenoiseEditMoveRight(&l);
            break;
        case CTRL('P'):
            linenoiseEditHistoryNext(&l, LINENOISE_HISTORY_PREV);
            break;
        case CTRL('N'):
            linenoiseEditHistoryNext(&l, LINENOISE_HISTORY_NEXT);
            break;
        case '\e': /* escape sequence */
            /* Read the next two bytes representing the escape sequence.
             * Use two calls to handle slow terminals returning the two
             * chars at different times. */
            if (nread < 2) break;
            if (seq[1] == '[') {
                if (nread < 3) break;
                if (seq[2] >= '0' && seq[2] <= '9') {
                    if (nread < 4) break;
                    if (seq[3] == '~') {
                        switch(seq[2]) {
                        case '1': /* "\e[1~" is home */
                            linenoiseEditMoveHome(&l);
                            break;
                        case '3': /* "\e[3~" is delete */
                            linenoiseEditDelete(&l);
                            break;
                        case '4': /* "\e[4~" is end */
                            linenoiseEditMoveEnd(&l);
                            break;
                        }
                    }
                } else {
                    switch(seq[2]) {
                    case 'A': /* "\e[A" is up */
                        linenoiseEditHistoryNext(&l, LINENOISE_HISTORY_PREV);
                        break;
                    case 'B': /* "\e[B" is down */
                        linenoiseEditHistoryNext(&l, LINENOISE_HISTORY_NEXT);
                        break;
                    case 'C': /* "\e[C" is right */
                        linenoiseEditMoveRight(&l);
                        break;
                    case 'D': /* "\e[D" is left */
                        linenoiseEditMoveLeft(&l);
                        break;
                    case 'H': /* "\e[H" is home */
                        linenoiseEditMoveHome(&l);
                        break;
                    case 'F': /* "\e[F" is end */
                        linenoiseEditMoveEnd(&l);
                        break;
                    }
                }
            }
            else if (seq[1] == 'O') {
                if (nread < 3) break;
                switch(seq[2]) {
                case 'H': /* "\eOH" is home */
                    linenoiseEditMoveHome(&l);
                    break;
                case 'F': /* "\eOF" is end */
                    linenoiseEditMoveEnd(&l);
                    break;
                }
            }
            else if (seq[1] == 'b') { /* "\eb" is alt-b */
                linenoiseEditMoveLeftWord(&l);
            }
            else if (seq[1] == 'f') { /* "\ef" is alt-f */
                linenoiseEditMoveRightWord(&l);
            }
            else if (seq[1] == 'd') { /* "\ed" is alt-d */
                linenoiseEditDeleteNextWord(&l);
            }
            else if (seq[1] == CTRL('H')) { /* "\e\b" is ctrl-alt-h */
                linenoiseEditDeletePrevWord(&l);
            }
            break;
        default:
            if (32 <= seq[0] && seq[0] < 127) {
                if (linenoiseEditInsert(&l,seq[0])==-1) {
                    return -1;
                }
            }
            break;
        case CTRL('U'): /* delete the line backwards */
            linenoiseEditKillLeft(&l);
            break;
        case CTRL('K'): /* delete from current to end of line */
            linenoiseEditKillRight(&l);
            break;
        case CTRL('A'): /* go to the start of the line */
            linenoiseEditMoveHome(&l);
            break;
        case CTRL('E'): /* go to the end of the line */
            linenoiseEditMoveEnd(&l);
            break;
        case CTRL('L'): /* clear screen */
            linenoiseClearScreen();
            refreshLine(&l);
            break;
        case CTRL('W'): /* delete previous word */
            linenoiseEditDeletePrevWord(&l);
            break;
        }
    }
    return l.len;
}

/* This function calls the line editing function linenoiseEdit() using
 * the STDIN file descriptor set in raw mode. */
static int linenoiseRaw(char *buf, size_t buflen, const char *prompt) {
    int count;
    if (buflen == 0) {
        errno = EINVAL;
        return -1;
    }
    if (enableRawMode(fileno(stdin)) == -1) return -1;
    count = linenoiseEdit(fileno(stdin), fileno(stdout), buf, buflen, prompt);
    linenoiseDisableRawMode(fileno(stdin));
    if (count != -1) printf("\n");
    return count;
}

/* This function is called when linenoise() is called with the standard
 * input file descriptor not attached to a TTY. So for example when the
 * program using linenoise is called in pipe or with a file redirected
 * to its standard input. In this case, we want to be able to return the
 * line regardless of its length (by default we are limited to 4k). */
static char *linenoiseNoTTY(void) {
    char *line = NULL;
    size_t len = 0, maxlen = 0;
    while(1) {
        if (len == maxlen) {
            if (maxlen == 0) maxlen = 16;
            maxlen *= 2;
            char *oldval = line;
            line = realloc(line,maxlen);
            if (line == NULL) {
                if (oldval) free(oldval);
                return NULL;
            }
        }
        int c = fgetc(stdin);
        if (c == EOF || c == '\n') {
            if (c == EOF && len == 0) {
                free(line);
                return NULL;
            } else {
                line[len] = '\0';
                return line;
            }
        } else {
            line[len] = c;
            len++;
        }
    }
}

/* The high level function that is the main API of the linenoise library.
 * This function checks if the terminal has basic capabilities, just checking
 * for a blacklist of stupid terminals, and later either calls the line
 * editing function or uses dummy fgets() so that you will be able to type
 * something even in the most desperate of the conditions. */
char *linenoise(const char *prompt) {
    int count;
    char *buf;
    if (!isatty(fileno(stdin))) {
        /* Not a tty: read from file / pipe. In this mode we don't want any
         * limit to the line size, so we call a function to handle that. */
        return linenoiseNoTTY();
    } else if (isUnsupportedTerm()) {
        printf("%s",prompt);
        fflush(stdout);
        return chomp(xgetline(stdin));
    } else {
        buf = malloc(LINENOISE_MAX_LINE);
        count = linenoiseRaw(buf,LINENOISE_MAX_LINE,prompt);
        if (count == -1) return NULL;
        return strdup(buf);
    }
}

/* This is just a wrapper the user may want to call in order to make sure
 * the linenoise returned buffer is freed with the same allocator it was
 * created with. Useful when the main program is using an alternative
 * allocator. */
void linenoiseFree(void *ptr) {
    free(ptr);
}

/* Free the history, but does not reset it. Only used when we have to
 * exit() to avoid memory leaks are reported by valgrind & co. */
void linenoiseHistoryFree(void) {
    int j;
    if (history) {
        for (j = 0; j < history_len; j++)
            free(history[j]);
        free(history);
        history_len = 0;
        history = 0;
    }
}

/* At exit we'll try to fix the terminal to the initial conditions. */
static void linenoiseAtExit(void) {
    linenoiseDisableRawMode(fileno(stdin));
    linenoiseHistoryFree();
}

/* This is the API call to add a new entry in the linenoise history.
 * It uses a fixed array of char pointers that are shifted (memmoved)
 * when the history max length is reached in order to remove the older
 * entry and make room for the new one, so it is not exactly suitable for huge
 * histories, but will work well for a few hundred of entries.
 *
 * Using a circular buffer is smarter, but a bit more complex to handle. */
int linenoiseHistoryAdd(const char *line) {
    char *linecopy;
    if (history_max_len == 0) return 0;
    /* Initialization on first call. */
    if (history == NULL) {
        history = malloc(sizeof(char*)*history_max_len);
        if (history == NULL) return 0;
        memset(history,0,(sizeof(char*)*history_max_len));
    }
    /* Don't add duplicated lines. */
    if (history_len && !strcmp(history[history_len-1], line)) return 0;
    /* Add an heap allocated copy of the line in the history.
     * If we reached the max length, remove the older line. */
    linecopy = strdup(line);
    if (!linecopy) return 0;
    if (history_len == history_max_len) {
        free(history[0]);
        memmove(history,history+1,sizeof(char*)*(history_max_len-1));
        history_len--;
    }
    history[history_len] = linecopy;
    history_len++;
    return 1;
}

/* Set the maximum length for the history. This function can be called even
 * if there is already some history, the function will make sure to retain
 * just the latest 'len' elements if the new history length value is smaller
 * than the amount of items already inside the history. */
int linenoiseHistorySetMaxLen(int len) {
    char **new;
    if (len < 1) return 0;
    if (history) {
        int tocopy = history_len;
        new = malloc(sizeof(char*)*len);
        if (new == NULL) return 0;
        /* If we can't copy everything, free the elements we'll not use. */
        if (len < tocopy) {
            int j;
            for (j = 0; j < tocopy-len; j++) free(history[j]);
            tocopy = len;
        }
        memset(new,0,sizeof(char*)*len);
        memcpy(new,history+(history_len-tocopy), sizeof(char*)*tocopy);
        free(history);
        history = new;
    }
    history_max_len = len;
    if (history_len > history_max_len)
        history_len = history_max_len;
    return 1;
}

/* Save the history in the specified file. On success 0 is returned
 * otherwise -1 is returned. */
int linenoiseHistorySave(const char *filename) {
    mode_t old_umask = umask(S_IXUSR|S_IRWXG|S_IRWXO);
    FILE *fp;
    int j;
    fp = fopen(filename,"w");
    umask(old_umask);
    if (fp == NULL) return -1;
    chmod(filename,S_IRUSR|S_IWUSR);
    for (j = 0; j < history_len; j++)
        fprintf(fp,"%s\n",history[j]);
    fclose(fp);
    return 0;
}

/* Load the history from the specified file. If the file does not exist
 * zero is returned and no operation is performed.
 *
 * If the file exists and the operation succeeded 0 is returned, otherwise
 * on error -1 is returned. */
int linenoiseHistoryLoad(const char *filename) {
    FILE *fp = fopen(filename,"r");
    char buf[LINENOISE_MAX_LINE];
    if (fp == NULL) return -1;
    while (fgets(buf,LINENOISE_MAX_LINE,fp) != NULL) {
        char *p;
        p = strchr(buf,'\r');
        if (!p) p = strchr(buf,'\n');
        if (p) *p = '\0';
        linenoiseHistoryAdd(buf);
    }
    fclose(fp);
    return 0;
}
