/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "dsp/tty/tty.h"
#include "libc/alg/arraylist2.internal.h"
#include "libc/assert.h"
#include "libc/bits/bits.h"
#include "libc/bits/safemacros.h"
#include "libc/calls/calls.h"
#include "libc/calls/ioctl.h"
#include "libc/calls/struct/iovec.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/struct/termios.h"
#include "libc/calls/struct/winsize.h"
#include "libc/calls/termios.h"
#include "libc/fmt/bing.internal.h"
#include "libc/fmt/conv.h"
#include "libc/fmt/fmt.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/pcmpeqb.h"
#include "libc/intrin/pmovmskb.h"
#include "libc/log/check.h"
#include "libc/log/color.internal.h"
#include "libc/log/log.h"
#include "libc/macros.h"
#include "libc/math.h"
#include "libc/mem/mem.h"
#include "libc/runtime/gc.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/sock.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/str/thompike.h"
#include "libc/str/tpdecode.internal.h"
#include "libc/str/tpencode.internal.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/sysv/consts/ex.h"
#include "libc/sysv/consts/exit.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/itimer.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/poll.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/termios.h"
#include "libc/sysv/errfuns.h"
#include "libc/time/time.h"
#include "libc/unicode/unicode.h"
#include "libc/x/x.h"
#include "third_party/gdtoa/gdtoa.h"
#include "third_party/getopt/getopt.h"
#include "tool/build/lib/address.h"
#include "tool/build/lib/breakpoint.h"
#include "tool/build/lib/case.h"
#include "tool/build/lib/cga.h"
#include "tool/build/lib/demangle.h"
#include "tool/build/lib/dis.h"
#include "tool/build/lib/endian.h"
#include "tool/build/lib/fds.h"
#include "tool/build/lib/flags.h"
#include "tool/build/lib/fpu.h"
#include "tool/build/lib/high.h"
#include "tool/build/lib/loader.h"
#include "tool/build/lib/machine.h"
#include "tool/build/lib/mda.h"
#include "tool/build/lib/memory.h"
#include "tool/build/lib/modrm.h"
#include "tool/build/lib/panel.h"
#include "tool/build/lib/pml4t.h"
#include "tool/build/lib/pty.h"
#include "tool/build/lib/stats.h"
#include "tool/build/lib/syscall.h"
#include "tool/build/lib/throw.h"
#include "tool/build/lib/xmmtype.h"

#define USAGE \
  " [-?HhrRstv] [ROM] [ARGS...]\n\
\n\
DESCRIPTION\n\
\n\
  Emulates x86 Linux Programs w/ Dense Machine State Visualization\n\
  Please keep still and only watchen astaunished das blinkenlights\n\
\n\
FLAGS\n\
\n\
  -h        help\n\
  -z        zoom\n\
  -v        verbosity\n\
  -r        real mode\n\
  -s        statistics\n\
  -H        disable highlight\n\
  -t        tui debugger mode\n\
  -R        reactive tui mode\n\
  -b ADDR   push a breakpoint\n\
  -L PATH   log file location\n\
\n\
ARGUMENTS\n\
\n\
  ROM files can be ELF or a flat αcτµαlly pδrταblε εxεcµταblε.\n\
  It should use x86_64 in accordance with the System Five ABI.\n\
  The SYSCALL ABI is defined as it is written in Linux Kernel.\n\
\n\
FEATURES\n\
\n\
  8086, 8087, i386, x86_64, SSE3, SSSE3, POPCNT, MDA, CGA, TTY\n\
\n"

#define MAXZOOM    16
#define DUMPWIDTH  64
#define DISPWIDTH  80
#define WHEELDELTA 1

#define RESTART  0x001
#define REDRAW   0x002
#define CONTINUE 0x004
#define STEP     0x008
#define NEXT     0x010
#define FINISH   0x020
#define FAILURE  0x040
#define WINCHED  0x080
#define INT      0x100
#define QUIT     0x200
#define EXIT     0x400
#define ALARM    0x800

#define kXmmDecimal 0
#define kXmmHex     1
#define kXmmChar    2

#define kMouseLeftDown      0
#define kMouseMiddleDown    1
#define kMouseRightDown     2
#define kMouseLeftUp        4
#define kMouseMiddleUp      5
#define kMouseRightUp       6
#define kMouseLeftDrag      32
#define kMouseMiddleDrag    33
#define kMouseRightDrag     34
#define kMouseWheelUp       64
#define kMouseWheelDown     65
#define kMouseCtrlWheelUp   80
#define kMouseCtrlWheelDown 81

#define CTRL(C) ((C) ^ 0100)

struct MemoryView {
  int64_t start;
  int zoom;
};

struct MachineState {
  uint64_t ip;
  uint8_t cs[8];
  uint8_t ss[8];
  uint8_t es[8];
  uint8_t ds[8];
  uint8_t fs[8];
  uint8_t gs[8];
  uint8_t reg[16][8];
  uint8_t xmm[16][16];
  struct MachineFpu fpu;
  struct MachineSse sse;
  struct MachineMemstat memstat;
};

struct Panels {
  union {
    struct {
      struct Panel disassembly;
      struct Panel breakpointshr;
      struct Panel breakpoints;
      struct Panel mapshr;
      struct Panel maps;
      struct Panel frameshr;
      struct Panel frames;
      struct Panel displayhr;
      struct Panel display;
      struct Panel registers;
      struct Panel ssehr;
      struct Panel sse;
      struct Panel codehr;
      struct Panel code;
      struct Panel readhr;
      struct Panel readdata;
      struct Panel writehr;
      struct Panel writedata;
      struct Panel stackhr;
      struct Panel stack;
      struct Panel status;
    };
    struct Panel p[21];
  };
};

static const signed char kThePerfectKernel[8] = {-1, -3, 3, 17, 17, 3, -3, -1};

static const char kRegisterNames[16][4] = {
    "RAX", "RCX", "RDX", "RBX", "RSP", "RBP", "RSI", "RDI",
    "R8",  "R9",  "R10", "R11", "R12", "R13", "R14", "R15",
};

static bool react;
static bool tuimode;
static bool alarmed;
static bool colorize;
static bool mousemode;
static bool printstats;
static bool showhighsse;

static int tyn;
static int txn;
static int tick;
static int speed;
static int vidya;
static int ttyin;
static int focus;
static int ttyout;
static int opline;
static int action;
static int xmmdisp;
static int exitcode;

static long ips;
static long rombase;
static long codesize;
static int64_t opstart;
static int64_t mapsstart;
static int64_t framesstart;
static int64_t breakpointsstart;
static uint64_t last_opcount;
static char *codepath;
static void *onbusted;
static char *statusmessage;
static struct Pty *pty;
static struct Machine *m;

static struct Panels pan;
static struct MemoryView codeview;
static struct MemoryView readview;
static struct MemoryView writeview;
static struct MemoryView stackview;
static struct MachineState laststate;
static struct Breakpoints breakpoints;
static struct MachineMemstat lastmemstat;
static struct XmmType xmmtype;
static struct Elf elf[1];
static struct Dis dis[1];

static long double last_seconds;
static long double statusexpires;
static struct termios oldterm;
static char logpath[PATH_MAX];
static char systemfailure[128];
static struct sigaction oldsig[4];

static void SetupDraw(void);
static void Redraw(void);

static char *FormatDouble(char buf[32], long double x) {
  g_xfmt_p(buf, &x, 15, 32, 0);
  return buf;
}

static int64_t SignExtend(uint64_t x, char b) {
  char k;
  assert(1 <= b && b <= 64);
  k = 64 - b;
  return (int64_t)(x << k) >> k;
}

static void SetCarry(bool cf) {
  m->flags = SetFlag(m->flags, FLAGS_CF, cf);
}

static bool IsCall(void) {
  return (m->xedd->op.dispatch == 0x0E8 ||
          (m->xedd->op.dispatch == 0x0FF && m->xedd->op.reg == 2));
}

static bool IsDebugBreak(void) {
  return m->xedd->op.map == XED_ILD_MAP0 && m->xedd->op.opcode == 0xCC;
}

static bool IsRet(void) {
  switch (m->xedd->op.dispatch) {
    case 0x0C2:
    case 0x0C3:
    case 0x0CA:
    case 0x0CB:
    case 0x0CF:
      return true;
    default:
      return false;
  }
}

static int GetXmmTypeCellCount(int r) {
  switch (xmmtype.type[r]) {
    case kXmmIntegral:
      return 16 / xmmtype.size[r];
    case kXmmFloat:
      return 4;
    case kXmmDouble:
      return 2;
    default:
      unreachable;
  }
}

static uint8_t CycleXmmType(uint8_t t) {
  switch (t) {
    default:
    case kXmmIntegral:
      return kXmmFloat;
    case kXmmFloat:
      return kXmmDouble;
    case kXmmDouble:
      return kXmmIntegral;
  }
}

static uint8_t CycleXmmDisp(uint8_t t) {
  switch (t) {
    default:
    case kXmmDecimal:
      return kXmmHex;
    case kXmmHex:
      return kXmmChar;
    case kXmmChar:
      return kXmmDecimal;
  }
}

static uint8_t CycleXmmSize(uint8_t w) {
  switch (w) {
    default:
    case 1:
      return 2;
    case 2:
      return 4;
    case 4:
      return 8;
    case 8:
      return 1;
  }
}

static int GetPointerWidth(void) {
  return 2 << (m->mode & 3);
}

static int64_t GetIp(void) {
  switch (GetPointerWidth()) {
    default:
    case 8:
      return m->ip;
    case 4:
      return Read64(m->cs) + (m->ip & 0xffff);
    case 2:
      return Read64(m->cs) + (m->ip & 0xffff);
  }
}

static int64_t GetSp(void) {
  switch (GetPointerWidth()) {
    default:
    case 8:
      return Read64(m->sp);
    case 4:
      return Read64(m->ss) + Read32(m->sp);
    case 2:
      return Read64(m->ss) + Read16(m->sp);
  }
}

static int64_t ReadWord(uint8_t *p) {
  switch (GetPointerWidth()) {
    default:
    case 8:
      return Read64(p);
    case 4:
      return Read32(p);
    case 2:
      return Read16(p);
  }
}

static void CopyMachineState(struct MachineState *ms) {
  ms->ip = m->ip;
  memcpy(ms->cs, m->cs, sizeof(m->cs));
  memcpy(ms->ss, m->ss, sizeof(m->ss));
  memcpy(ms->es, m->es, sizeof(m->es));
  memcpy(ms->ds, m->ds, sizeof(m->ds));
  memcpy(ms->fs, m->fs, sizeof(m->fs));
  memcpy(ms->gs, m->gs, sizeof(m->gs));
  memcpy(ms->reg, m->reg, sizeof(m->reg));
  memcpy(ms->xmm, m->xmm, sizeof(m->xmm));
  memcpy(&ms->fpu, &m->fpu, sizeof(m->fpu));
  memcpy(&ms->sse, &m->sse, sizeof(m->sse));
}

static void OnSigBusted(void) {
  CHECK(onbusted);
  longjmp(onbusted, 1);
}

