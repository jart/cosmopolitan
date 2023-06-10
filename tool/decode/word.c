#include "libc/assert.h"
#include "libc/fmt/conv.h"
#include "libc/stdio/stdio.h"

/**
 * Displays bytes as word, e.g.
 *
 *     o//tool/word.com 6e c7 ff ff
 *     %d   = -14482
 *     %ld  = 4294952814
 *     %#x  = 0xffffc76e
 *     %#lx = 0xffffc76e
 *
 */
int main(int argc, char *argv[]) {
  long x;
  int i, k, b;
  for (x = k = 0, i = 1; i < argc; ++i) {
    b = strtol(argv[i], 0, 16);
    assert(0 <= b && b <= 255);
    x |= (unsigned long)b << k;
    k += 8;
  }
  printf("%%d   = %d\n"
         "%%ld  = %ld\n"
         "%%#x  = %#x\n"
         "%%#lx = %#lx\n",
         (int)x, x, (int)x, x);
}
