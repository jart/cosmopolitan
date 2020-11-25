#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEFILEHEADER_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEFILEHEADER_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct NtImageFileHeader {
  uint16_t Machine;
  uint16_t NumberOfSections;
  uint32_t TimeDateStamp;
  uint32_t PointerToSymbolTable;
  uint32_t NumberOfSymbols;
  uint16_t SizeOfOptionalHeader;
  uint16_t Characteristics;
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEFILEHEADER_H_ */
