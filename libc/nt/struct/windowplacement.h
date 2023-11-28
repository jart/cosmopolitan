#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_WINDOWPLACEMENT_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_WINDOWPLACEMENT_H_
#include "libc/nt/struct/point.h"
#include "libc/nt/struct/rect.h"

struct NtWindowPlacement {
  uint32_t length;
  uint32_t flags;
  uint32_t showCmd;
  struct NtPoint ptMinPosition;
  struct NtPoint ptMaxPosition;
  struct NtRect rcNormalPosition;
  struct NtRect rcDevice;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_WINDOWPLACEMENT_H_ */
