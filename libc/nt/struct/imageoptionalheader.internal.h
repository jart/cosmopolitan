#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEOPTIONALHEADER_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEOPTIONALHEADER_H_
#include "libc/nt/pedef.internal.h"
#include "libc/nt/struct/imagedatadirectory.internal.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

/**
 * Portable Executable Optional Header.
 *
 * @see NtImageNtHeaders which encloses this
 * @see NtImageFileHeader which precedes this
 * @see NtImageSectionHeader which follows this
 */
struct NtImageOptionalHeader {

  /*
   * Must be kNtPe64bit.
   */
  uint16_t Magic;

  uint8_t MajorLinkerVersion;
  uint8_t MinorLinkerVersion;
  uint32_t SizeOfCode;
  uint32_t SizeOfInitializedData;
  uint32_t SizeOfUninitializedData;

  /*
   * The address of the entry point relative to the image base when the
   * executable file is loaded into memory. For program images, this is
   * the starting address. For device drivers, this is the address of
   * the initialization function. An entry point is optional for DLLs.
   * When no entry point is present, this field must be zero.
   */
  uint32_t AddressOfEntryPoint;

  uint32_t BaseOfCode;

  /*
   * The preferred address of the first byte of image when loaded into
   * memory; must be a multiple of 64 K. The default for DLLs is
   * 0x10000000. The default for Windows CE EXEs is 0x00010000. The
   * default for Windows NT, Windows 2000, Windows XP, Windows 95,
   * Windows 98, and Windows Me is 0x00400000.
   */
  uint64_t ImageBase;

  /*
   * The alignment (in bytes) of sections when they are loaded into
   * memory. It must be greater than or equal to FileAlignment. The
   * default is the page size for the architecture.
   */
  uint32_t SectionAlignment;

  /*
   * The alignment factor (in bytes) that is used to align the raw data
   * of sections in the image file. The value should be a power of 2
   * between 512 and 64 K, inclusive. The default is 512. If the
   * SectionAlignment is less than the architecture's page size, then
   * FileAlignment must match SectionAlignment.
   */
  uint32_t FileAlignment;

  uint16_t MajorOperatingSystemVersion;
  uint16_t MinorOperatingSystemVersion;
  uint16_t MajorImageVersion;
  uint16_t MinorImageVersion;
  uint16_t MajorSubsystemVersion;
  uint16_t MinorSubsystemVersion;
  uint32_t Win32VersionValue;

  /*
   * The size (in bytes) of the image, including all headers, as the
   * image is loaded in memory. It must be a multiple of
   * SectionAlignment.
   */
  uint32_t SizeOfImage;

  /*
   * The combined size of an MS-DOS stub, PE header, and section headers
   * rounded up to a multiple of FileAlignment.
   */
  uint32_t SizeOfHeaders;

  uint32_t CheckSum;
  uint16_t Subsystem;
  uint16_t DllCharacteristics;
  uint64_t SizeOfStackReserve;
  uint64_t SizeOfStackCommit;
  uint64_t SizeOfHeapReserve;
  uint64_t SizeOfHeapCommit;
  uint32_t LoaderFlags;
  uint32_t NumberOfRvaAndSizes;
  struct NtImageDataDirectory DataDirectory[];
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEOPTIONALHEADER_H_ */
