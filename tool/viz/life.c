/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "dsp/scale/scale.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/termios.h"
#include "libc/calls/struct/winsize.h"
#include "libc/calls/termios.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/popcnt.h"
#include "libc/intrin/safemacros.internal.h"
#include "libc/intrin/xchg.internal.h"
#include "libc/limits.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/nt2sysv.h"
#include "libc/nt/comdlg.h"
#include "libc/nt/dll.h"
#include "libc/nt/enum/bitblt.h"
#include "libc/nt/enum/color.h"
#include "libc/nt/enum/cw.h"
#include "libc/nt/enum/idc.h"
#include "libc/nt/enum/mb.h"
#include "libc/nt/enum/mf.h"
#include "libc/nt/enum/mk.h"
#include "libc/nt/enum/ofn.h"
#include "libc/nt/enum/rdw.h"
#include "libc/nt/enum/sc.h"
#include "libc/nt/enum/sw.h"
#include "libc/nt/enum/wm.h"
#include "libc/nt/enum/ws.h"
#include "libc/nt/events.h"
#include "libc/nt/messagebox.h"
#include "libc/nt/paint.h"
#include "libc/nt/struct/openfilename.h"
#include "libc/nt/windows.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/sysconf.h"
#include "libc/sock/struct/pollfd.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/str/strwidth.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/sysv/consts/ex.h"
#include "libc/sysv/consts/exit.h"
#include "libc/sysv/consts/poll.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/termios.h"
#include "libc/time/time.h"
#include "third_party/getopt/getopt.internal.h"

/**
 * @fileoverview Conway's Game of Life
 *
 * The Game of Life, also known simply as Life, is a cellular automaton
 * devised by the British mathematician John Horton Conway in 1970. It
 * is Turing complete and can simulate a universal constructor or any
 * other Turing machine.
 *
 * There's about 20 million Software Engineers in the world, which means
 * Game of Life has likely been implemented 20 million times before. Why
 * do we need this one?
 *
 *   - It's a tutorial on how to build an Actually Portable Executable
 *     that'll run as a GUI on Windows, and as a TUI on Linux/Mac/BSDs
 *     using roughly one thousand lines of code. For a much better GUI
 *     that's not as hackable, try Golly: http://golly.sourceforge.net
 *
 *   - It's a tutorial on how to implement XTERM mouse cursor dragging
 *     where zooming in/out can be performed too using ctrl+mousewheel
 *     which is an underused (but easily implemented) terminal feature
 *     that even the Windows10 Command Prompt supports these days.
 *
 *   - It uses bitboards. That's almost as simple as the naive approach
 *     but goes significantly faster, needing 150 picoseconds per board
 *     position. See "Bitboard Methods for Games" by Cameron Browne for
 *     further details on how it works. More advanced algorithms exist,
 *     such as Hashlife: quadtree memoization to make humongous numbers
 *     of generations tractable.
 *
 * Here's how you can compile this program on Linux:
 *
 *     git clone https://github.com/jart/cosmopolitan && cd cosmopolitan
 *     make -j12 o//tool/viz/life.com
 *
 * The output binary works on Linux, Windows, Mac, and FreeBSD:
 *
 *     o//tool/viz/life.com
 *
 * @see https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life
 * @see https://www.conwaylife.com/wiki/Run_Length_Encoded
 * @see http://golly.sourceforge.net/
 */

#define USAGE \
  " [-zNW] [-w WIDTH] [-h HEIGHT] [PATH]\n\
\n\
DESCRIPTION\n\
\n\
  Conway's Game of Life\n\
\n\
FLAGS\n\
\n\
  -?         help\n\
  -z         zoom\n\
  -w INT     board width\n\
  -h INT     board height\n\
  -N         natural scrolling\n\
  -W         white terminal background\n\
\n\
SHORTCUTS\n\
\n\
  space      step\n\
  left+drag  draw\n\
  right+drag move\n\
  ctrl+wheel zoom\n\
  ctrl+t     turbo\n\
  alt+t      slowmo\n\
  R          reset\n\
  q          quit\n\
\n"

#define MAXZOOM   14
#define VOIDSPACE "."
#define ALT       (1 << 29)

#define INTERRUPTED 1
#define RESIZED     2

#define IDM_ABOUT 0x10
#define IDM_OPEN  0x20

#define MOUSE_LEFT_DOWN       0
#define MOUSE_MIDDLE_DOWN     1
#define MOUSE_RIGHT_DOWN      2
#define MOUSE_LEFT_UP         4
#define MOUSE_MIDDLE_UP       5
#define MOUSE_RIGHT_UP        6
#define MOUSE_LEFT_DRAG       32
#define MOUSE_MIDDLE_DRAG     33
#define MOUSE_RIGHT_DRAG      34
#define MOUSE_WHEEL_UP        64
#define MOUSE_WHEEL_DOWN      65
#define MOUSE_CTRL_WHEEL_UP   80
#define MOUSE_CTRL_WHEEL_DOWN 81

struct Buffer {
  unsigned i, n;
  char *p;
};

static bool erase;
static bool white;
static bool natural;
static bool mousemode;
static bool isdragging;
static bool dimensioned;

static int out;
static int line;
static int column;
static int action;
static int color[2];

static long top;
static long bottom;
static long left;
static long right;
static long tyn;
static long txn;
static long byn;
static long bxn;
static long zoom;
static long speed;
static long save_y;
static long save_x;
static long save_top;
static long save_left;
static long generation;

