#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_MSG_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_MSG_H_
#include "libc/nt/struct/point.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct NtMsg {
  int64_t hwnd;
  uint32_t message;
  uint64_t wParam;
  int64_t lParam;
  uint32_t time;
  struct NtPoint pt;
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_MSG_H_ */
