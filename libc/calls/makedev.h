#ifndef COSMOPOLITAN_LIBC_CALLS_MAKEDEV_H_
#define COSMOPOLITAN_LIBC_CALLS_MAKEDEV_H_
#include "libc/dce.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

static inline uint64_t major(uint64_t x) {
  if (IsXnu()) {
    return (x >> 24) & 0xff;
  } else if (IsNetbsd()) {
    return (x & 0x000fff00) >> 8;
  } else if (IsOpenbsd()) {
    return (x >> 8) & 0xff;
  } else if (IsFreebsd()) {
    return ((x >> 32) & 0xffffff00) | ((x >> 8) & 0x000000ff);
  } else {
    return ((x >> 32) & 0xfffff000) | ((x >> 8) & 0x00000fff);
  }
}

static inline uint64_t minor(uint64_t x) {
  if (IsXnu()) {
    return x & 0x00ffffff;
  } else if (IsNetbsd()) {
    return (x & 0x000000ff) | (x & 0xfff00000) >> 12;
  } else if (IsOpenbsd()) {
    return (x & 0x000000ff) | (x & 0x0ffff000) >> 8;
  } else if (IsFreebsd()) {
    return ((x >> 24) & 0x0000ff00) | (x & 0xffff00ff);
  } else {
    return ((x >> 12) & 0xffffff00) | (x & 0x000000ff);
  }
}

static inline uint64_t makedev(uint64_t x, uint64_t y) {
  if (IsXnu()) {
    return x << 24 | y;
  } else if (IsNetbsd()) {
    return ((x << 8) & 0x000fff00) | ((y << 12) & 0xfff00000u) |
           (y & 0x000000ff);
  } else if (IsOpenbsd()) {
    return (x & 0xff) << 8 | (y & 0xff) | (y & 0xffff00) << 8;
  } else if (IsFreebsd()) {
    return (x & 0xffffff00) << 32 | (x & 0x000000ff) << 8 |
           (y & 0x0000ff00) << 24 | (y & 0xffff00ff);
  } else {
    return (x & 0xfffff000) << 32 | (x & 0x00000fff) << 8 |
           (y & 0xffffff00) << 12 | (y & 0x000000ff);
  }
}

#define major(x)      major(x)
#define minor(x)      minor(x)
#define makedev(x, y) makedev(x, y)

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_MAKEDEV_H_ */
