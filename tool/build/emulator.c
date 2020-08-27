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
#include "dsp/tty/tty.h"
#include "libc/alg/arraylist2.h"
#include "libc/bits/safemacros.h"
#include "libc/calls/calls.h"
#include "libc/calls/ioctl.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/struct/termios.h"
#include "libc/calls/struct/winsize.h"
#include "libc/calls/termios.h"
#include "libc/conv/conv.h"
#include "libc/conv/itoa.h"
#include "libc/errno.h"
#include "libc/fmt/bing.h"
#include "libc/fmt/fmt.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/macros.h"
#include "libc/math.h"
#include "libc/mem/mem.h"
#include "libc/runtime/gc.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/sock.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/str/tpdecode.h"
#include "libc/str/tpencode.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/sysv/consts/ex.h"
#include "libc/sysv/consts/exit.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/poll.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/termios.h"
#include "libc/unicode/unicode.h"
#include "libc/x/x.h"
#include "third_party/dtoa/dtoa.h"
#include "third_party/getopt/getopt.h"
#include "tool/build/lib/breakpoint.h"
#include "tool/build/lib/case.h"
#include "tool/build/lib/dis.h"
#include "tool/build/lib/endian.h"
#include "tool/build/lib/fds.h"
#include "tool/build/lib/flags.h"
#include "tool/build/lib/fpu.h"
#include "tool/build/lib/loader.h"
#include "tool/build/lib/machine.h"
#include "tool/build/lib/memory.h"
#include "tool/build/lib/modrm.h"
#include "tool/build/lib/panel.h"
#include "tool/build/lib/pml4t.h"
#include "tool/build/lib/pty.h"
#include "tool/build/lib/stats.h"
#include "tool/build/lib/throw.h"

STATIC_YOINK("die");

#define USAGE \
  " [-?Hhrstv] [ROM] [ARGS...]\n\
\n\
DESCRIPTION\n\
\n\
  NexGen32e Userspace Emulator w/ Debugger\n\
\n\
FLAGS\n\
\n\
  -h\n\
  -?        help\n\
  -v        verbosity\n\
  -s        statistics\n\
  -H        disable highlight\n\
  -t        tui debugger mode\n\
  -r        reactive tui mode\n\
  -b ADDR   push a breakpoint\n\
  -L PATH   log file location\n\
\n\
ARGUMENTS\n\
\n\
  ROM files can be ELF or a flat αcτµαlly pδrταblε εxεcµταblε.\n\
  It should use x86_64 in accordance with the System Five ABI.\n\
  The SYSCALL ABI is defined as it is written in Linux Kernel.\n\
\n\
PERFORMANCE\n\
\n\
       1500 MIPS w/ NOP loop\n\
  Over 9000 MIPS w/ SIMD & Algorithms\n\
\n\
PROTIP\n\
\n\
  Fix SSH keyboard latency for debugger TUI in CONTINUE mode:\n\
\n\
    sudo sh -c 'echo -n 8192 >/proc/sys/net/core/rmem_default'\n\
    sudo sh -c 'echo -n 8192 >/proc/sys/net/core/wmem_default'\n\
\n\
\n\
\n"

#define DUMPWIDTH 64

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

#define CTRL(C) ((C) ^ 0100)
#define ALT(C)  (('\e' << 010) | (C))

struct Panels {
  union {
    struct {
      struct Panel disassembly;
      struct Panel breakpointshr;
      struct Panel breakpoints;
      struct Panel mapshr;
      struct Panel maps;
      struct Panel tracehr;
      struct Panel trace;
      struct Panel terminalhr;
      struct Panel terminal;
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
    };
    struct Panel p[20];
  };
};

static const char kRegisterNames[16][4] = {
    "RAX", "RCX", "RDX", "RBX", "RSP", "RBP", "RSI", "RDI",
    "R8",  "R9",  "R10", "R11", "R12", "R13", "R14", "R15",
};

static int tyn;
static int txn;
static int ttyfd;
static bool react;
static bool ssehex;
static int exitcode;
static bool tuimode;
static long rombase;
static bool colorize;
static long codesize;
static long ssewidth;
static char *codepath;
static void *onbusted;
static unsigned focus;
static unsigned opline;
static bool printstats;
static unsigned action;
static int64_t opstart;
static int64_t memstart;
static struct Elf elf[1];
static struct Dis dis[1];
static struct Panels pan;
static int64_t readstart;
static int64_t writestart;
static int64_t stackstart;
static struct DisHigh high;
static struct Machine m[2];
static struct MachinePty *pty;
static char logpath[PATH_MAX];
static char systemfailure[128];
static struct sigaction oldsig[4];
static struct Breakpoints breakpoints;

static void SetupDraw(void);
static void Redraw(void);

static uint64_t SignExtend(uint64_t x, uint8_t b) {
  uint64_t s;
  s = 1;
  b -= 1;
  b &= 63;
  s <<= b;
  if (x & s) x |= ~(s - 1);
  return x;
}

