#include "third_party/chibicc/test/test.h"

#define CRASH    \
  ({             \
    asm(".err"); \
    666;         \
  })

int main(void) {

  if (0) {
    return CRASH;
  }

  if (1) {
  } else {
    return CRASH;
  }

  ASSERT(777, 777 ?: CRASH);
  ASSERT(777, 1 ? 777 : CRASH);
  ASSERT(777, 0 ? CRASH : 777);
  ASSERT(777, __builtin_popcount(__builtin_strlen("hihi")) == 1 ? 777 : CRASH);
  ASSERT(777, !__builtin_strpbrk("HELLO\n", "bxdinupo") ? 777 : CRASH);
  ASSERT(777, strpbrk("hihi", "ei") ? 777 : CRASH);
}
