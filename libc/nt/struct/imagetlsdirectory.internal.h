#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_IMAGETLSDIRECTORY_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_IMAGETLSDIRECTORY_H_

struct NtImageTlsDirectory {
  uint64_t StartAddressOfRawData;
  uint64_t EndAddressOfRawData;
  uint64_t AddressOfIndex;
  uint64_t AddressOfCallBacks;
  uint32_t SizeOfZeroFill;
  uint32_t Characteristics;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_IMAGETLSDIRECTORY_H_ */