static uint64_t *board;
static uint64_t *board2;
static size_t boardsize;
static int64_t oldcursor;

static struct Buffer buffer;
static struct termios oldterm;

static char name[64];
static char statusline[256];
static char16_t statusline16[256];

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § game of life » algorithm                                  ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define LEFT   0x0101010101010101ul
#define RIGHT  0x8080808080808080ul
#define TOP    0x00000000000000FFul
#define BOTTOM 0xFF00000000000000ul

#define CTRL(C)      ((C) ^ 0100)
#define GOUP(x)      ((x) >> 8)
#define GODOWN(x)    ((x) << 8)
#define GORIGHT(x)   (((x) & ~RIGHT) << 1)
#define GOLEFT(x)    (((x) & ~LEFT) >> 1)
#define LEFTMOST(x)  ((x)&LEFT)
#define RIGHTMOST(x) ((x)&RIGHT)
#define TOPMOST(x)   ((x)&TOP)
#define BOTMOST(x)   ((x)&BOTTOM)

#define ADD(X)       \
  do {               \
    uint64_t c1, c2; \
    c1 = r0 & (X);   \
    c2 = r1 & c1;    \
    r0 ^= (X);       \
    r1 ^= c1;        \
    r2 |= c2;        \
  } while (0)

#define STEP(RES, B00, B01, B02, B10, B11, B12, B20, B21, B22)             \
  do {                                                                     \
    uint64_t r0 = 0, r1 = 0, r2 = 0;                                       \
    ADD(GORIGHT(GODOWN(B11)) | GORIGHT(BOTMOST(B01) >> 56) |               \
        GODOWN(RIGHTMOST(B10) >> 7) | BOTMOST(RIGHTMOST(B00)) >> 7 >> 56); \
    ADD(GORIGHT(B11) | RIGHTMOST(B10) >> 7);                               \
    ADD(GORIGHT(GOUP(B11)) | GORIGHT(TOPMOST(B21) << 56) |                 \
        GOUP(RIGHTMOST(B10) >> 7) | TOPMOST(RIGHTMOST(B20)) >> 7 << 56);   \
    ADD(GODOWN(B11) | BOTMOST(B01) >> 56);                                 \
    ADD(GOUP(B11) | TOPMOST(B21) << 56);                                   \
    ADD(GOLEFT(GODOWN(B11)) | GOLEFT(BOTMOST(B01) >> 56) |                 \
        GODOWN(LEFTMOST(B12) << 7) | BOTMOST(LEFTMOST(B02)) << 7 >> 56);   \
    ADD(GOLEFT(B11) | LEFTMOST(B12) << 7);                                 \
    ADD(GOLEFT(GOUP(B11)) | GOLEFT(TOPMOST(B21) << 56) |                   \
        GOUP(LEFTMOST(B12) << 7) | TOPMOST(LEFTMOST(B22)) << 7 << 56);     \
    RES = (B11 | r0) & r1 & ~r2;                                           \
  } while (0)

static void SwapBoards(void) {
  uint64_t *t = board;
  board = board2;
  board2 = t;
}

static void Step(void) {
  long y, x, yn, xn;
  yn = byn >> 3;
  xn = bxn >> 3;
  for (y = 0; y < yn; ++y) {
    for (x = 0; x < xn; ++x) {
      STEP(board2[y * xn + x],
           board[(y ? y - 1 : yn - 1) * xn + (x ? x - 1 : xn - 1)],
           board[(y ? y - 1 : yn - 1) * xn + x],
           board[(y ? y - 1 : yn - 1) * xn + (x + 1 < xn ? x + 1 : 0)],
           board[y * xn + (x ? x - 1 : xn - 1)], board[y * xn + x],
           board[y * xn + (x + 1 < xn ? x + 1 : 0)],
           board[(y + 1 < yn ? y + 1 : 0) * xn + (x ? x - 1 : xn - 1)],
           board[(y + 1 < yn ? y + 1 : 0) * xn + x],
           board[(y + 1 < yn ? y + 1 : 0) * xn + (x + 1 < xn ? x + 1 : 0)]);
    }
  }
  SwapBoards();
  ++generation;
}

static bool Test(long y, long x) {
  return (board[(bxn >> 3) * (y >> 3) + (x >> 3)] >>
          (((y & 7) << 3) + (x & 7))) &
         1;
}

static void Set(long y, long x) {
  board[(bxn >> 3) * (y >> 3) + (x >> 3)] |= 1ul << (((y & 7) << 3) + (x & 7));
}

static void Unset(long y, long x) {
  board[(bxn >> 3) * (y >> 3) + (x >> 3)] &=
      ~(1ul << (((y & 7) << 3) + (x & 7)));
}

static long Population(void) {
  return _countbits(board, byn * bxn / 64 * 8);
}

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § game of life » buffer                                     ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

static void AppendData(char *data, unsigned len) {
  char *p;
  unsigned n;
  if (buffer.i + len + 1 > buffer.n) {
    n = MAX(buffer.i + len + 1, MAX(16, buffer.n + (buffer.n >> 1)));
    if (!(p = realloc(buffer.p, n))) return;
    buffer.p = p;
    buffer.n = n;
  }
  memcpy(buffer.p + buffer.i, data, len);
  buffer.p[buffer.i += len] = 0;
}

static void AppendChar(char c) {
  AppendData(&c, 1);
}

#define AppendStr(s) AppendData(s, strlen(s))

static void AppendInt(long x) {
  char ibuf[21];
  AppendData(ibuf, FormatInt64(ibuf, x) - ibuf);
}

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § game of life » board control                              ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

static void Generation(void) {
  long i;
  for (i = 0; i < speed; ++i) {
    Step();
  }
}

static void Dimension(void) {
  if (!dimensioned) {
    top = byn / 2 - tyn / 2;
    left = bxn / 2 - txn / 2;
    dimensioned = true;
  }
  right = left + txn;
  bottom = top + tyn;
}

static void Move(long dy, long dx) {
  long yn, xn;
  yn = zoom ? tyn * 2 : tyn;
  xn = txn;
  top = top + (dy << zoom);
  left = left + (dx << zoom);
  bottom = top + (yn << zoom);
  right = left + (xn << zoom);
}

static void OnUp(void) {
  Move(-1, 0);
}

static void OnDown(void) {
  Move(+1, 0);
}

static void OnLeft(void) {
  Move(0, -1);
}

static void OnRight(void) {
  Move(0, +1);
}

static void OnPageUp(void) {
  Move(-(tyn - 2), 0);
}

static void OnPageDown(void) {
  Move(+(tyn - 2), 0);
}

static void OnTurbo(void) {
  ++speed;
}

static void OnSlowmo(void) {
  --speed;
  if (speed < 1) speed = 1;
}

static void SetZoom(long y, long x, int d) {
  long a, b;
  if ((0 <= y && y < tyn) && (0 <= x && x < txn)) {
    a = zoom;
    b = MIN(MAXZOOM, MAX(0, a + d));
    zoom = b;
    Move(((y << (a + !!a)) - (y << (b + !!b))) >> b,
         ((x << a) - (x << b)) >> b);
  }
}

static void OnZoom(long y, long x) {
  SetZoom(y, x, +1);
}

static void OnUnzoom(long y, long x) {
  SetZoom(y, x, -1);
}

static void OnMouseLeftDrag(long y, long x) {
  int i;
  if (y == save_y && x == save_x) return;
  save_y = y;
  save_x = x;
  y = top + (y << (zoom + !!zoom));
  x = left + (x << zoom);
  y += _rand64() & ((1ul << (zoom + !!zoom)) - 1);
  x += _rand64() & ((1ul << zoom) - 1);
  if (y < 0 || y >= byn) return;
  if (x < 0 || x >= bxn) return;
  if (erase) {
    Unset(y, x);
  } else {
    for (i = 0; i < (2 << zoom); ++i) {
      Set(y + (rand() % (zoom + 1)) - (rand() % (zoom + 1)),
          x + (rand() % (zoom + 1)) - (rand() % (zoom + 1)));
    }
  }
}

static void OnMouseLeftUp(long y, long x) {
  isdragging = false;
}

static void OnMouseLeftDown(long y, long x) {
  isdragging = true;
  save_y = y;
  save_x = x;
  y = top + (y << (zoom + !!zoom));
  x = left + (x << zoom);
  erase = false;
  if (y < 0 || y >= byn) return;
  if (x < 0 || x >= bxn) return;
  if ((erase = Test(y, x))) {
    Unset(y, x);
  } else {
    Set(y, x);
  }
}

static void OnMouseRightUp(long y, long x) {
  isdragging = false;
}

static void OnMouseRightDown(long y, long x) {
  isdragging = true;
  save_y = y;
  save_x = x;
  save_top = top;
  save_left = left;
}

static void OnMouseRightDrag(long y, long x) {
  long dy, dx, h, w;
  dy = (save_y - y) << zoom;
  dx = (save_x - x) << zoom;
  if (zoom) dy <<= 1;
  if (natural) {
    dy = -dy;
    dx = -dx;
  }
  h = bottom - top;
  w = right - left;
  top = save_top + dy;
  left = save_left + dx;
  bottom = top + h;
  right = left + w;
}

static void *NewBoard(size_t *out_size) {
  char *p;
  size_t s, n, k;
  s = (byn * bxn) >> 3;
  k = getauxval(AT_PAGESZ) + ROUNDUP(s, getauxval(AT_PAGESZ));
  n = ROUNDUP(k + getauxval(AT_PAGESZ), sysconf(_SC_PAGESIZE));
  p = _mapanon(n);
  mprotect(p, getauxval(AT_PAGESZ), 0);
  mprotect(p + k, n - k, 0);
  if (out_size) *out_size = n;
  return p + getauxval(AT_PAGESZ);
}

static void FreeBoard(void *p, size_t n) {
  munmap((char *)p - getauxval(AT_PAGESZ), n);
}

static void AllocateBoardsWithHardwareAcceleratedMemorySafety(void) {
  if (board) {
    FreeBoard(board2, boardsize);
    FreeBoard(board, boardsize);
  }
  board = NewBoard(&boardsize);
  board2 = NewBoard(NULL);
}

static void GenerateStatusLine(void) {
  snprintf(statusline, sizeof(statusline),
           "%s :: %,ldg %,ldp %lds %ldx %ld×%ld (%ld,%ld,%ld,%ld)", name,
           generation, Population(), speed, zoom, byn, bxn, left, top, right,
           bottom);
}

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § game of life » files                                      ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