static char *FormatDouble(char *b, double x) {
  return g_fmt(b, x);
}

static bool IsCall(void) {
  return m->xedd->op.map == XED_ILD_MAP0 &&
         (m->xedd->op.opcode == 0xE8 ||
          (m->xedd->op.opcode == 0xFF && m->xedd->op.reg == 2));
}

static bool IsDebugBreak(void) {
  return m->xedd->op.map == XED_ILD_MAP0 && m->xedd->op.opcode == 0xCC;
}

static bool IsRet(void) {
  if (m->xedd->op.map == XED_ILD_MAP0) {
    switch (m->xedd->op.opcode) {
      case 0xC2:
      case 0xC3:
      case 0xCA:
      case 0xCB:
      case 0xCF:
        return true;
      default:
        return false;
    }
  } else {
    return false;
  }
}

static uint8_t CycleSseType(uint8_t t) {
  switch (t) {
    case kXmmIntegral:
      return kXmmFloat;
    case kXmmFloat:
      return kXmmDouble;
    case kXmmDouble:
      return kXmmIntegral;
    default:
      unreachable;
  }
}

static uint8_t CycleSseWidth(uint8_t w) {
  switch (w) {
    case 1:
      return 2;
    case 2:
      return 4;
    case 4:
      return 8;
    case 8:
      return 1;
    default:
      unreachable;
  }
}

