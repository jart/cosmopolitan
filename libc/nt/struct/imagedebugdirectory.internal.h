#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEDEBUGDIRECTORY_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEDEBUGDIRECTORY_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct NtImageDebugDirectory {
  uint32_t Characteristics;
  uint32_t TimeDateStamp;
  uint16_t MajorVersion;
  uint16_t MinorVersion;
  uint32_t Type;
  uint32_t SizeOfData;
  uint32_t AddressOfRawData;
  uint32_t PointerToRawData;
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEDEBUGDIRECTORY_H_ */
