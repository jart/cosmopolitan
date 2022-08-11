#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/log/gdb.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/stdio.h"

/**
 * @fileovierview gdbexec(s) demo
 * It basically launches an ephemeral `gdb -p $PID -ex "$s"`.
 */

int i;
int M[8][8] = {
    {772549, 921569, 407843, 352941, 717647, 78431, 666667, 627451},
    {321569, 419608, 227451, 396078, 223529, 882353, 952941, 937255},
    {15686, 545098, 31373, 7843, 15686, 298039, 976471, 352941},
    {70588, 858824, 415686, 184314, 25098, 5098, 141176, 47059},
    {141176, 541176, 658824, 227451, 490196, 301961, 937255, 678431},
    {188235, 823529, 858824, 87451, 545098, 611765, 188235, 576471},
    {580392, 913725, 996078, 592157, 7451, 176471, 862745, 784314},
    {278431, 945098, 843137, 439216, 878431, 529412, 262745, 43137},
};

int main(int argc, char *argv[]) {
  int y, x;
  for (i = 0;; ++i) {
    for (y = 0; y < 8; ++y) {
      for (x = 0; x < 8; ++x) {
        if (!(M[y][x] % 2)) {
          M[y][x] /= 2;
        } else {
          M[y][x] *= 3;
          M[y][x] += 1;
        }
      }
    }
    if (rand() % 10000 == 0) {
      gdbexec("print i");
      gdbexec("print M");
      break;
    }
  }
  printf("quitting\n");
  return 0;
}
