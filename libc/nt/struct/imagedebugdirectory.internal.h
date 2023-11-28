#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEDEBUGDIRECTORY_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEDEBUGDIRECTORY_H_

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

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEDEBUGDIRECTORY_H_ */
