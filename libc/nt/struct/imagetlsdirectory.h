#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_IMAGETLSDIRECTORY_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_IMAGETLSDIRECTORY_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct NtImageTlsDirectory {
  uint64_t StartAddressOfRawData;
  uint64_t EndAddressOfRawData;
  uint64_t AddressOfIndex;
  uint64_t AddressOfCallBacks;
  uint32_t SizeOfZeroFill;
  uint32_t Characteristics;
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_IMAGETLSDIRECTORY_H_ */
