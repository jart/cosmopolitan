#include "third_party/chibicc/test/test.h"

int x;

int main(void) {
  if (0) {
    asm(".error \"the assembler shall fail\"");
  }

  x = 1 ? 777 : ({
    asm(".error \"the system is down\"");
    666;
  });
  ASSERT(777, x);

  x = 0;
  x = 777 ?: ({
    asm(".error \"the system is down\"");
    666;
  });

  x = 0;
  x = __builtin_popcount(strlen("hihi")) == 1 ? 777 : ({
    asm(".error \"the system is down\"");
    666;
  });
  ASSERT(777, x);

  x = 0;
  x = strpbrk("hihi", "ei") ? 777 : ({
    asm(".error \"the system is down!\"");
    666;
  });
  ASSERT(777, x);

  x = 0;
  x = !__builtin_strpbrk("HELLO\n", "bxdinupo") ? 777 : ({
    asm(".error \"the system is down\"");
    666;
  });
  ASSERT(777, x);
}
