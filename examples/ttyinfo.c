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
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/termios.h"
#include "libc/errno.h"
#include "libc/fmt/fmt.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/nt/runtime.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/select.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/exit.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/termios.h"
#include "libc/x/xsigaction.h"

__static_yoink("WinMainStdin");

#define CTRL(C)                ((C) ^ 0b01000000)
#define WRITE(FD, SLIT)        write(FD, SLIT, strlen(SLIT))
#define ENABLE_SAFE_PASTE      "\e[?2004h"
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
  WRITE(1, DISABLE_MOUSE_TRACKING);
  tcsetattr(1, TCSANOW, &oldterm);
}

int rawmode(void) {
  static bool once;
  struct termios t;
  if (!once) {
    if (!tcgetattr(1, &oldterm)) {
      atexit(restoretty);
    } else {
      perror("tcgetattr");
    }
    once = true;
  }
  memcpy(&t, &oldterm, sizeof(t));

  t.c_cc[VMIN] = 1;
  t.c_cc[VTIME] = 1;

  // emacs does the following to remap ctrl-c to ctrl-g in termios
  //     t.c_cc[VINTR] = CTRL('G');
  // it can be restored using
  //     (set-quit-char (logxor ?C 0100))
  // but we are able to polyfill the remapping on windows
  // please note this is a moot point b/c ISIG is cleared

  t.c_iflag &= ~(INPCK | ISTRIP | PARMRK | INLCR | IGNCR | ICRNL | IXON |
                 IGNBRK | BRKINT);
  t.c_lflag &= ~(IEXTEN | ICANON | ECHO | ECHONL | ISIG);
  t.c_cflag &= ~(CSIZE | PARENB);
  t.c_oflag |= OPOST | ONLCR;
  t.c_cflag |= CS8;
  t.c_iflag |= IUTF8;

  if (tcsetattr(1, TCSANOW, &t)) {
    perror("tcsetattr");
  }

  /* WRITE(1, ENABLE_SAFE_PASTE); */
  /* WRITE(1, ENABLE_MOUSE_TRACKING); */
  /* WRITE(1, PROBE_DISPLAY_SIZE); */
  return 0;
}

void getsize(void) {
  if (tcgetwinsize(1, &wsize) != -1) {
    printf("termios says terminal size is %hu×%hu\r\n", wsize.ws_col,
           wsize.ws_row);
  } else {
    perror("tcgetwinsize");
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
        __builtin_unreachable();
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

// change the code above to enable ISIG if you want to trigger this
// then press ctrl-c or ctrl-\ in your pseudoteletypewriter console
void OnSignalThatWontEintrRead(int sig) {
  dprintf(1, "got %s (read()ing will SA_RESTART)\n", strsignal(sig));
}

int main(int argc, char *argv[]) {
  int e, c, y, x, n, yn, xn;
  signal(SIGINT, OnSignalThatWontEintrRead);
  signal(SIGQUIT, OnSignalThatWontEintrRead);
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
    bzero(code, sizeof(code));
    if ((n = readansi(0, code, sizeof(code))) == -1) {
      if (errno == EINTR) continue;
      perror("read");
      exit(1);
    }
    if (!n) {
      printf("got stdin eof\n");
      exit(0);
    }
    printf("%`'.*s (got %d) ", n, code, n);
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