static void OnBusted(void) {
  LOGF("OnBusted");
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

static void GetTtySize(void) {
  struct winsize wsize;
  wsize.ws_row = 25;
  wsize.ws_col = 80;
  getttysize(ttyfd, &wsize);
  tyn = wsize.ws_row;
  txn = wsize.ws_col;
}

static void TuiRejuvinate(void) {
  GetTtySize();
  ttyhidecursor(STDOUT_FILENO);
  ttyraw(0);
  xsigaction(SIGBUS, OnBusted, SA_NODEFER, 0, NULL);
}

static void OnCtrlC(void) {
  LOGF("OnCtrlC");
  action |= INT;
}

static void OnQ(void) {
  LOGF("OnQ");
  action |= INT;
  breakpoints.i = 0;
}

static void OnWinch(void) {
  LOGF("OnWinch");
  action |= WINCHED;
}

static void OnCont(void) {
  LOGF("OnCont");
  TuiRejuvinate();
  SetupDraw();
  Redraw();
}

static void TuiCleanup(void) {
  sigaction(SIGWINCH, oldsig + 0, NULL);
  sigaction(SIGCONT, oldsig + 2, NULL);
  ttyraw((enum TtyRawFlags)(-1u));
  ttyshowcursor(STDOUT_FILENO);
  CHECK_NE(-1, close(ttyfd));
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
  struct Breakpoint b = {.addr = m->ip + m->xedd->length, .oneshot = true};
  PushBreakpoint(&breakpoints, &b);
}

static void LoadSyms(void) {
  LoadDebugSymbols(elf);
  DisLoadElf(dis, elf);
}

static void TuiSetup(void) {
  static bool once;
  if (!once) {
    LOGF("loaded program %s\n%s", codepath, gc(FormatPml4t(m->cr3)));
    LoadSyms();
    ResolveBreakpoints();
    Dis(dis, m, elf->base);
    once = true;
  }
  CHECK_NE(-1, (ttyfd = open("/dev/tty", O_RDWR)));
  xsigaction(SIGWINCH, OnWinch, 0, 0, oldsig + 0);
  xsigaction(SIGCONT, OnCont, SA_RESTART, 0, oldsig + 2);
  memcpy(&m[1], &m[0], sizeof(m[0]));
  TuiRejuvinate();
}

static void ExecSetup(void) {
  static bool once;
  if (!once) {
    if (breakpoints.i) {
      LoadSyms();
      ResolveBreakpoints();
    }
    once = true;
  }
}

static void AppendPanel(struct Panel *p, long line, const char *s) {
  if (0 <= line && line < p->bottom - p->top) {
    AppendStr(&p->lines[line], s);
  }
}

static bool IsXmmNonZero(long start, long end) {
  long i, j;
  for (i = start; i < end; ++i) {
    for (j = 0; j < 16; ++j) {
      if (m->xmm[i / 8][i % 8][j]) {
        return true;
      }
    }
  }
  return false;
}

static int PickNumberOfXmmRegistersToShow(void) {
  if (IsXmmNonZero(0, 8) || IsXmmNonZero(8, 16)) {
    if (IsXmmNonZero(8, 16)) {
      return 16;
    } else {
      return 8;
    }
  } else {
    return 0;
  }
}

static void SetupDraw(void) {
  int i, j, n, a, b, cpuy, ssey, dx[2], c2y[3], c3y[5];

  for (i = 0; i < ARRAYLEN(pan.p); ++i) {
    n = pan.p[i].bottom - pan.p[i].top;
    for (j = 0; j < n; ++j) {
      free(pan.p[i].lines[j].p);
    }
    free(pan.p[i].lines);
  }

  cpuy = 9;
  ssey = PickNumberOfXmmRegistersToShow();
  if (ssey) ++ssey;

  a = 12 + 1 + DUMPWIDTH;
  b = DUMPWIDTH;
  dx[1] = txn >= a + b ? txn - a : txn;
  dx[0] = txn >= a + b + b ? txn - a - b : dx[1];

  a = 1 / 8. * tyn;
  b = 3 / 8. * tyn;
  c2y[0] = a;
  c2y[1] = a * 2;
  c2y[2] = a * 2 + b;

  a = (tyn - (cpuy + ssey) - 3) / 4;
  c3y[0] = cpuy;
  c3y[1] = cpuy + ssey;
  c3y[2] = cpuy + ssey + 1 + 1 + a * 1;
  c3y[3] = cpuy + ssey + 1 + 1 + a * 2 + 1;
  c3y[4] = cpuy + ssey + 1 + 1 + 1 + a * 3 + 1;

  /* COLUMN #1: DISASSEMBLY */

  pan.disassembly.top = 0;
  pan.disassembly.left = 0;
  pan.disassembly.bottom = tyn;
  pan.disassembly.right = dx[0];

  /* COLUMN #2: BREAKPOINTS, MEMORY MAPS, BACKTRACE, TERMINAL */

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

  pan.tracehr.top = c2y[1];
  pan.tracehr.left = dx[0];
  pan.tracehr.bottom = c2y[1] + 1;
  pan.tracehr.right = dx[1] - 1;

  pan.trace.top = c2y[1] + 1;
  pan.trace.left = dx[0];
  pan.trace.bottom = c2y[2];
  pan.trace.right = dx[1] - 1;

  pan.terminalhr.top = c2y[2];
  pan.terminalhr.left = dx[0];
  pan.terminalhr.bottom = c2y[2] + 1;
  pan.terminalhr.right = dx[1] - 1;

  pan.terminal.top = c2y[2] + 1;
  pan.terminal.left = dx[0];
  pan.terminal.bottom = tyn;
  pan.terminal.right = dx[1] - 1;

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
  pan.stack.bottom = tyn;
  pan.stack.right = txn;

  for (i = 0; i < ARRAYLEN(pan.p); ++i) {
    if (pan.p[i].top > pan.p[i].bottom) pan.p[i].top = pan.p[i].bottom = 0;
    if (pan.p[i].left > pan.p[i].right) pan.p[i].left = pan.p[i].right = 0;
    pan.p[i].lines =
        xcalloc(pan.p[i].bottom - pan.p[i].top, sizeof(struct Buffer));
  }

  if (pty->yn != pan.terminal.bottom - pan.terminal.top ||
      pty->xn != pan.terminal.right - pan.terminal.left) {
    LOGF("MachinePtyNew");
    MachinePtyFree(pty);
    pty = MachinePtyNew(pan.terminal.bottom - pan.terminal.top,
                        pan.terminal.right - pan.terminal.left);
  }
}

static long GetDisIndex(int64_t addr) {
  long i;
  if ((i = DisFind(dis, m->ip)) == -1) {
    Dis(dis, m, m->ip);
    CHECK_NE(-1, (i = DisFind(dis, m->ip)));
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
      AppendPanel(p, i, dis->ops.p[j].s);
      if (j == opline) AppendPanel(p, i, "\e[27m");
    }
  }
}

static void DrawTerminal(struct Panel *p) {
  long y, yn;
  for (yn = MIN(pty->yn, p->bottom - p->top), y = 0; y < yn; ++y) {
    MachinePtyAppendLine(pty, p->lines + y, y);
  }
}

static void DrawFlag(struct Panel *p, long i, char *name, bool value,
                     bool previous) {
  AppendPanel(p, i, " ");
  if (value) {
    AppendPanel(p, i, name);
  } else {
    AppendPanel(p, i, " ");
  }
  AppendPanel(p, i, " ");
}

static void DrawRegister(struct Panel *p, long i, long r) {
  char buf[32];
  uint64_t value, previous;
  value = Read64(m[0].reg[r]);
  previous = Read64(m[1].reg[r]);
  if (value != previous) AppendPanel(p, i, "\e[7m");
  snprintf(buf, sizeof(buf), "%-3s", kRegisterNames[r]);
  AppendPanel(p, i, buf);
  AppendPanel(p, i, " ");
  snprintf(buf, sizeof(buf), "0x%016lx", value);
  AppendPanel(p, i, buf);
  if (value != previous) AppendPanel(p, i, "\e[27m");
  AppendPanel(p, i, "  ");
}

