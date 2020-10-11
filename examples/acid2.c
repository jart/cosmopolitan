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
#include "libc/calls/struct/termios.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/termios.h"

int yn2, xn2;
int yn3, xn3;
char b[128], inbuf[128];
int y, x, yn, xn, my, mx;
struct termios term, oldterm;

int main(int argc, char *argv[]) {
  int i;
  setvbuf(stdout, inbuf, _IONBF, 128); /* make things slower */

  /* raw mode */
  ioctl(1, TCGETS, &oldterm);
  memcpy(&term, &oldterm, sizeof(term));
  term.c_cc[VMIN] = 1;
  term.c_cc[VTIME] = 1;
  term.c_iflag &= ~(INPCK | ISTRIP | PARMRK | INLCR | IGNCR | ICRNL | IXON);
  term.c_lflag &= ~(IEXTEN | ICANON | ECHO | ECHONL);
  term.c_cflag &= ~(CSIZE | PARENB);
  term.c_oflag &= ~OPOST;
  term.c_cflag |= CS8;
  term.c_iflag |= IUTF8;
  ioctl(1, TCSETSF, &term);

  /* get cursor position and display dimensions */
  printf("\e7\e[6n\e[9979;9979H\e[6n\e8");
  read(0, b, sizeof(b));
  sscanf(b, "\e[%d;%dR\e[%d;%dR", &y, &x, &yn, &xn);

  printf("\e[1q"); /* turn on led one */
  printf("\e[2J"); /* clear display */
  printf("\e#8");  /* fill display with E's */
  printf("\e[H");

  /* clear display again */
  printf("\e[2q"); /* turn on led two */
  for (i = 0; i < yn; ++i) {
    if (i) printf("\n");
    printf(" ");
    printf("\e[0K");
  }
  for (i = 0; i < yn - 1; ++i) {
    if (i) printf("\eM");
    printf("\e[1K");
  }

  printf("\e(0");  /* line drawing mode */
  printf("\e[3q"); /* turn on led three */
  printf("\e[H");

  /* move to center */
  my = yn / 2;
  mx = xn / 2 - 7;
  if (y > my) {
    printf("\e[%dA", y - my);
  } else if (y < my) {
    printf("\e[%dB", my - y);
  }
  if (x > mx) {
    printf("\e[%dD", x - mx);
  } else if (x < mx) {
    printf("\e[%dC", mx - x);
  }

  printf("\e[90;103m");                /* black on yellow */
  printf("\e[90;103ma      `      a"); /* draw nose */

  printf("\e[0m"); /* reset style */
  printf("\e(B");  /* ascii mode */

  /* draw corners */
  printf("\e[H"); /* top left */
  printf("A");
  printf("\e[9979C"); /* rightmost */
  printf("B");
  printf("\e[9979;9979H"); /* bottom right corner */
  printf("\e[C");          /* move right gets clamped */
  printf("D");             /* write, set redzone flag */
  printf("\e[2A");         /* move up, unsets redzone */

  /* gnu screen now reports out of bounds position */
  /* kitty hasnt got a redzone reporting next line */
  printf("\e[6n");
  read(0, b, sizeof(b));
  sscanf(b, "\e[%d;%dR", &yn2, &xn2);

  /* writes to (yn-3,xn-1) normally and (yn-2,0) in gnu screen */
  printf("!");

  /* draw ruler on top */
  printf("\e[H");
  for (i = 8; i + 1 < xn; i += 8) {
    printf("\e[%dG%d", i + 1, i); /* set column */
  }

  printf("\e[9979;9979H"); /* bottom right */
  printf("\e[9979D");      /* leftmost */
  printf("C");

  /* let's break gnu screen again with multimonospace redzone */
  printf("\e[%d;9979H", yn / 2); /* right middle */
  printf("\e[D");                /* left */
  printf("Ａ");
  printf("\e[6n");
  read(0, b, sizeof(b));
  sscanf(b, "\e[%d;%dR", &yn2, &xn2);

  printf("\e[%dH", yn / 2);
  printf("%d %d vs. %d %d\r\n", yn, xn, yn2, xn2);
  printf("%d %d vs. %d %d\r\n", yn / 2, xn, yn2, xn2);
  printf("\e#6double width\e#5\r\n");
  printf("\e[3mthis text is so \e[1mitalic\e[0m\r\n");
  printf("\e[1;20mpress any fraktur exit\e[0m");
  printf("\a");

  read(0, b, sizeof(b));
  printf("\r\n");
  ioctl(1, TCSETS, &oldterm);
  return 0;
}
