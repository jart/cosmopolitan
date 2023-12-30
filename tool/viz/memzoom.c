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
#include "dsp/scale/cdecimate2xuint8x8.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/siginfo.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/struct/termios.h"
#include "libc/calls/struct/winsize.h"
#include "libc/calls/termios.h"
#include "libc/calls/ucontext.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/bsf.h"
#include "libc/intrin/bsr.h"
#include "libc/intrin/hilbert.h"
#include "libc/intrin/safemacros.internal.h"
#include "libc/limits.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/pollfd.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/str/tab.internal.h"
#include "libc/str/unicode.h"
#include "libc/sysv/consts/ex.h"
#include "libc/sysv/consts/exit.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/poll.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/termios.h"
#include "libc/time/time.h"
#include "third_party/getopt/getopt.internal.h"

#define USAGE \
  " [-hznmHNW] [-p PID] [PATH]\n\
\n\
DESCRIPTION\n\
\n\
  Memory Viewer\n\
\n\
FLAGS\n\
\n\
  -h or -?   help\n\
  -z         zoom\n\
  -m         morton ordering\n\
  -H         hilbert ordering\n\
  -N         natural scrolling\n\
  -W         white terminal background\n\
  -p PID     shows process virtual memory\n\
  -f INT     frames per second [default 10]\n\
\n\
SHORTCUTS\n\
\n\
  z or +     zoom\n\
  Z or -     unzoom\n\
  ctrl+wheel zoom point\n\
  wheel      scroll\n\
  l          linearize\n\
  m          mortonize\n\
  h          hilbertify\n\
  n          next mapping\n\
  N          next mapping ending\n\
  p          prev mapping\n\
  P          prev mapping ending\n\
  k          up\n\
  j          down\n\
  b          page up\n\
  space      page down\n\
  g          home\n\
  G          end\n\
  q          quit\n\
\n"

#define CTRL(C) ((C) ^ 0100)

#define MAXZOOM 14
#define COLOR   253

#define LINEAR  0
#define MORTON  1
#define HILBERT 2

#define INTERRUPTED 0x1
#define RESIZED     0x2

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

struct Ranges {
  long i;
  struct Range {
    long a;
    long b;
  } p[512];
};

static const signed char kThePerfectKernel[8] = {-1, -3, 3, 17, 17, 3, -3, -1};

static bool white;
static bool natural;
static bool mousemode;

static int fd;
static int pid;
static int out;
static int fps;
static int zoom;
static int order;
static int action;

static long tyn;
static long txn;
static long size;
static long offset;
static long lowest;
static long highest;
static long canvassize;
static long buffersize;
static long displaysize;

static char *buffer;
static uint8_t *canvas;

static struct stat st;
static struct Ranges ranges;
static struct termios oldterm;

static char path[PATH_MAX];
static char mapspath[PATH_MAX];

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

static unsigned long rounddown2pow(unsigned long x) {
  return x ? 1ul << _bsrl(x) : 0;
}

static void GetTtySize(void) {
  struct winsize wsize;
  wsize.ws_row = tyn + 1;
  wsize.ws_col = txn;
  tcgetwinsize(out, &wsize);
  tyn = MAX(2, wsize.ws_row) - 1;
  txn = MAX(17, wsize.ws_col) - 16;
  tyn = rounddown2pow(tyn);
  txn = rounddown2pow(txn);
  tyn = MIN(tyn, txn);
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
  ShowCursor();
  DisableMouse();
  tcsetattr(out, TCSANOW, &oldterm);
}

static void OnSigInt(int sig, struct siginfo *sa, void *uc) {
  action |= INTERRUPTED;
}

static void OnSigWinch(int sig, struct siginfo *sa, void *uc) {
  action |= RESIZED;
}

static void Setup(void) {
  tyn = 80;
  txn = 24;
  action = RESIZED;
  tcgetattr(out, &oldterm);
  HideCursor();
  EnableRaw();
  EnableMouse();
  atexit(OnExit);
  sigaction(SIGINT, &(struct sigaction){.sa_sigaction = OnSigInt}, NULL);
  sigaction(SIGWINCH, &(struct sigaction){.sa_sigaction = OnSigWinch}, NULL);
}