static void DrawSt(struct Panel *p, long i, long r) {
  char buf[32];
  bool isempty, changed;
  long double value, previous;
  isempty = FpuGetTag(m, r) == kFpuTagEmpty;
  if (isempty) AppendPanel(p, i, "\e[2m");
  value = *FpuSt(&m[0], r);
  previous = *FpuSt(&m[1], r);
  changed = value != previous;
  if (!isempty && changed) AppendPanel(p, i, "\e[7m");
  snprintf(buf, sizeof(buf), "ST%d ", r);
  AppendPanel(p, i, buf);
  AppendPanel(p, i, FormatDouble(buf, value));
  if (changed) AppendPanel(p, i, "\e[27m");
  AppendPanel(p, i, "  ");
  if (isempty) AppendPanel(p, i, "\e[22m");
}

static void DrawHr(struct Panel *p, const char *s) {
  long i, wp, ws, wl, wr;
  if (p->bottom - p->top < 1) return;
  wp = p->right - p->left;
  ws = strlen(s);
  wl = wp / 4 - ws / 2;
  wr = wp - (wl + ws);
  for (i = 0; i < wl; ++i) AppendWide(&p->lines[0], u'─');
  AppendStr(&p->lines[0], s);
  for (i = 0; i < wr; ++i) AppendWide(&p->lines[0], u'─');
}

static void DrawCpu(struct Panel *p) {
  char buf[48];
  DrawRegister(p, 0, 7), DrawRegister(p, 0, 0), DrawSt(p, 0, 0);
  DrawRegister(p, 1, 6), DrawRegister(p, 1, 3), DrawSt(p, 1, 1);
  DrawRegister(p, 2, 2), DrawRegister(p, 2, 5), DrawSt(p, 2, 2);
  DrawRegister(p, 3, 1), DrawRegister(p, 3, 4), DrawSt(p, 3, 3);
  DrawRegister(p, 4, 8), DrawRegister(p, 4, 12), DrawSt(p, 4, 4);
  DrawRegister(p, 5, 9), DrawRegister(p, 5, 13), DrawSt(p, 5, 5);
  DrawRegister(p, 6, 10), DrawRegister(p, 6, 14), DrawSt(p, 6, 6);
  DrawRegister(p, 7, 11), DrawRegister(p, 7, 15), DrawSt(p, 7, 7);
  snprintf(buf, sizeof(buf), "RIP 0x%016x  FLG", m[0].ip);
  AppendPanel(p, 8, buf);
  DrawFlag(p, 8, "C", GetFlag(m[0].flags, FLAGS_CF),
           GetFlag(m[1].flags, FLAGS_CF));
  DrawFlag(p, 8, "Z", GetFlag(m[0].flags, FLAGS_ZF),
           GetFlag(m[1].flags, FLAGS_ZF));
  DrawFlag(p, 8, "S", GetFlag(m[0].flags, FLAGS_SF),
           GetFlag(m[1].flags, FLAGS_SF));
  DrawFlag(p, 8, "O", GetFlag(m[0].flags, FLAGS_OF),
           GetFlag(m[1].flags, FLAGS_OF));
  DrawFlag(p, 8, "P", GetFlag(m[0].flags, FLAGS_PF),
           GetFlag(m[1].flags, FLAGS_PF));
  DrawFlag(p, 8, "I", GetFlag(m[0].flags, FLAGS_IF),
           GetFlag(m[1].flags, FLAGS_IF));
  DrawFlag(p, 8, "D", GetFlag(m[0].flags, FLAGS_DF),
           GetFlag(m[1].flags, FLAGS_DF));
}