static void OnHeader(void) {
  size_t n;
  if (!buffer.i) return;
  switch (buffer.p[0]) {
    case 'N':
      if (buffer.i > 2) {
        n = MIN(buffer.i - 2, sizeof(name) - 1);
        memcpy(name, buffer.p + 2, n);
        name[n] = 0;
      }
      break;
    default:
      break;
  }
}

static int ReadChar(FILE *f) {
  int c;
  ++column;
  if ((c = fgetc(f)) == -1) return -1;
  if (c == '\n') {
    ++line;
    column = 0;
  }
  return c;
}

static int GetChar(FILE *f) {
  int c;
  for (;;) {
    if ((c = ReadChar(f)) == -1) return -1;
    if (c == '#' && column == 1) {
      for (;;) {
        if ((c = ReadChar(f)) == -1) return -1;
        if (c == '\r') {
          continue;
        } else if (c == '\n') {
          OnHeader();
          break;
        }
        AppendChar(c);
      }
      continue;
    }
    return c;
  }
}

static int LoadFile(const char *path) {
  FILE *f;
  long c, y, x, i, n, yn, xn, yo, xo;
  line = 0;
  f = fopen(path, "r");
  if (GetChar(f) != 'x') goto ReadError;
  if (GetChar(f) != ' ') goto ReadError;
  if (GetChar(f) != '=') goto ReadError;
  if (GetChar(f) != ' ') goto ReadError;
  xn = 0;
  for (;;) {
    if ((c = GetChar(f)) == -1) goto ReadError;
    if (!isdigit(c)) break;
    xn *= 10;
    xn += c - '0';
  }
  do {
    if ((c = GetChar(f)) == -1) goto ReadError;
  } while (!isdigit(c));
  yn = 0;
  do {
    yn *= 10;
    yn += c - '0';
    if ((c = GetChar(f)) == -1) goto ReadError;
  } while (isdigit(c));
  while (c != '\n') {
    if ((c = ReadChar(f)) == -1) goto ReadError;
  }
  if (yn > byn || xn > bxn) goto ReadError;
  SwapBoards();
  bzero(board, (byn * bxn) >> 3);
  yo = byn / 2 - yn / 2;
  xo = bxn / 2 - xn / 2;
  y = 0;
  x = 0;
  for (;;) {
    if ((c = GetChar(f)) == -1) goto ReadError;
    if (c == '!') {
      break;
    } else if (isspace(c)) {
      continue;
    }
    if (isdigit(c)) {
      n = c - '0';
      for (;;) {
        if ((c = GetChar(f)) == -1) goto ReadError;
        if (!isdigit(c)) break;
        n *= 10;
        n += c - '0';
      }
    } else {
      n = 1;
    }
    if (c == '$') {
      if (++y == yn) y = 0;
      x = 0;
    } else if (c == 'b' || c == 'o') {
      for (i = 0; i < n; ++i) {
        if (x >= xn) {
          if (++y == yn) y = 0;
          x = 0;
        }
        if (c == 'o') {
          Set(yo + y, xo + x);
        }
        ++x;
      }
    } else {
      goto ReadError;
    }
  }
  fclose(f);
  dimensioned = false;
  return 0;
ReadError:
  fclose(f);
  SwapBoards();
  return -1;
}

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § game of life » terminal user interface                    ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

static int Write(const char *s) {
  return write(out, s, strlen(s));
}

static wontreturn void PrintUsage(int rc) {
  Write("SYNOPSIS\n\n  ");
  Write(program_invocation_name);
  Write(USAGE);
  exit(rc);
}

static void GetOpts(int argc, char *argv[]) {
  int opt;
  while ((opt = getopt(argc, argv, "?hNWzw:h:")) != -1) {
    switch (opt) {
      case 'w':
        bxn = strtol(optarg, NULL, 0);
        bxn = ROUNDUP(MAX(8, bxn), 8);
        break;
      case 'h':
        byn = strtol(optarg, NULL, 0);
        byn = ROUNDUP(MAX(8, byn), 8);
        break;
      case 'z':
        ++zoom;
        break;
      case 'N':
        natural = true;
        break;
      case 'W':
        white = true;
        break;
      case '?':
        PrintUsage(EXIT_SUCCESS);
      default:
        PrintUsage(EX_USAGE);
    }
  }
}

static void HideTtyCursor(void) {
  Write("\e[?25l");
}

static void ShowTtyCursor(void) {
  Write("\e[?25h");
}

static void EnableMouse(void) {
  mousemode = true;
  Write("\e[?1000;1002;1015;1006h");
}

static void DisableMouse(void) {
  mousemode = false;
  Write("\e[?1000;1002;1015;1006l");
}

static void LeaveScreen(void) {
  Write("\e[H\e[0m\e[J");
}

static void GetTtySize(void) {
  struct winsize wsize;
  wsize.ws_row = tyn + 1;
  wsize.ws_col = txn;
  tcgetwinsize(out, &wsize);
  tyn = wsize.ws_row - 1;
  txn = wsize.ws_col;
  right = left + txn;
  bottom = top + tyn;
}

