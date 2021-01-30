#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/errno.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"
#include "third_party/xed/x86.h"

/**
 * @fileoverview x86 instruction length decoder by way of hex pipe.
 */

int fgethex(FILE *f) {
  int o, t = -1;
  while (!((o = fgetc(f)) & ~0xFF)) {
    switch (t) {
      case -1:
        t = isxdigit(o) ? hextoint(o) : -1;
        break;
      default:
        if (isxdigit(o)) {
          return t * 16 + hextoint(o);
        }
        break;
    }
  }
  if (t >= 0) return einval();
  return -1;
}

int main(int argc, char *argv[argc]) {
  unsigned c, i, j, l;
  enum XedError err;
  struct XedDecodedInst xedd;
  unsigned char buf[XED_MAX_INSTRUCTION_BYTES];
  memset(buf, 0, sizeof(buf));
  for (i = 0;;) {
    if (i < XED_MAX_INSTRUCTION_BYTES) {
      c = fgethex(stdin);
      if (c != -1) {
        buf[i++] = c;
        continue;
      } else if (i == 0) {
        break;
      }
    }
    if ((err = xed_instruction_length_decode(
             xed_decoded_inst_zero_set_mode(&xedd, XED_MACHINE_MODE_LONG_64),
             buf, i))) {
      errno = err;
      break;
    }
    l = xedd.length;
    if (l <= 0 || l > i) abort();
    for (j = 0; j < l; ++j) {
      fputc("0123456789ABCDEF"[(buf[j] & 0xf0) >> 4], stdout);
      fputc("0123456789ABCDEF"[(buf[j] & 0x0f) >> 0], stdout);
    }
    putchar('\n');
    memcpy(&buf[0], &buf[l], i -= l);
  }
  return errno;
}
