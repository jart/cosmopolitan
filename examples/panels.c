#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/calls/calls.h"
#include "libc/calls/ioctl.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/termios.h"
#include "libc/calls/struct/winsize.h"
#include "libc/dce.h"
#include "libc/log/check.h"
#include "libc/log/gdb.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/mem/gc.internal.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/termios.h"
#include "libc/x/x.h"
#include "libc/x/xasprintf.h"
#include "tool/build/lib/panel.h"

/**
 * @fileoverview Cosmopolitan Paneling Demo.
 *
 * This is useful for creating terminal user interfaces. We take the
 * simplest approach possible. The main thing we abstract is like,
 * truncating the lines that overflow within a panel. In order to do
 * that, we abstract the ANSI parsing and the implementation is able to
 * tell how many cells wide each UNICODE character is.
 *
 * There are smarter ways for Cosmopolitan to do this. For example, it'd
 * be great to have automatic flex boxes. It'd also be nice to be able
 * to use dynamic programming for low bandwidth display updates, like
 * Emacs does, but that's less of an issue these days and can actually
 * make things slower, since for heavy workloads like printing videos,
 * having ANSI codes bouncing around the display actually goes slower.
 *
 * Beyond basic paneling, a message box widget is also provided, which
 * makes it easier to do modal dialogs.
 */

struct Panels {
  union {
    struct {
      struct Panel left;
      struct Panel right;
    };
    struct Panel p[2];
  };
} pan;

long tyn;
long txn;
char key[8];
bool shutdown;
bool invalidated;
struct termios oldterm;

void OnShutdown(int sig) {
  shutdown = true;
}

void OnInvalidate(int sig) {
  invalidated = true;
}

void GetTtySize(void) {
  struct winsize wsize;
  wsize.ws_row = tyn;
  wsize.ws_col = txn;
  _getttysize(1, &wsize);
  tyn = wsize.ws_row;
  txn = wsize.ws_col;
}

int Write(const char *s) {
  return write(1, s, strlen(s));
}

void Setup(void) {
  CHECK_NE(-1, ioctl(1, TCGETS, &oldterm));
}

void Enter(void) {
  struct termios term;
  memcpy(&term, &oldterm, sizeof(term));
  term.c_cc[VMIN] = 1;
  term.c_cc[VTIME] = 1;
  term.c_iflag &= ~(INPCK | ISTRIP | PARMRK | INLCR | IGNCR | ICRNL | IXON);
  term.c_lflag &= ~(IEXTEN | ICANON | ECHO | ECHONL);
  term.c_cflag &= ~(CSIZE | PARENB);
  term.c_cflag |= CS8;
  term.c_iflag |= IUTF8;
  CHECK_NE(-1, ioctl(1, TCSETS, &term));
  Write("\e[?25l");
}

void Leave(void) {
  Write(gc(xasprintf("\e[?25h\e[%d;%dH\e[S\r\n", tyn, txn)));
  ioctl(1, TCSETS, &oldterm);
}

void Clear(void) {
  long i, j;
  for (i = 0; i < ARRAYLEN(pan.p); ++i) {
    for (j = 0; j < pan.p[i].n; ++j) {
      free(pan.p[i].lines[j].p);
    }
    free(pan.p[i].lines);
    pan.p[i].lines = 0;
    pan.p[i].n = 0;
  }
}

void Layout(void) {
  long i, j;
  i = txn >> 1;
  pan.left.top = 0;
  pan.left.left = 0;
  pan.left.bottom = tyn;
  pan.left.right = i;
  pan.right.top = 0;
  pan.right.left = i + 1;
  pan.right.bottom = tyn;
  pan.right.right = txn;
  pan.left.n = pan.left.bottom - pan.left.top;
  pan.left.lines = xcalloc(pan.left.n, sizeof(*pan.left.lines));
  pan.right.n = pan.right.bottom - pan.right.top;
  pan.right.lines = xcalloc(pan.right.n, sizeof(*pan.right.lines));
}

void Append(struct Panel *p, int i, const char *s) {
  if (i >= p->n) return;
  AppendStr(p->lines + i, s);
}

void Draw(void) {
  Append(&pan.left, 0, gc(xasprintf("you typed %`'s", key)));
  Append(&pan.left, ((tyn + 1) >> 1) + 0, "hello left 1 𐌰𐌱𐌲𐌳𐌴𐌵𐌶𐌷");
  Append(&pan.left, ((tyn + 1) >> 1) + 1, "hello left 2 (╯°□°)╯");
  Append(&pan.right, ((tyn + 1) >> 1) + 0, "hello right 1");
  Append(&pan.right, ((tyn + 1) >> 1) + 1, "hello right 2");
  PrintPanels(1, ARRAYLEN(pan.p), pan.p, tyn, txn);
}

int main(int argc, char *argv[]) {
  struct sigaction sa[2] = {{.sa_handler = OnShutdown},
                            {.sa_handler = OnInvalidate}};
  if (!NoDebug()) ShowCrashReports();
  Setup();
  Enter();
  GetTtySize();
  sigaction(SIGINT, &sa[0], 0);
  sigaction(SIGCONT, &sa[1], 0);
  sigaction(SIGWINCH, &sa[1], 0);
  atexit(Leave);
  do {
    Clear();
    Layout();
    Draw();
    if (invalidated) {
      Enter();
      GetTtySize();
      invalidated = false;
    } else {
      readansi(0, key, sizeof(key));
    }
  } while (!shutdown);
}
