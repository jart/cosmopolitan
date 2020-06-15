#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "dsp/tty/tty.h"
#include "libc/calls/calls.h"
#include "libc/calls/ioctl.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/termios.h"
#include "libc/errno.h"
#include "libc/fmt/fmt.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/runtime/gc.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/exit.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/termios.h"
#include "libc/x/x.h"

/*
  "\e[c"  → "\e[?1;2c"
  "\e[x"  → "\e[2;1;1;112;112;1;0x"
  "\e[>c" → "\e[>83;40500;0c"
  "\e[6n" → "\e[52;1R"
*/

#define CTRL(C)               ((C) ^ 0b01000000)
#define PROBE_VT100           "\e[c"   /* e.g. "\e[?1;2c", "\e[>0c" */
#define PROBE_SECONDARY       "\e[>c"  /* "\e[>83;40500;0c" (Screen v4.05.00) */
#define PROBE_PARAMETERS      "\e[x"   /* e.g. "\e[2;1;1;112;112;1;0x" */
#define PROBE_CURSOR_POSITION "\e[6n"  /* e.g. "\e[𝑦;𝑥R" */
#define PROBE_SUN_DTTERM_SIZE "\e[14t" /* e.g. "\e[𝑦;𝑥R" */

int fd_;
jmp_buf jb_;
ssize_t got_;
uint8_t buf_[128];
struct TtyIdent ti_;
struct winsize wsize_;
volatile bool resized_;

void OnResize(void) {
  resized_ = true;
}
void OnKilled(int sig) {
  longjmp(jb_, 128 + sig + 1);
}

void getsome(void) {
  if ((got_ = read(fd_, buf_, sizeof(buf_))) == -1 && errno != EINTR) {
    printf("%s%s\r\n", "error: ", strerror(errno));
    longjmp(jb_, EXIT_FAILURE + 1);
  }
  if (got_ >= 0) {
    printf("%`'.*s\r\n", got_, buf_);
    if (got_ > 0 && buf_[0] == CTRL('C')) {
      longjmp(jb_, EXIT_SUCCESS + 1);
    }
  }
  if (resized_) {
    CHECK_NE(-1, getttysize(fd_, &wsize_));
    printf("SIGWINCH → %hu×%hu\r\n", wsize_.ws_row, wsize_.ws_col);
    resized_ = false;
  }
}

void probe(const char *s) {
  printf("%`'s → ", s);
  write(fd_, s, strlen(s));
  getsome();
}

int main(int argc, char *argv[]) {
  int rc;
  char ttyname[64];
  struct termios old;
  CHECK_NE(-1, (fd_ = open("/dev/tty", O_RDWR)));
  CHECK_NE(-1, ttyconfig(fd_, ttysetrawmode, 0, &old));
  if (!(rc = setjmp(jb_))) {
    xsigaction(SIGTERM, OnKilled, 0, 0, NULL);
    xsigaction(SIGWINCH, OnResize, 0, 0, NULL);
    if (ttyident(&ti_, STDIN_FILENO, STDOUT_FILENO) != -1) {
      ttysendtitle(fd_, "justine was here", &ti_);
      fputs(ttydescribe(ttyname, sizeof(ttyname), &ti_), stdout);
    }
    fputs("\r\n", stdout);
    probe(PROBE_VT100);
    probe(PROBE_SECONDARY);
    probe(PROBE_PARAMETERS);
    probe(PROBE_CURSOR_POSITION);
    /* probe(PROBE_SUN_DTTERM_SIZE); */
    getsome();
    for (;;) getsome();
  }
  ttyrestore(fd_, &old);
  ttyidentclear(&ti_);
  return rc - 1;
}
