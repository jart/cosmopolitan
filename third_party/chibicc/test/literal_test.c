#include "third_party/chibicc/test/test.h"

int main() {
  ASSERT(97, 'a');
  ASSERT(10, '\n');
  ASSERT(-128, '\x80');

  ASSERT(511, 0777);
  ASSERT(0, 0x0);
  ASSERT(10, 0xa);
  ASSERT(10, 0XA);
  ASSERT(48879, 0xbeef);
  ASSERT(48879, 0xBEEF);
  ASSERT(48879, 0XBEEF);
  ASSERT(0, 0b0);
  ASSERT(1, 0b1);
  ASSERT(47, 0b101111);
  ASSERT(47, 0B101111);

  ASSERT(4, sizeof(0));
  ASSERT(8, sizeof(0L));
  ASSERT(8, sizeof(0LU));
  ASSERT(8, sizeof(0UL));
  ASSERT(8, sizeof(0LL));
  ASSERT(8, sizeof(0LLU));
  ASSERT(8, sizeof(0Ull));
  ASSERT(8, sizeof(0l));
  ASSERT(8, sizeof(0ll));
  ASSERT(8, sizeof(0x0L));
  ASSERT(8, sizeof(0b0L));
  ASSERT(4, sizeof(2147483647));
  ASSERT(8, sizeof(2147483648));
  ASSERT(-1, 0xffffffffffffffff);
  ASSERT(8, sizeof(0xffffffffffffffff));
  ASSERT(4, sizeof(4294967295U));
  ASSERT(8, sizeof(4294967296U));

  ASSERT(3, -1U >> 30);
  ASSERT(3, -1Ul >> 62);
  ASSERT(3, -1ull >> 62);

  ASSERT(1, 0xffffffffffffffffl >> 63);
  ASSERT(1, 0xffffffffffffffffll >> 63);

  ASSERT(-1, 18446744073709551615);
  ASSERT(8, sizeof(18446744073709551615));
  ASSERT(-1, 18446744073709551615 >> 63);

  ASSERT(-1, 0xffffffffffffffff);
  ASSERT(8, sizeof(0xffffffffffffffff));
  ASSERT(1, 0xffffffffffffffff >> 63);

  ASSERT(-1, 01777777777777777777777);
  ASSERT(8, sizeof(01777777777777777777777));
  ASSERT(1, 01777777777777777777777 >> 63);

  ASSERT(-1,
         0b1111111111111111111111111111111111111111111111111111111111111111);
  ASSERT(
      8,
      sizeof(
          0b1111111111111111111111111111111111111111111111111111111111111111));
  ASSERT(
      1,
      0b1111111111111111111111111111111111111111111111111111111111111111 >> 63);

  ASSERT(8, sizeof(2147483648));
  ASSERT(4, sizeof(2147483647));

  ASSERT(8, sizeof(0x1ffffffff));
  ASSERT(4, sizeof(0xffffffff));
  ASSERT(1, 0xffffffff >> 31);

  ASSERT(8, sizeof(040000000000));
  ASSERT(4, sizeof(037777777777));
  ASSERT(1, 037777777777 >> 31);

  ASSERT(8, sizeof(0b111111111111111111111111111111111));
  ASSERT(4, sizeof(0b11111111111111111111111111111111));
  ASSERT(1, 0b11111111111111111111111111111111 >> 31);

  ASSERT(-1, 1 << 31 >> 31);
  ASSERT(-1, 01 << 31 >> 31);
  ASSERT(-1, 0x1 << 31 >> 31);
  ASSERT(-1, 0b1 << 31 >> 31);

  0.0;
  1.0;
  3e+8;
  0x10.1p0;
  .1E4f;

  ASSERT(4, sizeof(8f));
  ASSERT(4, sizeof(0.3F));
  ASSERT(8, sizeof(0.));
  ASSERT(8, sizeof(.0));
  ASSERT(16, sizeof(5.l));
  ASSERT(16, sizeof(2.0L));

  Assert(1, size\
of(char),
         "sizeof(char)");

  ASSERT(4, sizeof(L'\0'));
  ASSERT(97, L'a');

  return 0;
}
