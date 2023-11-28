#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_WNDCLASS_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_WNDCLASS_H_
#include "libc/nt/typedef/wndproc.h"
COSMOPOLITAN_C_START_

struct NtWndClass {
  uint32_t style;
  NtWndProc lpfnWndProc;
  int32_t cbClsExtra;
  int32_t cbWndExtra;
  int64_t hInstance;
  int64_t hIcon;
  int64_t hCursor;
  int64_t hbrBackground;
  const char16_t *lpszMenuName;
  const char16_t *lpszClassName;
};

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_WNDCLASS_H_ */
