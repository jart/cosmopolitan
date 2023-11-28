#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEFILEHEADER_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEFILEHEADER_H_

struct NtImageFileHeader {

  /*
   * E.g. kNtImageFileMachineNexgen32e
   */
  uint16_t Machine;

  /*
   * The number of sections. This indicates the size of the section
   * table, which immediately follows the headers.
   */
  uint16_t NumberOfSections;

  uint32_t TimeDateStamp;

  uint32_t PointerToSymbolTable;

  uint32_t NumberOfSymbols;

  /*
   * [file size] The size of the optional header, which is required for
   * executable files but not for object files. This value should be
   * zero for an object file. For a description of the header format,
   * see Optional Header (Image Only).
   */
  uint16_t SizeOfOptionalHeader;

  /*
   * E.g. kNtPeFileExecutableImage | kNtImageFileLargeAddressAware
   */
  uint16_t Characteristics;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEFILEHEADER_H_ */
