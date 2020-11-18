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
#include "libc/bits/bits.h"
#include "libc/bits/safemacros.h"
#include "libc/calls/calls.h"
#include "libc/calls/ioctl.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/struct/termios.h"
#include "libc/calls/struct/winsize.h"
#include "libc/calls/termios-internal.h"
#include "libc/conv/conv.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/log/log.h"
#include "libc/macros.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/str/tpenc.h"
#include "libc/sysv/consts/termios.h"
#include "libc/x/x.h"

/**
 * @fileoverview Conway's Game of Life
 *
 * The Game of Life, also known simply as Life, is a cellular automaton
 * devised by the British mathematician John Horton Conway in 1970. It
 * is Turing complete and can simulate a universal constructor or any
 * other Turing machine.
 *
 * This program may be used by dragging the mouse in the terminal. Left
 * mouse draws or erases cells. Right mouse scrolls or moves the plane.
 * The space bar may be held down to move time forward.
 *
 * This implementation uses bitboards on an n×m wraparound plane. For an
 * explanation of how this technique works on an 8×8 plane see "Bitboard
 * Methods for Games" by Cameron Browne.
 */

#define INTERRUPTED 1
#define RESIZED     2

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
    c1 = r[0] & (X); \
    c2 = r[1] & c1;  \
    r[0] ^= (X);     \
    r[1] ^= c1;      \
    r[2] |= c2;      \
  } while (0)

#define STEP(RES, B00, B01, B02, B10, B11, B12, B20, B21, B22)             \
  do {                                                                     \
    uint64_t r[3] = {0};                                                   \
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
    RES = (B11 | r[0]) & r[1] & ~r[2];                                     \
  } while (0)

struct Buffer {
  unsigned i, n;
  char *p;
};

static bool erase;
static bool natural;
static bool mousemode;

static int out;
static int action;

static long top;
static long bottom;
static long left;
static long right;
static long tyn;
static long txn;
static long byn;
static long bxn;
static long save_y;
static long save_x;
static long save_top;
static long save_left;

static uint64_t *board;

static struct Buffer buffer;
static struct termios oldterm;

static int Write(const char *s) {
  return write(out, s, strlen(s));
}

static void HideCursor(void) {
  Write("\e[?25l");
}

static void ShowCursor(void) {
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
  Write("\e[H\e[J");
}

static void GetTtySize(void) {
  struct winsize wsize;
  wsize.ws_row = tyn;
  wsize.ws_col = txn;
  getttysize(out, &wsize);
  tyn = wsize.ws_row;
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
  ioctl(out, TCSETS, &term);
}

static void OnExit(void) {
  LeaveScreen();
  ShowCursor();
  DisableMouse();
  ioctl(out, TCSETS, &oldterm);
}

static void OnSigInt(int sig, struct siginfo *sa, struct ucontext *uc) {
  action |= INTERRUPTED;
}

