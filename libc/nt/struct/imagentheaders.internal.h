#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_IMAGENTHEADERS_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_IMAGENTHEADERS_H_
#include "libc/nt/struct/imagefileheader.internal.h"
#include "libc/nt/struct/imageoptionalheader.internal.h"

struct NtImageNtHeaders {
  uint32_t Signature;
  struct NtImageFileHeader FileHeader;
  struct NtImageOptionalHeader OptionalHeader;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_IMAGENTHEADERS_H_ */
