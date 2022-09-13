#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/mem/mem.h"
#include "libc/mem/gc.h"
#include "libc/stdio/stdio.h"

/**
 * Cosmopolitan C is just as awesome as Go!
 * Example contributed by @Keithcat1 (#266)
 */
main() {
  _defer(printf, "Done!\n");
  for (long i = 0; i < 100000; i++) {
    printf("%i ", i);
    _defer(free, malloc(100));  // longhand for _gc(malloc(100))
  }
}