static int VirtualBing(int64_t v) {
  int rc;
  uint8_t *p;
  jmp_buf busted;
  onbusted = busted;
  if ((p = FindReal(m, v))) {
    if (!setjmp(busted)) {
      rc = bing(p[0], 0);
    } else {
      rc = u'≀';
    }
  } else {
    rc = u'⋅';
  }
  onbusted = NULL;
  return rc;
}

static void ScrollOp(struct Panel *p, long op) {
  long n;
  opline = op;
  if ((n = p->bottom - p->top) > 1) {
    if (!(opstart + 1 <= op && op < opstart + n)) {
      opstart = MIN(MAX(0, op - n / 8), MAX(0, dis->ops.i - n));
    }
  }
}

static int TtyWriteString(const char *s) {
  return write(ttyout, s, strlen(s));
}

static void OnFeed(void) {
  TtyWriteString("\e[K\e[2J");
}

static void HideCursor(void) {
  TtyWriteString("\e[?25l");
}

static void ShowCursor(void) {
  TtyWriteString("\e[?25h");
}

static void EnableMouseTracking(void) {
  mousemode = true;
  TtyWriteString("\e[?1000;1002;1015;1006h");
}

static void DisableMouseTracking(void) {
  mousemode = false;
  TtyWriteString("\e[?1000;1002;1015;1006l");
}

static void ToggleMouseTracking(void) {
  if (mousemode) {
    DisableMouseTracking();
  } else {
    EnableMouseTracking();
  }
}

static void LeaveScreen(void) {
  TtyWriteString(gc(xasprintf("\e[%d;%dH\e[S\r\n", tyn, txn)));
}

static void GetTtySize(int fd) {
  struct winsize wsize;
  wsize.ws_row = tyn;
  wsize.ws_col = txn;
  getttysize(fd, &wsize);
  tyn = wsize.ws_row;
  txn = wsize.ws_col;
}

static void TuiRejuvinate(void) {
  struct termios term;
  DEBUGF("TuiRejuvinate");
  GetTtySize(ttyout);
  HideCursor();
  memcpy(&term, &oldterm, sizeof(term));
  term.c_cc[VMIN] = 1;
  term.c_cc[VTIME] = 1;
  term.c_iflag &= ~(INPCK | ISTRIP | PARMRK | INLCR | IGNCR | ICRNL | IXON);
  term.c_lflag &= ~(IEXTEN | ICANON | ECHO | ECHONL);
  term.c_cflag &= ~(CSIZE | PARENB);
  term.c_cflag |= CS8;
  term.c_iflag |= IUTF8;
  CHECK_NE(-1, ioctl(ttyout, TCSETS, &term));
  xsigaction(SIGBUS, OnSigBusted, SA_NODEFER, 0, NULL);
  EnableMouseTracking();
}

static void OnQ(void) {
  LOGF("OnQ");
  if (action & FAILURE) exit(1);
  action |= INT;
  breakpoints.i = 0;
}

static void OnV(void) {
  vidya = !vidya;
}

static void OnSigWinch(void) {
  action |= WINCHED;
}

static void OnSigInt(void) {
  if (tuimode) {
    action |= INT;
  } else {
    action |= EXIT;
  }
}

static void OnSigAlarm(void) {
  action |= ALARM;
}

static void OnSigCont(void) {
  TuiRejuvinate();
  Redraw();
}

static void TtyRestore1(void) {
  DEBUGF("TtyRestore1");
  ShowCursor();
  TtyWriteString("\e[0m");
}

static void TtyRestore2(void) {
  DEBUGF("TtyRestore2");
  ioctl(ttyout, TCSETS, &oldterm);
  DisableMouseTracking();
}

static void TuiCleanup(void) {
  sigaction(SIGCONT, oldsig + 2, NULL);
  TtyRestore1();
  DisableMouseTracking();
  tuimode = false;
}

static void ResolveBreakpoints(void) {
  long i, sym;
  for (i = 0; i < breakpoints.i; ++i) {
    if (breakpoints.p[i].symbol && !breakpoints.p[i].addr) {
      if ((sym = DisFindSymByName(dis, breakpoints.p[i].symbol)) != -1) {
        breakpoints.p[i].addr = dis->syms.p[sym].addr;
      } else {
        fprintf(stderr, "error: breakpoint not found: %s\n",
                breakpoints.p[i].symbol);
        exit(1);
      }
    }
  }
}

static void BreakAtNextInstruction(void) {
  struct Breakpoint b;
  memset(&b, 0, sizeof(b));
  b.addr = GetIp() + m->xedd->length;
  b.oneshot = true;
  PushBreakpoint(&breakpoints, &b);
}

static void LoadSyms(void) {
  LoadDebugSymbols(elf);
  DisLoadElf(dis, elf);
}

static int DrainInput(int fd) {
  char buf[32];
  struct pollfd fds[1];
  for (;;) {
    fds[0].fd = fd;
    fds[0].events = POLLIN;
    if (poll(fds, ARRAYLEN(fds), 0) == -1) return -1;
    if (!(fds[0].revents & POLLIN)) break;
    if (read(fd, buf, sizeof(buf)) == -1) return -1;
  }
  return 0;
}

static int ReadCursorPosition(int *out_y, int *out_x) {
  int y, x;
  char *p, buf[32];
  if (readansi(ttyin, buf, sizeof(buf)) == 1) return -1;
  p = buf;
  if (*p == '\e') ++p;
  if (*p == '[') ++p;
  y = strtol(p, &p, 10);
  if (*p == ';') ++p;
  x = strtol(p, &p, 10);
  if (*p != 'R') return ebadmsg();
  if (out_y) *out_y = MAX(1, y) - 1;
  if (out_x) *out_x = MAX(1, x) - 1;
  return 0;
}

static int GetCursorPosition(int *out_y, int *out_x) {
  TtyWriteString("\e[6n");
  return ReadCursorPosition(out_y, out_x);
}

static int GetTerminalDimensions(int *out_y, int *out_x) {
  TtyWriteString("\e7\e[9979;9979H\e[6n\e8");
  return ReadCursorPosition(out_y, out_x);
}

void CommonSetup(void) {
  static bool once;
  if (!once) {
    if (tuimode || breakpoints.i) {
      LoadSyms();
      ResolveBreakpoints();
    }
    once = true;
  }
}

void TuiSetup(void) {
  int y, x;
  bool report;
  static bool once;
  report = false;
  if (!once) {
    LOGF("loaded program %s\n%s", codepath, gc(FormatPml4t(m)));
    CommonSetup();
    ioctl(ttyout, TCGETS, &oldterm);
    xsigaction(SIGINT, OnSigInt, 0, 0, oldsig + 3);
    atexit(TtyRestore2);
    once = true;
    report = true;
  }
  setitimer(ITIMER_REAL, &((struct itimerval){0}), NULL);
  xsigaction(SIGCONT, OnSigCont, SA_RESTART | SA_NODEFER, 0, oldsig + 2);
  CopyMachineState(&laststate);
  TuiRejuvinate();
  if (report) {
    DrainInput(ttyin);
    y = 0;
    if (GetCursorPosition(&y, NULL) != -1) {
      TtyWriteString(gc(xasprintf("\e[%dS", y)));
    }
  }
}

static void ExecSetup(void) {
  CommonSetup();
  setitimer(ITIMER_REAL,
            &((struct itimerval){{0, 1. / 60 * 1e6}, {0, 1. / 60 * 1e6}}),
            NULL);
}

static void AppendPanel(struct Panel *p, long line, const char *s) {
  if (0 <= line && line < p->bottom - p->top) {
    AppendStr(&p->lines[line], s);
  }
}

static bool IsXmmNonZero(long start, long end) {
  long i;
  uint8_t v1[16], vz[16];
  for (i = start; i < end; ++i) {
    memset(vz, 0, 16);
    memcpy(v1, m->xmm[i], 16);
    pcmpeqb(v1, v1, vz);
    if (pmovmskb(v1) != 0xffff) {
      return true;
    }
  }
  return false;
}

static bool IsSegNonZero(void) {
  unsigned i;
  for (i = 0; i < 6; ++i) {
    if (Read64(GetSegment(m, 0, i))) {
      return true;
    }
  }
  return false;
}

static int PickNumberOfXmmRegistersToShow(void) {
  if (IsXmmNonZero(0, 8) || IsXmmNonZero(8, 16)) {
    if (showhighsse || IsXmmNonZero(8, 16)) {
      showhighsse = true;
      return 16;
    } else {
      return 8;
    }
  } else {
    showhighsse = false;
    return 0;
  }
}