static void EnableRaw(void) {
  struct termios term;
  memcpy(&term, &oldterm, sizeof(term));
  term.c_cc[VMIN] = 1;
  term.c_cc[VTIME] = 1;
  term.c_iflag &= ~(INPCK | ISTRIP | PARMRK | INLCR | IGNCR | ICRNL | IXON);
  term.c_lflag &= ~(IEXTEN | ICANON | ECHO | ECHONL);
  term.c_cflag &= ~(CSIZE | PARENB);
  term.c_cflag |= CS8;
  term.c_iflag |= IUTF8;
  tcsetattr(out, TCSANOW, &term);
}

static void OnExit(void) {
  LeaveScreen();
  ShowTtyCursor();
  DisableMouse();
  tcsetattr(out, TCSANOW, &oldterm);
}

static void OnSigInt(int sig) {
  action |= INTERRUPTED;
}

static void OnSigWinch(int sig) {
  action |= RESIZED;
}

static void OnMouse(char *p) {
  int e, x, y;
  e = strtol(p, &p, 10);
  if (*p == ';') ++p;
  x = min(txn, max(1, strtol(p, &p, 10))) - 1;
  if (*p == ';') ++p;
  y = min(tyn, max(1, strtol(p, &p, 10))) - 1;
  e |= (*p == 'm') << 2;
  switch (e) {
    case MOUSE_WHEEL_UP:
      if (natural) {
        OnDown();
        OnDown();
        OnDown();
      } else {
        OnUp();
        OnUp();
        OnUp();
      }
      break;
    case MOUSE_WHEEL_DOWN:
      if (natural) {
        OnUp();
        OnUp();
        OnUp();
      } else {
        OnDown();
        OnDown();
        OnDown();
      }
      break;
    case MOUSE_CTRL_WHEEL_UP:
      if (natural) {
        OnZoom(y, x);
      } else {
        OnUnzoom(y, x);
      }
      break;
    case MOUSE_CTRL_WHEEL_DOWN:
      if (natural) {
        OnUnzoom(y, x);
      } else {
        OnZoom(y, x);
      }
      break;
    case MOUSE_RIGHT_DOWN:
      OnMouseRightDown(y, x);
      break;
    case MOUSE_RIGHT_DRAG:
      OnMouseRightDrag(y, x);
      break;
    case MOUSE_RIGHT_UP:
      OnMouseRightUp(y, x);
      break;
    case MOUSE_LEFT_DOWN:
      OnMouseLeftDown(y, x);
      break;
    case MOUSE_LEFT_DRAG:
      OnMouseLeftDrag(y, x);
      break;
    case MOUSE_LEFT_UP:
      OnMouseLeftUp(y, x);
      break;
    default:
      break;
  }
}

static void Rando1(void) {
  long i, n;
  n = (byn * bxn) >> 6;
  for (i = 0; i < n; ++i) {
    board[i] = _rand64();
  }
}

static void Rando2(void) {
  long i, n;
  n = (byn * bxn) >> 6;
  for (i = 0; i < n; ++i) {
    board[i] = rand();
    board[i] <<= 31;
    board[i] |= rand();
    board[i] <<= 2;
    board[i] |= rand() & 0b11;
  }
}