static void DrawXmm(struct Panel *p, long i, long r) {
  float f;
  double d;
  long j, k, n;
  uint8_t type;
  bool changed;
  char buf[32];
  uint8_t xmm[16];
  uint64_t ival, itmp;
  int cells, left, cellwidth, panwidth;
  memcpy(xmm, m->xmm[r / 8][r % 8], sizeof(xmm));
  changed = memcmp(xmm, m[1].xmm[r / 8][r % 8], sizeof(xmm)) != 0;
  if (changed) AppendPanel(p, i, "\e[7m");
  left = sprintf(buf, "XMM%-2d", r);
  AppendPanel(p, i, buf);
  type = m->xmmtype[r / 8][r % 8];
  switch (type) {
    case kXmmFloat:
      cells = 4;
      break;
    case kXmmDouble:
      cells = 2;
      break;
    case kXmmIntegral:
      cells = 16 / ssewidth;
      break;
    default:
      unreachable;
  }
  panwidth = p->right - p->left;
  cellwidth = MIN(MAX(0, (panwidth - left) / cells - 1), sizeof(buf) - 1);
  for (j = 0; j < cells; ++j) {
    AppendPanel(p, i, " ");
    switch (type) {
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
        for (k = 0; k < ssewidth; ++k) {
          itmp = xmm[j * ssewidth + k] & 0xff;
          itmp <<= k * 8;
          ival |= itmp;
        }
        if (!ssehex) {
          if (0 && ssewidth == 1 && (040 <= ival && ival < 0200 - 1)) {
            sprintf(buf, "%`'c", ival);
          } else {
            int64toarray_radix10(SignExtend(ival, ssewidth * 8), buf);
          }
        } else {
          uint64toarray_fixed16(ival, buf, ssewidth * 8);
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
  long i;
  for (i = 0; i < MIN(16, MAX(0, p->bottom - p->top)); ++i) {
    DrawXmm(p, i, i);
  }
}

static void ScrollCode(struct Panel *p) {
  long i, n;
  n = p->bottom - p->top;
  i = m->ip / DUMPWIDTH;
  if (!(memstart <= i && i < memstart + n)) {
    memstart = i;
  }
}

static void ScrollReadData(struct Panel *p) {
  long i, n, addr;
  n = p->bottom - p->top;
  i = m->readaddr / DUMPWIDTH;
  if (!(readstart <= i && i < readstart + n)) {
    readstart = i - n / 3;
  }
}

static void ScrollWriteData(struct Panel *p) {
  long i, n, addr;
  n = p->bottom - p->top;
  i = m->writeaddr / DUMPWIDTH;
  if (!(writestart <= i && i < writestart + n)) {
    writestart = i - n / 3;
  }
}

static void ScrollStack(struct Panel *p) {
  long i, n;
  n = p->bottom - p->top;
  i = Read64(m->sp) / DUMPWIDTH;
  if (!(stackstart <= i && i < stackstart + n)) {
    stackstart = i;
  }
}

static void DrawMemory(struct Panel *p, long beg, long a, long b) {
  char buf[16];
  long i, j, k, c;
  bool high, changed;
  high = false;
  for (i = 0; i < p->bottom - p->top; ++i) {
    snprintf(buf, sizeof(buf), "%p ", (beg + i) * DUMPWIDTH);
    AppendStr(&p->lines[i], buf);
    for (j = 0; j < DUMPWIDTH; ++j) {
      k = (beg + i) * DUMPWIDTH + j;
      c = VirtualBing(k);
      changed = a <= k && k < b;
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

static void DrawMaps(struct Panel *p) {
  int i;
  char *text, *p1, *p2;
  p1 = text = FormatPml4t(m->cr3);
  for (i = 0; p1; ++i, p1 = p2) {
    if ((p2 = strchr(p1, '\n'))) *p2++ = '\0';
    AppendPanel(p, i, p1);
  }
  free(text);
}

static void DrawBreakpoints(struct Panel *p) {
  int64_t addr;
  char buf[128];
  const char *name;
  long i, line, sym;
  for (line = 0, i = breakpoints.i; i--;) {
    if (breakpoints.p[i].disable) continue;
    addr = breakpoints.p[i].addr;
    sym = DisFindSym(dis, addr);
    name = sym != -1 ? dis->syms.stab + dis->syms.p[sym].name : "UNKNOWN";
    snprintf(buf, sizeof(buf), "%p %s", addr, name);
    AppendPanel(p, line, buf);
    if (sym != -1 && addr != dis->syms.p[sym].addr) {
      snprintf(buf, sizeof(buf), "+%#lx", addr - dis->syms.p[sym].addr);
      AppendPanel(p, line, buf);
    }
    ++line;
  }
}

static void DrawTrace(struct Panel *p) {
  int i, n;
  long sym;
  uint8_t *r;
  char line[128];
  const char *name;
  int64_t sp, bp, rp;
  rp = m->ip;
  bp = Read64(m->bp);
  sp = Read64(m->sp);
  for (i = 0; i < p->bottom - p->top;) {
    sym = DisFindSym(dis, rp);
    name = sym != -1 ? dis->syms.stab + dis->syms.p[sym].name : "UNKNOWN";
    snprintf(line, sizeof(line), "%p %p %s", bp, rp, name);
    AppendPanel(p, i, line);
    if (sym != -1 && rp != dis->syms.p[sym].addr) {
      snprintf(line, sizeof(line), "+%#lx", rp - dis->syms.p[sym].addr);
      AppendPanel(p, i, line);
    }
    if (!bp) break;
    if (bp < sp) {
      AppendPanel(p, i, " [STRAY]");
    } else {
      snprintf(line, sizeof(line), " %,ld bytes", bp - sp);
      AppendPanel(p, i, line);
    }
    if (bp & 15 && i) AppendPanel(p, i, " [MISALIGN]");
    ++i;
    if ((bp & 0xfff) > 0xff0) break;
    if (!(r = FindReal(m, bp))) {
      AppendPanel(p, i, "CORRUPT FRAME POINTER");
      break;
    }
    sp = bp;
    bp = Read64(r + 0);
    rp = Read64(r + 8);
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
    if (!(r = FindReal(m, bp))) {
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

static void Redraw(void) {
  int i, j;
  for (i = 0; i < ARRAYLEN(pan.p); ++i) {
    for (j = 0; j < pan.p[i].bottom - pan.p[i].top; ++j) {
      pan.p[i].lines[j].i = 0;
    }
  }
  DrawDisassembly(&pan.disassembly);
  DrawTerminal(&pan.terminal);
  DrawCpu(&pan.registers);
  DrawSse(&pan.sse);
  ScrollCode(&pan.code);
  ScrollStack(&pan.stack);
  ScrollReadData(&pan.readdata);
  ScrollWriteData(&pan.writedata);
  DrawHr(&pan.breakpointshr, "BREAKPOINTS");
  DrawHr(&pan.mapshr, "MAPS");
  DrawHr(&pan.tracehr, m->bofram[0] ? "PROTECTED FRAMES" : "FRAMES");
  DrawHr(&pan.terminalhr, "TELETYPEWRITER");
  DrawHr(&pan.ssehr, "SSE");
  DrawHr(&pan.codehr, "CODE");
  DrawHr(&pan.readhr, "READ");
  DrawHr(&pan.writehr, "WRITE");
  DrawHr(&pan.stackhr, "STACK");
  DrawMaps(&pan.maps);
  DrawTrace(&pan.trace);
  DrawBreakpoints(&pan.breakpoints);
  DrawMemory(&pan.code, memstart, m->ip, m->ip + m->xedd->length);
  DrawMemory(&pan.readdata, readstart, m->readaddr, m->readaddr + m->readsize);
  DrawMemory(&pan.writedata, writestart, m->writeaddr,
             m->writeaddr + m->writesize);
  DrawMemory(&pan.stack, stackstart, Read64(m->sp), Read64(m->sp) + 8);
  if (PrintPanels(ttyfd, ARRAYLEN(pan.p), pan.p, tyn, txn) == -1) {
    LOGF("PrintPanels Interrupted");
    CHECK_EQ(EINTR, errno);
  }
}

static int OnPtyFdClose(int fd) {
  return 0;
}

static ssize_t OnPtyFdRead(int fd, void *data, size_t size) {
  return read(fd, data, size);
}

static ssize_t OnPtyFdWrite(int fd, const void *data, size_t size) {
  if (tuimode) {
    return MachinePtyWrite(pty, data, size);
  } else {
    MachinePtyWrite(pty, data, size);
    return write(fd, data, size);
  }
}

static const struct MachineFdCb kMachineFdCbPty = {
    .close = OnPtyFdClose,
    .read = OnPtyFdRead,
    .write = OnPtyFdWrite,
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
  die();
  strcpy(systemfailure, "UNDEFINED INSTRUCTION");
  LaunchDebuggerReactively();
}

static void OnDecodeError(void) {
  strcpy(stpcpy(systemfailure, "DECODE: "),
         indexdoublenulstring(kXedErrorNames, m->xedd->op.error));
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

static void OnHalt(int interrupt) {
  switch (interrupt) {
    case 1:
    case 3:
      OnDebug();
      break;
    case kMachineSegmentationFault:
      OnSegmentationFault();
      break;
    case kMachineProtectionFault:
      OnProtectionFault();
      break;
    case kMachineSimdException:
      OnSimdException();
      break;
    case kMachineUndefinedInstruction:
      OnUndefinedInstruction();
      break;
    case kMachineDecodeError:
      OnDecodeError();
      break;
    case kMachineDivideError:
      OnDivideError();
      break;
    case kMachineFpuException:
      OnFpuException();
      break;
    case kMachineExit:
    case kMachineHalt:
    default:
      OnExit(interrupt);
      break;
  }
}

static void OnPageUp(void) {
  opstart -= tyn / 2;
}

static void OnPageDown(void) {
  opstart += tyn / 2;
}

static void OnUpArrow(void) {
  opstart--;
}

static void OnDownArrow(void) {
  opstart++;
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

static void OnFeed(void) {
  write(ttyfd, "\e[K\e[J", 6);
}

static void OnUp(void) {
  uint8_t b[8];
  int64_t sp, bp, ret;
  bp = Read64(m->bp);
  sp = Read64(m->sp);
  if (bp >= sp) {
    VirtualSend(m, b, bp + 8, sizeof(b));
    ret = Read64(b);
    ScrollOp(&pan.disassembly, GetDisIndex(ret));
  }
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
  action |= CONTINUE;
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

static void OnSseType(void) {
  uint8_t t;
  long i, j;
  t = CycleSseType(m->xmmtype[0][0]);
  for (i = 0; i < 2; ++i) {
    for (j = 0; j < 8; ++j) {
      m->xmmtype[i][j] = t;
    }
  }
}

static void OnSseWidth(void) {
  ssewidth = CycleSseWidth(ssewidth);
}

static void OnSseHex(void) {
  ssehex = !ssehex;
}

static bool HasPendingKeyboard(void) {
  struct pollfd fds[1];
  fds[0].fd = ttyfd;
  fds[0].events = POLLIN;
  fds[0].revents = 0;
  poll(fds, ARRAYLEN(fds), 0);
  return fds[0].revents & (POLLIN | POLLERR);
}

static bool IsExecuting(void) {
  return (action & (CONTINUE | STEP | NEXT | FINISH)) && !(action & FAILURE);
}

static void ReadKeyboard(void) {
  int c;
  char b[20];
  ssize_t rc;
  size_t i, n;
  if ((rc = read(ttyfd, b, 16)) != -1) {
    for (n = rc, i = 0; i < n; ++i) {
      switch (b[i++]) {
        CASE('q', OnQ());
        CASE('s', OnStep());
        CASE('n', OnNext());
        CASE('f', OnFinish());
        CASE('x', OnSseHex());
        CASE('c', OnContinue());
        CASE('R', OnRestart());
        CASE('t', OnSseType());
        CASE('w', OnSseWidth());
        CASE('u', OnUp());
        CASE('d', OnDown());
        CASE('B', PopBreakpoint(&breakpoints));
        CASE('\t', OnTab());
        CASE('\r', OnEnter());
        CASE('\n', OnEnter());
        CASE(CTRL('C'), OnCtrlC());
        CASE(CTRL('D'), OnCtrlC());
        CASE(CTRL('\\'), OnQuit());
        CASE(CTRL('Z'), raise(SIGSTOP));
        CASE(CTRL('L'), OnFeed());
        CASE(CTRL('P'), OnUpArrow());
        CASE(CTRL('N'), OnDownArrow());
        CASE(CTRL('V'), OnPageDown());
        case '\e':
          switch (b[i++]) {
            CASE('v', OnPageUp());
            case '[':
              switch (b[i++]) {
                CASE('A', OnUpArrow());
                CASE('B', OnDownArrow());
                CASE('F', OnEnd());
                CASE('H', OnHome());
                case '1':
                  switch (b[i++]) {
                    CASE('~', OnHome());
                    default:
                      break;
                  }
                  break;
                case '4':
                  switch (b[i++]) {
                    CASE('~', OnEnd());
                    default:
                      break;
                  }
                  break;
                case '5':
                  switch (b[i++]) {
                    CASE('~', OnPageUp());
                    default:
                      break;
                  }
                  break;
                case '6':
                  switch (b[i++]) {
                    CASE('~', OnPageDown());
                    default:
                      break;
                  }
                  break;
                case '7':
                  switch (b[i++]) {
                    CASE('~', OnHome());
                    default:
                      break;
                  }
                  break;
                case '8':
                  switch (b[i++]) {
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
  } else if (errno == EINTR) {
    LOGF("ReadKeyboard Interrupted");
  } else if (errno == EAGAIN) {
    poll((struct pollfd[]){{ttyfd, POLLIN}}, 1, -1);
  } else {
    perror("ReadKeyboard");
    exit(1);
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
  struct Breakpoint b = {0};
  if (isdigit(*s)) {
    b.addr = ParseHexValue(s);
  } else {
    b.symbol = optarg;
  }
  PushBreakpoint(&breakpoints, &b);
}

static noreturn void PrintUsage(int rc, FILE *f) {
  fprintf(f, "SYNOPSIS\n\n  %s%s", program_invocation_name, USAGE);
  exit(rc);
}

static void LeaveScreen(void) {
  char buf[64];
  write(ttyfd, buf, sprintf(buf, "\e[%d;%dH\e[S\r\n", tyn - 1, txn - 1));
}

static void Exec(void) {
  long op;
  ssize_t bp;
  int interrupt;
  ExecSetup();
  if (!(interrupt = setjmp(m->onhalt))) {
    if ((bp = IsAtBreakpoint(&breakpoints, m->ip)) != -1) {
      LOGF("BREAK %p", breakpoints.p[bp].addr);
      tuimode = true;
      LoadInstruction(m);
      ExecuteInstruction(m);
      CheckFramePointer();
      ops++;
    } else {
      for (;;) {
        LoadInstruction(m);
        ExecuteInstruction(m);
        CheckFramePointer();
        ops++;
        if (action || breakpoints.i) {
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
          if ((bp = IsAtBreakpoint(&breakpoints, m->ip)) != -1) {
            LOGF("BREAK %p", breakpoints.p[bp].addr);
            tuimode = true;
            break;
          }
        }
      }
    }
  } else {
    OnHalt(interrupt);
  }
}

static void Tui(void) {
  long op;
  ssize_t bp;
  int interrupt;
  LOGF("TUI");
  TuiSetup();
  SetupDraw();
  ScrollOp(&pan.disassembly, GetDisIndex(m->ip));
  if (!(interrupt = setjmp(m->onhalt))) {
    for (;;) {
      if (!(action & FAILURE)) {
        LoadInstruction(m);
      } else {
        m->xedd = m->icache;
        m->xedd->length = 1;
        m->xedd->bytes[0] = 0xCC;
        m->xedd->op.opcode = 0xCC;
      }
      if (action & WINCHED) {
        LOGF("WINCHED");
        GetTtySize();
        action &= ~WINCHED;
      }
      SetupDraw();
      Redraw();
      if (action & FAILURE) {
        LOGF("TUI FAILURE");
        PrintMessageBox(ttyfd, systemfailure, tyn, txn);
        ReadKeyboard();
      } else if (!IsExecuting() || HasPendingKeyboard()) {
        ReadKeyboard();
      }
      if (action & INT) {
        LOGF("TUI INT");
        action &= ~INT;
        if (action & (CONTINUE | NEXT | FINISH)) {
          action &= ~(CONTINUE | NEXT | FINISH);
        } else {
          tuimode = false;
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
        op = GetDisIndex(m->ip);
        ScrollOp(&pan.disassembly, op);
        VERBOSEF("%s", dis->ops.p[op].s);
        memcpy(&m[1], &m[0], sizeof(m[0]));
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
          ExecuteInstruction(m);
        } else {
          m->ip += m->xedd->length;
          action &= ~NEXT;
          action &= ~FINISH;
          action &= ~CONTINUE;
        }
        CheckFramePointer();
        ops++;
        if (!(action & CONTINUE)) {
          ScrollOp(&pan.disassembly, GetDisIndex(m->ip));
          if ((action & FINISH) && IsRet()) action &= ~FINISH;
          if (((action & NEXT) && IsRet()) || (action & FINISH)) {
            action &= ~NEXT;
          }
        }
        if ((action & (FINISH | NEXT | CONTINUE)) &&
            (bp = IsAtBreakpoint(&breakpoints, m->ip)) != -1) {
          action &= ~(FINISH | NEXT | CONTINUE);
          LOGF("BREAK %p", breakpoints.p[bp].addr);
          break;
        }
      }
    }
  } else {
    OnHalt(interrupt);
    ScrollOp(&pan.disassembly, GetDisIndex(m->ip));
  }
  TuiCleanup();
}

static void GetOpts(int argc, char *argv[]) {
  int opt;
  stpcpy(stpcpy(stpcpy(logpath, kTmpPath), basename(argv[0])), ".log");
  while ((opt = getopt(argc, argv, "?hvtrsb:HL:")) != -1) {
    switch (opt) {
      case 't':
        tuimode = true;
        break;
      case 'r':
        react = true;
        break;
      case 's':
        printstats = true;
        break;
      case 'b':
        HandleBreakpointFlag(optarg);
        break;
      case 'H':
        g_dis_high = NULL;
        break;
      case 'v':
        ++g_loglevel;
        break;
      case 'L':
        strcpy(logpath, optarg);
        break;
      case 'h':
      case '?':
        PrintUsage(EXIT_SUCCESS, stdout);
      default:
        PrintUsage(EX_USAGE, stderr);
    }
  }
  g_logfile = fopen(logpath, "a");
  setvbuf(g_logfile, xmalloc(PAGESIZE), _IOLBF, PAGESIZE);
}

int Emulator(int argc, char *argv[]) {
  void *code;
  int rc, fd;
  codepath = argv[optind++];
  pty = MachinePtyNew(20, 80);
  InitMachine(m);
  m->fds.p = xcalloc((m->fds.n = 8), sizeof(struct MachineFd));
Restart:
  action = 0;
  LoadProgram(m, codepath, argv + optind, environ, elf);
  m->fds.i = 3;
  m->fds.p[0].fd = STDIN_FILENO;
  m->fds.p[0].cb = &kMachineFdCbPty;
  m->fds.p[1].fd = STDOUT_FILENO;
  m->fds.p[1].cb = &kMachineFdCbPty;
  m->fds.p[2].fd = STDERR_FILENO;
  m->fds.p[2].cb = &kMachineFdCbPty;
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
    fprintf(stderr, "ops:    %,ld\n", ops);
  }
  munmap(elf->ehdr, elf->size);
  DisFree(dis);
  return exitcode;
}

static void OnlyRunOnFirstCpu(void) {
  uint64_t bs;
  bs = 1;
  sched_setaffinity(0, sizeof(bs), &bs);
}

int main(int argc, char *argv[]) {
  int rc;
  ssewidth = 2; /* 16-bit is best bit */
  if (!NoDebug()) showcrashreports();
  // OnlyRunOnFirstCpu();
  if ((colorize = cancolor())) {
    high.keyword = 155;
    high.reg = 215;
    high.literal = 182;
    high.label = 221;
    high.comment = 112;
    g_dis_high = &high;
  }
  GetOpts(argc, argv);
  if (optind == argc) PrintUsage(EX_USAGE, stderr);
  rc = Emulator(argc, argv);
  return rc;
}