void SetupDraw(void) {
  int i, j, n, a, b, yn, cpuy, ssey, dx[2], c2y[3], c3y[5];

  cpuy = 9;
  if (IsSegNonZero()) cpuy += 2;
  ssey = PickNumberOfXmmRegistersToShow();
  if (ssey) ++ssey;

  a = 12 + 1 + DUMPWIDTH;
  b = DISPWIDTH + 1;
  dx[1] = txn >= a + b ? txn - a : txn;
  dx[0] = txn >= a + b + b ? txn - a - b : dx[1];

  yn = tyn - 1;
  a = 1 / 8. * yn;
  b = 3 / 8. * yn;
  c2y[0] = a * .7;
  c2y[1] = a * 2;
  c2y[2] = a * 2 + b;
  if (yn - c2y[2] > 26) {
    c2y[1] -= yn - c2y[2] - 26;
    c2y[2] = yn - 26;
  }
  if (yn - c2y[2] < 26) {
    c2y[2] = yn - 26;
  }

  a = (yn - (cpuy + ssey) - 3) / 4;
  c3y[0] = cpuy;
  c3y[1] = cpuy + ssey;
  c3y[2] = cpuy + ssey + 1 + 1 + a * 1;
  c3y[3] = cpuy + ssey + 1 + 1 + a * 2 + 1;
  c3y[4] = cpuy + ssey + 1 + 1 + 1 + a * 3 + 1;

  /* COLUMN #1: DISASSEMBLY */

  pan.disassembly.top = 0;
  pan.disassembly.left = 0;
  pan.disassembly.bottom = yn;
  pan.disassembly.right = dx[0] - 1;

  /* COLUMN #2: BREAKPOINTS, MEMORY MAPS, BACKTRACE, DISPLAY */

  pan.breakpointshr.top = 0;
  pan.breakpointshr.left = dx[0];
  pan.breakpointshr.bottom = 1;
  pan.breakpointshr.right = dx[1] - 1;

  pan.breakpoints.top = 1;
  pan.breakpoints.left = dx[0];
  pan.breakpoints.bottom = c2y[0];
  pan.breakpoints.right = dx[1] - 1;

  pan.mapshr.top = c2y[0];
  pan.mapshr.left = dx[0];
  pan.mapshr.bottom = c2y[0] + 1;
  pan.mapshr.right = dx[1] - 1;

  pan.maps.top = c2y[0] + 1;
  pan.maps.left = dx[0];
  pan.maps.bottom = c2y[1];
  pan.maps.right = dx[1] - 1;

  pan.frameshr.top = c2y[1];
  pan.frameshr.left = dx[0];
  pan.frameshr.bottom = c2y[1] + 1;
  pan.frameshr.right = dx[1] - 1;

  pan.frames.top = c2y[1] + 1;
  pan.frames.left = dx[0];
  pan.frames.bottom = c2y[2];
  pan.frames.right = dx[1] - 1;

  pan.displayhr.top = c2y[2];
  pan.displayhr.left = dx[0];
  pan.displayhr.bottom = c2y[2] + 1;
  pan.displayhr.right = dx[1] - 1;

  pan.display.top = c2y[2] + 1;
  pan.display.left = dx[0];
  pan.display.bottom = yn;
  pan.display.right = dx[1] - 1;

  /* COLUMN #3: REGISTERS, VECTORS, CODE, MEMORY READS, MEMORY WRITES, STACK */

  pan.registers.top = 0;
  pan.registers.left = dx[1];
  pan.registers.bottom = c3y[0];
  pan.registers.right = txn;

  pan.ssehr.top = c3y[0];
  pan.ssehr.left = dx[1];
  pan.ssehr.bottom = c3y[0] + (ssey ? 1 : 0);
  pan.ssehr.right = txn;

  pan.sse.top = c3y[0] + (ssey ? 1 : 0);
  pan.sse.left = dx[1];
  pan.sse.bottom = c3y[1];
  pan.sse.right = txn;

  pan.codehr.top = c3y[1];
  pan.codehr.left = dx[1];
  pan.codehr.bottom = c3y[1] + 1;
  pan.codehr.right = txn;

  pan.code.top = c3y[1] + 1;
  pan.code.left = dx[1];
  pan.code.bottom = c3y[2];
  pan.code.right = txn;

  pan.readhr.top = c3y[2];
  pan.readhr.left = dx[1];
  pan.readhr.bottom = c3y[2] + 1;
  pan.readhr.right = txn;

  pan.readdata.top = c3y[2] + 1;
  pan.readdata.left = dx[1];
  pan.readdata.bottom = c3y[3];
  pan.readdata.right = txn;

  pan.writehr.top = c3y[3];
  pan.writehr.left = dx[1];
  pan.writehr.bottom = c3y[3] + 1;
  pan.writehr.right = txn;

  pan.writedata.top = c3y[3] + 1;
  pan.writedata.left = dx[1];
  pan.writedata.bottom = c3y[4];
  pan.writedata.right = txn;

  pan.stackhr.top = c3y[4];
  pan.stackhr.left = dx[1];
  pan.stackhr.bottom = c3y[4] + 1;
  pan.stackhr.right = txn;

  pan.stack.top = c3y[4] + 1;
  pan.stack.left = dx[1];
  pan.stack.bottom = yn;
  pan.stack.right = txn;

  pan.status.top = yn;
  pan.status.left = 0;
  pan.status.bottom = yn + 1;
  pan.status.right = txn;

  for (i = 0; i < ARRAYLEN(pan.p); ++i) {
    if (pan.p[i].left > pan.p[i].right) {
      pan.p[i].left = pan.p[i].right = 0;
    }
    if (pan.p[i].top > pan.p[i].bottom) {
      pan.p[i].top = pan.p[i].bottom = 0;
    }
    n = pan.p[i].bottom - pan.p[i].top;
    if (n == pan.p[i].n) {
      for (j = 0; j < n; ++j) {
        pan.p[i].lines[j].i = 0;
      }
    } else {
      for (j = 0; j < pan.p[i].n; ++j) {
        free(pan.p[i].lines[j].p);
      }
      free(pan.p[i].lines);
      pan.p[i].lines = xcalloc(n, sizeof(struct Buffer));
      pan.p[i].n = n;
    }
  }

  PtyResize(pty, pan.display.bottom - pan.display.top,
            pan.display.right - pan.display.left);
}

static long Disassemble(void) {
  long lines, current;
  lines = pan.disassembly.bottom - pan.disassembly.top * 2;
  if (Dis(dis, m, GetIp(), m->ip, lines) != -1) {
    return DisFind(dis, GetIp());
  } else {
    return -1;
  }
}

static long GetDisIndex(void) {
  long i;
  if ((i = DisFind(dis, GetIp())) == -1) {
    i = Disassemble();
  }
  while (i + 1 < dis->ops.i && !dis->ops.p[i].size) ++i;
  return i;
}

static void DrawDisassembly(struct Panel *p) {
  long i, j;
  for (i = 0; i < p->bottom - p->top; ++i) {
    j = opstart + i;
    if (0 <= j && j < dis->ops.i) {
      if (j == opline) AppendPanel(p, i, "\e[7m");
      AppendPanel(p, i, DisGetLine(dis, m, j));
      if (j == opline) AppendPanel(p, i, "\e[27m");
    }
  }
}

static void DrawHr(struct Panel *p, const char *s) {
  long i, wp, ws, wl, wr;
  if (p->bottom - p->top < 1) return;
  wp = p->right - p->left;
  ws = strwidth(s);
  wl = wp / 4 - ws / 2;
  wr = wp - (wl + ws);
  for (i = 0; i < wl; ++i) AppendWide(&p->lines[0], u'─');
  AppendStr(&p->lines[0], s);
  for (i = 0; i < wr; ++i) AppendWide(&p->lines[0], u'─');
  AppendStr(&p->lines[0], "\e[0m");
}

static void DrawTerminalHr(struct Panel *p) {
  long i;
  if (p->bottom == p->top) return;
  if (pty->conf & kPtyBell) {
    if (!alarmed) {
      alarmed = true;
      setitimer(ITIMER_REAL, &((struct itimerval){{0, 0}, {0, 800000}}), NULL);
    }
    AppendStr(&p->lines[0], "\e[1m");
  }
  AppendFmt(&p->lines[0], "──────────TELETYPEWRITER──%s──%s──%s──%s",
            (pty->conf & kPtyLed1) ? "\e[1;31m◎\e[0m" : "○",
            (pty->conf & kPtyLed2) ? "\e[1;32m◎\e[0m" : "○",
            (pty->conf & kPtyLed3) ? "\e[1;33m◎\e[0m" : "○",
            (pty->conf & kPtyLed4) ? "\e[1;34m◎\e[0m" : "○");
  for (i = 36; i < p->right - p->left; ++i) {
    AppendWide(&p->lines[0], u'─');
  }
}

static void DrawTerminal(struct Panel *p) {
  long y, yn;
  if (p->top == p->bottom) return;
  for (yn = MIN(pty->yn, p->bottom - p->top), y = 0; y < yn; ++y) {
    PtyAppendLine(pty, p->lines + y, y);
    AppendStr(p->lines + y, "\e[0m");
  }
}

static void DrawDisplay(struct Panel *p) {
  switch (vidya) {
    case 7:
      DrawHr(&pan.displayhr, "MONOCHROME DISPLAY ADAPTER");
      if (0xb0000 + 25 * 80 * 2 > m->real.n) return;
      DrawMda(p, (void *)(m->real.p + 0xb0000));
      break;
    case 3:
      DrawHr(&pan.displayhr, "COLOR GRAPHICS ADAPTER");
      if (0xb8000 + 25 * 80 * 2 > m->real.n) return;
      DrawCga(p, (void *)(m->real.p + 0xb8000));
      break;
    default:
      DrawTerminalHr(&pan.displayhr);
      DrawTerminal(p);
      break;
  }
}

static void DrawFlag(struct Panel *p, long i, char name, bool value) {
  char str[3] = "  ";
  if (value) str[1] = name;
  AppendPanel(p, i, str);
}

static void DrawRegister(struct Panel *p, long i, long r) {
  char buf[32];
  uint64_t value, previous;
  value = Read64(m->reg[r]);
  previous = Read64(laststate.reg[r]);
  if (value != previous) AppendPanel(p, i, "\e[7m");
  snprintf(buf, sizeof(buf), "%-3s", kRegisterNames[r]);
  AppendPanel(p, i, buf);
  AppendPanel(p, i, " ");
  snprintf(buf, sizeof(buf), "0x%016lx", value);
  AppendPanel(p, i, buf);
  if (value != previous) AppendPanel(p, i, "\e[27m");
  AppendPanel(p, i, "  ");
}

static void DrawSegment(struct Panel *p, long i, const uint8_t seg[8],
                        const uint8_t last[8], const char *name) {
  char buf[32];
  uint64_t value, previous;
  value = Read64(seg);
  previous = Read64(last);
  if (value != previous) AppendPanel(p, i, "\e[7m");
  snprintf(buf, sizeof(buf), "%-3s", name);
  AppendPanel(p, i, buf);
  AppendPanel(p, i, " ");
  snprintf(buf, sizeof(buf), "0x%016lx", value);
  AppendPanel(p, i, buf);
  if (value != previous) AppendPanel(p, i, "\e[27m");
  AppendPanel(p, i, "  ");
}

static void DrawSt(struct Panel *p, long i, long r) {
  char buf[32];
  long double value;
  bool isempty, changed;
  isempty = FpuGetTag(m, r) == kFpuTagEmpty;
  if (isempty) AppendPanel(p, i, "\e[38;5;241m");
  value = m->fpu.st[(r + m->fpu.sp) & 0b111];
  changed = value != laststate.fpu.st[(r + m->fpu.sp) & 0b111];
  if (!isempty && changed) AppendPanel(p, i, "\e[7m");
  snprintf(buf, sizeof(buf), "ST%d ", r);
  AppendPanel(p, i, buf);
  AppendPanel(p, i, FormatDouble(buf, value));
  if (changed) AppendPanel(p, i, "\e[27m");
  AppendPanel(p, i, "  ");
  if (isempty) AppendPanel(p, i, "\e[39m");
}

