#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEEXPORTDIRECTORY_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEEXPORTDIRECTORY_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct NtImageExportDirectory {
  uint32_t Characteristics;
  uint32_t TimeDateStamp;
  uint16_t MajorVersion;
  uint16_t MinorVersion;
  uint32_t Name;
  uint32_t Base;
  uint32_t NumberOfFunctions;
  uint32_t NumberOfNames;
  uint32_t AddressOfFunctions;
  uint32_t AddressOfNames;
  uint32_t AddressOfNameOrdinals;
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEEXPORTDIRECTORY_H_ */
