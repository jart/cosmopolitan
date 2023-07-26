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
#include "libc/calls/termios.h"
#include "libc/isystem/unistd.h"
#include "libc/math.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/termios.h"

#ifdef __x86_64__

/**
 * @fileoverview Bare Metal VGA TTY demo.
 *
 * This program can boot as an operating system. Try it out:
 *
 *     make -j8 o//examples/vga.com
 *     qemu-system-x86_64 -hda o//examples/vga.com -serial stdio
 *
 * Please note that, by default, APE binaries only use the serial port
 * for stdio. To get the VGA console as an added bonus:
 *
 *     __static_yoink("vga_console");
 *
 * Should be added to the top of your main() program source file.
 */

__static_yoink("vga_console");

int main(int argc, char *argv[]) {
  volatile long double x = -.5;
  volatile long double y = 1.5;
  struct termios tio;
  char buf[16];
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
  printf("\e[92;44mHello World!\e[0m %.19Lg\n", atan2l(x, y));

  // read/print loop so machine doesn't reset on metal
  for (;;) {
    if ((res = readansi(0, buf, 16)) > 0) {
      printf("got \e[97m%`'.*s\e[0m\r\n", res, buf);
    }
  }
}

#endif /* __x86_64__ */