static void DrawCpu(struct Panel *p) {
  char buf[48];
  if (p->top == p->bottom) return;
  DrawRegister(p, 0, 7), DrawRegister(p, 0, 0), DrawSt(p, 0, 0);
  DrawRegister(p, 1, 6), DrawRegister(p, 1, 3), DrawSt(p, 1, 1);
  DrawRegister(p, 2, 2), DrawRegister(p, 2, 5), DrawSt(p, 2, 2);
  DrawRegister(p, 3, 1), DrawRegister(p, 3, 4), DrawSt(p, 3, 3);
  DrawRegister(p, 4, 8), DrawRegister(p, 4, 12), DrawSt(p, 4, 4);
  DrawRegister(p, 5, 9), DrawRegister(p, 5, 13), DrawSt(p, 5, 5);
  DrawRegister(p, 6, 10), DrawRegister(p, 6, 14), DrawSt(p, 6, 6);
  DrawRegister(p, 7, 11), DrawRegister(p, 7, 15), DrawSt(p, 7, 7);
  snprintf(buf, sizeof(buf), "RIP 0x%016x  FLG", m->ip);
  AppendPanel(p, 8, buf);
  DrawFlag(p, 8, 'C', GetFlag(m->flags, FLAGS_CF));
  DrawFlag(p, 8, 'P', GetFlag(m->flags, FLAGS_PF));
  DrawFlag(p, 8, 'A', GetFlag(m->flags, FLAGS_AF));
  DrawFlag(p, 8, 'Z', GetFlag(m->flags, FLAGS_ZF));
  DrawFlag(p, 8, 'S', GetFlag(m->flags, FLAGS_SF));
  DrawFlag(p, 8, 'I', GetFlag(m->flags, FLAGS_IF));
  DrawFlag(p, 8, 'D', GetFlag(m->flags, FLAGS_DF));
  DrawFlag(p, 8, 'O', GetFlag(m->flags, FLAGS_OF));
  AppendPanel(p, 8, "    ");
  if (m->fpu.ie) AppendPanel(p, 8, " IE");
  if (m->fpu.de) AppendPanel(p, 8, " DE");
  if (m->fpu.ze) AppendPanel(p, 8, " ZE");
  if (m->fpu.oe) AppendPanel(p, 8, " OE");
  if (m->fpu.ue) AppendPanel(p, 8, " UE");
  if (m->fpu.pe) AppendPanel(p, 8, " PE");
  if (m->fpu.sf) AppendPanel(p, 8, " SF");
  if (m->fpu.es) AppendPanel(p, 8, " ES");
  if (m->fpu.c0) AppendPanel(p, 8, " C0");
  if (m->fpu.c1) AppendPanel(p, 8, " C1");
  if (m->fpu.c2) AppendPanel(p, 8, " C2");
  if (m->fpu.bf) AppendPanel(p, 8, " BF");
  DrawSegment(p, 9, m->fs, laststate.fs, "FS");
  DrawSegment(p, 9, m->ds, laststate.ds, "DS");
  DrawSegment(p, 9, m->cs, laststate.cs, "CS");
  DrawSegment(p, 10, m->gs, laststate.gs, "GS");
  DrawSegment(p, 10, m->es, laststate.es, "ES");
  DrawSegment(p, 10, m->ss, laststate.ss, "SS");
}

static void DrawXmm(struct Panel *p, long i, long r) {
  float f;
  double d;
  long j, k, n;
  bool changed;
  char buf[32];
  uint8_t xmm[16];
  uint64_t ival, itmp;
  int cells, left, cellwidth, panwidth;
  memcpy(xmm, m->xmm[r], sizeof(xmm));
  changed = memcmp(xmm, laststate.xmm[r], sizeof(xmm)) != 0;
  if (changed) AppendPanel(p, i, "\e[7m");
  left = sprintf(buf, "XMM%-2d", r);
  AppendPanel(p, i, buf);
  cells = GetXmmTypeCellCount(r);
  panwidth = p->right - p->left;
  cellwidth = MIN(MAX(0, (panwidth - left) / cells - 1), sizeof(buf) - 1);
  for (j = 0; j < cells; ++j) {
    AppendPanel(p, i, " ");
    switch (xmmtype.type[r]) {
      case kXmmFloat:
        memcpy(&f, xmm + j * sizeof(f), sizeof(f));
        FormatDouble(buf, f);
        break;
      case kXmmDouble:
        memcpy(&d, xmm + j * sizeof(d), sizeof(d));
        FormatDouble(buf, d);
        break;
      case kXmmIntegral:
        ival = 0;
        for (k = 0; k < xmmtype.size[r]; ++k) {
          itmp = xmm[j * xmmtype.size[r] + k] & 0xff;
          itmp <<= k * 8;
          ival |= itmp;
        }
        if (xmmdisp == kXmmHex || xmmdisp == kXmmChar) {
          if (xmmdisp == kXmmChar && iswalnum(ival)) {
            sprintf(buf, "%lc", ival);
          } else {
            uint64toarray_fixed16(ival, buf, xmmtype.size[r] * 8);
          }
        } else {
          int64toarray_radix10(SignExtend(ival, xmmtype.size[r] * 8), buf);
        }
        break;
      default:
        unreachable;
    }
    buf[cellwidth] = '\0';
    AppendPanel(p, i, buf);
    n = cellwidth - strlen(buf);
    for (k = 0; k < n; ++k) {
      AppendPanel(p, i, " ");
    }
  }
  if (changed) AppendPanel(p, i, "\e[27m");
}

static void DrawSse(struct Panel *p) {
  long i, n;
  n = p->bottom - p->top;
  if (n > 0) {
    for (i = 0; i < MIN(16, n); ++i) {
      DrawXmm(p, i, i);
    }
  }
}

static void ScrollMemoryView(struct Panel *p, struct MemoryView *v, int64_t a) {
  long i, n;
  n = p->bottom - p->top;
  i = a / (DUMPWIDTH * (1ull << v->zoom));
  if (!(v->start <= i && i < v->start + n)) {
    v->start = i;
  }
}

static void ZoomMemoryView(struct MemoryView *v, long y, long x, int dy) {
  long a, b, i, s;
  s = v->start;
  a = v->zoom;
  b = MIN(MAXZOOM, MAX(0, a + dy));
  i = y * DUMPWIDTH - x;
  s *= DUMPWIDTH * (1L << a);
  s += i * (1L << a) - i * (1L << b);
  s /= DUMPWIDTH * (1L << b);
  v->zoom = b;
  v->start = s;
}

static void ScrollMemoryViews(void) {
  ScrollMemoryView(&pan.code, &codeview, GetIp());
  ScrollMemoryView(&pan.readdata, &readview, m->readaddr);
  ScrollMemoryView(&pan.writedata, &writeview, m->writeaddr);
  ScrollMemoryView(&pan.stack, &stackview, GetSp());
}

static void ZoomMemoryViews(struct Panel *p, int y, int x, int dy) {
  if (p == &pan.code) {
    ZoomMemoryView(&codeview, y, x, dy);
  } else if (p == &pan.readdata) {
    ZoomMemoryView(&readview, y, x, dy);
  } else if (p == &pan.writedata) {
    ZoomMemoryView(&writeview, y, x, dy);
  } else if (p == &pan.stack) {
    ZoomMemoryView(&stackview, y, x, dy);
  }
}

static void DrawMemoryZoomed(struct Panel *p, struct MemoryView *view,
                             long histart, long hiend) {
  bool high, changed;
  uint8_t *canvas, *chunk, *invalid;
  int64_t a, b, c, d, n, i, j, k, size;
  struct ContiguousMemoryRanges ranges;
  a = view->start * DUMPWIDTH * (1ull << view->zoom);
  b = (view->start + (p->bottom - p->top)) * DUMPWIDTH * (1ull << view->zoom);
  size = (p->bottom - p->top) * DUMPWIDTH;
  canvas = xcalloc(1, size);
  invalid = xcalloc(1, size);
  memset(&ranges, 0, sizeof(ranges));
  FindContiguousMemoryRanges(m, &ranges);
  for (k = i = 0; i < ranges.i; ++i) {
    if ((a >= ranges.p[i].a && a < ranges.p[i].b) ||
        (b >= ranges.p[i].a && b < ranges.p[i].b) ||
        (a < ranges.p[i].a && b >= ranges.p[i].b)) {
      c = MAX(a, ranges.p[i].a);
      d = MIN(b, ranges.p[i].b);
      n = ROUNDUP(ROUNDUP(d - c, 16), 1ull << view->zoom);
      chunk = xmalloc(n);
      VirtualSend(m, chunk, c, d - c);
      memset(chunk + (d - c), 0, n - (d - c));
      for (j = 0; j < view->zoom; ++j) {
        cDecimate2xUint8x8(ROUNDUP(n, 16), chunk, kThePerfectKernel);
        n >>= 1;
      }
      j = (c - a) / (1ull << view->zoom);
      memset(invalid + k, -1, j - k);
      memcpy(canvas + j, chunk, MIN(n, size - j));
      k = j + MIN(n, size - j);
      free(chunk);
    }
  }
  memset(invalid + k, -1, size - k);
  free(ranges.p);
  high = false;
  for (c = i = 0; i < p->bottom - p->top; ++i) {
    AppendFmt(&p->lines[i], "%p ",
              (view->start + i) * DUMPWIDTH * (1ull << view->zoom));
    for (j = 0; j < DUMPWIDTH; ++j, ++c) {
      a = ((view->start + i) * DUMPWIDTH + j + 0) * (1ull << view->zoom);
      b = ((view->start + i) * DUMPWIDTH + j + 1) * (1ull << view->zoom);
      changed = ((histart >= a && hiend < b) ||
                 (histart && hiend && histart >= a && hiend < b));
      if (changed && !high) {
        high = true;
        AppendStr(&p->lines[i], "\e[7m");
      } else if (!changed && high) {
        AppendStr(&p->lines[i], "\e[27m");
        high = false;
      }
      if (invalid[c]) {
        AppendWide(&p->lines[i], u'⋅');
      } else {
        AppendWide(&p->lines[i], kCp437[canvas[c]]);
      }
    }
    if (high) {
      AppendStr(&p->lines[i], "\e[27m");
      high = false;
    }
  }
  free(invalid);
  free(canvas);
}

static void DrawMemoryUnzoomed(struct Panel *p, struct MemoryView *view,
                               long histart, long hiend) {
  long i, j, k, c;
  bool high, changed;
  high = false;
  for (i = 0; i < p->bottom - p->top; ++i) {
    AppendFmt(&p->lines[i], "%p ", (view->start + i) * DUMPWIDTH);
    for (j = 0; j < DUMPWIDTH; ++j) {
      k = (view->start + i) * DUMPWIDTH + j;
      c = VirtualBing(k);
      changed = histart <= k && k < hiend;
      if (changed && !high) {
        high = true;
        AppendStr(&p->lines[i], "\e[7m");
      } else if (!changed && high) {
        AppendStr(&p->lines[i], "\e[27m");
        high = false;
      }
      AppendWide(&p->lines[i], c);
    }
    if (high) {
      AppendStr(&p->lines[i], "\e[27m");
      high = false;
    }
  }
}

static void DrawMemory(struct Panel *p, struct MemoryView *view, long histart,
                       long hiend) {
  if (p->top == p->bottom) return;
  if (view->zoom) {
    DrawMemoryZoomed(p, view, histart, hiend);
  } else {
    DrawMemoryUnzoomed(p, view, histart, hiend);
  }
}

static void DrawMaps(struct Panel *p) {
  int i;
  char *text, *p1, *p2;
  if (p->top == p->bottom) return;
  p1 = text = FormatPml4t(m);
  for (i = 0; p1; ++i, p1 = p2) {
    if ((p2 = strchr(p1, '\n'))) *p2++ = '\0';
    if (i >= mapsstart) {
      AppendPanel(p, i - mapsstart, p1);
    }
  }
  free(text);
}

static void DrawBreakpoints(struct Panel *p) {
  int64_t addr;
  const char *name;
  char *s, buf[256];
  long i, line, sym;
  if (p->top == p->bottom) return;
  for (line = 0, i = breakpoints.i; i--;) {
    if (breakpoints.p[i].disable) continue;
    if (line >= breakpointsstart) {
      addr = breakpoints.p[i].addr;
      sym = DisFindSym(dis, addr);
      name = sym != -1 ? dis->syms.stab + dis->syms.p[sym].name : "UNKNOWN";
      s = buf;
      s += sprintf(s, "%p ", addr);
      CHECK_LT(Demangle(s, name, DIS_MAX_SYMBOL_LENGTH), buf + ARRAYLEN(buf));
      AppendPanel(p, line - breakpointsstart, buf);
      if (sym != -1 && addr != dis->syms.p[sym].addr) {
        snprintf(buf, sizeof(buf), "+%#lx", addr - dis->syms.p[sym].addr);
        AppendPanel(p, line, buf);
      }
    }
    ++line;
  }
}

