#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_IMAGESECTIONHEADER_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_IMAGESECTIONHEADER_H_
#include "libc/nt/pedef.internal.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct NtImageSectionHeader {
  uint8_t Name[kNtImageSizeofShortName];
  union {
    uint32_t PhysicalAddress;
    uint32_t VirtualSize;
  } Misc;
  uint32_t VirtualAddress;
  uint32_t SizeOfRawData;
  uint32_t PointerToRawData;
  uint32_t PointerToRelocations;
  uint32_t PointerToLinenumbers;
  uint16_t NumberOfRelocations;
  uint16_t NumberOfLinenumbers;
  uint32_t Characteristics;
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_IMAGESECTIONHEADER_H_ */
