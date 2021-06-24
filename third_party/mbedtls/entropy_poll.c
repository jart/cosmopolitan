#include "libc/nexgen32e/rdtsc.h"
#include "libc/str/str.h"
#include "third_party/mbedtls/entropy_poll.h"

int mbedtls_hardclock_poll(void *data, unsigned char *output, size_t len,
                           size_t *olen) {
  unsigned long timer;
  timer = rdtsc();
  *olen = 0;
  if (len < sizeof(unsigned long)) return 0;
  memcpy(output, &timer, sizeof(unsigned long));
  *olen = sizeof(unsigned long);
  return 0;
}