static int GetPreferredStackAlignmentMask(void) {
  switch (m->mode & 3) {
    case XED_MODE_LONG:
      return 15;
    case XED_MODE_LEGACY:
      return 3;
    case XED_MODE_REAL:
      return 3;
    default:
      unreachable;
  }
}

static void DrawFrames(struct Panel *p) {
  int i, n;
  long sym;
  uint8_t *r;
  const char *name;
  char *s, line[256];
  int64_t sp, bp, rp;
  if (p->top == p->bottom) return;
  rp = m->ip;
  bp = Read64(m->bp);
  sp = Read64(m->sp);
  for (i = 0; i < p->bottom - p->top;) {
    sym = DisFindSym(dis, rp);
    name = sym != -1 ? dis->syms.stab + dis->syms.p[sym].name : "UNKNOWN";
    s = line;
    s += sprintf(s, "%p %p ", Read64(m->ss) + bp, rp);
    s = Demangle(s, name, DIS_MAX_SYMBOL_LENGTH);
    AppendPanel(p, i - framesstart, line);
    if (sym != -1 && rp != dis->syms.p[sym].addr) {
      snprintf(line, sizeof(line), "+%#lx", rp - dis->syms.p[sym].addr);
      AppendPanel(p, i - framesstart, line);
    }
    if (!bp) break;
    if (bp < sp) {
      AppendPanel(p, i - framesstart, " [STRAY]");
    } else if (bp - sp <= 0x1000) {
      snprintf(line, sizeof(line), " %,ld bytes", bp - sp);
      AppendPanel(p, i - framesstart, line);
    }
    if (bp & GetPreferredStackAlignmentMask() && i) {
      AppendPanel(p, i - framesstart, " [MISALIGN]");
    }
    ++i;
    if (((Read64(m->ss) + bp) & 0xfff) > 0xff0) break;
    if (!(r = FindReal(m, Read64(m->ss) + bp))) {
      AppendPanel(p, i - framesstart, "CORRUPT FRAME POINTER");
      break;
    }
    sp = bp;
    bp = ReadWord(r + 0);
    rp = ReadWord(r + 8);
  }
}

static void CheckFramePointerImpl(void) {
  uint8_t *r;
  int64_t bp, sp, rp;
  static int64_t lastbp;
  bp = Read64(m->bp);
  if (bp && bp == lastbp) return;
  lastbp = bp;
  rp = m->ip;
  sp = Read64(m->sp);
  while (bp) {
    if (!(r = FindReal(m, Read64(m->ss) + bp))) {
      LOGF("corrupt frame: %p", bp);
      ThrowProtectionFault(m);
    }
    sp = bp;
    bp = Read64(r + 0) - 0;
    rp = Read64(r + 8) - 1;
    if (!bp && !(m->bofram[0] <= rp && rp <= m->bofram[1])) {
      LOGF("bad frame !(%p <= %p <= %p)", m->bofram[0], rp, m->bofram[1]);
      ThrowProtectionFault(m);
    }
  }
}

forceinline void CheckFramePointer(void) {
  if (m->bofram[0]) {
    CheckFramePointerImpl();
  }
}

static bool IsExecuting(void) {
  return (action & (CONTINUE | STEP | NEXT | FINISH)) && !(action & FAILURE);
}

static int AppendStat(struct Buffer *b, const char *name, int64_t value,
                      bool changed) {
  int width;
  AppendChar(b, ' ');
  if (changed) AppendStr(b, "\e[31m");
  width = AppendFmt(b, "%,8ld %s", value, name);
  if (changed) AppendStr(b, "\e[39m");
  return 1 + width;
}

static void DrawStatus(struct Panel *p) {
  int yn, xn, rw;
  struct Buffer *s;
  struct MachineMemstat *a, *b;
  yn = p->top - p->bottom;
  xn = p->right - p->left;
  if (!yn || !xn) return;
  rw = 0;
  a = &m->memstat;
  b = &lastmemstat;
  s = xmalloc(sizeof(struct Buffer));
  memset(s, 0, sizeof(*s));
  if (ips > 0) rw += AppendStat(s, "ips", ips, false);
  rw += AppendStat(s, "kb", m->real.n / 1024, false);
  rw += AppendStat(s, "reserve", a->reserved, a->reserved != b->reserved);
  rw += AppendStat(s, "commit", a->committed, a->committed != b->committed);
  rw += AppendStat(s, "freed", a->freed, a->freed != b->freed);
  rw += AppendStat(s, "tables", a->pagetables, a->pagetables != b->pagetables);
  rw += AppendStat(s, "fds", m->fds.i, false);
  AppendFmt(&p->lines[0], "\e[7m%-*s%s\e[0m", xn - rw,
            statusmessage && nowl() < statusexpires ? statusmessage
                                                    : "das blinkenlights",
            s->p);
  free(s->p);
  free(s);
  memcpy(b, a, sizeof(*a));
}

static void PreventBufferbloat(void) {
  long double now, rate;
  static long double last;
  now = nowl();
  rate = 1. / 60;
  if (now - last < rate) {
    dsleep(rate - (now - last));
  }
  last = now;
}

static void Redraw(void) {
  int i, j;
  ScrollOp(&pan.disassembly, GetDisIndex());
  if (last_opcount) {
    ips = unsignedsubtract(opcount, last_opcount) / (nowl() - last_seconds);
  }
  SetupDraw();
  for (i = 0; i < ARRAYLEN(pan.p); ++i) {
    for (j = 0; j < pan.p[i].bottom - pan.p[i].top; ++j) {
      pan.p[i].lines[j].i = 0;
    }
  }
  DrawDisassembly(&pan.disassembly);
  DrawDisplay(&pan.display);
  DrawCpu(&pan.registers);
  DrawSse(&pan.sse);
  DrawHr(&pan.breakpointshr, "BREAKPOINTS");
  DrawHr(&pan.mapshr, "PML4T");
  DrawHr(&pan.frameshr, m->bofram[0] ? "PROTECTED FRAMES" : "FRAMES");
  DrawHr(&pan.ssehr, "SSE");
  DrawHr(&pan.codehr, "CODE");
  DrawHr(&pan.readhr, "READ");
  DrawHr(&pan.writehr, "WRITE");
  DrawHr(&pan.stackhr, "STACK");
  DrawMaps(&pan.maps);
  DrawFrames(&pan.frames);
  DrawBreakpoints(&pan.breakpoints);
  DrawMemory(&pan.code, &codeview, GetIp(), GetIp() + m->xedd->length);
  DrawMemory(&pan.readdata, &readview, m->readaddr, m->readaddr + m->readsize);
  DrawMemory(&pan.writedata, &writeview, m->writeaddr,
             m->writeaddr + m->writesize);
  DrawMemory(&pan.stack, &stackview, GetSp(), GetSp() + GetPointerWidth());
  DrawStatus(&pan.status);
  PreventBufferbloat();
  if (PrintPanels(ttyout, ARRAYLEN(pan.p), pan.p, tyn, txn) == -1) {
    LOGF("PrintPanels Interrupted");
    CHECK_EQ(EINTR, errno);
  }
  last_opcount = opcount;
  last_seconds = nowl();
  CopyMachineState(&laststate);
}

static void ReactiveDraw(void) {
  if (tuimode) {
    m->ip -= m->xedd->length;
    Redraw();
    m->ip += m->xedd->length;
    tick = speed;
  }
}

static void HandleAlarm(void) {
  alarmed = false;
  action &= ~ALARM;
  pty->conf &= ~kPtyBell;
  free(statusmessage);
  statusmessage = NULL;
}

static void HandleAppReadInterrupt(void) {
  DEBUGF("HandleAppReadInterrupt");
  if (action & ALARM) {
    HandleAlarm();
  }
  if (action & WINCHED) {
    GetTtySize(ttyout);
    action &= ~WINCHED;
  }
  if (action & INT) {
    action &= ~INT;
    if (action & CONTINUE) {
      action &= ~CONTINUE;
    } else {
      if (tuimode) {
        LeaveScreen();
        TuiCleanup();
      }
      exit(0);
    }
  }
}

static int OnPtyFdClose(int fd) {
  return close(fd);
}

static bool HasPendingInput(int fd) {
  struct pollfd fds[1];
  fds[0].fd = fd;
  fds[0].events = POLLIN;
  fds[0].revents = 0;
  poll(fds, ARRAYLEN(fds), 0);
  return fds[0].revents & (POLLIN | POLLERR);
}

static ssize_t ReadPtyFdDirect(int fd, void *data, size_t size) {
  char *buf;
  ssize_t rc;
  DEBUGF("ReadPtyFdDirect");
  buf = malloc(PAGESIZE);
  pty->conf |= kPtyBlinkcursor;
  if (tuimode) DisableMouseTracking();
  for (;;) {
    ReactiveDraw();
    if ((rc = read(fd, buf, PAGESIZE)) != -1) break;
    CHECK_EQ(EINTR, errno);
    HandleAppReadInterrupt();
  }
  if (tuimode) EnableMouseTracking();
  pty->conf &= ~kPtyBlinkcursor;
  if (rc > 0) {
    PtyWriteInput(pty, buf, rc);
    ReactiveDraw();
    rc = PtyRead(pty, data, size);
  }
  free(buf);
  return rc;
}

static ssize_t OnPtyFdReadv(int fd, const struct iovec *iov, int iovlen) {
  int i;
  ssize_t rc;
  void *data;
  size_t size;
  for (size = i = 0; i < iovlen; ++i) {
    if (iov[i].iov_len) {
      data = iov[i].iov_base;
      size = iov[i].iov_len;
      break;
    }
  }
  if (size) {
    if (!(rc = PtyRead(pty, data, size))) {
      rc = ReadPtyFdDirect(fd, data, size);
    }
    return rc;
  } else {
    return 0;
  }
}

static void DrawDisplayOnly(struct Panel *p) {
  struct Buffer b;
  int i, y, yn, xn, tly, tlx, conf;
  yn = MIN(tyn, p->bottom - p->top);
  xn = MIN(txn, p->right - p->left);
  for (i = 0; i < yn; ++i) {
    p->lines[i].i = 0;
  }
  DrawDisplay(p);
  memset(&b, 0, sizeof(b));
  tly = tyn / 2 - yn / 2;
  tlx = txn / 2 - xn / 2;
  AppendStr(&b, "\e[0m\e[H");
  for (y = 0; y < tyn; ++y) {
    if (y) AppendStr(&b, "\r\n");
    if (tly <= y && y <= tly + yn) {
      for (i = 0; i < tlx; ++i) {
        AppendChar(&b, ' ');
      }
      AppendData(&b, p->lines[y - tly].p, p->lines[y - tly].i);
    }
    AppendStr(&b, "\e[0m\e[K");
  }
  write(ttyout, b.p, b.i);
  free(b.p);
}

static ssize_t OnPtyFdWritev(int fd, const struct iovec *iov, int iovlen) {
  int i;
  size_t size;
  for (size = i = 0; i < iovlen; ++i) {
    PtyWrite(pty, iov[i].iov_base, iov[i].iov_len);
    size += iov[i].iov_len;
  }
  return size;
}

static int OnPtyFdTiocgwinsz(int fd, struct winsize *ws) {
  ws->ws_row = pty->yn;
  ws->ws_col = pty->xn;
  return 0;
}

static int OnPtyFdTcgets(int fd, struct termios *c) {
  memset(c, 0, sizeof(*c));
  if (!(pty->conf & kPtyNocanon)) c->c_iflag |= ICANON;
  if (!(pty->conf & kPtyNoecho)) c->c_iflag |= ECHO;
  if (!(pty->conf & kPtyNoopost)) c->c_oflag |= OPOST;
  return 0;
}

static int OnPtyFdTcsets(int fd, uint64_t request, struct termios *c) {
  if (c->c_iflag & ICANON) {
    pty->conf &= ~kPtyNocanon;
  } else {
    pty->conf |= kPtyNocanon;
  }
  if (c->c_iflag & ECHO) {
    pty->conf &= ~kPtyNoecho;
  } else {
    pty->conf |= kPtyNoecho;
  }
  if (c->c_oflag & OPOST) {
    pty->conf &= ~kPtyNoopost;
  } else {
    pty->conf |= kPtyNoopost;
  }
  return 0;
}

static int OnPtyFdIoctl(int fd, uint64_t request, void *memory) {
  if (request == TIOCGWINSZ) {
    return OnPtyFdTiocgwinsz(fd, memory);
  } else if (request == TCGETS) {
    return OnPtyFdTcgets(fd, memory);
  } else if (request == TCSETS || request == TCSETSW || request == TCSETSF) {
    return OnPtyFdTcsets(fd, request, memory);
  } else {
    return einval();
  }
}

static const struct MachineFdCb kMachineFdCbPty = {
    .close = OnPtyFdClose,
    .readv = OnPtyFdReadv,
    .writev = OnPtyFdWritev,
    .ioctl = OnPtyFdIoctl,
};

static void LaunchDebuggerReactively(void) {
  LOGF("%s", systemfailure);
  if (tuimode) {
    action |= FAILURE;
  } else {
    if (react) {
      tuimode = true;
      action |= FAILURE;
    } else {
      fprintf(stderr, "ERROR: %s\n", systemfailure);
      exit(1);
    }
  }
}

static void OnDebug(void) {
  strcpy(systemfailure, "IT'S A TRAP");
  LaunchDebuggerReactively();
}

static void OnSegmentationFault(void) {
  snprintf(systemfailure, sizeof(systemfailure), "SEGMENTATION FAULT %p",
           m->faultaddr);
  LaunchDebuggerReactively();
}

static void OnProtectionFault(void) {
  strcpy(systemfailure, "PROTECTION FAULT");
  LaunchDebuggerReactively();
}

static void OnSimdException(void) {
  strcpy(systemfailure, "SIMD FAULT");
  LaunchDebuggerReactively();
}

static void OnUndefinedInstruction(void) {
  strcpy(systemfailure, "UNDEFINED INSTRUCTION");
  LaunchDebuggerReactively();
}

static void OnDecodeError(void) {
  strcpy(stpcpy(systemfailure, "DECODE: "),
         IndexDoubleNulString(kXedErrorNames, m->xedd->op.error));
  LaunchDebuggerReactively();
}

static void OnDivideError(void) {
  strcpy(systemfailure, "DIVIDE BY ZERO OR BANE/-1");
  LaunchDebuggerReactively();
}

static void OnFpuException(void) {
  strcpy(systemfailure, "FPU EXCEPTION");
  LaunchDebuggerReactively();
}

static void OnExit(int rc) {
  exitcode = rc;
  action |= EXIT;
}

static size_t GetLastIndex(size_t size, unsigned unit, int i, unsigned limit) {
  unsigned q, r;
  if (!size) return 0;
  q = size / unit;
  r = size % unit;
  if (!r) --q;
  q += i;
  if (q > limit) q = limit;
  return q;
}

static void OnDiskServiceReset(void) {
  m->ax[1] = 0x00;
  SetCarry(false);
}

static void OnDiskServiceBadCommand(void) {
  m->ax[1] = 0x01;
  SetCarry(true);
}

static void OnDiskServiceGetParams(void) {
  size_t lastsector, lasttrack, lasthead;
  lasthead = GetLastIndex(elf->mapsize, 512 * 63 * 1024, 0, 255);
  lasttrack = GetLastIndex(elf->mapsize, 512 * 63, 0, 1023);
  lastsector = GetLastIndex(elf->mapsize, 512, 1, 63);
  m->dx[0] = 1;
  m->dx[1] = lasthead;
  m->cx[0] = lasttrack >> 8 << 6 | lastsector;
  m->cx[1] = lasttrack;
  m->ax[1] = 0;
  Write64(m->es, 0);
  Write16(m->di, 0);
  SetCarry(false);
}

static void OnDiskServiceReadSectors(void) {
  static int x;
  uint64_t addr, size;
  int64_t sectors, drive, head, track, sector, offset;
  sectors = m->ax[0];
  drive = m->dx[0];
  head = m->dx[1];
  track = (m->cx[0] & 0b11000000) << 2 | m->cx[1];
  sector = (m->cx[0] & 0b00111111) - 1;
  offset = head * track * sector * 512;
  size = sectors * 512;
  offset = sector * 512 + track * 512 * 63 + head * 512 * 63 * 1024;
  VERBOSEF("bios read sectors %d "
           "@ sector %ld track %ld head %ld drive %ld offset %#lx",
           sectors, sector, track, head, drive, offset);
  if (0 <= sector && offset + size <= elf->mapsize) {
    addr = Read64(m->es) + Read16(m->bx);
    if (addr + size <= m->real.n) {
      SetWriteAddr(m, addr, size);
      memcpy(m->real.p + addr, elf->map + offset, size);
      m->ax[1] = 0x00;
      SetCarry(false);
    } else {
      m->ax[0] = 0x00;
      m->ax[1] = 0x02;
      SetCarry(true);
    }
  } else {
    WARNF("bios read sector failed 0 <= %d && %lx + %lx <= %lx", sector, offset,
          size, elf->mapsize);
    m->ax[0] = 0x00;
    m->ax[1] = 0x0d;
    SetCarry(true);
  }
}

static void OnDiskService(void) {
  switch (m->ax[1]) {
    case 0x00:
      OnDiskServiceReset();
      break;
    case 0x02:
      OnDiskServiceReadSectors();
      break;
    case 0x08:
      OnDiskServiceGetParams();
      break;
    default:
      OnDiskServiceBadCommand();
      break;
  }
}

static void OnVidyaServiceSetMode(void) {
  if (FindReal(m, 0xB0000)) {
    vidya = m->ax[0];
  } else {
    WARNF("maybe you forgot -r flag");
  }
}

static void OnVidyaServiceGetMode(void) {
  m->ax[0] = vidya;
  m->ax[1] = 80;  // columns
  m->bx[1] = 0;   // page
}

static void OnVidyaServiceSetCursorPosition(void) {
  PtySetY(pty, m->dx[1]);
  PtySetX(pty, m->dx[0]);
}

static void OnVidyaServiceGetCursorPosition(void) {
  m->dx[1] = pty->y;
  m->dx[0] = pty->x;
  m->cx[1] = 5;  // cursor ▂ scan lines 5..7 of 0..7
  m->cx[0] = 7 | !!(pty->conf & kPtyNocursor) << 5;
}

static int GetVidyaByte(unsigned char b) {
  if (0x20 <= b && b <= 0x7F) return b;
  if (b == 0xFF) b = 0x00;
  return kCp437[b];
}

static void OnVidyaServiceWriteCharacter(void) {
  int i, n, y, x;
  char *p, buf[32];
  p = buf;
  p += FormatCga(m->bx[0], p);
  p = stpcpy(p, "\e7");
  p += tpencode(p, 8, GetVidyaByte(m->ax[0]), false);
  p = stpcpy(p, "\e8");
  for (i = Read16(m->cx); i--;) {
    PtyWrite(pty, buf, p - buf);
  }
}

static char16_t VidyaServiceXlatTeletype(uint8_t c) {
  switch (c) {
    case '\a':
    case '\b':
    case '\r':
    case '\n':
    case 0177:
      return c;
    default:
      return GetVidyaByte(c);
  }
}

static void OnVidyaServiceTeletypeOutput(void) {
  int n;
  char buf[12];
  n = FormatCga(m->bx[0], buf);
  n += tpencode(buf + n, 6, VidyaServiceXlatTeletype(m->ax[0]), false);
  PtyWrite(pty, buf, n);
}

static void OnVidyaService(void) {
  switch (m->ax[1]) {
    case 0x00:
      OnVidyaServiceSetMode();
      break;
    case 0x02:
      OnVidyaServiceSetCursorPosition();
      break;
    case 0x03:
      OnVidyaServiceGetCursorPosition();
      break;
    case 0x09:
      OnVidyaServiceWriteCharacter();
      break;
    case 0x0E:
      OnVidyaServiceTeletypeOutput();
      break;
    case 0x0F:
      OnVidyaServiceGetMode();
      break;
    default:
      break;
  }
}

static void OnKeyboardServiceReadKeyPress(void) {
  uint8_t b;
  ssize_t rc;
  pty->conf |= kPtyBlinkcursor;
  if (tuimode) DisableMouseTracking();
  for (;;) {
    ReactiveDraw();
    if ((rc = read(0, &b, 1)) != -1) break;
    CHECK_EQ(EINTR, errno);
    HandleAppReadInterrupt();
  }
  if (tuimode) EnableMouseTracking();
  pty->conf &= ~kPtyBlinkcursor;
  ReactiveDraw();
  if (b == 0x7F) b = '\b';
  m->ax[0] = b;
  m->ax[1] = 0;
}

static void OnKeyboardService(void) {
  switch (m->ax[1]) {
    case 0x00:
      OnKeyboardServiceReadKeyPress();
      break;
    default:
      break;
  }
}

static void OnApmService(void) {
  if (Read16(m->ax) == 0x5300 && Read16(m->bx) == 0x0000) {
    Write16(m->bx, 'P' << 8 | 'M');
    SetCarry(false);
  } else if (Read16(m->ax) == 0x5301 && Read16(m->bx) == 0x0000) {
    SetCarry(false);
  } else if (Read16(m->ax) == 0x5307 && m->bx[0] == 1 && m->cx[0] == 3) {
    LOGF("APM SHUTDOWN");
    exit(0);
  } else {
    SetCarry(true);
  }
}

