#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEEPILOGUEDYNAMICRELOCATIONHEADER_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEEPILOGUEDYNAMICRELOCATIONHEADER_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct NtImageEpilogueDynamicRelocationHeader {
  uint32_t EpilogueCount;
  uint8_t EpilogueByteCount;
  uint8_t BranchDescriptorElementSize;
  uint16_t BranchDescriptorCount;
  /* uint8_t BranchDescriptors[...]; */
  /* uint8_t BranchDescriptorBitMap[...]; */
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEEPILOGUEDYNAMICRELOCATIONHEADER_H_ \
        */
