#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_PAINTSTRUCT_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_PAINTSTRUCT_H_
#include "libc/nt/struct/rect.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct NtPaintStruct {
  int64_t hdc;
  bool32 fErase;
  struct NtRect rcPaint;
  bool32 fRestore;
  bool32 fIncUpdate;
  uint8_t rgbReserved[32];
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_PAINTSTRUCT_H_ */