static void OnE820(void) {
  uint8_t p[20];
  uint64_t addr;
  addr = Read64(m->es) + Read16(m->di);
  if (Read32(m->dx) == 0x534D4150 && Read32(m->cx) == 24 &&
      addr + sizeof(p) <= m->real.n) {
    if (!Read32(m->bx)) {
      Write64(p + 000, 0);
      Write64(p + 010, m->real.n);
      Write32(p + 014, 1);
      memcpy(m->real.p + addr, p, sizeof(p));
      SetWriteAddr(m, addr, sizeof(p));
      Write32(m->cx, sizeof(p));
      Write32(m->bx, 1);
    } else {
      Write32(m->bx, 0);
      Write32(m->cx, 0);
    }
    Write32(m->ax, 0x534D4150);
    SetCarry(false);
  } else {
    SetCarry(true);
  }
}

static void OnInt15h(void) {
  if (Read32(m->ax) == 0xE820) {
    OnE820();
  } else if (m->ax[1] == 0x53) {
    OnApmService();
  } else {
    SetCarry(true);
  }
}

static bool OnHalt(int interrupt) {
  ReactiveDraw();
  switch (interrupt) {
    case 1:
    case 3:
      OnDebug();
      return false;
    case 0x13:
      OnDiskService();
      return true;
    case 0x10:
      OnVidyaService();
      return true;
    case 0x15:
      OnInt15h();
      return true;
    case 0x16:
      OnKeyboardService();
      return true;
    case kMachineSegmentationFault:
      OnSegmentationFault();
      return false;
    case kMachineProtectionFault:
      OnProtectionFault();
      return false;
    case kMachineSimdException:
      OnSimdException();
      return false;
    case kMachineUndefinedInstruction:
      OnUndefinedInstruction();
      return false;
    case kMachineDecodeError:
      OnDecodeError();
      return false;
    case kMachineDivideError:
      OnDivideError();
      return false;
    case kMachineFpuException:
      OnFpuException();
      return false;
    case kMachineExit:
    case kMachineHalt:
    default:
      OnExit(interrupt);
      return false;
  }
}

static void OnBinbase(struct Machine *m) {
  unsigned i;
  int64_t skew;
  skew = m->xedd->op.disp * 512;
  LOGF("skew binbase %,ld @ %p", skew, GetIp());
  for (i = 0; i < dis->syms.i; ++i) {
    dis->syms.p[i].addr += skew;
  }
  for (i = 0; i < dis->loads.i; ++i) {
    dis->loads.p[i].addr += skew;
  }
  Disassemble();
}

static void OnLongBranch(struct Machine *m) {
  if (tuimode) {
    Disassemble();
  }
}

static void OnPageUp(void) {
  opstart -= tyn / 2;
}

static void OnPageDown(void) {
  opstart += tyn / 2;
}

static void SetStatus(const char *fmt, ...) {
  char *s;
  va_list va;
  int y, x, n;
  va_start(va, fmt);
  s = xvasprintf(fmt, va);
  va_end(va);
  free(statusmessage);
  statusmessage = s;
  statusexpires = nowl() + 1;
  setitimer(ITIMER_REAL, &((struct itimerval){{0, 0}, {1, 0}}), NULL);
}

static int ClampSpeed(int s) {
  return MAX(-0x1000, MIN(0x40000000, s));
}

static void OnTurbo(void) {
  if (!speed || speed == -1) {
    speed = 1;
  } else if (speed > 0) {
    speed = ClampSpeed(speed << 1);
  } else {
    speed = ClampSpeed(speed >> 1);
  }
  SetStatus("speed %,d", speed);
}

static void OnSlowmo(void) {
  if (!speed || speed == 1) {
    speed = -1;
  } else if (speed > 0) {
    speed = ClampSpeed(speed >> 1);
  } else {
    speed = ClampSpeed(speed << 1);
  }
  SetStatus("speed %,d", speed);
}

static void OnUpArrow(void) {
  --opstart;
}

static void OnDownArrow(void) {
  ++opstart;
}

static void OnHome(void) {
  opstart = 0;
}

static void OnEnd(void) {
  opstart = dis->ops.i - (pan.disassembly.bottom - pan.disassembly.top);
}

static void OnEnter(void) {
  action &= ~FAILURE;
}

static void OnTab(void) {
  focus = (focus + 1) % ARRAYLEN(pan.p);
}

static void OnUp(void) {
}

static void OnDown(void) {
}

static void OnStep(void) {
  if (action & FAILURE) return;
  action |= STEP;
  action &= ~NEXT;
  action &= ~CONTINUE;
}

static void OnNext(void) {
  if (action & FAILURE) return;
  action ^= NEXT;
  action &= ~STEP;
  action &= ~FINISH;
  action &= ~CONTINUE;
}

static void OnFinish(void) {
  if (action & FAILURE) return;
  action ^= FINISH;
  action &= ~NEXT;
  action &= ~FAILURE;
  action &= ~CONTINUE;
}

static void OnContinue(void) {
  action ^= CONTINUE;
  action &= ~STEP;
  action &= ~NEXT;
  action &= ~FINISH;
  action &= ~FAILURE;
}

static void OnQuit(void) {
  action |= QUIT;
}

static void OnRestart(void) {
  action |= RESTART;
}

static void OnXmmType(void) {
  uint8_t t;
  unsigned i;
  t = CycleXmmType(xmmtype.type[0]);
  for (i = 0; i < 16; ++i) {
    xmmtype.type[i] = t;
  }
}

static void SetXmmSize(int bytes) {
  unsigned i;
  for (i = 0; i < 16; ++i) {
    xmmtype.size[i] = bytes;
  }
}

static void SetXmmDisp(int disp) {
  xmmdisp = disp;
}

static void OnXmmSize(void) {
  SetXmmSize(CycleXmmSize(xmmtype.size[0]));
}

static void OnXmmDisp(void) {
  SetXmmDisp(CycleXmmDisp(xmmdisp));
}

static bool HasPendingKeyboard(void) {
  return HasPendingInput(ttyin);
}

static void Sleep(int ms) {
  poll((struct pollfd[]){{ttyin, POLLIN}}, 1, ms);
}

static void OnMouseWheelUp(struct Panel *p, int y, int x) {
  if (p == &pan.disassembly) {
    opstart -= WHEELDELTA;
  } else if (p == &pan.code) {
    codeview.start -= WHEELDELTA;
  } else if (p == &pan.readdata) {
    readview.start -= WHEELDELTA;
  } else if (p == &pan.writedata) {
    writeview.start -= WHEELDELTA;
  } else if (p == &pan.stack) {
    stackview.start -= WHEELDELTA;
  } else if (p == &pan.maps) {
    mapsstart = MAX(0, mapsstart - 1);
  } else if (p == &pan.frames) {
    framesstart = MAX(0, framesstart - 1);
  } else if (p == &pan.breakpoints) {
    breakpointsstart = MAX(0, breakpointsstart - 1);
  }
}

static void OnMouseWheelDown(struct Panel *p, int y, int x) {
  if (p == &pan.disassembly) {
    opstart += WHEELDELTA;
  } else if (p == &pan.code) {
    codeview.start += WHEELDELTA;
  } else if (p == &pan.readdata) {
    readview.start += WHEELDELTA;
  } else if (p == &pan.writedata) {
    writeview.start += WHEELDELTA;
  } else if (p == &pan.stack) {
    stackview.start += WHEELDELTA;
  } else if (p == &pan.maps) {
    mapsstart += 1;
  } else if (p == &pan.frames) {
    framesstart += 1;
  } else if (p == &pan.breakpoints) {
    breakpointsstart += 1;
  }
}

static void OnMouseCtrlWheelUp(struct Panel *p, int y, int x) {
  ZoomMemoryViews(p, y, x, -1);
}

static void OnMouseCtrlWheelDown(struct Panel *p, int y, int x) {
  ZoomMemoryViews(p, y, x, +1);
}

static struct Panel *LocatePanel(int y, int x) {
  int i;
  for (i = 0; i < ARRAYLEN(pan.p); ++i) {
    if ((pan.p[i].left <= x && x < pan.p[i].right) &&
        (pan.p[i].top <= y && y < pan.p[i].bottom)) {
      return &pan.p[i];
    }
  }
  return NULL;
}

static void OnMouse(char *p) {
  int e, x, y;
  struct Panel *ep;
  e = strtol(p, &p, 10);
  if (*p == ';') ++p;
  x = min(txn, max(1, strtol(p, &p, 10))) - 1;
  if (*p == ';') ++p;
  y = min(tyn, max(1, strtol(p, &p, 10))) - 1;
  e |= (*p == 'm') << 2;
  if ((ep = LocatePanel(y, x))) {
    y -= ep->top;
    x -= ep->left;
    switch (e) {
      case kMouseWheelUp:
        OnMouseWheelUp(ep, y, x);
        break;
      case kMouseWheelDown:
        OnMouseWheelDown(ep, y, x);
        break;
      case kMouseCtrlWheelUp:
        OnMouseCtrlWheelUp(ep, y, x);
        break;
      case kMouseCtrlWheelDown:
        OnMouseCtrlWheelDown(ep, y, x);
        break;
      default:
        break;
    }
  }
}

