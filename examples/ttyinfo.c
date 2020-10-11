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
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/exit.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/termios.h"
#include "libc/x/x.h"

#define CTRL(C)                ((C) ^ 0b01000000)
#define ENABLE_MOUSE_TRACKING  "\e[?1000;1002;1015;1006h"
#define DISABLE_MOUSE_TRACKING "\e[?1000;1002;1015;1006l"
#define PROBE_DISPLAY_SIZE     "\e7\e[9979;9979H\e[6n\e8"

char code[512];
struct winsize wsize;
struct termios oldterm;
volatile bool resized, killed;

void onresize(void) {
  resized = true;
}

void onkilled(int sig) {
  killed = true;
}

void restoretty(void) {
  write(1, DISABLE_MOUSE_TRACKING, strlen(DISABLE_MOUSE_TRACKING));
  ioctl(1, TCSETS, &oldterm);
}

int rawmode(void) {
  static bool once;
  struct termios t;
  if (!once) {
    if (ioctl(1, TCGETS, &oldterm) != -1) {
      atexit(restoretty);
    } else {
      return -1;
    }
    once = true;
  }
  memcpy(&t, &oldterm, sizeof(t));
  t.c_cc[VMIN] = 1;
  t.c_cc[VTIME] = 1;
  t.c_iflag &= ~(INPCK | ISTRIP | PARMRK | INLCR | IGNCR | ICRNL | IXON |
                 IGNBRK | BRKINT);
  t.c_lflag &= ~(IEXTEN | ICANON | ECHO | ECHONL | ISIG);
  t.c_cflag &= ~(CSIZE | PARENB);
  t.c_oflag &= ~OPOST;
  t.c_cflag |= CS8;
  t.c_iflag |= IUTF8;
  ioctl(1, TCSETS, &t);
  write(1, ENABLE_MOUSE_TRACKING, strlen(ENABLE_MOUSE_TRACKING));
  write(1, PROBE_DISPLAY_SIZE, strlen(PROBE_DISPLAY_SIZE));
  return 0;
}

void getsize(void) {
  if (getttysize(1, &wsize) != -1) {
    printf("termios says terminal size is %hu×%hu\r\n", wsize.ws_col,
           wsize.ws_row);
  } else {
    printf("%s\n", strerror(errno));
  }
}

const char *describemouseevent(int e) {
  static char buf[64];
  buf[0] = 0;
  if (e & 0x10) {
    strcat(buf, " ctrl");
  }
  if (e & 0x40) {
    strcat(buf, " wheel");
    if (e & 0x01) {
      strcat(buf, " down");
    } else {
      strcat(buf, " up");
    }
  } else {
    switch (e & 3) {
      case 0:
        strcat(buf, " left");
        break;
      case 1:
        strcat(buf, " middle");
        break;
      case 2:
        strcat(buf, " right");
        break;
      default:
        unreachable;
    }
    if (e & 0x20) {
      strcat(buf, " drag");
    } else if (e & 0x04) {
      strcat(buf, " up");
    } else {
      strcat(buf, " down");
    }
  }
  return buf + 1;
}

int main(int argc, char *argv[]) {
  int e, c, y, x, n, yn, xn;
  xsigaction(SIGTERM, onkilled, 0, 0, NULL);
  xsigaction(SIGWINCH, onresize, 0, 0, NULL);
  xsigaction(SIGCONT, onresize, 0, 0, NULL);
  rawmode();
  getsize();
  while (!killed) {
    if (resized) {
      printf("SIGWINCH ");
      getsize();
      resized = false;
    }
    if ((n = readansi(0, code, sizeof(code))) == -1) {
      if (errno == EINTR) continue;
      printf("ERROR: READ: %s\r\n", strerror(errno));
      exit(1);
    }
    printf("%`'.*s ", n, code);
    if (iscntrl(code[0]) && !code[1]) {
      printf("is CTRL-%c a.k.a. ^%c\r\n", CTRL(code[0]), CTRL(code[0]));
      if (code[0] == CTRL('C') || code[0] == CTRL('D')) break;
    } else if (startswith(code, "\e[") && endswith(code, "R")) {
      yn = 1, xn = 1;
      sscanf(code, "\e[%d;%dR", &yn, &xn);
      printf("inband signalling says terminal size is %d×%d\r\n", xn, yn);
    } else if (startswith(code, "\e[<") &&
               (endswith(code, "m") || endswith(code, "M"))) {
      e = 0, y = 1, x = 1;
      sscanf(code, "\e[<%d;%d;%d%c", &e, &y, &x, &c);
      printf("mouse %s at %d×%d\r\n", describemouseevent(e | (c == 'm') << 2),
             x, y);
    } else {
      printf("\r\n");
    }
  }
  return 0;
}