static void ReadKeyboard(void) {
  char buf[32], *p = buf;
  bzero(buf, sizeof(buf));
  if (readansi(0, buf, sizeof(buf)) == -1) {
    if (errno == EINTR) return;
    exit(errno);
  }
  switch (*p++) {
    case 'q':
      exit(0);
    case ' ':
    case 's':
    case '\t':
      Generation();
      break;
    case 'k':
    case CTRL('P'):
      OnUp();
      break;
    case 'j':
    case CTRL('N'):
      OnDown();
      break;
    case CTRL('V'):
      OnPageDown();
      break;
    case CTRL('R'):
      Rando1();
      break;
    case CTRL('G'):
      Rando2();
      break;
    case 'M':
      if (mousemode) {
        DisableMouse();
      } else {
        EnableMouse();
      }
      break;
    case 'R':
      bzero(board, (byn * bxn) >> 3);
      break;
    case CTRL('T'):
      OnTurbo();
      break;
    case '\e':
      switch (*p++) {
        case 'v':
          OnPageUp();
          break;
        case 't':
          OnSlowmo();
          break;
        case '[':
          switch (*p++) {
            case '<':
              OnMouse(p);
              break;
            case 'A':
              OnUp();
              break;
            case 'B':
              OnDown();
              break;
            case 'D':
              OnLeft();
              break;
            case 'C':
              OnRight();
              break;
            case '5':
              switch (*p++) {
                case '~':
                  OnPageUp();
                  break;
                default:
                  break;
              }
              break;
            case '6':
              switch (*p++) {
                case '~':
                  OnPageDown();
                  break;
                default:
                  break;
              }
              break;
            default:
              break;
          }
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}

static int InvertXtermGreyscale(int x) {
  return -(x - 232) + 255;
}

static int ByteToColor(int x) {
  uint8_t c;
  c = x / 256. * 24 + 232;
  if (white) c = InvertXtermGreyscale(c);
  return c;
}

static void SetColor(int x, bool isbg) {
  if (x != color[isbg]) {
    AppendStr("\e[");
    AppendInt(38 + 10 * isbg);
    AppendStr(";5;");
    AppendInt(x);
    AppendStr("m");
    color[isbg] = x;
  }
}

static void SetFg(int x) {
  SetColor(x, false);
}

static void SetBg(int x) {
  SetColor(x, true);
}

static void BitsToBytes(uint8_t a[8], uint64_t x) {
  a[0] = -((x >> 0) & 1);
  a[1] = -((x >> 1) & 1);
  a[2] = -((x >> 2) & 1);
  a[3] = -((x >> 3) & 1);
  a[4] = -((x >> 4) & 1);
  a[5] = -((x >> 5) & 1);
  a[6] = -((x >> 6) & 1);
  a[7] = -((x >> 7) & 1);
}

static void Raster(void) {
  long y, x;
  SetBg(ByteToColor(0));
  SetFg(ByteToColor(255));
  for (y = top; y < bottom; ++y) {
    for (x = left; x < right; ++x) {
      if ((0 <= y && y < byn) && (0 <= x && x < bxn)) {
        if (Test(y, x)) {
          AppendStr("█");
        } else {
          AppendStr(" ");
        }
      } else {
        AppendStr(VOIDSPACE);
      }
    }
  }
}

static void RasterZoomed(long t, long l, long b, long r,
                         uint8_t p[b - t][r - l]) {
  uint64_t c;
  uint8_t b1, b2;
  long i, y, x, yn, xn;
  for (y = MAX(0, t); y < MIN(b, byn); y += 8) {
    for (x = MAX(0, l); x < MIN(r, bxn); x += 8) {
      c = board[(bxn >> 3) * (y >> 3) + (x >> 3)];
      for (i = 0; i < 8; ++i) {
        BitsToBytes(&p[y - t + i][x - l], c);
        c >>= 8;
      }
    }
  }
  yn = b - t;
  xn = r - l;
  for (i = 0; i < zoom; ++i) {
    Magikarp2xX(b - t, r - l, p, yn, xn);
    Magikarp2xY(b - t, r - l, p, yn, xn);
    yn >>= 1;
    xn >>= 1;
  }
  for (y = top; y < bottom; y += 2ul << zoom) {
    for (x = left; x < right; x += 1ul << zoom) {
      if ((0 <= y && y < byn) && (0 <= x && x < bxn)) {
        b1 = p[((top - t) + ((y + 0) - top)) >> zoom]
              [((left - l) + (x - left)) >> zoom];
        b2 = y + (1ul << zoom) < bottom
                 ? p[((top - t) + ((y + 1) - top)) >> zoom]
                    [((left - l) + (x - left)) >> zoom]
                 : 0;
        if (b1 || b2) {
          SetBg(ByteToColor(b1));
          SetFg(ByteToColor(b2));
          AppendStr("▄");
        } else {
          SetBg(ByteToColor(0));
          SetFg(ByteToColor(255));
          AppendStr(" ");
        }
      } else {
        SetBg(ByteToColor(0));
        SetFg(ByteToColor(255));
        AppendStr(VOIDSPACE);
      }
    }
  }
}

static void Draw(void) {
  void *m;
  long t, l, b, r, i, n;
  buffer.i = 0;
  color[0] = -1;
  color[1] = -1;
  AppendStr("\e[H");
  if (!zoom) {
    Raster();
  } else {
    t = ROUNDDOWN(top, 16);
    l = ROUNDDOWN(left, 16);
    b = ROUNDUP(bottom, 16);
    r = ROUNDUP(right, 16);
    if ((m = calloc((b - t) * (r - l), 1))) {
      RasterZoomed(t, l, b, r, m);
      free(m);
    }
  }
  AppendStr("\e[0;7m");
  GenerateStatusLine();
  AppendStr(statusline);
  n = txn - strwidth(statusline, 0);
  for (i = 0; i < n; ++i) {
    AppendStr(" ");
  }
  AppendStr("\e[0m");
  write(out, buffer.p, buffer.i);
}

static bool HasPendingInput(void) {
  struct pollfd fds[1];
  fds[0].fd = 0;
  fds[0].events = POLLIN;
  fds[0].revents = 0;
  poll(fds, ARRAYLEN(fds), 0);
  return fds[0].revents & (POLLIN | POLLERR);
}

static bool ShouldDraw(void) {
  struct timespec now;
  static struct timespec next;
  if (!isdragging) return true;
  now = timespec_real();
  if (timespec_cmp(now, next) > 0 && !HasPendingInput()) {
    next = timespec_add(now, timespec_frommicros(1. / 24 * 1e6));
    return true;
  } else {
    return false;
  }
}

static void Tui(void) {
  GetTtySize();
  Dimension();
  tcgetattr(out, &oldterm);
  HideTtyCursor();
  EnableRaw();
  EnableMouse();
  atexit(OnExit);
  sigaction(SIGINT, &(struct sigaction){.sa_handler = OnSigInt}, 0);
  sigaction(SIGWINCH, &(struct sigaction){.sa_handler = OnSigWinch}, 0);
  do {
    if (action & RESIZED) {
      GetTtySize();
      action &= ~RESIZED;
      Draw();
    } else if (ShouldDraw()) {
      Draw();
    }
    ReadKeyboard();
  } while (!(action & INTERRUPTED));
}

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § game of life » graphical user interface                   ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

static const char16_t kClassName[] = u"apelife";

static void OnMenuAbout(int64_t hwnd) {
  MessageBox(hwnd, u"\
Apelife\r\n\
Cosmopolitan C Library\r\n\
αcτµαlly pδrταblε εxεcµταblε\r\n\
By Justine Tunney <jtunney@gmail.com>\r\n\
In memory of John Horton Conway, 1937-2020\r\n\
https://github.com/jart/cosmopolitan\r\n\
\r\n\
- Hold space to animate\r\n\
- Hold left mouse to draw cells\r\n\
- Hold right mouse to move view\r\n\
- Press t or alt+t to adjust speed",
             u"Conway's Game of Life", kNtMbOk | kNtMbIconinformation);
}

static void OnMenuOpen(int64_t hwnd) {
  char buf8[PATH_MAX];
  char16_t buf16[PATH_MAX];
  struct NtOpenFilename ofn;
  bzero(&ofn, sizeof(ofn));
  ofn.lStructSize = sizeof(ofn);
  ofn.hwndOwner = hwnd;
  ofn.lpstrFile = buf16;
  ofn.nMaxFile = ARRAYLEN(buf16);
  ofn.lpstrFilter = u"RLE Format (*.RLE;*.LIF;*.LIFE)\0"
                    u"*.RLE;*.LIF;*.LIFE\0"
                    u"All (*.*)\0"
                    u"*.*\0";
  ofn.Flags = kNtOfnPathmustexist | kNtOfnFilemustexist | kNtOfnExplorer;
  if (GetOpenFileName(&ofn)) {
    tprecode16to8(buf8, sizeof(buf8), ofn.lpstrFile);
    if (LoadFile(buf8) == -1) {
      MessageBox(hwnd, u"Failed to open run-length encoded game file",
                 u"Open Failed", kNtMbOk | kNtMbIconerror);
    }
    RedrawWindow(hwnd, 0, 0, kNtRdwInvalidate);
  }
}

static bool IsMaximized(int64_t hwnd) {
  struct NtWindowPlacement wp;
  wp.length = sizeof(wp);
  return GetWindowPlacement(hwnd, &wp) && wp.showCmd == kNtSwMaximize;
}

static void OnWindowPaint(int64_t hwnd) {
  int y, x;
  int64_t mdc, mbm;
  struct NtRect r, w;
  struct NtPaintStruct ps;
  BeginPaint(hwnd, &ps);
  r.top = 0;
  r.left = 0;
  r.right = ps.rcPaint.right - ps.rcPaint.left;
  r.bottom = ps.rcPaint.bottom - ps.rcPaint.top;
  w.top = MAX(r.top, -(top + ps.rcPaint.top));
  w.left = MAX(r.left, -(left + ps.rcPaint.left));
  w.right = MIN(r.right, bxn - (left + ps.rcPaint.left));
  w.bottom = MIN(r.bottom, byn - (top + ps.rcPaint.top));
  mdc = CreateCompatibleDC(ps.hdc);
  mbm = CreateCompatibleBitmap(ps.hdc, r.right, r.bottom);
  SelectObject(mdc, mbm);
  FillRect(mdc, &r, kNtColorInactiveborder);
  FillRect(mdc, &w, kNtColorAppworkspace);
  for (y = w.top; y < w.bottom; ++y) {
    for (x = w.left; x < w.right; ++x) {
      if (Test(top + ps.rcPaint.top + y, left + ps.rcPaint.left + x)) {
        SetPixel(mdc, x, y, 0);
      }
    }
  }
  BitBlt(ps.hdc, ps.rcPaint.left, ps.rcPaint.top, r.right, r.bottom, mdc, 0, 0,
         kNtSrccopy);
  DeleteObject(mbm);
  DeleteDC(mdc);
  GenerateStatusLine();
  tprecode8to16(statusline16, ARRAYLEN(statusline16), statusline);
  SetWindowText(hwnd, statusline16);
  EndPaint(hwnd, &ps);
}

static void OnWindowCharStep(int64_t hwnd, int64_t wParam, int64_t lParam) {
  int i, repeats;
  repeats = lParam & 0xFFFF;
  for (i = 0; i < repeats; ++i) {
    Generation();
  }
  RedrawWindow(hwnd, 0, 0, kNtRdwInvalidate);
}

static void OnWindowChar(int64_t hwnd, int64_t wParam, int64_t lParam) {
  switch (wParam) {
    case ' ':
    case 's':
    case '\t':
      OnWindowCharStep(hwnd, wParam, lParam);
      break;
    case 't':
      if (lParam & ALT) {
        OnSlowmo();
      } else {
        OnTurbo();
      }
      break;
    case '\r':
      if (IsMaximized(hwnd)) {
        SendMessage(hwnd, kNtWmSyscommand, kNtScRestore, 0);
      } else {
        SendMessage(hwnd, kNtWmSyscommand, kNtScMaximize, 0);
      }
      break;
    default:
      break;
  }
}

static void OnWindowSize(int64_t lParam) {
  txn = (lParam & 0x0000FFFF) >> 000;
  tyn = (lParam & 0xFFFF0000) >> 020;
  Dimension();
}

static void OnWindowLbuttondown(int64_t hwnd, int64_t wParam, int64_t lParam) {
  int y, x;
  y = (lParam & 0xFFFF0000) >> 020;
  x = (lParam & 0x0000FFFF) >> 000;
  SetCapture(hwnd);
  OnMouseLeftDown(y, x);
  RedrawWindow(hwnd, &(struct NtRect){x, y, x + 1, y + 1}, 0, kNtRdwInvalidate);
}

static void OnWindowLbuttonup(int64_t hwnd, int64_t wParam, int64_t lParam) {
  int y, x;
  y = (lParam & 0xFFFF0000) >> 020;
  x = (lParam & 0x0000FFFF) >> 000;
  OnMouseLeftUp(y, x);
  ReleaseCapture();
}

static void OnWindowRbuttondown(int64_t hwnd, int64_t wParam, int64_t lParam) {
  int y, x;
  y = (lParam & 0xFFFF0000) >> 020;
  x = (lParam & 0x0000FFFF) >> 000;
  oldcursor = GetCursor();
  SetCapture(hwnd);
  SetCursor(LoadCursor(0, kNtIdcSizeall));
  OnMouseRightDown(y, x);
  RedrawWindow(hwnd, NULL, 0, kNtRdwInvalidate);
}

static void OnWindowRbuttonup(int64_t hwnd, int64_t wParam, int64_t lParam) {
  int y, x;
  y = (lParam & 0xFFFF0000) >> 020;
  x = (lParam & 0x0000FFFF) >> 000;
  OnMouseRightUp(y, x);
  SetCursor(oldcursor);
  ReleaseCapture();
}

static void OnWindowMousemove(int64_t hwnd, int64_t wParam, int64_t lParam) {
  int y, x;
  y = (lParam & 0xFFFF0000) >> 020;
  x = (lParam & 0x0000FFFF) >> 000;
  if (wParam & kNtMkLbutton) {
    OnMouseLeftDrag(y, x);
    RedrawWindow(hwnd, &(struct NtRect){x, y, x + 1, y + 1}, 0,
                 kNtRdwInvalidate);
  } else if (wParam & kNtMkRbutton) {
    OnMouseRightDrag(y, x);
    RedrawWindow(hwnd, NULL, 0, kNtRdwInvalidate);
  }
}

static int64_t WindowProc(int64_t hwnd, uint32_t uMsg, uint64_t wParam,
                          int64_t lParam) {
  switch (uMsg) {
    case kNtWmDestroy:
      PostQuitMessage(0);
      return 0;
    case kNtWmSize:
      OnWindowSize(lParam);
      return 0;
    case kNtWmPaint:
      OnWindowPaint(hwnd);
      return 0;
    case kNtWmChar:
      OnWindowChar(hwnd, wParam, lParam);
      return 0;
    case kNtWmLbuttondown:
      OnWindowLbuttondown(hwnd, wParam, lParam);
      return 0;
    case kNtWmLbuttonup:
      OnWindowLbuttonup(hwnd, wParam, lParam);
      return 0;
    case kNtWmRbuttondown:
      OnWindowRbuttondown(hwnd, wParam, lParam);
      return 0;
    case kNtWmRbuttonup:
      OnWindowRbuttonup(hwnd, wParam, lParam);
      return 0;
    case kNtWmMousemove:
      OnWindowMousemove(hwnd, wParam, lParam);
      return 0;
    case kNtWmCommand:
    case kNtWmSyscommand:
      switch (wParam & ~0xF) {
        case IDM_ABOUT:
          OnMenuAbout(hwnd);
          return 0;
        case IDM_OPEN:
          OnMenuOpen(hwnd);
          return 0;
      }
      /* fallthrough */
    default:
      return DefWindowProc(hwnd, uMsg, wParam, lParam);
  }
}

static void Gui(void) {
  int64_t hwnd, mh;
  struct NtMsg msg;
  struct NtWndClass wc;
  bzero(&wc, sizeof(wc));
  wc.lpfnWndProc = NT2SYSV(WindowProc);
  wc.hInstance = GetModuleHandle(NULL);
  wc.hCursor = LoadCursor(0, kNtIdcCross);
  wc.lpszClassName = kClassName;
  wc.hbrBackground = kNtColorInactiveborder;
  CHECK(RegisterClass(&wc));
  CHECK((hwnd = CreateWindowEx(0, kClassName, u"Conway's Game of Life",
                               kNtWsOverlappedwindow, kNtCwUsedefault,
                               kNtCwUsedefault, kNtCwUsedefault,
                               kNtCwUsedefault, 0, 0, wc.hInstance, 0)));
  mh = GetSystemMenu(hwnd, false);
  AppendMenu(mh, kNtMfSeparator, 0, 0);
  AppendMenu(mh, kNtMfEnabled, IDM_OPEN, u"&Open File...");
  AppendMenu(mh, kNtMfEnabled, IDM_ABOUT, u"&About...");
  ShowWindow(hwnd, kNtSwNormal);
  while (GetMessage(&msg, 0, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
}

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § game of life » program                                    ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

int main(int argc, char *argv[]) {
  if (!NoDebug()) ShowCrashReports();
  out = 1;
  speed = 1;
  tyn = right = 80;
  txn = bottom = 24;
  byn = 64 * 64;
  bxn = 64 * 64;
  strcpy(name, "apelife");
  GetOpts(argc, argv);
  AllocateBoardsWithHardwareAcceleratedMemorySafety();
  if (optind < argc) {
    if (LoadFile(argv[optind]) == -1) {
      fprintf(
          stderr, "%s:%d:%d: %s\n", argv[optind], line + 1, column,
          "error: failed to load game of life run length encoded (rle) file");
      return 1;
    }
  }
  if (0 && IsWindows()) {
    Gui();
  } else {
    Tui();
  }
  return 0;
}
