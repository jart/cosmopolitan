#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/math.h"
#include "libc/calls/termios.h"
#include "libc/isystem/unistd.h"
#include "libc/str/str.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/termios.h"

STATIC_YOINK("vga_console");

int main(int argc, char *argv[]) {
  volatile long double x = -.5;
  volatile long double y = 1.5;
  struct termios tio;
  char buf[4];
  ssize_t res;
  if (tcgetattr(0, &tio) != -1) {
    tio.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(0, TCSANOW, &tio);
  }
  write(1, "\e[5n", 4);
  res = read(0, buf, 4);
  if (res != 4 || memcmp(buf, "\e[0n", 4) != 0) {
    printf("res = %d?\n", res);
    return -1;
  }
  printf("Hello World! %.19Lg\n", atan2l(x, y));
  return 0;
}