static wontreturn void FailPath(const char *s, int rc) {
  Write("error: ");
  Write(s);
  Write(": ");
  Write(path);
  Write("\n");
  exit(rc);
}

static void SetExtent(long lo, long hi) {
  lowest = lo;
  highest = hi;
  offset = MIN(hi, MAX(lo, offset));
}

static void Open(void) {
  if ((fd = open(path, O_RDONLY)) == -1) {
    FailPath("open() failed", errno);
  }
  fstat(fd, &st);
  size = st.st_size;
  SetExtent(0, size);
}

static void *Allocate(size_t n) {
  return mmap(NULL, n, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1,
              0);
}

static void SetupCanvas(void) {
  if (canvassize) {
    munmap(canvas, canvassize);
    munmap(buffer, buffersize);
  }
  displaysize = ROUNDUP(ROUNDUP((tyn * txn) << zoom, 16), 1ul << zoom);
  canvassize = ROUNDUP(displaysize, FRAMESIZE);
  buffersize = ROUNDUP(tyn * txn * 16 + 4096, FRAMESIZE);
  canvas = Allocate(canvassize);
  buffer = Allocate(buffersize);
}

/**
 * Interleaves bits.
 * @see https://en.wikipedia.org/wiki/Z-order_curve
 * @see unmorton()
 */
static unsigned long morton(unsigned long y, unsigned long x) {
  x = (x | x << 020) & 0x0000FFFF0000FFFF;
  x = (x | x << 010) & 0x00FF00FF00FF00FF;
  x = (x | x << 004) & 0x0F0F0F0F0F0F0F0F;
  x = (x | x << 002) & 0x3333333333333333;
  x = (x | x << 001) & 0x5555555555555555;
  y = (y | y << 020) & 0x0000FFFF0000FFFF;
  y = (y | y << 010) & 0x00FF00FF00FF00FF;
  y = (y | y << 004) & 0x0F0F0F0F0F0F0F0F;
  y = (y | y << 002) & 0x3333333333333333;
  y = (y | y << 001) & 0x5555555555555555;
  return x | y << 1;
}

static long IndexSquare(long y, long x) {
  switch (order) {
    case LINEAR:
      return y * txn + x;
    case MORTON:
      return morton(y, x);
    case HILBERT:
      return hilbert(txn, y, x);
    default:
      return 0;
  }
}

static long Index(long y, long x) {
  long i;
  if (order == LINEAR) {
    i = 0;
  } else {
    i = x / tyn;
    x = x % tyn;
  }
  return i * tyn * tyn + IndexSquare(y, x);
}

static void PreventBufferbloat(void) {
  struct timespec now, rate;
  static struct timespec last;
  now = timespec_real();
  rate = timespec_frommicros(1. / fps * 1e6);
  if (timespec_cmp(timespec_sub(now, last), rate) < 0) {
    timespec_sleep(timespec_sub(rate, timespec_sub(now, last)));
  }
  last = now;
}

static bool HasPendingInput(void) {
  struct pollfd fds[1];
  if (IsWindows()) return true; /* XXX */
  fds[0].fd = 0;
  fds[0].events = POLLIN;
  fds[0].revents = 0;
  poll(fds, ARRAYLEN(fds), 0);
  return fds[0].revents & (POLLIN | POLLERR);
}

static int GetCurrentRange(void) {
  int i;
  if (ranges.i) {
    for (i = 0; i < ranges.i; ++i) {
      if (offset < ranges.p[i].a) return MAX(0, i - 1);
      if (offset < ranges.p[i].b) return i;
    }
    return ranges.i - 1;
  } else {
    return -1;
  }
}

static void Move(long d) {
  d <<= zoom;
  offset = MIN(highest, MAX(lowest, (offset + d) >> zoom << zoom));
}

