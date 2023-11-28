#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_MSG_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_MSG_H_
#include "libc/nt/struct/point.h"

struct NtMsg {
  int64_t hwnd;
  uint32_t dwMessage;
  uint64_t wParam;
  int64_t lParam;
  uint32_t dwTime;
  struct NtPoint pt;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_MSG_H_ */