static void ReadKeyboard(void) {
  char buf[64], *p = buf;
  memset(buf, 0, sizeof(buf));
  if (readansi(ttyin, buf, sizeof(buf)) == -1) {
    if (errno == EINTR) {
      LOGF("readkeyboard interrupted");
      return;
    }
    FATALF("readkeyboard failed: %s", strerror(errno));
  }
  switch (*p++) {
    CASE('q', OnQ());
    CASE('v', OnV());
    CASE('s', OnStep());
    CASE('n', OnNext());
    CASE('f', OnFinish());
    CASE('c', OnContinue());
    CASE('R', OnRestart());
    CASE('x', OnXmmDisp());
    CASE('t', OnXmmType());
    CASE('T', OnXmmSize());
    CASE('u', OnUp());
    CASE('d', OnDown());
    CASE('B', PopBreakpoint(&breakpoints));
    CASE('M', ToggleMouseTracking());
    CASE('\t', OnTab());
    CASE('\r', OnEnter());
    CASE('\n', OnEnter());
    CASE(CTRL('C'), OnSigInt());
    CASE(CTRL('D'), OnSigInt());
    CASE(CTRL('\\'), OnQuit());
    CASE(CTRL('Z'), raise(SIGSTOP));
    CASE(CTRL('L'), OnFeed());
    CASE(CTRL('P'), OnUpArrow());
    CASE(CTRL('N'), OnDownArrow());
    CASE(CTRL('V'), OnPageDown());
    CASE(CTRL('T'), OnTurbo());
    case '\e':
      switch (*p++) {
        CASE('v', OnPageUp());
        CASE('t', OnSlowmo());
        case '[':
          switch (*p++) {
            CASE('<', OnMouse(p));
            CASE('A', OnUpArrow());
            CASE('B', OnDownArrow());
            CASE('F', OnEnd());
            CASE('H', OnHome());
            case '1':
              switch (*p++) {
                CASE('~', OnHome());
                default:
                  break;
              }
              break;
            case '4':
              switch (*p++) {
                CASE('~', OnEnd());
                default:
                  break;
              }
              break;
            case '5':
              switch (*p++) {
                CASE('~', OnPageUp());
                default:
                  break;
              }
              break;
            case '6':
              switch (*p++) {
                CASE('~', OnPageDown());
                default:
                  break;
              }
              break;
            case '7':
              switch (*p++) {
                CASE('~', OnHome());
                default:
                  break;
              }
              break;
            case '8':
              switch (*p++) {
                CASE('~', OnEnd());
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

static int64_t ParseHexValue(const char *s) {
  char *ep;
  int64_t x;
  x = strtoll(s, &ep, 16);
  if (*ep) {
    fputs("ERROR: bad hexadecimal: ", stderr);
    fputs(s, stderr);
    fputc('\n', stderr);
    exit(EXIT_FAILURE);
  }
  return x;
}

static void HandleBreakpointFlag(const char *s) {
  struct Breakpoint b;
  memset(&b, 0, sizeof(b));
  if (isdigit(*s)) {
    b.addr = ParseHexValue(s);
  } else {
    b.symbol = optarg;
  }
  PushBreakpoint(&breakpoints, &b);
}

static wontreturn void PrintUsage(int rc, FILE *f) {
  fprintf(f, "SYNOPSIS\n\n  %s%s", program_invocation_name, USAGE);
  exit(rc);
}

static void Exec(void) {
  long op;
  ssize_t bp;
  int interrupt;
  ExecSetup();
  if (!(interrupt = setjmp(m->onhalt))) {
    if (!(action & CONTINUE) &&
        (bp = IsAtBreakpoint(&breakpoints, GetIp())) != -1) {
      LOGF("BREAK1 %p", breakpoints.p[bp].addr);
      tuimode = true;
      LoadInstruction(m);
      ExecuteInstruction(m);
      ++opcount;
      CheckFramePointer();
    } else {
      action &= ~CONTINUE;
      for (;;) {
        LoadInstruction(m);
        if ((bp = IsAtBreakpoint(&breakpoints, GetIp())) != -1) {
          LOGF("BREAK2 %p", breakpoints.p[bp].addr);
          action &= ~(FINISH | NEXT | CONTINUE);
          tuimode = true;
          break;
        }
        ExecuteInstruction(m);
        ++opcount;
      KeepGoing:
        CheckFramePointer();
        if (action & ALARM) {
          /* TODO(jart): Fix me */
          /* DrawDisplayOnly(&pan.display); */
          action &= ~ALARM;
        }
        if (action & EXIT) {
          LOGF("EXEC EXIT");
          break;
        }
        if (action & INT) {
          LOGF("EXEC INT");
          if (react) {
            LOGF("REACT");
            action &= ~(INT | STEP | FINISH | NEXT);
            tuimode = true;
          }
          break;
        }
      }
    }
  } else {
    if (OnHalt(interrupt)) {
      goto KeepGoing;
    }
  }
}

static void Tui(void) {
  long op;
  ssize_t bp;
  int interrupt;
  bool interactive;
  LOGF("TUI");
  TuiSetup();
  SetupDraw();
  ScrollOp(&pan.disassembly, GetDisIndex());
  if (!(interrupt = setjmp(m->onhalt))) {
    for (;;) {
      if (!(action & FAILURE)) {
        LoadInstruction(m);
        if ((action & (FINISH | NEXT | CONTINUE)) &&
            (bp = IsAtBreakpoint(&breakpoints, GetIp())) != -1) {
          action &= ~(FINISH | NEXT | CONTINUE);
          LOGF("BREAK %p", breakpoints.p[bp].addr);
          break;
        }
      } else {
        m->xedd = (struct XedDecodedInst *)m->icache[0];
        m->xedd->length = 1;
        m->xedd->bytes[0] = 0xCC;
        m->xedd->op.opcode = 0xCC;
      }
      if (action & WINCHED) {
        GetTtySize(ttyout);
        action &= ~WINCHED;
      }
      interactive = ++tick > speed;
      if (interactive && speed < 0) {
        Sleep(-speed);
      }
      if (action & ALARM) {
        HandleAlarm();
      }
      if (action & FAILURE) {
        ScrollMemoryViews();
      }
      if (!(action & CONTINUE) || interactive) {
        tick = 0;
        Redraw();
      }
      if (action & FAILURE) {
        LOGF("TUI FAILURE");
        PrintMessageBox(ttyout, systemfailure, tyn, txn);
        ReadKeyboard();
        if (action & INT) {
          LOGF("TUI INT");
          LeaveScreen();
          exit(1);
        }
      } else if (!IsExecuting() || (!(action & CONTINUE) && !(action & INT) &&
                                    HasPendingKeyboard())) {
        ReadKeyboard();
      }
      if (action & INT) {
        LOGF("TUI INT");
        action &= ~INT;
        if (action & (CONTINUE | NEXT | FINISH)) {
          action &= ~(CONTINUE | NEXT | FINISH);
        } else {
          tuimode = false;
          action |= CONTINUE;
          break;
        }
      }
      if (action & EXIT) {
        LeaveScreen();
        LOGF("TUI EXIT");
        break;
      }
      if (action & QUIT) {
        LOGF("TUI QUIT");
        action &= ~QUIT;
        raise(SIGQUIT);
        continue;
      }
      if (action & RESTART) {
        LOGF("TUI RESTART");
        break;
      }
      if (IsExecuting()) {
        if (!(action & CONTINUE)) {
          action &= ~STEP;
          if (action & NEXT) {
            action &= ~NEXT;
            if (IsCall()) {
              BreakAtNextInstruction();
              break;
            }
          }
          if (action & FINISH) {
            if (IsCall()) {
              BreakAtNextInstruction();
              break;
            } else if (IsRet()) {
              action &= ~FINISH;
            }
          }
        }
        if (!IsDebugBreak()) {
          UpdateXmmType(m, &xmmtype);
          ExecuteInstruction(m);
          ++opcount;
          if (!(action & CONTINUE) || interactive) {
            ScrollMemoryViews();
          }
        } else {
          m->ip += m->xedd->length;
          action &= ~NEXT;
          action &= ~FINISH;
          action &= ~CONTINUE;
        }
      KeepGoing:
        CheckFramePointer();
        if (!(action & CONTINUE)) {
          ScrollOp(&pan.disassembly, GetDisIndex());
          if ((action & FINISH) && IsRet()) action &= ~FINISH;
          if (((action & NEXT) && IsRet()) || (action & FINISH)) {
            action &= ~NEXT;
          }
        }
      }
    }
  } else {
    if (OnHalt(interrupt)) {
      goto KeepGoing;
    }
    ScrollOp(&pan.disassembly, GetDisIndex());
  }
  TuiCleanup();
}

static void GetOpts(int argc, char *argv[]) {
  int opt;
  stpcpy(stpcpy(stpcpy(logpath, kTmpPath), basename(argv[0])), ".log");
  while ((opt = getopt(argc, argv, "hvtrzRsb:HL:")) != -1) {
    switch (opt) {
      case 't':
        tuimode = true;
        break;
      case 'R':
        react = true;
        break;
      case 'r':
        m->mode = XED_MACHINE_MODE_REAL;
        g_disisprog_disable = true;
        break;
      case 's':
        printstats = true;
        break;
      case 'b':
        HandleBreakpointFlag(optarg);
        break;
      case 'H':
        memset(&g_high, 0, sizeof(g_high));
        break;
      case 'v':
        ++g_loglevel;
        break;
      case 'L':
        strcpy(logpath, optarg);
        break;
      case 'z':
        ++codeview.zoom;
        ++readview.zoom;
        ++writeview.zoom;
        ++stackview.zoom;
        break;
      case 'h':
        PrintUsage(EXIT_SUCCESS, stdout);
      default:
        PrintUsage(EX_USAGE, stderr);
    }
  }
  g_logfile = fopen(logpath, "a");
  setvbuf(g_logfile, xmalloc(PAGESIZE), _IOLBF, PAGESIZE);
}

static int OpenDevTty(void) {
  return open("/dev/tty", O_RDWR | O_NOCTTY);
}

static void AddHostFd(int fd) {
  int i = m->fds.i++;
  CHECK_NE(-1, (m->fds.p[i].fd = fd));
  m->fds.p[i].cb = &kMachineFdCbHost;
}

int Emulator(int argc, char *argv[]) {
  void *code;
  int rc, fd;
  codepath = argv[optind++];
  m->fds.p = xcalloc((m->fds.n = 8), sizeof(struct MachineFd));
Restart:
  action = 0;
  LoadProgram(m, codepath, argv + optind, environ, elf);
  AddHostFd(STDIN_FILENO);
  AddHostFd(STDOUT_FILENO);
  AddHostFd(STDERR_FILENO);
  if (tuimode) {
    ttyin = isatty(STDIN_FILENO) ? STDIN_FILENO : OpenDevTty();
    ttyout = isatty(STDOUT_FILENO) ? STDOUT_FILENO : OpenDevTty();
  } else {
    ttyin = -1;
    ttyout = -1;
  }
  if (ttyout != -1) {
    atexit(TtyRestore1);
    xsigaction(SIGWINCH, OnSigWinch, 0, 0, 0);
    tyn = 24;
    txn = 80;
    GetTtySize(ttyout);
    if (isatty(STDIN_FILENO)) m->fds.p[STDIN_FILENO].cb = &kMachineFdCbPty;
    if (isatty(STDOUT_FILENO)) m->fds.p[STDOUT_FILENO].cb = &kMachineFdCbPty;
    if (isatty(STDERR_FILENO)) m->fds.p[STDERR_FILENO].cb = &kMachineFdCbPty;
  }
  while (!(action & EXIT)) {
    if (!tuimode) {
      Exec();
    } else {
      Tui();
    }
    if (action & RESTART) {
      goto Restart;
    }
  }
  if (tuimode) {
    LeaveScreen();
  }
  if (printstats) {
    fprintf(stderr, "taken:  %,ld\n", taken);
    fprintf(stderr, "ntaken: %,ld\n", ntaken);
    fprintf(stderr, "ops:    %,ld\n", opcount);
  }
  munmap(elf->ehdr, elf->size);
  DisFree(dis);
  return exitcode;
}

static void OnlyRunOnFirstCpu(void) {
  uint64_t bs = 1;
  sched_setaffinity(0, sizeof(bs), &bs);
}

int main(int argc, char *argv[]) {
  if (!NoDebug()) showcrashreports();
  pty = NewPty();
  m = NewMachine();
  m->mode = XED_MACHINE_MODE_LONG_64;
  m->onbinbase = OnBinbase;
  m->onlongbranch = OnLongBranch;
  speed = 16;
  SetXmmSize(2);
  SetXmmDisp(kXmmHex);
  if ((colorize = cancolor())) {
    g_high.keyword = 155;
    g_high.reg = 215;
    g_high.literal = 182;
    g_high.label = 221;
    g_high.comment = 112;
    g_high.quote = 215;
  }
  GetOpts(argc, argv);
  xsigaction(SIGALRM, OnSigAlarm, 0, 0, 0);
  if (optind == argc) PrintUsage(EX_USAGE, stderr);
  return Emulator(argc, argv);
}