static void SetZoom(long y, long x, int d) {
  long a, b, i;
  if ((0 <= y && y < tyn) && (0 <= x && x < txn)) {
    i = Index(y, x);
    a = zoom;
    b = MIN(MAXZOOM, MAX(0, a + d));
    zoom = b;
    Move((i << a) - (i << b));
    SetupCanvas();
  }
}

static void OnZoom(long y, long x) {
  SetZoom(y, x, +1);
}

static void OnUnzoom(long y, long x) {
  SetZoom(y, x, -1);
}

static void OnUp(void) {
  Move(-(txn));
}

static void OnDown(void) {
  Move(txn);
}

static void OnPageUp(void) {
  Move(-(txn * (tyn - 2)));
}

static void OnPageDown(void) {
  Move(txn * (tyn - 2));
}

static void OnHome(void) {
  offset = lowest;
}

static void OnEnd(void) {
  offset = MAX(lowest, highest - txn * tyn);
}

static void OnLinear(void) {
  order = LINEAR;
  GetTtySize();
  SetupCanvas();
}

static void OnMorton(void) {
  order = MORTON;
  SetupCanvas();
}

static void OnHilbert(void) {
  order = HILBERT;
  SetupCanvas();
}

static void OnNext(void) {
  int i;
  if ((i = GetCurrentRange()) != -1) {
    if (i + 1 < ranges.i) {
      offset = ranges.p[i + 1].a;
    }
  }
}

static void OnPrev(void) {
  int i;
  if ((i = GetCurrentRange()) != -1) {
    if (i) {
      offset = ranges.p[i - 1].a;
    }
  }
}

static void OnNextEnd(void) {
  long i, n;
  if ((i = GetCurrentRange()) != -1) {
    n = (tyn * txn) << zoom;
    if (offset < ranges.p[i].b - n) {
      offset = ranges.p[i].b - n;
    } else if (i + 1 < ranges.i) {
      offset = MAX(ranges.p[i + 1].a, ranges.p[i + 1].b - n);
    }
  }
}

static void OnPrevEnd(void) {
  long i, n;
  if ((i = GetCurrentRange()) != -1) {
    n = (tyn * txn) << zoom;
    if (i) {
      offset = MAX(ranges.p[i - 1].a, ranges.p[i - 1].b - n);
    }
  }
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
    default:
      break;
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
    case '+':
    case 'z':
      OnZoom(0, 0);
      break;
    case '-':
    case 'Z':
      OnUnzoom(0, 0);
      break;
    case 'b':
      OnPageUp();
      break;
    case 'n':
      OnNext();
      break;
    case 'p':
      OnPrev();
      break;
    case 'N':
      OnNextEnd();
      break;
    case 'P':
      OnPrevEnd();
      break;
    case ' ':
    case CTRL('V'):
      OnPageDown();
      break;
    case 'g':
      OnHome();
      break;
    case 'G':
      OnEnd();
      break;
    case 'k':
    case CTRL('P'):
      OnUp();
      break;
    case 'j':
    case CTRL('N'):
      OnDown();
      break;
    case 'l':
      OnLinear();
      break;
    case 'm':
      if (order == MORTON) {
        OnLinear();
      } else {
        OnMorton();
      }
      break;
    case 'M':
      if (mousemode) {
        DisableMouse();
      } else {
        EnableMouse();
      }
      break;
    case 'h':
    case 'H':
      if (order == HILBERT) {
        OnLinear();
      } else {
        OnHilbert();
      }
      break;
    case '\e':
      switch (*p++) {
        case 'v':
          OnPageUp();
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
            case 'F':
              OnEnd();
              break;
            case 'H':
              OnHome();
              break;
            case '1':
              switch (*p++) {
                case '~':
                  OnHome();
                  break;
                default:
                  break;
              }
              break;
            case '4':
              switch (*p++) {
                case '~':
                  OnEnd();
                  break;
                default:
                  break;
              }
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
            case '7':
              switch (*p++) {
                case '~':
                  OnHome();
                  break;
                default:
                  break;
              }
              break;
            case '8':
              switch (*p++) {
                case '~':
                  OnEnd();
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

static void LoadRanges(void) {
  char b[512];
  struct Range range;
  int i, t, n, fd, err;
  if ((fd = open(mapspath, O_RDONLY)) == -1) {
    err = errno;
    Write("error: process died\n");
    exit(err);
  }
  t = 0;
  range.a = 0;
  range.b = 0;
  ranges.i = 0;
  for (;;) {
    if ((n = read(fd, b, sizeof(b))) == -1) exit(1);
    if (!n) break;
    for (i = 0; i < n; ++i) {
      switch (t) {
        case 0:
          if (isxdigit(b[i])) {
            range.a <<= 4;
            range.a += kHexToInt[b[i] & 255];
          } else if (b[i] == '-') {
            t = 1;
          }
          break;
        case 1:
          if (isxdigit(b[i])) {
            range.b <<= 4;
            range.b += kHexToInt[b[i] & 255];
          } else if (b[i] == ' ') {
            t = 2;
          }
          break;
        case 2:
          if (b[i] == '\n') {
            if (ranges.i < ARRAYLEN(ranges.p)) {
              ranges.p[ranges.i++] = range;
            }
            range.a = 0;
            range.b = 0;
            t = 0;
          }
          break;
        default:
          __builtin_unreachable();
      }
    }
  }
  close(fd);
  if (ranges.i) {
    SetExtent(ranges.p[0].a, ranges.p[ranges.i - 1].b);
  } else {
    SetExtent(0, 0);
  }
}

static int InvertXtermGreyscale(int x) {
  return -(x - 232) + 255;
}

static void Render(void) {
  char *p;
  int c, fg2, rc, fg;
  long i, y, x, w, n, got;
  p = buffer;
  p = stpcpy(p, "\e[H");
  for (y = 0; y < tyn; ++y) {
    fg = -1;
    for (x = 0; x < txn; ++x) {
      c = canvas[Index(y, x)];
      if (c < 32) {
        fg2 = 237 + c * ((COLOR - 237) / 32.);
      } else if (c >= 232) {
        fg2 = COLOR + (c - 232) * ((255 - COLOR) / (256. - 232));
      } else {
        fg2 = COLOR;
      }
      if (fg2 != fg) {
        fg = fg2;
        if (white) {
          fg = InvertXtermGreyscale(fg);
        }
        p = stpcpy(p, "\e[38;5;");
        p = FormatInt64(p, fg);
        *p++ = 'm';
      }
      w = tpenc(kCp437[c]);
      do {
        *p++ = w & 0xff;
        w >>= 8;
      } while (w);
    }
    p = stpcpy(p, "\e[0m ");
    p += uint64toarray_radix16(offset + ((y * txn) << zoom), p);
    p = stpcpy(p, "\e[K\r\n");
  }
  p = stpcpy(p, "\e[7m\e[K");
  n = strlen(path);
  if (n > txn - 3 - 1 - 7) {
    p = mempcpy(p, path, txn - 1 - 7 - 3);
    p = stpcpy(p, "...");
  } else {
    p = stpcpy(p, path);
    for (i = n; i < txn - 1 - 7; ++i) {
      *p++ = ' ';
    }
  }
  p = stpcpy(p, " memzoom\e[0m ");
  if (!pid) {
    p = FormatUint32(p, MIN(offset / (long double)size * 100, 100));
    p = stpcpy(p, "%-");
    p = FormatUint32(
        p,
        MIN((offset + ((tyn * txn) << zoom)) / (long double)size * 100, 100));
    p = stpcpy(p, "% ");
  }
  p = FormatUint32(p, 1L << zoom);
  p = stpcpy(p, "x\e[J");
  PreventBufferbloat();
  for (i = 0, n = p - buffer; i < n; i += got) {
    got = 0;
    if ((rc = write(out, buffer + i, n - i)) == -1) {
      if (errno == EINTR) continue;
      exit(errno);
    }
    got = rc;
  }
}

static void Zoom(long have) {
  long i, n, r;
  n = canvassize;
  for (i = 0; i < zoom; ++i) {
    cDecimate2xUint8x8(n, canvas, kThePerfectKernel);
    n >>= 1;
  }
  if (n < tyn * txn) {
    bzero(canvas + n, canvassize - n);
  }
  if (have != -1) {
    n = have >> zoom;
    i = n / txn;
    r = n % txn;
    if (r) ++i;
    if (order == LINEAR) {
      for (; i < tyn; ++i) {
        canvas[txn * i] = '~';
      }
    }
  }
}

static void FileZoom(void) {
  long have;
  have = MIN(displaysize, size - offset);
  have = pread(fd, canvas, have, offset);
  have = MAX(0, have);
  bzero(canvas + have, canvassize - have);
  Zoom(have);
  Render();
}

static void RangesZoom(void) {
  long a, b, c, d, i;
  LoadRanges();
  memset(canvas, 1, canvassize);
  a = offset;
  b = MIN(highest, offset + ((tyn * txn) << zoom));
  for (i = 0; i < ranges.i; ++i) {
    if ((a >= ranges.p[i].a && a < ranges.p[i].b) ||
        (b >= ranges.p[i].a && b < ranges.p[i].b) ||
        (a < ranges.p[i].a && b >= ranges.p[i].b)) {
      c = MAX(a, ranges.p[i].a);
      d = MIN(b, ranges.p[i].b);
      pread(fd, canvas + (c - offset), d - c, c);
    }
  }
  Zoom(-1);
  Render();
}

static void MemZoom(void) {
  bool ok;
  ok = false;
  do {
    if (action & RESIZED) {
      GetTtySize();
      SetupCanvas();
      action &= ~RESIZED;
    }
    if (ok && HasPendingInput()) {
      ReadKeyboard();
      if (!IsWindows()) continue; /* XXX */
    }
    ok = true;
    if (pid) {
      RangesZoom();
    } else {
      FileZoom();
    }
  } while (!(action & INTERRUPTED));
}

static wontreturn void PrintUsage(int rc, int fd) {
  tinyprint(fd, "SYNOPSIS\n\n ", program_invocation_name, USAGE, NULL);
  exit(rc);
}

static void GetOpts(int argc, char *argv[]) {
  int opt;
  char *p;
  fps = 10;
  while ((opt = getopt(argc, argv, "?hmzHNWf:p:")) != -1) {
    switch (opt) {
      case 'z':
        ++zoom;
        break;
      case 'm':
        order = MORTON;
        break;
      case 'H':
        order = HILBERT;
        break;
      case 'W':
        white = true;
        break;
      case 'N':
        natural = true;
        break;
      case 'f':
        fps = strtol(optarg, NULL, 0);
        fps = MAX(1, fps);
        break;
      case 'p':
        if (strcmp(optarg, "self") == 0) {
          pid = getpid();
        } else {
          pid = strtol(optarg, NULL, 0);
        }
        break;
      case 'h':
      case '?':
      default:
        if (opt == optopt) {
          PrintUsage(EXIT_SUCCESS, STDOUT_FILENO);
        } else {
          PrintUsage(EX_USAGE, STDERR_FILENO);
        }
    }
  }
  if (pid) {
    p = stpcpy(path, "/proc/");
    p = FormatInt64(p, pid);
    stpcpy(p, "/mem");
    p = stpcpy(mapspath, "/proc/");
    p = FormatInt64(p, pid);
    stpcpy(p, "/maps");
  } else {
    if (optind == argc) {
      PrintUsage(EX_USAGE, STDERR_FILENO);
    }
    if (!memccpy(path, argv[optind], '\0', sizeof(path))) {
      PrintUsage(EX_USAGE, STDERR_FILENO);
    }
  }
}

int main(int argc, char *argv[]) {
  if (!NoDebug()) ShowCrashReports();
  out = 1;
  GetOpts(argc, argv);
  Open();
  Setup();
  MemZoom();
  return 0;
}
