#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_IMAGENTHEADERS_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_IMAGENTHEADERS_H_
#include "libc/nt/struct/imagefileheader.internal.h"
#include "libc/nt/struct/imageoptionalheader.internal.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct NtImageNtHeaders {
  uint32_t Signature;
  struct NtImageFileHeader FileHeader;
  struct NtImageOptionalHeader OptionalHeader;
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_IMAGENTHEADERS_H_ */
