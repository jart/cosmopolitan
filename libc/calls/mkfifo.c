#include "libc/calls/calls.h"
#include "libc/sysv/consts/s.h"

int mkfifo(const char *path, uint32_t mode) {
  return mknod(path, mode | S_IFIFO, 0);
}
