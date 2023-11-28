#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_IMAGESEPARATEDEBUGHEADER_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_IMAGESEPARATEDEBUGHEADER_H_

struct NtImageSeparateDebugHeader {
  uint16_t Signature;
  uint16_t Flags;
  uint16_t Machine;
  uint16_t Characteristics;
  uint32_t TimeDateStamp;
  uint32_t CheckSum;
  uint32_t ImageBase;
  uint32_t SizeOfImage;
  uint32_t NumberOfSections;
  uint32_t ExportedNamesSize;
  uint32_t DebugDirectorySize;
  uint32_t SectionAlignment;
  uint32_t Reserved[2];
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_IMAGESEPARATEDEBUGHEADER_H_ */
