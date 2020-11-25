#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEIMPORTDESCRIPTOR_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEIMPORTDESCRIPTOR_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct NtImageImportDescriptor {
  union {
    uint32_t Characteristics;
    uint32_t OriginalFirstThunk;
  };
  uint32_t TimeDateStamp;
  uint32_t ForwarderChain;
  uint32_t Name;
  uint32_t FirstThunk;
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEIMPORTDESCRIPTOR_H_ */
