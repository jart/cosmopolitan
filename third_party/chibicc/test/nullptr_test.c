#include "third_party/chibicc/test/test.h"

int main() {
  nullptr_t zero = nullptr;
  ASSERT(8, sizeof(nullptr));
  ASSERT(8, alignof(nullptr));
  ASSERT(8, sizeof(nullptr_t));
  ASSERT(8, alignof(nullptr_t));
  return zero;
}