static void OnSigWinch(int sig, struct siginfo *sa, struct ucontext *uc) {
  action |= RESIZED;
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

static void Setup(void) {
  out = 1;
  tyn = 80;
  txn = 24;
  byn = 64 * 4;
  bxn = 64 * 8;
  board = xcalloc((byn * bxn) >> 6, 8);
  GetTtySize();
  top = byn / 2 - tyn / 2;
  left = bxn / 2 - txn / 2;
  right = left + txn;
  bottom = top + tyn;
  ioctl(out, TCGETS, &oldterm);
  HideCursor();
  EnableRaw();
  EnableMouse();
  atexit(OnExit);
  sigaction(SIGINT, &(struct sigaction){.sa_sigaction = OnSigInt}, NULL);
  sigaction(SIGWINCH, &(struct sigaction){.sa_sigaction = OnSigWinch}, NULL);
}

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

static void AppendStr(const char *s) {
  AppendData(s, strlen(s));
}

static void AppendWide(wint_t wc) {
  unsigned i;
  uint64_t wb;
  char buf[8];
  i = 0;
  wb = tpenc(wc);
  do {
    buf[i++] = wb & 0xFF;
    wb >>= 8;
  } while (wb);
  AppendData(buf, i);
}

static void Move(long dy, long dx) {
  top = top + dy;
  bottom = bottom + dy;
  left = left + dx;
  right = right + dx;
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

static void Generation(void) {
  uint64_t *board2;
  long y, x, yn, xn, yp, ym, xp, xm;
  yn = byn >> 3;
  xn = bxn >> 3;
  board2 = xmalloc(yn * xn * 8);
  for (y = 0; y < yn; ++y) {
    for (x = 0; x < xn; ++x) {
      ym = y ? y - 1 : yn - 1;
      yp = y + 1 < yn ? y + 1 : 0;
      xm = x ? x - 1 : xn - 1;
      xp = x + 1 < xn ? x + 1 : 0;
      STEP(board2[y * xn + x], board[ym * xn + xm], board[ym * xn + x],
           board[ym * xn + xp], board[y * xn + xm], board[y * xn + x],
           board[y * xn + xp], board[yp * xn + xm], board[yp * xn + x],
           board[yp * xn + xp]);
    }
  }
  free(board);
  board = board2;
}

static void OnMouseLeftDrag(long y, long x) {
  if (y == save_y && x == save_x) return;
  save_y = y;
  save_x = x;
  y += top;
  x += left;
  if (y < 0 || y >= byn) return;
  if (x < 0 || x >= bxn) return;
  if (erase) {
    Unset(y, x);
  } else {
    Set(y, x);
  }
}

static void OnMouseLeftDown(long y, long x) {
  save_y = y;
  save_x = x;
  y += top;
  x += left;
  erase = false;
  if (y < 0 || y >= byn) return;
  if (x < 0 || x >= bxn) return;
  if ((erase = Test(y, x))) {
    Unset(y, x);
  } else {
    Set(y, x);
  }
}

static void OnMouseRightDown(long y, long x) {
  save_y = y;
  save_x = x;
  save_top = top;
  save_left = left;
}

static void OnMouseRightDrag(long y, long x) {
  long dy, dx, h, w;
  dy = save_y - y;
  dx = save_x - x;
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
    case MOUSE_RIGHT_DOWN:
      OnMouseRightDown(y, x);
      break;
    case MOUSE_RIGHT_DRAG:
      OnMouseRightDrag(y, x);
      break;
    case MOUSE_LEFT_DOWN:
      OnMouseLeftDown(y, x);
      break;
    case MOUSE_LEFT_DRAG:
      OnMouseLeftDrag(y, x);
      break;
    default:
      break;
  }
}

static void ReadKeyboard(void) {
  char buf[32], *p = buf;
  memset(buf, 0, sizeof(buf));
  if (readansi(0, buf, sizeof(buf)) == -1) {
    if (errno == EINTR) return;
    exit(errno);
  }
  switch (*p++) {
    case 'q':
      exit(0);
    case ' ':
    case 's':
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
    case 'M':
      if (mousemode) {
        DisableMouse();
      } else {
        EnableMouse();
      }
      break;
    case '\e':
      switch (*p++) {
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

static void Draw(void) {
  long y, x;
  buffer.i = 0;
  AppendStr("\e[H");
  for (y = top; y < bottom; ++y) {
    if (y > top) AppendStr("\e[K\r\n");
    for (x = left; x < right; ++x) {
      if ((0 <= y && y < byn) && (0 <= x && x < bxn)) {
        if (Test(y, x)) {
          AppendWide(u'█');
        } else {
          AppendChar(' ');
        }
      } else {
        AppendWide(u'∙');
      }
    }
  }
  write(out, buffer.p, buffer.i);
}

static void Life(void) {
  do {
    if (action & RESIZED) {
      GetTtySize();
      action &= ~RESIZED;
    }
    Draw();
    ReadKeyboard();
  } while (!(action & INTERRUPTED));
}

int main(int argc, char *argv[]) {
  if (!NoDebug()) showcrashreports();
  Setup();
  Life();
  return 0;
}
