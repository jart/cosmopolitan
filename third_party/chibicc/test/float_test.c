#include "third_party/chibicc/test/test.h"

int main() {
  ASSERT(35, (float)(char)35);
  ASSERT(35, (float)(short)35);
  ASSERT(35, (float)(int)35);
  ASSERT(35, (float)(long)35);
  ASSERT(35, (float)(unsigned char)35);
  ASSERT(35, (float)(unsigned short)35);
  ASSERT(35, (float)(unsigned int)35);
  ASSERT(35, (float)(unsigned long)35);

  ASSERT(35, (double)(char)35);
  ASSERT(35, (double)(short)35);
  ASSERT(35, (double)(int)35);
  ASSERT(35, (double)(long)35);
  ASSERT(35, (double)(unsigned char)35);
  ASSERT(35, (double)(unsigned short)35);
  ASSERT(35, (double)(unsigned int)35);
  ASSERT(35, (double)(unsigned long)35);

  ASSERT(35, (char)(float)35);
  ASSERT(35, (short)(float)35);
  ASSERT(35, (int)(float)35);
  ASSERT(35, (long)(float)35);
  ASSERT(35, (unsigned char)(float)35);
  ASSERT(35, (unsigned short)(float)35);
  ASSERT(35, (unsigned int)(float)35);
  ASSERT(35, (unsigned long)(float)35);

  ASSERT(35, (char)(double)35);
  ASSERT(35, (short)(double)35);
  ASSERT(35, (int)(double)35);
  ASSERT(35, (long)(double)35);
  ASSERT(35, (unsigned char)(double)35);
  ASSERT(35, (unsigned short)(double)35);
  ASSERT(35, (unsigned int)(double)35);
  ASSERT(35, (unsigned long)(double)35);

  ASSERT(0x8000000000000000, (double)(unsigned long)(long)-1);

  ASSERT(1, 2e3 == 2e3);
  ASSERT(0, 2e3 == 2e5);
  ASSERT(1, 2.0 == 2);
  ASSERT(0, 5.1 < 5);
  ASSERT(0, 5.0 < 5);
  ASSERT(1, 4.9 < 5);
  ASSERT(0, 5.1 <= 5);
  ASSERT(1, 5.0 <= 5);
  ASSERT(1, 4.9 <= 5);

  ASSERT(1, 2e3f == 2e3);
  ASSERT(0, 2e3f == 2e5);
  ASSERT(1, 2.0f == 2);
  ASSERT(0, 5.1f < 5);
  ASSERT(0, 5.0f < 5);
  ASSERT(1, 4.9f < 5);
  ASSERT(0, 5.1f <= 5);
  ASSERT(1, 5.0f <= 5);
  ASSERT(1, 4.9f <= 5);

  ASSERT(6, 2.3 + 3.8);
  ASSERT(-1, 2.3 - 3.8);
  ASSERT(-3, -3.8);
  ASSERT(13, 3.3 * 4);
  ASSERT(2, 5.0 / 2);

  ASSERT(6, 2.3f + 3.8f);
  ASSERT(6, 2.3f + 3.8);
  ASSERT(-1, 2.3f - 3.8);
  ASSERT(-3, -3.8f);
  ASSERT(13, 3.3f * 4);
  ASSERT(2, 5.0f / 2);

  ASSERT(0, 0.0 / 0.0 == 0.0 / 0.0);
  ASSERT(1, 0.0 / 0.0 != 0.0 / 0.0);

  ASSERT(0, 0.0 / 0.0 < 0);
  ASSERT(0, 0.0 / 0.0 <= 0);
  ASSERT(0, 0.0 / 0.0 > 0);
  ASSERT(0, 0.0 / 0.0 >= 0);

  ASSERT(0, !3.);
  ASSERT(1, !0.);
  ASSERT(0, !3.f);
  ASSERT(1, !0.f);

  ASSERT(5, 0.0 ? 3 : 5);
  ASSERT(3, 1.2 ? 3 : 5);

  return 0;
}
