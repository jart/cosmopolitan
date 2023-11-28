#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_IMAGESECTIONHEADER_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_IMAGESECTIONHEADER_H_

struct NtImageSectionHeader {

  char Name[8];

  union {

    uint32_t PhysicalAddress;

    /*
     * The total size of the section when loaded into memory. If this
     * value is greater than SizeOfRawData, the section is zero-padded.
     * This field is valid only for executable images and should be set
     * to zero for object files.
     */
    uint32_t VirtualSize;

  } Misc;

  /*
   * In an image file, the VAs for sections must be assigned by the
   * linker so that they are in ascending order and adjacent, and they
   * must be a multiple of the SectionAlignment value in the optional
   * header.
   *
   * For executable images, the address of the first byte of the section
   * relative to the image base when the section is loaded into memory.
   * For object files, this field is the address of the first byte
   * before relocation is applied; for simplicity, compilers should set
   * this to zero. Otherwise, it is an arbitrary value that is
   * subtracted from offsets during relocation.
   */
  uint32_t VirtualAddress;

  /*
   * The size of the section (for object files) or the size of the
   * initialized data on disk (for image files). For executable images,
   * this must be a multiple of FileAlignment from the optional header.
   * If this is less than VirtualSize, the remainder of the section is
   * zero-filled. Because the SizeOfRawData field is rounded but the
   * VirtualSize field is not, it is possible for SizeOfRawData to be
   * greater than VirtualSize as well. When a section contains only
   * uninitialized data, this field should be zero.
   */
  uint32_t SizeOfRawData;

  /*
   * The file pointer to the first page of the section within the COFF
   * file. For executable images, this must be a multiple of
   * FileAlignment from the optional header. For object files, the value
   * should be aligned on a 4-byte boundary for best performance. When a
   * section contains only uninitialized data, this field should be
   * zero.
   */
  uint32_t PointerToRawData;

  uint32_t PointerToRelocations;
  uint32_t PointerToLinenumbers;
  uint16_t NumberOfRelocations;
  uint16_t NumberOfLinenumbers;
  uint32_t Characteristics;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_IMAGESECTIONHEADER_H_ */
