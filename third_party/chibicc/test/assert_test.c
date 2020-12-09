#include "third_party/chibicc/test/test.h"

_Static_assert(1);
_Static_assert(1, "hey");

main() {
  _Static_assert(sizeof(int) == 4, "wut");
}
