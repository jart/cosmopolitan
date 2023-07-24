#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEIMPORTDESCRIPTOR_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEIMPORTDESCRIPTOR_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct NtImageImportDescriptor {
  uint32_t ImportLookupTable;
  uint32_t TimeDateStamp;
  uint32_t ForwarderChain;
  uint32_t DllNameRva;
  uint32_t ImportAddressTable;
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEIMPORTDESCRIPTOR_H_ */
