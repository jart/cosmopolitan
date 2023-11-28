#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEROMOPTIONALHEADER_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEROMOPTIONALHEADER_H_

struct NtImageRomOptionalHeader {
  uint16_t Magic;
  uint8_t MajorLinkerVersion;
  uint8_t MinorLinkerVersion;
  uint32_t SizeOfCode;
  uint32_t SizeOfInitializedData;
  uint32_t SizeOfUninitializedData;
  uint32_t AddressOfEntryPoint;
  uint32_t BaseOfCode;
  uint32_t BaseOfData;
  uint32_t BaseOfBss;
  uint32_t GprMask;
  uint32_t CprMask[4];
  uint32_t GpValue;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEROMOPTIONALHEADER_H_ */
