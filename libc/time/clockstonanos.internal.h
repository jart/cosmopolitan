#ifndef COSMOPOLITAN_LIBC_TIME_CLOCKSTONANOS_INTERNAL_H_
#define COSMOPOLITAN_LIBC_TIME_CLOCKSTONANOS_INTERNAL_H_
COSMOPOLITAN_C_START_

static inline uint64_t ClocksToNanos(uint64_t x, uint64_t y) {
  // approximation of round(x*.323018) which is usually
  // the ratio between inva rdtsc ticks and nanoseconds
  uint128_t difference = x - y;
  return (difference * 338709) >> 20;
}

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_TIME_CLOCKSTONANOS_INTERNAL_H_ */
